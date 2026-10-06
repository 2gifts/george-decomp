/* Synthetic callback/ownership tests. Lookup and pool helpers execute existing
 * project C; manager/heap/virtual operations retain explicit observation hooks. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_base.h"
#include "george/deimos_tables.h"
#include "george/algorithm_templates.h"
#include "resource_base_golden.h"

/* Preserve the frozen one-parameter pool source and header. This bridge records
 * the three original allocation lanes before invoking its real implementation. */
#define func_002AD700 resource_native_pool_take
#define func_002AD748 resource_native_pool_release
#include "../../src/game/pool_slots.c"
#undef func_002AD700
#undef func_002AD748
GEORGE_DEFINE_MAP_LOOKUP(resource_native_map_lookup)

enum { BUFFER=0x20000,END=0x22800,WORDS=(END-BUFFER)/4,
    RECORD=0x22400,OTHER=0x22440,TABLE=0x22480,MAP=0x22540,
    BASE_TABLE=0x43B1C8,READY=0xF00000A0,DISPATCH=0xF00000B0,
    CALLBACK_A=0xF00000C0,CALLBACK_B=0xF00000D0 };
static const u32 allocators[]={0x20000,0x21000},managers[]={0x22000,0x22200};
static const u32 pools[]={0x22500,0x22520},nodes[]={0x22600,0x22610,0x22620,0x22630,0x22640,0x22650,0x22660,0x22670};
static const u32 arguments[]={0x22700,0x22710},keys[]={0x22740,0x22750},payloads[]={0x22780,0x227A0};
static union { u32 words[WORDS];u8 bytes[WORDS*4]; } memory;
static u32 expected[WORDS],calls[12],events[512],event_count;
static const struct ResourceBaseGolden *fixture;
static unsigned checks,failures;
GeorgeResourceManagerPrefix *D_003F960C;
GeorgeSlotPool *D_003F9610;
/* Authored identity-only object. Original table contents are not copied. */
const u8 D_0043B1C8[48]={0};

#define CHECK(condition) do { ++checks;if (!(condition)) { \
    if (failures<16) printf("line%u routine%u flags%X graph%u mutation%u alias%u\n", \
        __LINE__,fixture?fixture->routine:0,fixture?fixture->flags:0, \
        fixture?fixture->graph:0,fixture?fixture->mutation:0,fixture?fixture->alias:0); \
    ++failures; } } while (0)
