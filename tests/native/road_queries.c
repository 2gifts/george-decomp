#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/road_queries.h"
#include "road_queries_golden.h"

#define BUFFER 0x30000U
#define END 0x35000U
#define RECORDS 0x30434U
#define FAR_RECORD (RECORDS+0xFFFFU*0x34U)
#define WORDS ((END-BUFFER)/4U+13U)
#define OUTPUT 0x30C00U
#define CELL 0x30B00U
#define INDEX 0x30A00U
static union { u32 words[(FAR_RECORD+0x34U-BUFFER)/4U]; float values[1]; } arena;
static unsigned checks,current;
GeorgeGoalMapOwner *D_003F8C28;
GeorgeRegistryRoot *D_003F9468;
extern float func_0037AF00(float);
extern u32 func_00374748(float);

static void check(int condition,const char *what,unsigned index)
{
    ++checks;
    if(!condition) {
        fprintf(stderr,"road fixture %u %s index %u\n",current,what,index);
        exit(1);
    }
}
static u32 address(unsigned index)
{
    return index<(END-BUFFER)/4U?BUFFER+4U*index:FAR_RECORD+4U*(index-(END-BUFFER)/4U);
}
static void *physical(u32 p)
{
    if(p==0) return NULL;
    check((p>=BUFFER && p<END)||(p>=FAR_RECORD && p<FAR_RECORD+0x34U),"authored pointer window",p);
    return (u8 *)&arena+p-BUFFER;
}
static u32 guest(const void *p)
{
    if(p==NULL) return 0;
    check((u32)p>=(u32)&arena && (u32)p-(u32)&arena<sizeof(arena),"returned arena pointer",(u32)p);
    return BUFFER+(u32)p-(u32)&arena;
}
static u32 canonical(u32 p)
{
    if(p>=(u32)&arena && p-(u32)&arena<sizeof(arena)) return BUFFER+p-(u32)&arena;
    return p;
}
static float number(u32 w) { union { u32 u;float f; } x;x.u=w;return x.f; }
static u32 bits(float f) { union { u32 u;float f; } x;x.f=f;return x.u; }
static const u32 pointer_cells[]={
    0x30008,0x3000C,0x30010,0x30050,0x30070,0x30090,
    0x3012C,0x3016C,0x301AC,
    0x30224,0x30230,0x30234,0x30238,0x30248,
    0x30284,0x30290,0x30294,0x30298,0x302A8,
    0x302E4,0x302F0,0x302F4,0x302F8,0x30308,
    0x30B04,0x30B08,0x30C00
};
static int pointer_cell(u32 p)
{
    unsigned i;
    for(i=0;i<sizeof(pointer_cells)/sizeof(pointer_cells[0]);++i)
        if(p==pointer_cells[i]) return 1;
    return 0;
}

/* Unused hooks in the genuine bounds/spatial/vector TUs fail if reached.
 * Every helper actually called by these selected bodies executes real C. */
static float unexpected(void) { fputs("unexpected unused road helper\n",stderr);exit(1);return 0; }
float func_0029B940(float y,float x) { (void)y;(void)x;return unexpected(); }
float func_0029C090(float x) { (void)x;return unexpected(); }
float func_0029C168(float x) { (void)x;return unexpected(); }
float func_0029C230(float x) { (void)x;return unexpected(); }
void *func_002AEC28(u32 n) { (void)n;unexpected();return NULL; }

static void runtime_invariants(void)
{
    static const u32 raw[][2]={
        {0x80000000U,0},{0xC2200000U,0},{0x00000001U,0},{0x80000001U,0},
        {0x7FC12345U,0},{0x7F800000U,0xFFFFFFFFU},{0xFF800000U,0},
        {0x421F999AU,39},{0x4EFFFFFFU,0x7FFFFF80U},{0x4F000000U,0x80000000U},
        {0x4F7FFFFFU,0xFFFFFF00U},{0x4F800000U,0xFFFFFFFFU}
    };
    unsigned i;
    check(func_00374748(-40.0f)==0,"real unsigned conversion clamps negative",0);
    check(func_00374748(39.9f)==39,"real unsigned conversion truncates",1);
    check(func_00374748(4294967296.0f)==0xFFFFFFFFU,"real unsigned conversion saturates",2);
    check(bits(func_0037AF00(-0.25f))==0x80000000U,"real ceil retains negative zero",3);
    check(bits(func_0037AF00(40.01f))==bits(41.0f),"real ceil crosses grid width",4);
    for(i=0;i<sizeof(raw)/sizeof(raw[0]);++i)
        check(func_00374748(number(raw[i][0]))==raw[i][1],
              "authentic unsigned conversion raw classification/shift",i);
}

int main(void)
{
    unsigned i,j;
    runtime_invariants();
    for(i=0;i<sizeof(road_golden)/sizeof(road_golden[0]);++i) {
        const struct RoadGolden *g=&road_golden[i];
        u32 expected[WORDS],result,initial_cell_count,initial_row_count;
        current=i;memset(&arena,0,sizeof(arena));memset(expected,0,sizeof(expected));
        for(j=0;j<g->initial_count;++j) {
            u32 a=address(g->initial[j].index);
            check(g->initial[j].index<WORDS,"initial sparse extent",j);
            *(u32 *)physical(a)=expected[g->initial[j].index]=g->initial[j].value;
        }
        for(j=0;j<g->changed_count;++j) {
            check(g->changed[j].index<WORDS,"changed sparse extent",j);
            expected[g->changed[j].index]=g->changed[j].value;
        }
        initial_cell_count=*(u16 *)physical(CELL);
        initial_row_count=*(u32 *)physical(INDEX+4);
        for(j=0;j<sizeof(pointer_cells)/sizeof(pointer_cells[0]);++j) {
            u32 *slot=physical(pointer_cells[j]);
            if((*slot>=BUFFER && *slot<END)||(*slot>=FAR_RECORD && *slot<FAR_RECORD+0x34U))
                *slot=(u32)physical(*slot);
        }
        D_003F8C28=physical(BUFFER);
        if(g->routine==0) result=guest(func_001CC710(physical(g->args[0])));
        else if(g->routine==1) result=bits(func_001CE620(physical(g->args[0]),physical(g->args[1]),
                    (s32)g->args[2],(s32)g->args[3],physical(g->args[4]),physical(g->args[5]),number(g->scalar)));
        else result=guest(func_001CEA80(physical(g->args[0]),physical(g->args[1]),
                    physical(g->args[2]),physical(g->args[3]),number(g->scalar)));
        check(result==g->result,"whole original return",0);
        if(g->routine==1 && (s16)g->args[3]<=1) {
            check(result==g->scalar,"signed count early return preserves threshold",1);
            check(g->changed_count==0,"signed count early return preserves outputs",2);
        }
        if(g->routine==2 && initial_cell_count==0 && initial_row_count!=0 && g->args[1]==OUTPUT)
            check(canonical(*(u32 *)physical(OUTPUT))==0x30200U,
                  "zero-count found cell publishes road even without candidate",3);
        for(j=0;j<WORDS;++j) {
            u32 a=address(j),actual=*(u32 *)physical(a);
            if(pointer_cell(a)||(g->routine==2 && a==g->args[1])) actual=canonical(actual);
            if(actual!=expected[j])fprintf(stderr,"address %08X actual %08X expected %08X\n",a,actual,expected[j]);
            check(actual==expected[j],"complete initialized memory windows",j);
        }
    }
    printf("road queries: %u checks passed\n",checks);
    return 0;
}
