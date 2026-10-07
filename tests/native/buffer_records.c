/* Execute all six new C bodies with the published length TU and a pinned,
 * unchanged generic Newlib byte-path strncpy supporting bridge. */
#include "george/buffer_records.h"
#include <stdio.h>
#include <stdlib.h>
#include "buffer_records_golden.h"

#define GUEST 0x20000u
#define OWNER 0x20100u
#define EMPTY 0xF0000001u
static u8 storage[65536u+BUFFER_RECORD_WORDS*4u];
static u8 *arena;
static u32 expected[BUFFER_RECORD_WORDS];
static u32 events[8],event_count,checks;
static const struct BufferGolden *current;

extern char *buffer_support_strncpy(char *,const char *,size_t);
extern u32 buffer_real_strlen(const signed char *);

static void equal(u32 actual,u32 want,const char *what)
{
    ++checks;
    if (actual!=want) {
        printf("buffer check %u: %s: %08X != %08X\n",checks,what,actual,want);
        exit(1);
    }
}

static u32 guest(const void *pointer)
{
    u32 p=(u32)pointer,b=(u32)arena;
    if (p==0) return 0;
    if (p<b || p-b>=BUFFER_RECORD_WORDS*4u) {
        printf("buffer pointer outside owned representation\n");exit(1);
    }
    return GUEST+(p-b);
}

static void *host(u32 p)
{
    if (p==0) return 0;
    if (p<GUEST || p-GUEST>=BUFFER_RECORD_WORDS*4u) {
        printf("buffer guest pointer outside arena\n");exit(1);
    }
    return arena+(p-GUEST);
}

static u32 read_word(u32 offset)
{
    return *(u32 *)(arena+offset);
}

static void event(u32 kind,u32 first,u32 second,u32 count)
{
    if (event_count>4) {printf("buffer observer event bound\n");exit(1);}
    events[event_count++]=kind;events[event_count++]=first;
    events[event_count++]=second;events[event_count++]=count;
}

signed char *func_00394010(signed char *destination,const signed char *source,u32 count)
{
    u32 d=guest(destination),s;
    unsigned int i;
    u32 p=(u32)source,b=(u32)arena;
    if (p>=b && p-b<BUFFER_RECORD_WORDS*4u) s=guest(source);
    else {
        equal(current->routine,1,"outside source only local empty next string");
        equal((u8)*source,0,"local empty terminator");s=EMPTY;
    }
    if (count>32u || d-GUEST+count>BUFFER_RECORD_WORDS*4u ||
            (s!=EMPTY && s-GUEST+count>BUFFER_RECORD_WORDS*4u)) {
        printf("buffer support copy outside bounded input\n");exit(1);
    }
    if (s!=EMPTY && count && !(d+count<=s || s+count<=d)) {
        printf("buffer support requires nonoverlap\n");exit(1);
    }
    event(1,d,s,count);
    buffer_support_strncpy((char *)destination,(const char *)source,(size_t)count);
    /* Native representation only: a complete encoded guest-pointer store
     * into the known data-pointer cell is translated after the byte helper.
     * No new callback mutation or production helper behavior is introduced. */
    for (i=0;i<sizeof(buffer_pointer_cells)/sizeof(buffer_pointer_cells[0]);++i) {
        u32 cell=GUEST+4u*buffer_pointer_cells[i];
        if (d<cell+4u && cell<d+count) {
            equal(d,cell,"whole typed pointer byte store start");
            equal(count,4,"whole typed pointer byte store width");
            *(u32 *)(arena+cell-GUEST)=(u32)host(read_word(cell-GUEST));
        }
    }
    return destination;
}

u32 func_00295050(const signed char *text)
{
    event(2,guest(text),0,0);
    return buffer_real_strlen(text);
}

int main(void)
{
    unsigned int k,i,j;
    /* Matching low16 address bits makes the selected one/two-byte pointer
     * alias fixture representation exact. Higher pointer-byte edits excluded. */
    arena=(u8 *)(((u32)storage+65535u)&~65535u);
    equal(sizeof(void *),4,"native pointer");equal(sizeof(u32),4,"native word");
    equal(sizeof(GeorgeBufferRecord),12,"record prefix");
    for (k=0;k<sizeof(buffer_golden)/sizeof(buffer_golden[0]);++k) {
        const struct BufferGolden *c=&buffer_golden[k];
        GeorgeBufferRecords *owner=(GeorgeBufferRecords *)host(OWNER);
        u32 result=0;
        current=c;event_count=0;
        for (i=0;i<BUFFER_RECORD_WORDS;++i) expected[i]=0x5A5A0001u^(i*0x10203u);
        for (i=0;i<c->initial_count;++i) expected[c->initial[i].index]=c->initial[i].value;
        for (i=0;i<BUFFER_RECORD_WORDS;++i) ((u32 *)arena)[i]=expected[i];
        for (i=0;i<sizeof(buffer_pointer_cells)/sizeof(buffer_pointer_cells[0]);++i) {
            u32 index=buffer_pointer_cells[i];((u32 *)arena)[index]=(u32)host(expected[index]);
        }
        for (i=0;i<c->change_count;++i) expected[c->changes[i].index]=c->changes[i].value;
        switch(c->routine) {
        case 0:func_002A5230(owner);break;
        case 1:func_002A5290(owner);break;
        case 2:func_002A5398(owner);break;
        case 3:result=guest(func_002A53D0(owner,c->argument));break;
        case 4:result=guest(func_002A53E8(owner));break;
        case 5:func_002A53F8(owner,(const signed char *)host(c->argument));break;
        default:printf("unreviewed buffer routine\n");return 1;
        }
        equal(result,c->result,"observed pointer return");equal(event_count,c->event_count,"event count");
        for (i=0;i<event_count;++i) equal(events[i],c->events[i],"call order/arguments");
        for (i=0;i<BUFFER_RECORD_WORDS;++i) {
            u32 value=((u32 *)arena)[i];
            for (j=0;j<sizeof(buffer_pointer_cells)/sizeof(buffer_pointer_cells[0]);++j)
                if (i==buffer_pointer_cells[j]) {value=guest((void *)value);break;}
            equal(value,expected[i],"whole arena");
        }
    }
    printf("%u buffer_records checks passed\n",checks);
    return 0;
}
