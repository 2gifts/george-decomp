/* Authored bounded contracts. Four production manager bodies execute in a
 * separate TU. The frozen pool implementation executes through a three-lane
 * observation bridge; filesystem, queue, map and heap behavior is controlled. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_manager.h"
#include "resource_manager_golden.h"
#define func_002AD700 manager_native_pool_take
#include "../../src/game/pool_slots.c"
#undef func_002AD700

enum { BUFFER=0x20000,END=0x24000,WORDS=(END-BUFFER)/4,
    RECORD=0x22B00,QUEUE=0x22F00,PAYLOAD=0x22F80,KEY=0x13579BDF,
    COMPLETION=0x225D38,OUTPUT_ROLE=0xF0000100 };
static const u32 managers[]={0x22000,0x22200},providers[]={0x22400,0x22420,0x22440};
static const u32 objects[]={0x22500,0x22800},pools[]={0x22C00,0x22C20},maps[]={0x22E00,0x22E20};
static union { u32 words[WORDS];u8 bytes[WORDS*4]; } storage;
static u32 expected[WORDS],calls[12],events[400],event_count;
static const struct ResourceManagerGolden *fixture;
static unsigned checks,failures;
GeorgeResourceManagerPrefix *D_003F960C;

#define CHECK(condition) do { ++checks;if (!(condition)) { \
    if (failures<20) printf("line%u routine%u kind%X provider%u allocator%u mutation%u alias%u\n", \
        __LINE__,fixture?fixture->routine:0,fixture?fixture->kind:0, \
        fixture?fixture->provider:0,fixture?fixture->allocator:0, \
        fixture?fixture->mutation:0,fixture?fixture->alias:0);++failures; } } while (0)
static void *pointer(u32 address) { return storage.bytes+address-BUFFER; }
static GeorgeResourceBasePrefix *base(void) { return pointer(fixture->record); }
static void completion(GeorgeResourceRecord *record) { (void)record;CHECK(0);abort(); }
void func_00225D38(GeorgeResourceRecord *record) { completion(record); }
static u32 native_value(u32 value)
{
    if (value>=BUFFER&&value<END) return (u32)pointer(value);
    if (value==COMPLETION) return (u32)func_00225D38;
    return value;
}
static u32 canonical(u32 value)
{
    u32 start=(u32)storage.bytes;
    if (value>=start&&value<start+sizeof storage) return BUFFER+value-start;
    if (value==(u32)func_00225D38) return COMPLETION;
    return value;
}
static void write(u32 address,u32 value) { *(u32 *)pointer(address)=native_value(value); }
static void event(u32 value) { CHECK(event_count<400);if (event_count<400) events[event_count++]=value; }
static void call(u32 index,const u32 *args,u32 count)
{
    u32 i,kind=base()->kind;
    GeorgeResourceManagerState *manager=(GeorgeResourceManagerState *)D_003F960C;
    ++calls[index];event(index);
    for (i=0;i<9;++i) event(i<count?args[i]:0);
    event(canonical((u32)manager));
    for (i=0;i<7;++i) {
        static const u32 offsets[]={0,4,8,12,16,20,28};
        event(canonical(*(u32 *)((u32)base()+offsets[i])));
    }
    event(kind<8?manager->kind_count98[kind]:0);
    event(kind<8?manager->kind_peakB8[kind]:0);
}
static void call1(u32 index,u32 a) { u32 args[]={a};call(index,args,1); }

u32 func_00229FC8(void *object)
{
    CHECK(object==pointer(objects[0])||object==pointer(objects[1]));
    call1(0,canonical((u32)object));
    return *(u32 *)((u32)object+0x1B8)!=0&&*(u32 *)((u32)object+0x98)!=0;
}
static u32 resolve(u32 index,void *object,u32 command,const void *key,u32 *address,u32 *size)
{
    u32 args[5],argc=index==1?5:4,output=index==1?3:2;
    CHECK(object==pointer(objects[0])||object==pointer(objects[1]));
    CHECK(address!=0&&canonical((u32)address)==(u32)address&&size==&base()->size);
    if (index==1) CHECK(command<6);
    args[0]=canonical((u32)object);
    if (index==1) args[1]=command;
    args[output-1]=canonical((u32)key);args[output]=OUTPUT_ROLE;
    args[output+1]=canonical((u32)size);call(index,args,argc);
    if (fixture->resolution) { *address=0xA1234567;*size=base()->size; }
    if (fixture->mutation==1) {
        D_003F960C=pointer(managers[1]);write(providers[0],9);write(providers[0]+16,objects[1]);
    }
    if (fixture->mutation==2) { base()->kind=3;base()->flags=0x40;base()->key=(void *)(KEY+1U); }
    if (fixture->mutation==8) func_00224678(0,1,(GeorgeResourceRecord *)base());
    return fixture->resolution;
}
u32 func_00229FF0(void *object,u32 command,const void *key,u32 *address,u32 *size)
{ return resolve(1,object,command,key,address,size); }
u32 func_00229020(void *object,const void *key,u32 *address,u32 *size)
{ return resolve(2,object,0,key,address,size); }
void *func_002AD700(GeorgeSlotPool *pool,u32 size,u32 mode)
{
    u32 args[]={canonical((u32)pool),size,mode};
    CHECK(pool==pointer(pools[0])||pool==pointer(pools[1]));CHECK(mode==1);
    call(3,args,3);
    if (fixture->mutation==3) { D_003F960C=pointer(managers[1]);base()->size=257;base()->field08=0xB7654321; }
    return manager_native_pool_take(pool);
}
void *func_0020E598(u32 size)
{
    call1(4,size);
    if (fixture->mutation==4) { base()->size=511;D_003F960C=pointer(managers[1]); }
    return (void *)native_value(fixture->heap_result);
}
void func_0021ABD8(void *queue,u32 auxiliary,const void *key,u32 address,u32 size,
                  void *payload,GeorgeResourceCompletion callback,GeorgeResourceRecord *record,u32 special)
{
    u32 args[]={canonical((u32)queue),auxiliary,canonical((u32)key),address,size,
        canonical((u32)payload),canonical((u32)callback),canonical((u32)record),special};
    CHECK(queue==pointer(QUEUE)&&callback==func_00225D38&&record==(GeorgeResourceRecord *)base());
    CHECK(special<2);call(5,args,9);
    if (fixture->mutation==5) { D_003F960C=pointer(managers[1]);base()->kind=6;base()->key=(void *)(KEY+2U);base()->size=513; }
}
static void *lookup(u32 index,const void *key)
{
    call1(index,canonical((u32)key));
    if (fixture->mutation==5) { base()->kind=4;base()->flags=0x81; }
    return fixture->lookup?pointer(PAYLOAD):0;
}
void *func_00217F78(const void *key) { return lookup(6,key); }
void *func_00218020(const void *key) { return lookup(7,key); }
void *func_00217FB0(const void *key) { return lookup(8,key); }
void *func_00217FE8(const void *key) { return lookup(9,key); }
void func_0021A0B8(GeorgeGenericMap *map,u32 key,void *value)
{
    u32 args[]={canonical((u32)map),canonical(key),canonical((u32)value)};
    CHECK(map==pointer(maps[0])||map==pointer(maps[1]));CHECK(value==base());call(10,args,3);
    if (fixture->mutation==6) { D_003F960C=pointer(managers[1]);base()->kind=1;write(managers[1]+0x98+6*4,0xFFFFFFFF); }
}
u32 func_0021A190(GeorgeGenericMap *map,u32 key)
{
    u32 args[]={canonical((u32)map),canonical(key)};
    CHECK(map==pointer(maps[0])||map==pointer(maps[1]));call(11,args,2);
    if (fixture->mutation==7) D_003F960C=pointer(managers[1]);
    return fixture->remove_result;
}
/* Required only by the unchanged pool source's unexecuted methods. */
void *func_002AEC28(u32 size) { (void)size;CHECK(0);abort(); }
void func_002AEE40(void *value) { (void)value;CHECK(0);abort(); }
void func_002AF100(void *value) { (void)value;CHECK(0);abort(); }

