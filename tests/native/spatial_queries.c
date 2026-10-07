#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/spatial_queries.h"
#include "spatial_queries_golden.h"

static unsigned checks, current;
static union { u32 words[640]; float values[640]; } arena __attribute__((aligned(16)));
GeorgeGoalMapOwner *D_003F8C28;

static void check(int condition, const char *message, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr,"spatial fixture %u index %u: %s\n",current,index,message);
        exit(1);
    }
}
static void *address(unsigned offset)
{
    check(offset < sizeof(arena),"synthetic address range",offset);
    return (char *)&arena + offset;
}
static u32 guest(const void *pointer)
{
    if (pointer == NULL) return 0;
    check((u32)pointer >= (u32)&arena && (u32)pointer < (u32)&arena + sizeof(arena),
          "returned pointer addresses synthetic arena",0);
    return 0x30000U + (u32)pointer - (u32)&arena;
}
static float unexpected(void)
{
    fprintf(stderr,"unexpected spatial engine/allocator hook\n");
    exit(1);
    return 0;
}
/* The genuine bounds/vector TUs contain these unused engine hooks. */
float func_0029B940(float y,float x) { (void)y; (void)x; return unexpected(); }
float func_0029C090(float x) { (void)x; return unexpected(); }
float func_0029C168(float x) { (void)x; return unexpected(); }
float func_0029C230(float x) { (void)x; return unexpected(); }
void *func_002AEC28(u32 size) { (void)size; unexpected(); return NULL; }

static const unsigned pointer_offsets[] = {
    8,12,16,20, 0x50,0x70,0x90,0xB0, 0x12C,0x16C,0x1AC,0x1EC,
    0x23C,0x240,0x28C,0x290,0x2DC,0x2E0,0x32C,0x330
};

static void setup(const struct SpatialGolden *g)
{
    unsigned j;
    memcpy(arena.words,g->initial,sizeof(arena));
    for(j=0;j<sizeof(pointer_offsets)/sizeof(pointer_offsets[0]);++j) {
        u32 *slot=(u32 *)address(pointer_offsets[j]);
        if(*slot!=0) {
            check(*slot>=0x30000U && *slot<0x30A00U,"typed pointer translation",j);
            *slot=(u32)address(*slot-0x30000U);
        }
    }
    D_003F8C28=(GeorgeGoalMapOwner *)address(0);
}
static u32 invoke(unsigned routine,u32 key,unsigned point_offset)
{
    void *point=address(point_offset);
    if(routine==0) return guest(func_001CC960((GeorgeMathVec3 *)point));
    if(routine==1) return guest(func_001CCA28(key));
    if(routine==2) return guest(func_001CF340(point));
    check(routine==3,"selected complete entry",routine);
    return (u32)func_001CF588(point,key);
}
static void golden_checks(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(spatial_golden)/sizeof(spatial_golden[0]);++i) {
        const struct SpatialGolden *g=&spatial_golden[i];
        current=i;
        setup(g);
        check(invoke(g->routine,g->key,g->point_offset)==g->result,"original-derived return",0);
        check(D_003F8C28==(GeorgeGoalMapOwner *)&arena,"global pointer preserved",0);
        for(j=0;j<sizeof(pointer_offsets)/sizeof(pointer_offsets[0]);++j) {
            u32 *slot=(u32 *)address(pointer_offsets[j]);
            *slot=guest((void *)*slot);
        }
        for(j=0;j<640;++j)
            check(arena.words[j]==g->expected[j],"entire synthetic arena preserved",j);
    }
}
static u32 bits(float value)
{
    union { float scalar;u32 word; } v;
    v.scalar=value;
    return v.word;
}
static void extra_cases(void)
{
    const struct SpatialGolden *g=NULL;
    GeorgeMathVec3 *point;
    unsigned i;
    current=0xFFFFFFFFU;
    for(i=0;i<sizeof(spatial_golden)/sizeof(spatial_golden[0]);++i)
        if(spatial_golden[i].routine==3 && spatial_golden[i].result==1) {
            g=&spatial_golden[i];break;
        }
    check(g!=NULL,"successful baseline available",0);
    setup(g);
    point=(GeorgeMathVec3 *)address(g->point_offset);
    point->x=0;point->y=0;point->z=0;
    arena.words[0x704/4]&=0xFFFFFF00U;
    arena.words[0x704/4]|=1U;
    arena.words[0x708/4]=bits(0);
    arena.words[0x70C/4]=bits(0);
    arena.words[0x710/4]=bits(0);
    check(func_001CF588(point,0x11110000U)==1,"one vertex accepts arbitrary XZ",0);
    point->x=50;point->z=-70;
    check(func_001CF588(point,0x11110000U)==1,"no invented minimum polygon count",0);
    for(i=0;i<2;++i) {
        point->y=i? -2.0f:2.0f;
        check(func_001CF588(point,0x11110000U)==0,"strict height boundary",i);
    }
    point->x=0;point->y=0;point->z=1;
    arena.words[0x704/4]=(arena.words[0x704/4]&0xFFFFFF00U)|2U;
    arena.words[0x714/4]=bits(0.09999999403953552f);
    arena.words[0x718/4]=bits(0);
    arena.words[0x71C/4]=bits(0);
    check(func_001CF588(point,0x11110000U)==1,"strict tiny-edge skip below threshold",0);
    arena.words[0x714/4]=bits(0.10000000149011612f);
    check(func_001CF588(point,0x11110000U)==0,"edge at exact threshold is evaluated",0);
    setup(g);
    arena.words[0x12C/4]=0;
    check(func_001CC960((GeorgeMathVec3 *)address(g->point_offset))==NULL,
          "first bounds hit with missing nested pointer does not resume",0);
    check(func_001CCA28(0x9111FFFFU)==NULL,"masked matching header missing nested pointer",0);
    setup(g);
    check(guest(func_001CCA28(0x9111FFFFU))==0x30200U,"bit31 and low16 are masked",0);
    setup(g);
    arena.words[0x204/4]=(arena.words[0x204/4]&0xFFFF0000U)|2U;
    arena.words[0x704/4]=(arena.words[0x704/4]&0xFFFFFF00U)|4U;
    arena.words[0x754/4]&=0xFFFFFF00U;
    for(i=0;i<4;++i) arena.words[(0x70C+i*12)/4]=bits(1);
    point=(GeorgeMathVec3 *)address(g->point_offset);
    point->x=3;point->z=0;point->y=2.5f;
    check(guest(func_001CF340(point))==0x30460U,
          "later empty polygon inherits earlier nonzero scratch Y",0);
    point->y=3;
    check(func_001CF340(point)==NULL,"inherited Y strict upper boundary",0);
    point->y=-1;
    check(func_001CF340(point)==NULL,"inherited Y strict lower boundary",0);
}
int main(void)
{
    check(sizeof(void *)==4 && sizeof(float)==4,"native target widths",0);
    check(sizeof(GeorgeBounds)==24 && sizeof(GeorgeGoalRoadEntry)==0x60,
          "published observed layouts",0);
    golden_checks();
    extra_cases();
    printf("spatial queries: %u checks passed\n",checks);
    return 0;
}
