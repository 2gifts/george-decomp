/* Selected production and exact recovered helper closures run in separate TUs.
 * Allocation/free/SDK/module/scheduler are explicit bounded observer controls.
 * Integer-address tokens are translated into authored owned fixture storage. */
#include "george/file_archive_lifecycle.h"
#include "george/string_algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "file_archive_lifecycle_golden.h"
static u32 arena[LIFECYCLE_WORDS],events[100][11],event_count,checks;
static const struct LifecycleGolden *current;
static u32 allocations,small_allocations,buffers,frees,sync_attempts,read_attempts[37];
u32 D_003FD23C,D_003FD240,D_003FD200;
void *D_003FD244;
GeorgeHeap *D_003FD204,*D_00469B80[20];
GeorgeFileSlot D_00469BD0[20];
extern const char D_004471F8[],D_00447238[],D_00447398[],D_004473C8[],D_004473D8[],D_004473E0[],D_004473E8[],D_004473F8[],D_00447410[],D_00447428[];
extern char *lifecycle_real_strncpy(char *,const char *,u32);
extern void *lifecycle_real_memcpy(void *,const void *,u32);
extern void *lifecycle_real_memset(void *,int,u32);
extern char *lifecycle_real_strcpy(char *,const char *),*lifecycle_real_strcat(char *,const char *);
extern char *lifecycle_real_strncat(char *,const char *,u32),*lifecycle_real_strlwr(char *);
extern int lifecycle_real_strncmp(const char *,const char *,u32);
char *func_003934F8(char *d,const char *s,u32 n){return (char *)lifecycle_real_memcpy(d,s,n);}
void *func_003936A0(void *d,s32 v,u32 n){return lifecycle_real_memset(d,v,n);}
char *func_00393758(char *d,const char *s){return lifecycle_real_strcat(d,s);}
char *func_00393B74(char *d,const char *s){return lifecycle_real_strcpy(d,s);}
char *func_00393C90(char *d,const char *s,u32 n){return lifecycle_real_strncat(d,s,n);}
s32 func_00393E48(const char *a,const char *b,u32 n){return lifecycle_real_strncmp(a,b,n);}
char *func_003984D8(char *s){return lifecycle_real_strlwr(s);}
#define BASE 0x20000u
#define END (BASE+LIFECYCLE_WORDS*4u)
#define CTX 0x20000u
#define MAP 0x20400u
#define ALTMAP 0x20440u
#define ALTBUCKETS 0x20540u
#define HEAP 0x22000u
static void fail(const char *why,u32 actual,u32 expected)
{fprintf(stderr,"lifecycle case%u %s: %08X != %08X\n",(unsigned)(current-lifecycle_golden),why,(unsigned)actual,(unsigned)expected);exit(1);}
static void equal(const char *why,u32 actual,u32 expected){++checks;if(actual!=expected)fail(why,actual,expected);}
static void *ptr(u32 value){if(value<BASE || value>=END)fail("pointer input outside arena",value,BASE);return (u8 *)arena+value-BASE;}
static u32 translate(u32 value){return value>=BASE && value<END?(u32)ptr(value):value;}
static u32 canonical(u32 value)
{
    u32 first=(u32)arena;
    if(value>=first && value<first+sizeof(arena))return BASE+value-first;
    if(value==(u32)D_00447398)return 0x447398;
    if(value==(u32)D_004473F8)return 0x4473F8;
    if(value==(u32)D_00447410)return 0x447410;
    if(value==(u32)D_00447428)return 0x447428;
    return value;
}
static u32 word(u32 address){return *(u32 *)ptr(address);}
static void store(u32 address,u32 value){*(u32 *)ptr(address)=translate(value);}
static void event(u32 kind,u32 a,u32 b,u32 c,u32 d)
{
    u32 *row;
    if(event_count>=100)fail("event bound",event_count,99);
    row=events[event_count++];row[0]=kind;row[1]=canonical(a);row[2]=canonical(b);row[3]=canonical(c);row[4]=canonical(d);
    row[5]=canonical(word(CTX+0x20));row[6]=canonical(word(CTX+0x24));row[7]=canonical(word(CTX+0x50));row[8]=canonical(word(CTX+0x54));row[9]=D_003FD200;row[10]=D_003FD23C;
}
void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment)
{
    u32 result=0;
    equal("core heap",canonical((u32)heap),HEAP);
    if(alignment!=3 && alignment!=7)fail("core alignment",alignment,3);
    event(1,(u32)heap,size,alignment,0);++allocations;
    if(size==88)result=CTX;
    else if(size==0x3C000)result=HEAP;
    else if(size==8108 || current->routine==6)result=0x23000;
    else if(size==2048){result=0x21000+2048*buffers++;if(buffers>4)fail("buffer contract bound",buffers,4);}
    else if(size==12){result=(small_allocations%2==0?0x25000:0x25800)+(small_allocations/2)*32;++small_allocations;}
    else fail("allocation size",size,12);
    if(current->failure&(1u<<(allocations-1)))result=0;
    if((current->mutation&1) && size==12)store(CTX+0x50,ALTMAP);
    return result?ptr(result):0;
}
void func_002AE158(void *memory)
{
    event(2,(u32)memory,0,0,0);++frees;
    if((current->mutation&2) && frees==1){store(MAP+12,ALTBUCKETS);store(MAP+8,7);}
    if(current->mutation&4)store(CTX+0x50,ALTMAP);
}
s32 func_00394F68(const char *format,...)
{
    va_list ap;GeorgeHeap *heap;u32 size,alignment;
    equal("diagnostic literal",(u32)(format==D_00447238),1);
    va_start(ap,format);heap=va_arg(ap,GeorgeHeap *);size=va_arg(ap,u32);alignment=va_arg(ap,u32);va_end(ap);
    event(3,(u32)heap,size,alignment,0);
    return 0;
}
/* Native-only bridge, full160-byte original create order reviewed separately.
 * It calls the genuine published wrapper and pinned generic strncpy. */
