#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/actor_route.h"
#include "actor_route_golden.h"

#define BUFFER 0x30000U
#define END 0x3A000U
#define WORDS ((END-BUFFER)/4U)
#define ACTOR 0x36000U
#define STATE 0x36200U
#define SETUP 0x37000U
#define VOBJECT 0x38800U
#define TABLE 0x38900U
#define FRAME 0x38A00U
#define POSITION 0x38C00U
#define OTHER_MANAGER 0x39000U
#define ROADS 0x30200U
#define RECORDS 0x30434U
#define POSITIONS 0x31000U
static union { u32 words[WORDS+1]; float f; } arena;
static unsigned checks,current,mutation;
static s32 adjustment;
static u32 calls[3],events[32],event_count;
GeorgeGoalMapOwner *D_003F8C28;
GeorgeRegistryRoot *D_003F9468;

static void check(int value,const char *what,unsigned i)
{
    ++checks;
    if(!value) { fprintf(stderr,"actor route fixture %u %s %u\n",current,what,i);exit(1); }
}
static void *physical(u32 p)
{
    if(!p)return NULL;
    check(p>=BUFFER-4U && p<END,"authored pointer",p);
    return (u8 *)&arena+p-BUFFER+4U;
}
static u32 canonical(u32 p)
{
    if(p>=(u32)&arena && p-(u32)&arena<sizeof(arena))return BUFFER-4U+p-(u32)&arena;
    return p;
}
static u32 bits(float f) { union { u32 u;float f; } v;v.f=f;return v.u; }
/* Exact authored pointer cells: provider links, road/header/list pointers,
 * actor/state/reference fields, observed queue slots and setup array store.
 * These are fixture views, not a general game format or capacity claim. */
static int pointer_cell(u32 address)
{
    static const u32 cells[]={
        0x30008,0x3000C,0x30010,0x30050,0x3006C,0x30070,0x30090,0x300DC,0x300E0,
        0x3012C,0x3016C,0x301AC,0x30224,0x30230,0x30234,0x30238,0x30248,
        0x30284,0x30290,0x30294,0x30298,0x302A8,0x302E4,0x302F0,0x302F4,
        0x302F8,0x30308,0x30B04,0x30B08,0x36038,0x3603C,0x36210,0x36310,
        0x37CB0,0x38804,0x3892C,0x3899C,0x38B20,
        0x39008,0x3900C,0x39010,0x39050,0x39070,0x39090
    };
    unsigned i;
    for(i=0;i<sizeof(cells)/sizeof(cells[0]);++i)if(cells[i]==address)return 1;
    return 0;
}
static int guest_pointer_word(u32 v)
{
    return (v>=BUFFER-4U && v<END) || v==0x60000000U || v==0x60000004U;
}
static void push(u32 v) { check(event_count<32,"controlled event extent",event_count);events[event_count++]=canonical(v); }
static void *frame_query(void *object)
{
    u32 normal;
    ++calls[0];push(0);push((u32)object);
    check(canonical((u32)object)==(u32)(VOBJECT+adjustment),"signed frame receiver",0);
    normal=*(u32 *)physical(ROADS+0x48)+*(u16 *)physical(RECORDS+0x10)*12U;
    if(mutation==1 || mutation==2) {
        float *v=(float *)normal;
        v[0]=mutation==1?0.0f:-1.0f;v[1]=mutation==1?1.0f:2.0f;v[2]=mutation==1?0.0f:3.0f;
    }
    if(mutation==2)*(void **)physical(ROADS+0x48)=physical(POSITIONS+12);
    if(mutation==3)*(u32 *)physical(ACTOR+0x38)=0;
    if(mutation==4)*(u32 *)physical(RECORDS)|=0x10000U;
    if(mutation==5)D_003F8C28=physical(OTHER_MANAGER);
    return physical(FRAME);
}
static const GeorgeMathVec3 *position_query(void *object)
{
    ++calls[1];push(1);push((u32)object);
    check(canonical((u32)object)==(u32)(VOBJECT+adjustment),"signed position receiver",0);
    return physical(POSITION);
}
const GeorgeMathVec3 *func_00120B68(void *object)
{
    ++calls[2];push(2);push((u32)object);
    check(canonical((u32)object)==VOBJECT,"numeric position argument",0);
    return physical(POSITION);
}
/* Supporting-only observers: no recovered-source or byte credit. The getter
 * consumes only its one original GPR; setup consumes the seven observed GPRs.
 * Setup reproduces its reviewed scalar stores for these bounded fixtures. */
