/* Real selected C and genuine helper TUs; external SDK/core effects controlled. */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "george/file_operations.h"
#include "george/heap.h"
#include "george/string_registry.h"
#include "george/string_algorithms.h"
#include "george/deimos_tables.h"
#include "file_operations_golden.h"

GeorgeFileSlot D_00469BD0[20];
u32 D_003FD23C,D_003FD240;
void *D_003FD244;
u32 D_003FD1D8,D_003FD1DC;
GeorgeStringRegistryPair *D_003FD1E0;
GeorgeHeap *D_003FD204;
char D_00469A00[128],D_00469A80[128],D_00469B00[128];
const char D_00447238[]="controlled allocation diagnostic";

static u32 storage[2048],expected[2048],slot_expected[140];
static unsigned char text_expected[384];
static const struct FileGolden *current;
static u32 events[160][16],io_counts[10],event_count,core_calls,cache_calls,dma_calls,polls;
static unsigned long checks;

#define BASE 0x20000u
#define END (BASE+sizeof(storage))
#define SLOTS 0x469BD0u
#define PATH 0x20401u
#define HEAP 0x20A00u
#define ALT_HEAP 0x20A40u
#define TABLE 0x20B00u
#define CONTEXT 0x20D00u
#define ALT_CONTEXT 0x20D80u
#define MAP 0x20E00u
#define RECORD 0x20F00u
#define ALLOCATION 0x21200u
#define DATA 0x20800u

static void fail(const char *what){fprintf(stderr,"file test %s case%lu routine%u\n",what,(unsigned long)(current-file_golden),current->routine);}
static int equal(u32 a,u32 b,const char *what){++checks;if(a==b)return 1;fail(what);fprintf(stderr,"actual%08X expected%08X\n",a,b);return 0;}
static void *pointer(u32 a){
    if(a==0)return 0;
    if(a>=BASE && a<END)return (unsigned char *)storage+(a-BASE);
    if(a>=SLOTS && a<SLOTS+sizeof(D_00469BD0))return (unsigned char *)D_00469BD0+(a-SLOTS);
    fail("unmapped authored pointer");return 0;
}
static u32 canonical(u32 a){
    u32 begin=(u32)storage,slots=(u32)D_00469BD0;
    if(a>=begin && a<begin+sizeof(storage))return BASE+a-begin;
    if(a>=slots && a<slots+sizeof(D_00469BD0))return SLOTS+a-slots;
    return a;
}
static u32 hash(const char *text){u32 h=2166136261u;while(*text){h=(h^(unsigned char)*text++)*16777619u;}return h;}
static void event(u32 kind,u32 a,u32 b,u32 c,u32 d,u32 e){
    u32 *out,*slot=(u32 *)&D_00469BD0[current->slot];unsigned i;
    if(event_count>=160){fail("event bound");return;}
    out=events[event_count++];out[0]=kind;out[1]=a;out[2]=b;out[3]=c;out[4]=d;out[5]=e;
    out[6]=D_003FD23C;out[7]=D_003FD240;out[8]=canonical((u32)D_003FD244);
    for(i=0;i<7;++i)out[9+i]=canonical(slot[i]);
    ++io_counts[kind];
}