GeorgeHeap *func_002AE830(u32 size,const char *name,u32 alignment)
{
    GeorgeHeap *heap=func_002AEB60(size,alignment);
    GeorgeHeapBlock *initial=(GeorgeHeapBlock *)((u32)heap+0x38);
    heap->alignment=3;heap->initial.field00.next=initial;heap->minimum_free=size-0x40u;
    heap->next=0;heap->flags=0;heap->field20=heap;heap->total=size;heap->used=0;
    heap->cursor=initial;heap->initial.size=size-0x40u;
    if(name==0)name=D_004471F8;
    lifecycle_real_strncpy(heap->name,name,24);heap->name[23]=0;return heap;
}
u32 func_00377820(s32 mode)
{equal("sync mode",(u32)mode,1);event(4,(u32)mode,0,0,0);++sync_attempts;return current->retry && sync_attempts%2?1:0;}
u32 func_00378038(s32 mode)
{equal("readiness mode",(u32)mode,0);event(5,(u32)mode,0,0,0);return 0;}
static void le16(u8 *b,u32 value){b[0]=(u8)value;b[1]=(u8)(value>>8);}
static void le32(u8 *b,u32 value){b[0]=(u8)value;b[1]=(u8)(value>>8);b[2]=(u8)(value>>16);b[3]=(u8)(value>>24);}
static void record(u8 *buffer,u32 *offset,const char *name,u32 n,u32 extent,u32 size,u32 flags)
{
    u32 stride=(33+n+3)&~3u;u8 *b=buffer+*offset;
    if(*offset+stride+2>2048)fail("authored sector bound",*offset,0);
    le16(b,stride);le32(b+2,extent);le32(b+10,size);b[25]=(u8)flags;b[32]=(u8)n;
    if(n)memcpy(b+33,name,n);*offset+=stride;
}
u32 func_00378258(u32 sector,u32 count,void *destination,const void *mode)
{
    u8 *b=destination;u32 offset=0,scenario=current->scenario,attempt;
    equal("read count",count,1);equal("read mode",canonical((u32)mode),CTX+0x28);
    if(sector>=37)fail("sector bound",sector,36);
    event(6,sector,count,(u32)destination,(u32)mode);attempt=read_attempts[sector]++;
    memset(b,0,2048);
    if(sector==16){
        b[0]=(u8)(scenario==5?2:1);memcpy(b+1,scenario==6?"WRONG":"CD001",5);
        memcpy(b+40,current->prefix,strlen(current->prefix));if(scenario==7)b[40]='Z';
        le16(b+128,scenario==8?1024:2048);le32(b+140,9);
    }else if(sector==9)record(b,&offset,"\0",1,10,2048,2);
    else if(sector==10){
        record(b,&offset,"\0",1,10,scenario==4?4096:2048,2);
        if(scenario==1)record(b,&offset,"A.TXT;1",7,30,123,0);
        if(scenario==2){record(b,&offset,"\1",1,0,0,0);record(b,&offset,"",0,31,7,0);record(b,&offset,"XY",2,32,8,0);}
        if(scenario==3){record(b,&offset,"DIR1",4,20,2048,2);record(b,&offset,"B.TXT;1",7,33,9,0);record(b,&offset,"DIR2",4,21,2048,2);}
    }else if(sector==11)record(b,&offset,"C2.TXT;1",8,34,11,0);
    else if(sector==20 || sector==21){record(b,&offset,"\0",1,sector,2048,2);record(b,&offset,sector==20?"C.DAT;1":"D.DAT;1",7,sector==20?35:36,sector==20?12:13,0);}
    else fail("authored unknown sector",sector,10);
    if((current->mutation&8) && sector==16)store(CTX+0x54,HEAP);
    return sector==16 || sector==9?(current->retry && attempt==0):(current->retry==0 || attempt>0);
}
void func_00295D30(const char *path)
{u32 a=canonical((u32)path);if(a!=0x4473F8 && a!=0x447410 && a!=0x447428)fail("module literal",a,0);event(7,(u32)path,0,0,0);}
void func_002C3BC8(s32 a){equal("control0",(u32)a,0);event(8,(u32)a,0,0,0);}
void func_002C8C38(s32 a,s32 b){equal("control3",(u32)a,3);equal("control7",(u32)b,7);event(9,(u32)a,(u32)b,0,0);}
void func_002C35B0(s32 a,s32 b,u32 c,u32 d)
{equal("scheduler400",(u32)a,400);equal("scheduler16",(u32)b,16);equal("scheduler captured5",c,current->arg5);equal("scheduler captured6",d,current->arg6);event(10,(u32)a,(u32)b,c,d);}
static u32 initial_word(u32 i){return 0xA14D0301u^(i*0x10203u);}
int main(void)
{
    u32 number,i,j,result,expected[LIFECYCLE_WORDS];
    for(number=0;number<sizeof(lifecycle_golden)/sizeof(lifecycle_golden[0]);++number){
        current=&lifecycle_golden[number];event_count=allocations=small_allocations=buffers=frees=sync_attempts=0;memset(read_attempts,0,sizeof(read_attempts));
        for(i=0;i<LIFECYCLE_WORDS;++i)arena[i]=initial_word(i);
        for(i=0;i<current->initial_count;++i){const struct LifecyclePair *p=&lifecycle_initial[current->initial_index+i];arena[p->index]=translate(p->value);}
        for(i=0;i<LIFECYCLE_WORDS;++i)expected[i]=canonical(arena[i]);
        D_003FD23C=current->globals_initial[0];D_003FD240=current->globals_initial[1];D_003FD244=(void *)translate(current->globals_initial[2]);D_003FD200=current->globals_initial[3];D_003FD204=(GeorgeHeap *)translate(current->globals_initial[4]);
        for(i=0;i<140;++i)((u32 *)D_00469BD0)[i]=translate(current->slots_initial[i]);
        for(i=0;i<20;++i)D_00469B80[i]=(GeorgeHeap *)translate(current->overrides_initial[i]);
        result=0;
        switch(current->routine){
        case 0:result=func_002B15C8(ptr(current->args[0]),current->args[1],ptr(current->args[2]),current->args[3]);break;
        case 1:result=func_002B1900(ptr(current->args[0]));break;
        case 2:result=canonical((u32)func_002B1D30(ptr(current->args[0]),current->args[1],current->args[2]));break;
        case 3:func_002B1E88(ptr(current->args[0]));break;
        case 4:func_002B0860(current->args[0],current->args[1]);break;
        case 5:result=canonical((u32)func_002A7E68(ptr(current->args[0]),current->args[1]?ptr(current->args[1]):0));break;
        case 6:result=canonical((u32)func_002A7F58(current->args[0]));break;
        case 7:func_002A7FC8(ptr(current->args[0]));break;
        default:fail("routine",current->routine,7);
        }
        equal("return",result,current->result);
        for(i=0;i<current->change_count;++i){const struct LifecyclePair *p=&lifecycle_changes[current->change_index+i];expected[p->index]=p->value;}
        for(i=0;i<LIFECYCLE_WORDS;++i)equal("arena",canonical(arena[i]),expected[i]);
        equal("globalcount",D_003FD23C,current->globals_expected[0]);equal("globalmode",D_003FD240,current->globals_expected[1]);equal("globalcontext",canonical((u32)D_003FD244),current->globals_expected[2]);equal("heapstackcount",D_003FD200,current->globals_expected[3]);equal("heapglobal",canonical((u32)D_003FD204),current->globals_expected[4]);
        for(i=0;i<140;++i)equal("slots",canonical(((u32 *)D_00469BD0)[i]),current->slots_expected[i]);
        for(i=0;i<20;++i)equal("override",canonical((u32)D_00469B80[i]),current->overrides_expected[i]);
        equal("eventcount",event_count,current->event_count);
        for(i=0;i<event_count;++i)for(j=0;j<11;++j)equal("event",events[i][j],lifecycle_events[current->event_index+i][j]);
    }
    printf("file_archive_lifecycle: %u checks\n",(unsigned)checks);return 0;
}
