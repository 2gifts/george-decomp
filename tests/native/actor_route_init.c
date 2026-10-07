#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/actor_route_init.h"
#include "george/actor_movement.h"
#include "george/resource_registry.h"
#include "george/route_setup.h"
#include "actor_route_init_golden.h"

#define BUFFER 0x2F000U
#define END 0x37000U
#define WORDS ((END-BUFFER)/4U)
#define STATE 0x31000U
#define VOBJECT 0x35100U
#define POSITION 0x35300U
#define TABLE 0x35200U
static union { u32 words[WORDS]; double alignment; } arena;
static u32 expected[WORDS],calls[2],events[16],event_count;
static u32 movement_calls,setup_calls;
static unsigned checks,current,mutation;
static s32 adjustment;
GeorgeGoalMapOwner *D_003F8C28;

static void check(int v,const char *why,unsigned i)
{
    ++checks;
    if(!v) { fprintf(stderr,"route init fixture %u: %s %u\n",current,why,i);exit(1); }
}
static void *physical(u32 value)
{
    if(!value)return NULL;
    check(value>=BUFFER && value<END,"authored pointer",value);
    return (u8 *)&arena+value-BUFFER;
}
static u32 canonical(u32 value)
{
    if(value>=(u32)&arena && value-(u32)&arena<sizeof(arena))
        return BUFFER+value-(u32)&arena;
    return value;
}
static void push(u32 v)
{
    check(event_count<16,"bounded controlled event",event_count);
    events[event_count++]=canonical(v);
}
static const GeorgeMathVec3 *position_call(void *receiver)
{
    ++calls[0];push(0);push((u32)receiver);
    check(canonical((u32)receiver)==(u32)(VOBJECT+adjustment),"one-GPR signed adjustment",0);
    if(mutation==1)*(void **)physical(STATE)=physical(0x35400U);
    if(mutation==2)*(u32 *)physical(STATE+0xC0)=0;
    if(mutation==3)D_003F8C28=physical(0x30100U);
    return physical(POSITION);
}
const GeorgeMathVec3 *func_00120B68(void *receiver)
{
    ++calls[1];push(1);push((u32)receiver);
    check(canonical((u32)receiver)==VOBJECT,"numeric one-GPR position",0);
    if(mutation==1)*(void **)physical(STATE)=physical(0x35400U);
    if(mutation==2)*(u32 *)physical(STATE+0xC0)=0;
    if(mutation==3)D_003F8C28=physical(0x30100U);
    return physical(POSITION);
}
/* Explicit compiler aliases compile the complete unchanged published bodies.
 * These bridges record arguments, then execute the real source with them. */
extern void route_init_published_setup(void *,void *,u32,u32,u32,
                                       const GeorgeMathVec3 *,const GeorgeMathVec3 *);
extern s32 route_init_published_movement(GeorgeGoalEntity *,const GeorgeMathVec3 *);
void func_0020BEA8(void *owner,void *state,u32 kind,u32 first,u32 second,
                   const GeorgeMathVec3 *point,const GeorgeMathVec3 *target)
{
    ++setup_calls;
    push(2);push((u32)owner);push((u32)state);push(kind);push(first);push(second);
    push((u32)point);push((u32)target);
    check(canonical((u32)state)==STATE+0x14 && kind==12 && canonical((u32)target)==STATE+0xC4,
          "actual seven-GPR supporting source integration",0);
    route_init_published_setup(owner,state,kind,first,second,point,target);
}
s32 func_00177E48(GeorgeGoalEntity *entity,const GeorgeMathVec3 *zero)
{
    u32 state=entity->field0C;
    ++movement_calls;
    check(*(float *)((u8 *)entity+0x438)>0.0f ||
          (state<42U && state!=0 && state!=2 && state!=5 && state!=12 && state!=23 && state!=31 && state!=32),
          "movement source restricted reached state",0);
    check(zero->x==0.0f && zero->y==0.0f && zero->z==0.0f,"actual zero movement input",0);
    return route_init_published_movement(entity,zero);
}
/* No fixture reaches the movement body's deeper external effects. These are
 * fail-fast dependency guards, not replacements for its recovered function. */