GeorgeGoalOwner *func_001BA610(u32 actor) { return (GeorgeGoalOwner *)(actor+0x34U); }
void func_0020D080(void *owner,void *array,u32 kind,u32 first,u32 second,
                 const GeorgeMathVec3 *point,const GeorgeMathVec3 *target)
{
    u8 *p=owner;
    float *a=(float *)(p+0xCF4),*b=(float *)(p+0xD00);
    u8 *q=p+0xCC4,*r=p+0xCDC;
    unsigned i;
    push(3);push((u32)owner);push((u32)array);push(kind);push(first);push(second);push((u32)point);push((u32)target);
    check(canonical((u32)array)==STATE+0x14 && kind==5,"setup seven GPR contract",0);
    *(void **)(p+0xCB0)=array;p[0xCB5]=(u8)kind;
    *(u32 *)(p+0xCBC)=first;*(u32 *)(p+0xCC0)=second;
    a[0]=point->x;a[1]=point->y;a[2]=point->z;
    b[0]=target->x;b[1]=target->y;b[2]=target->z;
    p[0xCB6]=0;*(u32 *)q=0xFFFFFFFFU;*(float *)(q+0x14)=1000000.0f;
    *(u32 *)(q+4)=0xFFFFFFFFU;q[8]=1;q[9]=0;q[10]=0;q[11]=0;
    *(float *)(q+12)=1000000.0f;*(float *)(q+16)=1000000.0f;
    *(u32 *)r=0xFFFFFFFFU;r[8]=1;*(u32 *)(r+4)=0xFFFFFFFFU;
    r[9]=0;r[10]=0;r[11]=0;
    *(float *)(r+12)=1000000.0f;*(float *)(r+16)=1000000.0f;*(float *)(r+20)=1000000.0f;
    p[0xCB4]=255;
    p[0xCB9]=0;p[0xCB8]=0;p[0xCB7]=0;p[0xCBA]=0;p[0xCBB]=0;p[0]=0;
    if(*(u32 *)(p+0xCBC)==0xFFFFFFFFU || *(u32 *)(p+0xCC0)==0xFFFFFFFFU)p[0xCB6]=6;
    if(*(u32 *)(p+0xCBC)==*(u32 *)(p+0xCC0))p[0xCB6]=8;
    i=bits(*(float *)(q+0x14));check(i==0x49742400U,"setup literal bits",0);
}
static float unexpected(void) { fputs("unexpected unused route helper\n",stderr);exit(1);return 0; }
float func_0029B940(float y,float x) { (void)y;(void)x;return unexpected(); }
float func_0029C090(float x) { (void)x;return unexpected(); }
float func_0029C168(float x) { (void)x;return unexpected(); }
float func_0029C230(float x) { (void)x;return unexpected(); }
void *func_002AEC28(u32 n) { (void)n;unexpected();return NULL; }

int main(void)
{
    unsigned n,i;
    for(n=0;n<sizeof(actor_route_golden)/sizeof(actor_route_golden[0]);++n) {
        const struct ActorRouteGolden *g=&actor_route_golden[n];
        u32 expected[WORDS+1],result=0;
        current=n;mutation=g->mutation;adjustment=g->adjustment;
        memset(&arena,0,sizeof(arena));memset(expected,0,sizeof(expected));
        memset(calls,0,sizeof(calls));event_count=0;
        for(i=0;i<g->initial_count;++i) {
            u32 index=g->initial[i].index,v=g->initial[i].value;
            check(index<=WORDS,"initial sparse index",i);expected[index]=v;
            if(index<WORDS)arena.words[index+1]=v;
        }
        for(i=0;i<g->changed_count;++i) {
            check(g->changed[i].index<=WORDS,"changed sparse index",i);
            expected[g->changed[i].index]=g->changed[i].value;
        }
        D_003F8C28=physical(expected[WORDS]);
        /* Only the explicit typed synthetic cells are relocated. Reject any
         * accidental address-valued scalar rather than guessing its type. */
        for(i=0;i<WORDS;++i) {
            u32 v=arena.words[i+1];
            int is_pointer=pointer_cell(BUFFER+i*4U);
            check(is_pointer || (!guest_pointer_word(v) && !guest_pointer_word(expected[i])),"typed pointer fixture ledger",i);
            if(is_pointer) {
                if(v>=BUFFER-4U && v<END)arena.words[i+1]=(u32)physical(v);
                if(v==0x60000000U)arena.words[i+1]=(u32)frame_query;
                if(v==0x60000004U)arena.words[i+1]=(u32)position_query;
            }
        }
        if(g->routine==0)func_001B7AB8(physical(g->args[0]),physical(g->args[1]));
        else if(g->routine==1)func_001E2D88(physical(g->args[0]),physical(g->args[1]));
        else result=func_001CC628(physical(g->args[0]));
        for(i=0;i<WORDS;++i) {
            u32 v=arena.words[i+1];
            if(pointer_cell(BUFFER+i*4U)) {
                v=canonical(v);
                if(v==(u32)frame_query)v=0x60000000U;
                if(v==(u32)position_query)v=0x60000004U;
            }
            check(v==expected[i],"whole authored word",i);
        }
        check(canonical((u32)D_003F8C28)==expected[WORDS],"fresh manager global",0);
        check(result==g->result,"original queue result only",0);
        for(i=0;i<3;++i)check(calls[i]==g->calls[i],"actual controlled calls",i);
        check(event_count==g->event_count,"controlled event count",0);
        for(i=0;i<event_count;++i)check(events[i]==g->events[i],"controlled argument event",i);
    }
    printf("actor route: %u checks passed\n",checks);
    return 0;
}
