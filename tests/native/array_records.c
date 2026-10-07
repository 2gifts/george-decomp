/* Selected array C, real published heap-wrapper spans/count accessor/qsort,
 * and a pinned generic memcpy support TU. Core heap/free remain authored
 * mutation/return observers; no allocator or OS behavior is claimed. */
#include "george/array_records.h"
#include "george/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "array_records_golden.h"

#define BASE 0x20000u
#define ARRAY 0x20100u
#define HEADER 0x20180u
#define OLD 0x20400u
#define ALT 0x20500u
#define NEW 0x20600u
#define ELEMENT 0x20900u
#define HEAP 0x20A00u
static u8 arena[ARRAY_RECORD_WORDS*4u];
static u32 expected[ARRAY_RECORD_WORDS],events[48],event_count,checks,copy_calls;
static const struct ArrayGolden *current;
GeorgeHeap *D_003FD204;
const char D_00447238[]="authored allocation diagnostic observer";
extern void *array_support_memcpy(void *,const void *,size_t);
extern void array_real_qsort(void *,size_t,size_t,int (*)(const void *,const void *));
extern u32 func_002AAF88(const void *);

static void equal(u32 actual,u32 want,const char *what)
{
    ++checks;
    if(actual!=want){printf("array fixture %u check %u %s: %08X != %08X\n",
        (u32)(current-array_golden),checks,what,actual,want);exit(1);}
}

static void *host(u32 p)
{
    if(!p)return 0;
    if(p<BASE || p-BASE>=sizeof(arena)){printf("unowned native array pointer\n");exit(1);}
    return arena+p-BASE;
}

/* Indexed results may be wrapping numeric addresses which are never
 * dereferenced. Address rebasing is low32 arithmetic, not allocation proof. */
static u32 guest(const void *p)
{
    return p ? BASE+((u32)p-(u32)arena) : 0;
}

static void event(u32 kind,u32 a,u32 b,u32 c)
{
    if(event_count>44){printf("array observer event bound\n");exit(1);}
    events[event_count++]=kind;events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
}

static void mutation(u32 phase)
{
    GeorgeArrayRecords *a=host(ARRAY);
    u32 mode=current->mutation,routine=current->routine;
    if(phase==1 && (mode&1) && (routine==5 || routine==6)){
        a->element_size=8;a->used=2;a->data=host(ALT);a->capacity=99;
    }
    if(phase==2 && (mode&2) && (routine==5 || routine==6) && copy_calls==1){
        a->data=host(ALT);a->used=1;a->element_size=4;
    }
    if(phase==3 && (mode&4) && routine>=3 && routine<=6){a->used=2;a->element_size=4;}
    if(phase==2 && (mode&8) && routine==6 && copy_calls==2)a->used=5;
}

void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment)
{
    equal(guest(heap),HEAP,"published allocator core argument");
    event(1,guest(heap),size,alignment);mutation(1);
    return current->success ? host(current->routine==1 ? HEADER : NEW) : 0;
}

void func_002AE158(void *memory)
{
    GeorgeArrayRecords *a=host(ARRAY);
    event(2,guest(memory),a->used,a->capacity);mutation(3);
}

s32 func_00394F68(const char *format,...)
{
    va_list args;void *heap;u32 size,alignment;
    equal(format==D_00447238,1,"numeric diagnostic observer format binding");
    va_start(args,format);heap=va_arg(args,void *);size=va_arg(args,u32);alignment=va_arg(args,u32);va_end(args);
    event(4,guest(heap),size,alignment);return 0;
}

void *func_003934F8(void *destination,const void *source,u32 size)
{
    u32 d=guest(destination),s=guest(source);
    equal(size<=64,1,"bounded supporting copy");
    equal(!size || d+size<=s || s+size<=d,1,"supporting copy nonoverlap");
    ++copy_calls;event(3,d,s,size);
    array_support_memcpy(destination,source,size);
    mutation(2);return destination;
}

static s32 compare(const void *a,const void *b)
{
    s32 x=*(const s32 *)a,y=*(const s32 *)b;
    if(current->mutation&16)((GeorgeArrayRecords *)host(ARRAY))->used=31;
    return (x>y)-(x<y);
}

void func_00396788(void *data,u32 count,u32 size,GeorgeArrayCompare callback)
{
    equal(callback==compare,1,"actual typed comparator pointer");
    equal(count<=12 && size==4,1,"native sort compatibility domain");
    event(5,guest(data),count,size);
    array_real_qsort(data,count,size,callback);
}

int main(void)
{
    u32 k,i,j;
    for(k=0;k<sizeof(array_golden)/sizeof(array_golden[0]);++k){
        const struct ArrayGolden *c=&array_golden[k];GeorgeArrayRecords *a=host(ARRAY);u32 result=0;
        current=c;event_count=copy_calls=0;
        equal(sizeof(void *),4,"native32 pointer");equal(sizeof(GeorgeArrayRecords),16,"whole header");
        for(i=0;i<ARRAY_RECORD_WORDS;++i)expected[i]=0x5A3C0001u^(i*0x10203u);
        for(i=0;i<c->initial_count;++i)expected[c->initial[i].index]=c->initial[i].value;
        for(i=0;i<ARRAY_RECORD_WORDS;++i)((u32 *)arena)[i]=expected[i];
        for(i=0;i<sizeof(array_pointer_cells)/sizeof(array_pointer_cells[0]);++i){
            u32 index=array_pointer_cells[i];((u32 *)arena)[index]=(u32)host(expected[index]);
        }
        D_003FD204=host(HEAP);
        for(i=0;i<c->change_count;++i)expected[c->changes[i].index]=c->changes[i].value;
        switch(c->routine){
        case 0:result=guest(func_002AAF50(a,(s32)c->index));break;
        case 1:result=guest(func_002AAF98(c->stride));break;
        case 2:func_002AAFD8(a);break;
        case 3:func_002AAFE0(a);break;
        case 4:func_002AB020(a);break;
        case 5:result=(u32)func_002AB068(a,(s32)c->index);break;
        case 6:result=(u32)func_002AB110(a,host(ELEMENT));break;
        case 7:result=func_002AB330(a,(s32)c->index);break;
        case 8:func_002AB3C8(a,compare);break;
        default:printf("unsupported selected array routine\n");return 1;
        }
        equal(result,c->result,"actual low32 result");equal(event_count,c->event_count,"event count");
        for(i=0;i<event_count;++i)equal(events[i],c->events[i],"call order/lanes/captured header");
        equal(func_002AAF88(a),expected[(ARRAY+8-BASE)/4],"genuine published count accessor");
        for(i=0;i<ARRAY_RECORD_WORDS;++i){
            u32 value=((u32 *)arena)[i];
            for(j=0;j<sizeof(array_pointer_cells)/sizeof(array_pointer_cells[0]);++j)
                if(i==array_pointer_cells[j]){value=guest((void *)value);break;}
            equal(value,expected[i],"whole initialized arena");
        }
    }
    printf("%u array record checks passed\n",checks);return 0;
}