static void ready(GeorgeResourceRecord *);
static void dispatch(void *,s32);
static void callback_a(u32,void *);
static void callback_b(u32,void *);
static void *pointer(u32 value) { return memory.bytes+value-BUFFER; }
static GeorgeResourceBasePrefix *base(void) { return pointer(RECORD); }
static u32 native_value(u32 value)
{
    if (value>=BUFFER&&value<END) return (u32)pointer(value);
    if ((value&0xF0000000)==0xF0000000&&(value&0x0FFFFFFF)>=BUFFER&&(value&0x0FFFFFFF)<END)
        return (u32)pointer(value&0x0FFFFFFF)|0xF0000000;
    if (value==BASE_TABLE) return (u32)D_0043B1C8;
    if (value==READY) return (u32)ready;
    if (value==DISPATCH) return (u32)dispatch;
    if (value==CALLBACK_A) return (u32)callback_a;
    if (value==CALLBACK_B) return (u32)callback_b;
    return value;
}
static u32 canonical(u32 value)
{
    u32 start=(u32)memory.bytes;
    if (value>=start&&value<start+sizeof memory) return BUFFER+value-start;
    if ((value&0xF0000000)==0xF0000000&&(value&0x0FFFFFFF)>=start&&(value&0x0FFFFFFF)<start+sizeof memory)
        return 0xF0000000|(BUFFER+(value&0x0FFFFFFF)-start);
    if (value==(u32)D_0043B1C8) return BASE_TABLE;
    if (value==(u32)ready) return READY;
    if (value==(u32)dispatch) return DISPATCH;
    if (value==(u32)callback_a) return CALLBACK_A;
    if (value==(u32)callback_b) return CALLBACK_B;
    return value;
}
static u32 canonical_word(u32 index,u32 value)
{
    u32 result=canonical(value),i;
    if (result!=value) return result;
    /* Freeing a live callback node overwrites only its low16 link. Normalize
     * its retained native function-address high16 at these exact node fields. */
    for (i=0;i<8;++i) if (index==(nodes[i]-BUFFER)/4) {
        if ((value&0xFFFF0000)==((u32)callback_a&0xFFFF0000)||
            (value&0xFFFF0000)==((u32)callback_b&0xFFFF0000))
            return 0xF0000000|(value&0xFFFF);
    }
    return result;
}
static void event(u32 value) { CHECK(event_count<512);if (event_count<512) events[event_count++]=value; }
static void call(u32 index,u32 a,u32 b,u32 c)
{
    ++calls[index];event(index);event(canonical(a));event(canonical(b));event(canonical(c));
    event(base()->count);event(base()->flags);event(canonical((u32)base()->payload));
    event(canonical((u32)base()->callbacks));event(canonical((u32)D_003F9610));
    event(canonical((u32)D_003F960C));event(canonical((u32)base()->key));
}
void *func_00219FF0(GeorgeGenericMap *map,u32 key)
{
    CHECK(map==pointer(MAP)&&key==0x13579BDF);call(0,(u32)map,key,0);
    return resource_native_map_lookup(map,key);
}
void *func_002AD700(GeorgeSlotPool *pool,u32 size,u32 mode)
{
    CHECK(pool==pointer(pools[0])||pool==pointer(pools[1]));CHECK(size==16&&mode==0);
    call(4,(u32)pool,size,mode);
    if (fixture->mutation==1) base()->callbacks=pointer(nodes[3]);
    return resource_native_pool_take(pool);
}
void func_002AD748(GeorgeSlotPool *pool,void *node)
{
    CHECK(pool==pointer(pools[0])||pool==pointer(pools[1]));
    CHECK(canonical((u32)node)>=BUFFER&&canonical((u32)node)<END);
    call(5,(u32)pool,(u32)node,0);resource_native_pool_release(pool,node);
    if (fixture->mutation==2&&calls[5]==1) {
        D_003F9610=pointer(pools[1]);base()->callbacks=pointer(nodes[3]);
    }
}
void func_00224AB8(GeorgeResourceRecord *record)
{
    CHECK(record==pointer(RECORD));call(1,(u32)record,0,0);
    if (fixture->mutation==11||fixture->mutation==12) {
        base()->payload=fixture->mutation==11?pointer(payloads[0]):NULL;
        base()->flags=(u8)fixture->flags;base()->size=0x123;
    }
}
void func_00225C28(const void *key,u32 kind)
{
    CHECK(kind<=255);call(2,(u32)key,kind,0);
    if (fixture->mutation==7) { base()->payload=NULL;base()->flags=0x10; }
    if (fixture->mutation==8) {
        base()->payload=(void *)native_value(payloads[0]|0xF0000000);
        base()->flags=0;base()->size=128;D_003F960C=pointer(managers[1]);
    }
}
void func_00224750(u32 first,u32 second,GeorgeResourceRecord *record)
{
    CHECK(first==0&&second==1&&record==pointer(RECORD));call(3,first,second,(u32)record);
    if (fixture->mutation==9) { base()->payload=NULL;D_003F960C=pointer(managers[1]); }
    if (fixture->mutation==10) {
        D_003F960C=pointer(managers[1]);base()->size=0xFFFFFFFF;
        base()->payload=(void *)native_value(payloads[0]|0xF0000000);
    }
}
void func_0020E5C0(void *payload)
{ CHECK(payload==pointer(payloads[0]));call(6,(u32)payload,0,0); }
void func_002AF100(void *record)
{ CHECK(record==pointer(RECORD));call(7,(u32)record,0,0); }
/* Complete reused pool TU references these unrelated allocation paths. They
 * must never execute in a resource-base fixture. */
