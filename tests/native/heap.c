#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "george/heap.h"

u32 D_003FD200;
GeorgeHeap *D_003FD204,*D_003FD208,*D_003FD20C,*D_003FD210;
GeorgeHeap *D_003FD218,*D_003FD21C,*D_003FD220,*D_003FD228,*D_003FD230;
GeorgeHeap *D_00469B80[16];
GeorgeHeap D_004D84B0;
const char D_004471F8[]="anonymous";
const char D_00447208[]="overrun";
const char D_00447238[]="failure";
static unsigned checks,failures,diagnostics,overruns;
static u8 arenas[12][4096] __attribute__((aligned(256)));
static int diagnostic_mode;
static GeorgeHeap *diagnostic_next,*last_diagnostic_heap;
static u32 last_diagnostic_size,last_diagnostic_alignment;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n",__LINE__); } } while(0)
char *func_00394010(char *destination,const char *source,u32 size)
{
    return strncpy(destination,source,size);
}
s32 func_00394F68(const char *format,...)
{
    va_list args;va_start(args,format);
    last_diagnostic_heap=va_arg(args,GeorgeHeap *);
    last_diagnostic_size=va_arg(args,u32);
    if (format==D_00447238) {
        ++diagnostics;last_diagnostic_alignment=va_arg(args,u32);
        if (diagnostic_mode==1 && last_diagnostic_heap) last_diagnostic_heap->next=diagnostic_next;
    } else {
        CHECK(format==D_00447208);++overruns;
        if (diagnostic_mode==2) {
            last_diagnostic_heap->total=4096;
            last_diagnostic_heap->used=8;
            last_diagnostic_heap->minimum_free=4050;
        }
    }
    va_end(args);return 0;
}
static GeorgeHeap *heap(unsigned index,u32 size)
{
    GeorgeHeap *value=(GeorgeHeap *)arenas[index];
    memset(arenas[index],0xCD,sizeof arenas[index]);
    CHECK(func_002AE7B0(value,size,0)==value);
    return value;
}
static void reset(void)
{
    diagnostics=overruns=0;diagnostic_mode=0;D_003FD200=1;
    memset(D_00469B80,0,sizeof D_00469B80);
}
static GeorgeHeapBlock *block(void *memory)
{
    return (GeorgeHeapBlock *)((u32)memory-8);
}
static void basic_and_coalescing(void)
{
    static const unsigned orders[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
    unsigned permutation,i,j;
    for(permutation=0;permutation<6;++permutation) {
        GeorgeHeap *h=heap(0,1024);void *memory[3];u32 consumed[3],minimum;
        CHECK(h->name[23]==0 && strcmp(h->name,D_004471F8)==0);
        CHECK(h->alignment==3 && h->flags==0 && h->next==0 && h->field20==h);
        CHECK(h->initial.size==960 && h->cursor==&h->initial && h->initial.field00.next==&h->initial);
        CHECK(func_002AE730(h,0)==1024 && func_002AE730(h,1)==952 && func_002AE730(h,2)==952);
        for(i=0;i<3;++i) {
            memory[i]=func_002ADF60(h,40,3);
            CHECK(memory[i]==arenas[0]+984-i*48 && ((u32)memory[i]&7)==0);
            CHECK(block(memory[i])->field00.owner==h && block(memory[i])->size==40);
            consumed[i]=block(memory[i])->size+8;memset(memory[i],10+i,40);
        }
        minimum=h->minimum_free;CHECK(h->used==144 && minimum==816);
        for(i=0;i<3;++i) {
            unsigned which=orders[permutation][i];
            for(j=0;j<40;++j)CHECK(((u8 *)memory[which])[j]==10+which);
            func_002AE158(memory[which]);consumed[which]=0;
            CHECK(h->used==consumed[0]+consumed[1]+consumed[2]);
            CHECK(h->minimum_free==minimum);
        }
        CHECK(h->initial.size==960 && h->initial.field00.next==&h->initial && h->cursor==&h->initial);
        CHECK(func_002AE730(h,1)==952 && func_002AE730(h,2)==952 && h->used==0);
    }
    {
        GeorgeHeap *h=heap(0,1024);char long_name[40];
        memset(long_name,'X',39);long_name[39]=0;
        func_002AE7B0(h,1024,long_name);
        CHECK(strlen(h->name)==23 && h->name[0]=='X' && h->name[22]=='X');
        h->flags=0x80000004;func_002AE6E8(h);CHECK(h->flags==0x80000005 && func_002AE710(h)==1);
        func_002AE6F8(h);CHECK(h->flags==0x80000004 && func_002AE710(h)==0);
        CHECK(func_002AE6C0()==&D_004D84B0);
        CHECK(func_002AE730(h,-1)==1024 && func_002AE730(h,3)==1024 && func_002AE730(h,0x7FFFFFFF)==1024);
    }
}
static void best_fit_and_cursor(void)
{
    GeorgeHeap *h=heap(0,2048);
    GeorgeHeapBlock *first=(GeorgeHeapBlock *)(arenas[0]+256),*second=(GeorgeHeapBlock *)(arenas[0]+512),*third=(GeorgeHeapBlock *)(arenas[0]+768);
    void *memory;
    h->initial.size=128;h->initial.field00.next=first;
    first->size=128;first->field00.next=second;
    second->size=64;second->field00.next=third;
    third->size=64;third->field00.next=&h->initial;
    CHECK(func_002AE730(h,1)==120 && func_002AE730(h,2)==56);
    memory=func_002ADF60(h,40,3);
    CHECK(memory==(u8 *)second+32 && second->size==16);
    CHECK(first->size==128 && third->size==64 && h->cursor==third);
    CHECK(func_002AE730(h,2)==8);
    memory=func_002ADF60(h,40,3);
    CHECK(memory==(u8 *)third+32 && third->size==16 && h->cursor==second);
    CHECK(h->used==96);
    /* Equal sizes retain the first fitting block in scan order. */
    h=heap(0,2048);h->initial.size=64;h->initial.field00.next=first;
    first->size=64;first->field00.next=second;
    second->size=64;second->field00.next=&h->initial;
    memory=func_002ADF60(h,40,3);
    CHECK(memory==(u8 *)first+32 && h->cursor==second && h->initial.size==64);
    /* A retained zero-size free header participates in the smallest query. */
    first->size=0;CHECK(func_002AE730(h,2)==0xFFFFFFF8U);
}
static void alignment_and_redirects(void)
{
    unsigned exponent,index;void *memory;
    for(exponent=0;exponent<=8;++exponent) {
        GeorgeHeap *h=heap(0,2048);u32 used;
        memory=func_002ADF60(h,37,exponent);
        CHECK(memory!=0 && ((u32)memory&((1U<<exponent)-1))==0);
        CHECK(block(memory)->size>=37 && block(memory)->field00.owner==h);
        used=block(memory)->size+8;CHECK(h->used==used && h->minimum_free==2048-64-used);
        memset(memory,88,37);func_002AE158(memory);
        CHECK(h->used==0 && h->initial.size==1984);
    }
    {
        static const struct {u32 size,alignment,arena,rounded;} cases[]={
            {0,2,1,4},{4,3,1,4},{5,3,2,8},{8,4,2,8},
            {9,3,3,16},{16,4,4,16},{16,5,5,16},
            {17,2,6,24},{24,3,6,24},{17,4,7,32},
            {25,3,7,32},{32,5,7,32},{33,3,0,33},{9,35,5,16}};
        for(index=0;index<sizeof(cases)/sizeof(cases[0]);++index) {
            GeorgeHeap *all[8];unsigned j;
            for(j=0;j<8;++j)all[j]=heap(j,2048);
            D_003FD20C=all[1];D_003FD210=all[2];D_003FD218=all[3];D_003FD21C=all[4];
            D_003FD220=all[5];D_003FD228=all[6];D_003FD230=all[7];D_003FD200=0;
            memory=func_002ADF60(all[0],cases[index].size,cases[index].alignment);
            CHECK(memory!=0 && block(memory)->field00.owner==all[cases[index].arena]);
            CHECK(block(memory)->size>=cases[index].rounded);
            for(j=0;j<8;++j)CHECK((all[j]->used!=0)==(j==cases[index].arena));
            func_002AE158(memory);
        }
    }
    {
        GeorgeHeap *h=heap(0,2048);D_003FD200=1;
        memory=func_002ADF60(h,9,35);CHECK(block(memory)->field00.owner==h && ((u32)memory&7)==0);
        func_002AE158(memory);
        CHECK(func_002ADF60(h,0x80000000U,3)==0 && diagnostics==1);
        CHECK(last_diagnostic_heap==h && last_diagnostic_size==0x80000000U && h->used==0);
    }
}
static void wrappers_override_and_callbacks(void)
{
    static void *(*word_allocators[])(u32)={func_002AEC28,func_002AED98,func_002AEE60,func_002AEF08,func_002AEFB0,func_002AF058,func_002AF140};
    static void (*free_wrappers[])(void *)={func_002AE990,func_002AEE40,func_002AF100,func_002AF120,func_002AF1E8};
    unsigned index;void *memory;GeorgeHeap *a,*b,*c;
    for(index=0;index<7;++index){
        a=heap(0,2048);b=heap(1,2048);D_003FD200=1;D_003FD204=a;D_003FD208=b;
        memory=word_allocators[index](40);
        CHECK(memory!=0 && block(memory)->field00.owner==(index==1?b:a));
        CHECK(((u32)memory&(index>=2 && index<6?15:7))==0);
        free_wrappers[index%5](memory);CHECK(a->used==0 && b->used==0);
    }
    a=heap(0,2048);b=heap(1,2048);D_003FD204=a;D_003FD208=b;
    memory=func_002AEB60(40,6);CHECK(((u32)memory&63)==0 && block(memory)->field00.owner==a);func_002AE158(memory);
    memory=func_002AECD0(40,5);CHECK(((u32)memory&31)==0 && block(memory)->field00.owner==b);func_002AE158(memory);
    a->alignment=5;memory=func_002AEA30(a,40);CHECK(((u32)memory&31)==0);func_002AE158(memory);
    func_002AF100(0);func_002AF120(0);CHECK(a->used==0 && b->used==0);
    D_003FD200=0;D_003FD204=a;D_00469B80[0]=a;
    func_002AEAF0(b);CHECK(D_003FD200==1 && D_003FD204==b && D_00469B80[1]==b);
    func_002AEAF0(a);CHECK(D_003FD200==2 && D_003FD204==a && D_00469B80[2]==a);
    func_002AEAF0(0);CHECK(D_003FD200==1 && D_003FD204==b);
    func_002AEAF0(0);CHECK(D_003FD200==0 && D_003FD204==a);
    func_002AEAF0(0);CHECK(D_003FD200==0 && D_003FD204==a);
    D_003FD200=1;a=heap(0,128);b=heap(1,2048);a->next=b;D_003FD204=a;
    memory=func_002AEC28(104);CHECK(memory!=0 && block(memory)->field00.owner==b && a->used==0);func_002AE158(memory);
    /* Allocation failure callback redirects the chain before the wrapper reload. */
    a=heap(0,2048);b=heap(1,2048);c=heap(2,2048);a->initial.size=0;a->next=b;
    D_003FD204=a;diagnostic_mode=1;diagnostic_next=c;
    memory=func_002AEC28(40);CHECK(memory!=0 && block(memory)->field00.owner==c && b->used==0);func_002AE158(memory);
    diagnostic_mode=0;D_003FD204=0;diagnostics=0;
    CHECK(func_002AEB60(40,5)==0 && diagnostics==1 && last_diagnostic_heap==0 && last_diagnostic_alignment==5);
    /* Unsigned threshold subtraction intentionally wraps; callback statistics reload. */
    a=heap(0,2048);a->total=1;D_003FD204=a;diagnostic_mode=2;overruns=0;
    memory=func_002AEC28(40);CHECK(memory!=0 && overruns==1 && a->used==8 && a->minimum_free==4024);
    /* The updated total/used produce remaining=4024, below the callback's 4050. */
    CHECK(a->total==4096 && a->cursor==&a->initial);
    diagnostic_mode=0;
}
int main(void)
{
    reset();basic_and_coalescing();best_fit_and_cursor();alignment_and_redirects();wrappers_override_and_callbacks();
    printf("heap semantic checks: %u, failures: %u\n",checks,failures);
    return failures!=0;
}