void func_00363AE0(s32 mode){
    if(mode!=0)fail("cache mode");event(2,0,0,0,0,0);++cache_calls;
    if(cache_calls==1){
        if(current->mutation==1){D_003FD23C=0xFFFFFFFFu;D_003FD240^=1u;D_003FD244=pointer(ALT_CONTEXT);*(char *)pointer(PATH)='Z';}
        else if(current->mutation==6)D_00469BD0[current->slot].field00=0;
        else if(current->mutation==8)memcpy(D_00469A00,"changed/",9);
    }
}
s32 func_003689B0(const char *path,s32 flags){
    if(flags!=1 && flags!=0x602)fail("open flags");event(3,hash(path),(u32)flags,0,0,0);
    if(current->mutation==2)D_00469BD0[current->slot].field04=0x11223344;
    return (s32)current->sdk_result;
}
s32 func_00368C40(s32 descriptor){event(4,(u32)descriptor,0,0,0,0);return (s32)current->sdk_result;}
s32 func_00368DB8(s32 descriptor,s32 offset,s32 whence){
    event(5,(u32)descriptor,(u32)offset,(u32)whence,0,0);
    if(current->mutation==3 && io_counts[5]==1)D_00469BD0[current->slot].field04=0;
    return (s32)(io_counts[5]==1?current->position:io_counts[5]==2?current->end:current->sdk_result);
}
s32 func_00368FF8(s32 descriptor,void *destination,s32 count){
    s32 result;u32 n,i;
    event(6,(u32)descriptor,canonical((u32)destination),(u32)count,0,0);n=io_counts[6];
    result=(s32)(current->routine==0?(n<=current->read_count?current->reads[n-1]:0):current->sdk_result);
    if(result>0)for(i=0;i<(u32)(result<16?result:16);++i)((unsigned char *)destination)[i]=(unsigned char)(0x51+i+n);
    return result;
}
s32 func_00369268(s32 descriptor,const void *source,s32 count){
    event(7,(u32)descriptor,canonical((u32)source),(u32)count,0,0);return (s32)current->sdk_result;
}
u32 func_00363C20(const GeorgeFileDmaPacket *packet,s32 count){
    if(count!=1)fail("DMA count");
    event(8,canonical(packet->source),packet->destination,packet->size,packet->attribute,(u32)count);++dma_calls;
    if(current->mutation==5 && dma_calls==1){GeorgeFileDmaPacket *write=(GeorgeFileDmaPacket *)packet;write->destination=0x12345678;write->size=0x76543210;write->attribute=0x55;}
    if(dma_calls>24)fail("DMA termination");return dma_calls%3==1?0:0x80000005u;
}
s32 func_00363C00(u32 token){
    if(token!=0x80000005u)fail("DMA token");event(9,token,0,0,0,0);++polls;
    if(polls>48)fail("DMA status termination");return polls%2?0:-1;
}
void *func_002ADF60(GeorgeHeap *heap,u32 n,u32 align){
    if((heap!=pointer(HEAP) && heap!=pointer(ALT_HEAP)) || (align!=3 && align!=6))fail("core allocation arguments");
    event(0,canonical((u32)heap),n,align,0,0);++core_calls;
    if(current->mutation==7 && core_calls==1)((GeorgeHeap *)pointer(HEAP))->next=pointer(ALT_HEAP);
    if(align==3)return current->registry_fail?0:pointer(TABLE);
    return core_calls<=current->failures?0:pointer(ALLOCATION);
}
void func_002AE158(void *memory){event(1,canonical((u32)memory),0,0,0,0);}
s32 func_00394F68(const char *format,...){
    va_list args;GeorgeHeap *heap;u32 n,align;
    if(format!=D_00447238)fail("diagnostic pointer");va_start(args,format);heap=va_arg(args,GeorgeHeap *);n=va_arg(args,u32);align=va_arg(args,u32);va_end(args);
    event(1,canonical((u32)heap),n,align,1,0);return 0;
}

/* Genuine unchanged licensed string functions are separate compiled TUs.
 * Their generic native byte paths are compatibility contracts, not retail
 * optimized-source identity or SDK implementation awards. */
extern void *file_real_memcpy(void *,const void *,size_t);
extern char *file_real_strcpy(char *,const char *);
extern char *file_real_strcat(char *,const char *);
extern int file_real_strcmp(const char *,const char *);
extern char *file_real_strlwr(char *);
extern char *file_real_strstr(const char *,const char *);
void *func_003934F8(void *d,const void *s,u32 n){return file_real_memcpy(d,s,n);}
char *func_00393B74(char *d,const char *s){return file_real_strcpy(d,s);}
char *func_00393758(char *d,const char *s){return file_real_strcat(d,s);}
s32 func_00393A28(const char *a,const char *b){return file_real_strcmp(a,b);}
char *func_003984D8(char *text){return file_real_strlwr(text);}
char *func_00398628(const char *a,const char *b){return file_real_strstr(a,b);}

/* Native-only bounded bridge for the complete92B archive helper order. It
 * executes genuine resolver/strstr/CRC/map sources; it is not target source
 * recovery, an enforced original path capacity or an archive class claim. */
extern const char D_004473C0[];
void *func_002B1BA8(void *context,const char *path){
    char resolved[64];char *found;u32 crc;GeorgeGenericMap *map;
    func_002ABAE8(path,resolved);found=func_00398628(resolved,D_004473C0);
    crc=func_0029C648((const signed char *)(found?found+7:resolved));
    map=*(GeorgeGenericMap **)((unsigned char *)context+0x50);
    return func_002A7C08(map,crc);
}