int main(void)
{
    u32 c,i,j;
    if ((u32)storage.bytes>0x80000000U-sizeof storage) {
        fputs("native storage extent must remain below bit 31\n",stderr);return 2;
    }
    for (c=0;c<sizeof resource_manager_golden/sizeof resource_manager_golden[0];++c) {
        fixture=&resource_manager_golden[c];memset(storage.bytes,0,sizeof storage);
        for (i=0;i<fixture->initial_run_count;++i) {
            const u32 *run=&fixture->initial_runs[i*3];CHECK(run[0]+run[1]<=WORDS);
            for (j=0;j<run[1];++j) { expected[run[0]+j]=run[2];storage.words[run[0]+j]=native_value(run[2]); }
        }
        for (i=0;i<fixture->change_count;++i) { u32 index=fixture->changes[i*2];CHECK(index<WORDS);expected[index]=fixture->changes[i*2+1]; }
        D_003F960C=(GeorgeResourceManagerPrefix *)native_value(fixture->global_initial);
        memset(calls,0,sizeof calls);memset(events,0,sizeof events);event_count=0;
        switch (fixture->routine) {
        case 0:func_00224AB8((GeorgeResourceRecord *)base());break;
        case 1:func_00225C28((void *)KEY,fixture->kind);break;
        case 2:func_00224750(fixture->first,fixture->second,(GeorgeResourceRecord *)base());break;
        case 3:func_00224678(fixture->first,fixture->second,(GeorgeResourceRecord *)base());break;
        default:CHECK(0);abort();
        }
        for (i=0;i<WORDS;++i) CHECK(canonical(storage.words[i])==expected[i]);
        CHECK(canonical((u32)D_003F960C)==fixture->global_expected);
        for (i=0;i<12;++i) CHECK(calls[i]==fixture->calls[i]);
        CHECK(event_count==fixture->event_count);
        for (i=0;i<event_count;++i) CHECK(events[i]==fixture->events[i]);
    }
    printf("resource_manager: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
