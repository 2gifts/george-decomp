#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_registry.h"
#include "george/algorithm_templates.h"
#include "resource_registry_golden.h"

/* The existing accessor is linked from src/game/accessors.c. All new calls,
 * including nested index/provider lookups, run production resource_registry.c. */
GeorgeRegistryRoot *D_003F9468;
static union { double alignment[2]; u32 words[1024]; } storage;
static unsigned long checks;
static unsigned int fixture_index;
static u32 actual_base;
/* Same reviewed ordinary C macro used by the published 219FF0 source. */
GEORGE_DEFINE_MAP_LOOKUP(published_map_lookup)

static u32 physical(u32 value)
{
    if (value>=0x20000U && value<0x21000U) return actual_base+(value-0x20000U);
    return value;
}
static u32 canonical(u32 value)
{
    if (value>=actual_base && value-actual_base<sizeof(storage.words))
        return 0x20000U+(value-actual_base);
    return value;
}
static void equal(u32 actual,u32 expected,const char *what,unsigned int offset)
{
    ++checks;
    if (actual!=expected) {
        fprintf(stderr,"registry fixture %u %s +%x: %08x != %08x\n",
                fixture_index,what,offset,actual,expected);
        exit(1);
    }
}
static u32 invoke(u32 function,const u32 *a)
{
    switch (function) {
    case 0:return (u32)func_00217F78((const void *)a[0]);
    case 1:return (u32)func_00217FB0((const void *)a[0]);
    case 2:return (u32)func_00217FE8((const void *)a[0]);
    case 3:return (u32)func_00218020((const void *)a[0]);
    case 4:return (u32)func_002193A0((GeorgeRegistryProviderList *)a[0],a[1],(const void *)a[2]);
    case 5:func_0021A0B8((GeorgeGenericMap *)a[0],a[1],(void *)a[2]);return 0;
    case 6:return func_0021A190((GeorgeGenericMap *)a[0],a[1]);
    case 7:return func_0021A238((GeorgeGenericMap *)a[0],a[1],(void **)a[2]);
    case 8:return func_00229020((void *)a[0],(const void *)a[1],(u32 *)a[2],(u32 *)a[3]);
    case 9:return func_00229FF0((void *)a[0],a[1],(const void *)a[2],(u32 *)a[3],(u32 *)a[4]);
    case 10:return (u32)func_002A77F0((GeorgeRegistryDataGroup *)a[0],a[1],(const void *)a[2]);
    case 11:return (u32)func_002AF9A0((GeorgeRegistryData *)a[0],(const void *)a[1],(u32 *)a[2]);
    case 12:return (u32)func_002A8250((GeorgeRegistryIndex *)a[0],(const void *)a[1]);
    default:fprintf(stderr,"unowned registry fixture\n");exit(1);
    }
}

static void map_sequence(void)
{
    GeorgeRegistryPooledMap *map=(GeorgeRegistryPooledMap *)storage.words;
    GeorgeRegistryNodePool *pool=(GeorgeRegistryNodePool *)((u8 *)storage.words+32);
    GeorgeGenericMapNode **slots=(GeorgeGenericMapNode **)((u8 *)storage.words+64);
    GeorgeGenericMapNode **buckets=(GeorgeGenericMapNode **)((u8 *)storage.words+128);
    GeorgeGenericMapNode *nodes=(GeorgeGenericMapNode *)((u8 *)storage.words+256);
    void *output=(void *)0x123;
    memset(&storage,0,sizeof(storage));
    map->map.bucket_count=3;map->map.buckets=buckets;map->pool=pool;
    pool->capacity=0;pool->slots=slots;pool->storage=nodes;
    slots[0]=nodes;slots[1]=nodes+1;
    func_0021A0B8(&map->map,7,(void *)0x12345000);
    equal(map->map.count,1,"sequence insert count",0);
    equal(pool->index,1,"sequence insert index",0);
    equal((u32)published_map_lookup(&map->map,7),0x12345000,"published lookup",0);
    func_0021A0B8(&map->map,7,(void *)0x23456000);
    equal(map->map.count,1,"sequence replacement count",0);
    equal(pool->index,1,"sequence replacement index",0);
    equal((u32)published_map_lookup(&map->map,7),0x23456000,"replacement lookup",0);
    map->map.flags=1;func_0021A0B8(&map->map,7,(void *)0x34567000);
    equal(map->map.count,2,"sequence duplicate count",0);
    equal((u32)published_map_lookup(&map->map,7),0x34567000,"first duplicate lookup",0);
    equal(func_0021A238(&map->map,7,&output),1,"output removal",0);
    equal((u32)output,0x34567000,"first duplicate output",0);
    equal(pool->index,1,"returned slot index",0);
    equal((u32)slots[1],(u32)(nodes+1),"returned slot",0);
    equal(func_0021A190(&map->map,7),1,"plain removal",0);
    equal(map->map.count,0,"empty count",0);
    equal(pool->index,0,"empty pool index",0);
    equal((u32)published_map_lookup(&map->map,7),0,"empty lookup",0);
    output=(void *)0x123;
    equal(func_0021A238(&map->map,7,&output),0,"missing removal",0);
    equal((u32)output,0x123,"missing output untouched",0);
    slots[0]=0;func_0021A0B8(&map->map,7,(void *)0x45678000);
    equal(pool->index,1,"null take advances index",0);
    equal(map->map.count,0,"failed take count",0);
    equal((u32)slots[0],0,"failed take clears slot",0);
    equal(pool->capacity,0,"capacity remains untouched",0);
}

int main(void)
{
    unsigned int i;
    actual_base=(u32)storage.words;
    if (sizeof(void *)!=4 || actual_base>=0x80000000U ||
        actual_base+sizeof(storage.words)<actual_base ||
        actual_base+sizeof(storage.words)>0x80000000U) {
        fprintf(stderr,"registry harness needs its whole storage below 0x80000000\n");return 1;
    }
    for (fixture_index=0;fixture_index<sizeof(registry_golden)/sizeof(registry_golden[0]);++fixture_index) {
        const struct RegistryGolden *g=&registry_golden[fixture_index];
        u32 args[5],result;
        for (i=0;i<1024;++i) storage.words[i]=physical(g->initial[i]);
        for (i=0;i<5;++i) args[i]=physical(g->args[i]);
        D_003F9468=(GeorgeRegistryRoot *)physical(g->global);
        result=invoke(g->function,args);
        if (g->function!=5) equal(canonical(result),g->result,"return",0);
        equal(canonical((u32)D_003F9468),g->expected_global,"global",0);
        for (i=0;i<1024;++i) equal(canonical(storage.words[i]),g->expected[i],"memory",i*4);
    }
    map_sequence();
    printf("resource_registry: %lu checks passed\n",checks);
    return 0;
}