static int test(const struct FileGolden *c){
    unsigned i,j;u32 result=0,globals[7];unsigned char *texts[3];
    current=c;event_count=core_calls=cache_calls=dma_calls=polls=0;memset(io_counts,0,sizeof(io_counts));
    for(i=0;i<2048;++i)storage[i]=expected[i]=0xA50B0301u^(i*0x10203u);
    for(i=0;i<c->initial_count;++i)storage[c->initial[i].index]=expected[c->initial[i].index]=c->initial[i].value;
    for(i=0;i<c->pointer_count;++i){j=c->pointers[i];storage[j]=(u32)pointer(storage[j]);}
    for(i=0;i<c->change_count;++i)expected[c->changes[i].index]=c->changes[i].value;
    for(i=0;i<20;++i){
        u32 *words=(u32 *)&D_00469BD0[i];
        for(j=0;j<7;++j)words[j]=slot_expected[i*7+j]=0x13570000u+i*32+j;
        words[0]=slot_expected[i*7]=i<c->slot;
    }
    for(i=0;i<c->slot_change_count;++i)slot_expected[c->slot_changes[i].index]=c->slot_changes[i].value;
    D_003FD23C=c->open_count;D_003FD240=c->mode;D_003FD244=pointer(CONTEXT);
    D_003FD1D8=c->registry_table?1:0;D_003FD1DC=10;D_003FD1E0=c->registry_table?pointer(TABLE):0;D_003FD204=pointer(HEAP);
    texts[0]=(unsigned char *)D_00469A00;texts[1]=(unsigned char *)D_00469A80;texts[2]=(unsigned char *)D_00469B00;
    for(i=0;i<384;++i){text_expected[i]=i%128?0xA5:0;texts[i/128][i%128]=text_expected[i];}
    for(i=0;i<c->text_change_count;++i)text_expected[c->text_changes[i].index]=(unsigned char)c->text_changes[i].value;
    switch(c->routine){
    case 0:result=func_002B0F60(c->descriptor,0x11000000u,(s32)c->count);break;
    case 1:result=func_002B1138(pointer(PATH));break;
    case 2:func_002B1198(c->descriptor);break;
    case 3:result=(u32)func_002B11F0(c->descriptor,c->buffer_alias?(void *)&D_00469BD0[c->slot].field04:pointer(DATA),(s32)c->count);break;
    case 4:result=(u32)func_002B1408(c->descriptor,(s32)c->position,2);break;
    case 5:result=(u32)func_002B1468(c->descriptor);break;
    case 6:result=canonical((u32)func_002B0710(pointer(PATH)));break;
    case 7:result=func_002B10D8(pointer(PATH));break;
    case 8:result=(u32)func_002B1260(c->descriptor,pointer(DATA),(s32)c->count);break;
    case 9:result=(u32)func_002B1510(pointer(PATH));break;
    default:return 0;
    }
    if(!equal(result,c->result,"return"))return 0;
    for(i=0;i<2048;++i)if(!equal(canonical(storage[i]),expected[i],"arena")){fprintf(stderr,"word%u\n",i);return 0;}
    for(i=0;i<140;++i)if(!equal(canonical(((u32 *)D_00469BD0)[i]),slot_expected[i],"slots")){fprintf(stderr,"slotword%u\n",i);return 0;}
    globals[0]=D_003FD23C;globals[1]=D_003FD240;globals[2]=canonical((u32)D_003FD244);globals[3]=D_003FD1D8;globals[4]=D_003FD1DC;globals[5]=canonical((u32)D_003FD1E0);globals[6]=canonical((u32)D_003FD204);
    for(i=0;i<7;++i)if(!equal(globals[i],c->expected_globals[i],"globals"))return 0;
    for(i=0;i<384;++i)if(!equal(texts[i/128][i%128],text_expected[i],"registry text"))return 0;
    if(!equal(event_count,c->event_count,"event count"))return 0;
    for(i=0;i<event_count;++i)for(j=0;j<16;++j)if(!equal(events[i][j],c->events[16*i+j],"event")){fprintf(stderr,"event%u field%u\n",i,j);return 0;}
    return 1;
}
int main(void){unsigned i;for(i=0;i<sizeof(file_golden)/sizeof(file_golden[0]);++i)if(!test(&file_golden[i]))return 1;printf("file operations: %lu checks passed\n",checks);return 0;}
