/* Actual selected C plus authentic constructed C++/licensed helper closure.
 * This is a measured GNU native prefix/alias observation, not a class or
 * hardware identity claim. Typed pointer cells alone translate addresses. */
#include "george/timer_integer.h"
#include "george/heap.h"
#include "timer_integer_golden.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

enum { BASE=0x20000, WORDS=6144, TIMER=0x20080, RATE=0x20100, ALT=0x20140,
 RECORD=0x20180, SLOTS=0x20300, POOL=0x21000, HEAP=0x24000, REENT=0x24800,
 ATNEW=0x25000, RATEGLOBAL=0x3FD4D8, RANGE=0x46A0F0, HEAPGLOBAL=0x3FD204,
 REENTGLOBAL=0x405694, HANDLER=0x3F21B0, HEADS=0x3F21B8, START=0x3F21F8,
 FINISH=0x3F21FC, HEAPSIZE=0x3F2200 };
GeorgeTimerRate *D_003FD4D8;
extern GeorgeActorPointerRange D_0046A0F0;
GeorgeHeap *D_003FD204;
struct _reent;
struct _reent *D_00405694;
extern const u8 D_00447AA0[];
extern const char D_00447238[];
extern void func_002BD340(void);
extern u8 timer_heads[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE");
extern u8 timer_begin[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE");
extern u8 timer_end[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE");
extern u8 timer_size[] __asm__("__ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE");
extern u8 timer_handler[] __asm__("__ZN23__malloc_alloc_templateILi0EE26__malloc_alloc_oom_handlerE");
extern void timer_native_bind(void **,void **,void **);
extern void timer_native_construct_slots(void **,u32);
extern void timer_native_layout(u32 *);
extern const u32 timer_native_atexit_abi[7];
extern void *timer_real_memmove(void *,const void *,u32);

static u32 *arena;
static const struct TimerCase *current;
static u32 events[256],event_count,allocations,counts,checks,case_index,exit_allocating;
static u32 initial_word(u32 i) { return 0x8C370501U ^ (i*0x10203U); }
static void check(int value) {
    ++checks;
    if (!value) { fprintf(stderr,"timer_integer fixture%u check%u failed\n",case_index,checks);fflush(stderr);_exit(2); }
}
static void same(u32 actual,u32 expected,const char *label,u32 index) {
    ++checks;
    if (actual!=expected) {
        fprintf(stderr,"timer_integer fixture%u %s%u actual%08x expected%08x check%u\n",case_index,label,index,actual,expected,checks);
        fflush(stderr);_exit(2);
    }
}
void timer_unexecuted(void) { fprintf(stderr,"timer_integer unexecuted fixture%u\n",case_index);fflush(stderr);_exit(3); }
static u32 native_word(u32 value) {
    if (BASE<=value && value<BASE+WORDS*4U) return (u32)arena+value-BASE;
    if (value==0x447AA0) return (u32)D_00447AA0;
    if (value==0x2BD340) return (u32)func_002BD340;
    if (RANGE<=value && value<RANGE+12U) return (u32)&D_0046A0F0+value-RANGE;
    return value;
}
static u32 guest_word(u32 value) {
    u32 start=(u32)arena;
    if (start<=value && value<start+WORDS*4U) return BASE+value-start;
    if (value==(u32)D_00447AA0) return 0x447AA0;
    if (value==(u32)D_00447238) return 0x447238;
    if (value==(u32)func_002BD340) return 0x2BD340;
    if ((u32)&D_0046A0F0<=value && value<(u32)&D_0046A0F0+12U) return RANGE+value-(u32)&D_0046A0F0;
    return value;
}
static void event(u32 k,u32 a,u32 b,u32 c) {
    check(event_count+4<=256);events[event_count++]=k;events[event_count++]=a;
    events[event_count++]=b;events[event_count++]=c;
}
static u32 *cell(u32 guest) { check(BASE<=guest && guest<BASE+WORDS*4U && !(guest&3));return arena+(guest-BASE)/4; }
void func_002BD340(void) { timer_unexecuted(); }
void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment) {
    check(heap==(GeorgeHeap *)cell(HEAP));check(size>0 && size<=6000);
    if (alignment==4) {
        ++allocations;check(size==12 && allocations<=2);
        event(1,HEAP,size,allocations);
        if (allocations==2 && current->p[8]&1) D_003FD4D8=(GeorgeTimerRate *)cell(ALT);
        return cell(allocations==1?RATE:RECORD);
    }
    check(alignment==3);
    if (exit_allocating) {
        check(size==0x88);event(5,HEAP,size,current->p[12]);
        return current->p[12]?0:cell(ATNEW);
    }
    event(2,HEAP,size,0);return cell(POOL);
}
void func_002AE158(void *memory) {
    check(memory==cell(SLOTS));event(6,SLOTS,guest_word((u32)D_0046A0F0.field04),guest_word((u32)D_0046A0F0.field08));
}
s32 func_00394F68(const char *format,...) {
    va_list ap;void *heap;u32 size,alignment;
    va_start(ap,format);heap=va_arg(ap,void *);size=va_arg(ap,u32);alignment=va_arg(ap,u32);va_end(ap);
    check(format==D_00447238 && heap==0 && size==0x88 && alignment==3);
    event(7,0x447238,0,size);return 0;
}
void *george_sgi_engine_allocate(u32 size) { return func_002AF140(size); }
void george_sgi_engine_release(void *p) { func_002AF1E8(p); }
void *memmove(void *destination,const void *source,u32 bytes) {
    event(4,guest_word((u32)destination),guest_word((u32)source),bytes);
    return timer_real_memmove(destination,source,bytes);
}
void *timer_exit_malloc(u32 bytes) {
    void *p;exit_allocating=1;p=func_002AF140(bytes);exit_allocating=0;return p;
}
u32 george_tree_test_counter(void) {
    u32 timer=current->p[9]?RATE+8:TIMER;
    ++counts;check(counts==1);event(3,current->p[4],current->p[8],current->p[9]);
    if (current->p[8]&2) *cell(timer)=current->p[5]+17U;
    if (current->p[8]&4) D_003FD4D8=(GeorgeTimerRate *)cell(ALT);
    if (current->p[8]&8) {
        u32 captured=current->p[1] && (current->p[8]&1)?ALT:RATE;
        *cell(captured+8)=7;
    }
    return current->p[4];
}
static u32 *global_cell(u32 index) {
    if (index==0) return (u32 *)&D_003FD4D8;
    if (index<4) return (u32 *)&D_0046A0F0+index-1;
    if (index==4) return (u32 *)&D_003FD204;
    if (index==5) return (u32 *)&D_00405694;
    if (index==6) return (u32 *)timer_handler;
    if (index<23) return (u32 *)timer_heads+index-7;
    if (index==23) return (u32 *)timer_begin;
    if (index==24) return (u32 *)timer_end;
    check(index==25);return (u32 *)timer_size;
}
static int pointer_cell(u32 index) {
    u32 j;
    for (j=0;j<current->pointer_count;++j) if (current->pointers[j]==index) return 1;
    return 0;
}
int main(void) {
    u32 i,j,layout[10];
    timer_native_layout(layout);
    check(layout[0]==4 && layout[1]==12 && layout[2]==4 && layout[3]==4 && layout[4]==64 && layout[5]==4 && layout[6]==4);
    check(layout[7]==0 && layout[8]==4 && layout[9]==8);
    printf("timer_integer native_layout:");for(i=0;i<10;++i)printf(" %u",layout[i]);printf("\n");
    { const u32 expected[7]={136,0,4,8,0x148,0x14C,32};
      printf("timer_integer native_atexit_layout:");
      for(i=0;i<7;++i){same(timer_native_atexit_abi[i],expected[i],"atexit_layout",i);printf(" %u",timer_native_atexit_abi[i]);}printf("\n"); }
    arena=(u32 *)malloc(WORDS*4U);check(arena!=0);
    timer_native_construct_slots((void **)arena,WORDS);
    for (case_index=0;case_index<sizeof(timer_cases)/sizeof(timer_cases[0]);++case_index) {
        u32 result=0;
        current=timer_cases+case_index;event_count=allocations=counts=exit_allocating=0;
        timer_native_bind(0,0,0);
        for (i=0;i<WORDS;++i) arena[i]=initial_word(i);
        for (i=0;i<current->initial_count;++i) arena[current->initial[i].index]=current->initial[i].value;
        for (i=0;i<current->pointer_count;++i) {
            u32 at=current->pointers[i];check(at<WORDS);arena[at]=native_word(arena[at]);
        }
        check(current->global_count==26);
        for (i=0;i<current->global_count;++i) *global_cell(i)=i==25?current->globals_initial[i]:native_word(current->globals_initial[i]);
        if (current->p[0]==0) result=func_002BD4F8();
        else if (current->p[0]==1) result=func_002BDC18(current->p[6],current->p[5]);
        else result=func_002BDD50();
        same(result,current->result,"result",0);check(counts==(current->p[0]==0));same(event_count,current->event_count,"events_count",0);
        for (i=0;i<event_count;++i) same(events[i],current->events[i],"event",i);
        for (i=0;i<WORDS;++i) {
            u32 expected=initial_word(i),actual=arena[i];
            for (j=0;j<current->initial_count;++j) if (current->initial[j].index==i) expected=current->initial[j].value;
            for (j=0;j<current->change_count;++j) if (current->changes[j].index==i) expected=current->changes[j].value;
            if (pointer_cell(i)) actual=guest_word(actual);
            same(actual,expected,"word",i);
        }
        for (i=0;i<current->global_count;++i) {
            u32 actual=*global_cell(i);if(i!=25)actual=guest_word(actual);
            same(actual,current->globals_expected[i],"global",i);
        }
        /* Independent numeric invariants after real nested callbacks. */
        if (current->p[0]==0) same(result,current->p[4],"independent_count_low32",0);
        if (current->p[0]==1) same(result,current->p[5]/D_003FD4D8->field08,"independent_unsigned_quotient",0);
        if (current->p[0]==2) same(result,D_003FD4D8->field04,"independent_fresh_constant",0);
        if (!current->p[1]) check(allocations==0);
        if (current->p[1]) {
            check(allocations==2);check(*cell(RATE+4)==0x1193FF10U);
            check(*cell(RECORD)==0xFFFFFFFFU);
            check(guest_word(*cell(RECORD+4))==0x447AA0);
            check(*cell(RATE)==initial_word((RATE-BASE)/4));
            if (current->p[8]&16) {
                same(guest_word((u32)D_0046A0F0.field04),RECORD+4,"fresh_end_after_aliased_append",0);
                same(guest_word((u32)D_0046A0F0.field08),RANGE+8,"untouched_end_capacity",0);
            }
        }
    }
    timer_native_bind(0,0,0);free(arena);
    printf("timer_integer: %u checks\n",checks);return 0;
}