void *func_002AEC28(u32 size) { CHECK(0);(void)size;return NULL; }
void func_002AEE40(void *memory_) { CHECK(0);(void)memory_; }
static void ready(GeorgeResourceRecord *record)
{
    CHECK(canonical((u32)record)==RECORD+fixture->adjustment);call(8,(u32)record,0,0);
}
static void dispatch(void *record,s32 mode)
{
    CHECK(canonical((u32)record)==RECORD+fixture->adjustment&&mode==0);call(9,(u32)record,(u32)mode,0);
}
static void callback(u32 index,u32 resource,void *argument)
{
    CHECK(resource==(u32)pointer(RECORD));CHECK(argument==pointer(arguments[0])||argument==pointer(arguments[1]));
    call(index,resource,(u32)argument,0);
    if (argument==pointer(arguments[0])) {
        if (fixture->mutation==3)
            func_00226E38(pointer(RECORD),callback_b,pointer(arguments[1]),pointer(keys[1]));
        if (fixture->mutation==4)
            func_00226ED8(pointer(RECORD),pointer(keys[1]));
        if (fixture->mutation==5) {
            D_003F9610=pointer(pools[1]);
            if (base()->callbacks) base()->callbacks->callback=callback_b;
        }
        if (fixture->mutation==6) base()->callbacks=NULL;
    }
}
static void callback_a(u32 resource,void *argument) { callback(10,resource,argument); }
static void callback_b(u32 resource,void *argument) { callback(11,resource,argument); }

int main(void)
{
    u32 n,i,j,result;GeorgeResourceRecord *record=pointer(RECORD);
    CHECK(sizeof(void*)==4&&sizeof(GeorgeResourceBasePrefix)==36&&sizeof(GeorgeResourceCallbackNode)==16);
    /* The original release mask preserves only low28 pointer bits; these actual
     * native pointer fixtures require a mapped buffer wholly below bit28. */
    if ((u32)memory.bytes>=0x10000000||(u32)memory.bytes+sizeof memory>=0x10000000) {
        fprintf(stderr,"resource_base native storage must fit the original low28 pointer mask\n");return 2;
    }
    for (n=0;n<sizeof resource_base_golden/sizeof resource_base_golden[0];++n) {
        fixture=&resource_base_golden[n];
        for (i=0;i<fixture->initial_run_count;++i) {
            u32 start=fixture->initial_runs[i*3],count=fixture->initial_runs[i*3+1],value=fixture->initial_runs[i*3+2];
            CHECK(start+count<=WORDS);
            for (j=0;j<count;++j) { expected[start+j]=value;memory.words[start+j]=native_value(value); }
        }
        for (i=0;i<fixture->change_count;++i) {
            CHECK(fixture->changes[i*2]<WORDS);expected[fixture->changes[i*2]]=fixture->changes[i*2+1];
        }
        D_003F960C=(void *)native_value(fixture->globals_initial[0]);
        D_003F9610=(void *)native_value(fixture->globals_initial[1]);
        memset(calls,0,sizeof calls);event_count=0;result=0;
        switch (fixture->routine) {
        case 0:result=canonical((u32)func_00225E80((void *)0x13579BDF,fixture->kind));break;
        case 1:func_002267D0(record,fixture->mode);break;
        case 2:result=canonical((u32)func_00226BC0(record,pointer(keys[0]),fixture->kind,fixture->mode,0x1234ABCD));break;
        case 3:func_00226D78(record);break;
        case 4:result=func_00226E38(record,callback_a,pointer(arguments[0]),pointer(keys[0]));break;
        case 5:result=func_00226ED8(record,pointer(fixture->alias==1?keys[1]:fixture->alias==2?keys[0]:arguments[0]));break;
        case 6:func_00226F40(record);break;
        default:func_00226FB0(record);break;
        }
        for (i=0;i<WORDS;++i) CHECK(canonical_word(i,memory.words[i])==expected[i]);
        CHECK(canonical((u32)D_003F960C)==fixture->globals_expected[0]);
        CHECK(canonical((u32)D_003F9610)==fixture->globals_expected[1]);CHECK(result==fixture->result);
        for (i=0;i<12;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count&&i<fixture->event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("resource_base: %u checks, %u failures\n",checks,failures);return failures?1:0;
}
