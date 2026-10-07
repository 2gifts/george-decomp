/* Initialized finite native observations. Genuine selected C and the complete
 * SGI fill/refill/chunk/OOM definitions run as separate translation units.
 * Engine heap core and its callbacks are controls, not recovered SDK behavior.
 * Pointer translation is limited to the trace's typed store ledger. */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "george/packet_construction.h"
#include "george/heap.h"
#include "packet_construction_golden.h"
extern unsigned char observed_heads[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE");
extern unsigned char observed_begin[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE");
extern unsigned char observed_end[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE");
extern unsigned char observed_size[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE");
extern void packet_install_handler(void (*handler)(void));
GeorgeHeap *D_003FD204;
const char D_00447238[]="controlled allocation failure";
static unsigned int *arena,checks,case_index,allocations,failed,event_count,events[128];
static const PacketCase *current;
#define BASE 0x20000u
#define WORDS 4096u
#define FIRST 0x20600u
#define SECOND 0x20700u
#define POOL 0x21000u
#define RANGE 0x20140u
static unsigned int initial_word(unsigned int i) { return 0xe25a8301u^(i*0x10203u); }
static void check(int truth) { ++checks;if(!truth){printf("packet case%u check%u failed\n",case_index,checks);exit(1);} }
static unsigned int bits(const void *address) {
    const unsigned char *p=(const unsigned char *)address;
    return p[0]|((unsigned int)p[1]<<8)|((unsigned int)p[2]<<16)|((unsigned int)p[3]<<24);
}
static void put_bits(void *address,unsigned int value) {
    unsigned char *p=(unsigned char *)address;unsigned int i;
    for(i=0;i<4;++i)p[i]=(unsigned char)(value>>(i*8));
}
static unsigned int native_word(unsigned int value) {
    if(BASE<=value && value<=BASE+WORDS*4)return (unsigned int)arena+value-BASE;
    return value;
}
static unsigned int guest_word(unsigned int value) {
    unsigned int start=(unsigned int)arena;
    if(start<=value && value<=start+WORDS*4)return BASE+value-start;
    return value;
}
static void *address(unsigned int guest) { check(BASE<=guest && guest<BASE+WORDS*4);return (unsigned char *)arena+guest-BASE; }
static void event(unsigned int kind,unsigned int a,unsigned int b,unsigned int c) {
    check(event_count+4<=128);events[event_count++]=kind;events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
}
static void put(unsigned int guest,const char *value) {
    unsigned char *p=(unsigned char *)address(guest);
    do { *p++=(unsigned char)*value; } while(*value++);
}
static void mutate(void) {
    if(current->p[4]==1){put(FIRST,"X");put(SECOND,"Y");}
    else if(current->p[4]==2){put(FIRST,"LongerFirstInput");put(SECOND,"LongerSecondInput");}
    else if(current->p[4]==3){put(FIRST,"");put(SECOND,"");}
}
static void handler(void) { event(3,failed,0,0);mutate(); }
void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment) {
    check(heap==(GeorgeHeap *)address(0x20040));check(alignment==3 || alignment==4);
    ++allocations;event(1,size,alignment,allocations);
    if(allocations==1){check(size==12 && alignment==4);mutate();return address(RANGE);}
    if(current->p[5] && !failed){++failed;return 0;}
    return address(POOL);
}
s32 func_00394F68(const char *format,...) {
    /* This specific diagnostic always carries final cursor NULL, size, align.
     * Read its authentic arguments rather than substituting a side effect. */
    va_list ap;void *heap;unsigned int size,align;
    check(format==D_00447238);va_start(ap,format);heap=va_arg(ap,void *);size=va_arg(ap,unsigned int);align=va_arg(ap,unsigned int);va_end(ap);
    event(2,guest_word((unsigned int)heap),size,align);return 0;
}
/* The primary SGI allocator's source-local malloc name calls the real reviewed
 * engine wrapper; no replacement allocator algorithm is inserted. */
void *george_sgi_engine_allocate(unsigned int size) { return func_002AF140(size); }
void george_sgi_engine_release(void *memory) { (void)memory;abort(); }
static unsigned char *global_address(unsigned int i) {
    if(i<16)return observed_heads+4*i;
    return i==16?observed_begin:i==17?observed_end:observed_size;
}
int main(void) {
    unsigned int i,j,result;arena=(unsigned int *)malloc(WORDS*4);check(arena!=0);
    for(case_index=0;case_index<sizeof(packet_cases)/sizeof(packet_cases[0]);++case_index){
        current=packet_cases+case_index;allocations=failed=event_count=0;
        for(i=0;i<WORDS;++i)arena[i]=initial_word(i);
        for(i=0;i<current->initial_count;++i)arena[current->initial[i].index]=current->initial[i].value;
        for(i=0;i<current->pointer_initial_count;++i){j=current->pointer_initial[i];arena[j]=native_word(arena[j]);}
        for(i=0;i<19;++i)put_bits(global_address(i),i==18?current->globals_initial[i]:native_word(current->globals_initial[i]));
        packet_install_handler(handler);D_003FD204=(GeorgeHeap *)native_word(current->globals_initial[20]);
        switch(current->p[0]){
        case 0:result=(u32)func_002BCB50((GeorgePacketRange **)native_word(current->args[0]),current->args[1],current->args[2],(const void *)native_word(current->args[3]),current->args[4]);break;
        case 1:result=(u32)func_002BCCA0((GeorgePacketRange **)native_word(current->args[0]),(const signed char *)native_word(current->args[1]),(const signed char *)native_word(current->args[2]),current->args[3]);break;
        case 2:result=func_002BCFA8((u8 *)native_word(current->args[0]),current->args[1],current->args[2],current->args[3],(const void *)native_word(current->args[4]),current->args[5]);break;
        case 3:result=func_002BD000((u8 *)native_word(current->args[0]),current->args[1],(const signed char *)native_word(current->args[2]),(const signed char *)native_word(current->args[3]),current->args[4]);break;
        default:result=(u32)func_002BD1F0((u8 *)native_word(current->args[0]),current->args[1],(const u8 *)native_word(current->args[2]));break;
        }
        if(current->p[0]==0 || current->p[0]==1 || current->p[0]==4)result=guest_word(result);
        check(result==current->result);check(event_count==current->event_count);
        for(i=0;i<event_count;++i)check(events[i]==current->events[i]);
        for(i=0;i<WORDS;++i){
            unsigned int expected=initial_word(i),actual=bits(arena+i);
            for(j=0;j<current->initial_count;++j)if(current->initial[j].index==i)expected=current->initial[j].value;
            for(j=0;j<current->change_count;++j)if(current->changes[j].index==i)expected=current->changes[j].value;
            for(j=0;j<current->pointer_expected_count;++j)if(current->pointer_expected[j]==i){actual=guest_word(actual);break;}
            if(actual!=expected){printf("word%u actual%08X expected%08X\n",i,actual,expected);}
            check(actual==expected);
        }
        for(i=0;i<19;++i)check((i==18?bits(global_address(i)):guest_word(bits(global_address(i))))==current->globals_expected[i]);
        check(current->globals_expected[19]==0xf1100000u);check(guest_word((u32)D_003FD204)==current->globals_expected[20]);
    }
    printf("packet_native_abi: pointer%u size%u long%u count%u range%u heads64\n",(unsigned int)sizeof(void *),(unsigned int)sizeof(size_t),(unsigned int)sizeof(long),(unsigned int)sizeof(u32),(unsigned int)sizeof(GeorgePacketRange));
    free(arena);printf("packet_construction: %u checks\n",checks);return 0;
}