void func_0018D8A0(GeorgeGoalEntity *e,const GeorgeMathVec3 *p)
{ (void)e;(void)p;check(0,"unobserved movement command",0); }
void func_0018FD30(GeorgeGoalEntity *e,GeorgeActorBits64 m)
{ (void)e;(void)m;check(0,"unobserved movement flags set",0); }
void func_0018FD40(GeorgeGoalEntity *e,GeorgeActorBits64 m)
{ (void)e;(void)m;check(0,"unobserved movement flags clear",0); }
s32 func_00179070(GeorgeGoalEntity *e,float t)
{ (void)e;(void)t;check(0,"unobserved movement turn",0);return 0; }

static int pointer_cell(const struct InitGolden *g,u32 address)
{
    u32 i;
    for(i=0;i<g->pointer_count;++i)if(g->pointers[i]==address)return 1;
    return 0;
}
int main(void)
{
    unsigned n,i;
    /* Storage/address extent is a host-fixture precondition, not a game limit. */
    check(sizeof(void *)==4,"32-bit host",0);
    check((u32)&arena<0x80000000U && sizeof(arena)<0x80000000U-(u32)&arena,"host address extent",0);
    for(n=0;n<sizeof(init_golden)/sizeof(init_golden[0]);++n) {
        const struct InitGolden *g=init_golden+n;
        u32 result=0,global=0;
        current=n;mutation=g->mutation;adjustment=g->adjustment;
        memset(&arena,0,sizeof(arena));memset(expected,0,sizeof(expected));
        memset(calls,0,sizeof(calls));event_count=0;
        movement_calls=setup_calls=0;
        for(i=0;i<g->initial_count;++i) {
            u32 index=g->initial[i].index,value=g->initial[i].value;
            check(index<=WORDS,"initial synthetic index",index);
            if(index==WORDS)global=value;
            else arena.words[index]=expected[index]=value;
        }
        for(i=0;i<g->changed_count;++i) {
            u32 index=g->changed[i].index;
            check(index<=WORDS,"changed synthetic index",index);
            if(index==WORDS)global=g->changed[i].value;
            else expected[index]=g->changed[i].value;
        }
        D_003F8C28=physical(0x30000U);
        for(i=0;i<WORDS;++i) {
            u32 address=BUFFER+i*4U,value=arena.words[i];
            if(pointer_cell(g,address) && value>=BUFFER && value<END)
                arena.words[i]=(u32)physical(value);
            if(pointer_cell(g,address) && value==0x60000000U)
                arena.words[i]=(u32)position_call;
        }
        if(g->routine==0)
            result=func_001CD578(physical(g->args[0]),physical(g->args[1]),physical(g->args[2]),physical(g->args[3]));
        else if(g->routine==1) {
            func_001DB250(physical(g->args[0]));
        } else result=func_0020D190(physical(g->args[0]),physical(g->args[1]));
        check(result==g->result,"result",0);
        for(i=0;i<WORDS;++i) {
            u32 actual=arena.words[i];
            if(pointer_cell(g,BUFFER+i*4U)) {
                if(actual==(u32)position_call)actual=0x60000000U;
                else actual=canonical(actual);
            }
            check(actual==expected[i],"complete memory word",i);
        }
        check(canonical((u32)D_003F8C28)==global,"fresh global",0);
        for(i=0;i<2;++i)check(calls[i]==g->calls[i],"controlled call count",i);
        check(movement_calls==g->movement_calls,"actual movement source integration",0);
        check(setup_calls==g->setup_calls,"actual setup source integration",0);
        check(event_count==g->event_count,"controlled/actual setup event count",0);
        for(i=0;i<g->event_count;++i)check(events[i]==g->events[i],"controlled/actual setup event order",i);
    }
    printf("actor route init: %u checks passed (%u fixtures)\n",checks,n);
    return 0;
}
