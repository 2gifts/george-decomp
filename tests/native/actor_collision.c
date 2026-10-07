#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/actor_collision.h"
#include "actor_collision_golden.h"

#define BUFFER 0x30000u
#define WORDS (0x2800u / 4u)
#define OFF(x) ((x) - BUFFER)
typedef unsigned long long Bits64;
static union { u32 words[WORDS]; float floats[WORDS]; } arena __attribute__((aligned(16)));
static unsigned checks, current_fixture, event_count, position_count, radius_count, query_count;
static u32 events[16384];
static const struct CollisionGolden *current;
static void **object_output;
GeorgeSlotPool D_0045C6A0;
GeorgeGoalMapOwner *D_003F8C28;
void *D_003F8B58;
const GeorgeMathVec3 D_FLT_00437288 = {0.0f,1.0f,0.0f};

static void check(int condition,const char *message,unsigned index)
{
    ++checks;
    if(!condition) {
        fprintf(stderr,"collision fixture %u index %u: %s\n",current_fixture,index,message);
        exit(1);
    }
}
static u32 bits(float value) { union {float f;u32 u;} x;x.f=value;return x.u; }
static float value(u32 word) { union {float f;u32 u;} x;x.u=word;return x.f; }
static void event(u32 word) { check(event_count<16384,"event observer bound",event_count);events[event_count++]=word; }
static void *pointer(u32 guest)
{
    check(guest>=BUFFER && guest<BUFFER+sizeof(arena),"arena pointer",guest);
    return (u8 *)&arena+guest-BUFFER;
}
static u32 guest(const void *p)
{
    u32 address=(u32)p;
    check(address>=(u32)&arena && address<(u32)&arena+sizeof(arena),"host pointer",address);
    return BUFFER+address-(u32)&arena;
}
static void soft_event(u32 kind,Bits64 a,Bits64 b,float f)
{ event(kind);event((u32)a);event((u32)(a>>32));event((u32)b);event((u32)(b>>32));event(bits(f)); }
Bits64 func_00374848(float f)
{ union {double d;Bits64 u;} x;soft_event(0,0,0,f);x.d=(double)f;return x.u; }
s32 func_00373250(Bits64 a,Bits64 b)
{ union {double d;Bits64 u;} x,y;soft_event(1,a,b,0);x.u=a;y.u=b;return (x.d>y.d)-(x.d<y.d); }
Bits64 func_00372CC0(Bits64 a,Bits64 b)
{ union {double d;Bits64 u;} x,y;soft_event(2,a,b,0);x.u=a;y.u=b;x.d-=y.d;return x.u; }
float func_003734F8(Bits64 a)
{ union {double d;Bits64 u;} x;soft_event(3,a,0,0);x.u=a;return (float)x.d; }

/* Explicit controlled engine/virtual observation contracts. Published pool,
 * normalization, angles, spatial queries and segment distance execute real C
 * in distinct translation units, without replacing those implementations. */
u32 func_001C1540(void *manager,const GeorgeMathVec3 *first,const GeorgeMathVec3 *second,
                 void **output,u32 capacity,float radius)
{
    unsigned i;
    check(manager==pointer(0x32100)&&capacity==32&&bits(radius)==bits(1.0f),"broadphase ABI",0);
    event(10);event(bits(first->x));event(bits(first->y));event(bits(first->z));
    event(bits(second->x));event(bits(second->y));event(bits(second->z));event(capacity);event(bits(radius));
    object_output=output;
    for(i=0;i<current->objects;++i)output[i]=pointer(i?0x31C00:0x31A00);
    if(current->mutation==1)*(float *)pointer(0x30800)=0.125f;
    return current->objects;
}
static const GeorgeMathVec3 *position_call(void *receiver)
{
    u32 address=guest(receiver);
    check(address==0x31804||address==0x31A04||address==0x31C04,"position signed adjustment",address);
    ++position_count;event(11);event(address);
    if(current->mutation==2 && position_count==2)*(float *)pointer(0x31F04)=0.0625f;
    if(current->mutation==2 && position_count==3)*(float *)pointer(0x31F08)=0.125f;
    return pointer(0x31F00);
}
static float radius_call(void *receiver)
{
    u32 address=guest(receiver);
    float radius=address==0x31804?0.25f:0.125f;
    check(address==0x31804||address==0x31A04||address==0x31C04,"radius signed adjustment",address);
    ++radius_count;event(12);event(address);event(bits(radius));
    if(current->mutation==3 && radius_count==1)object_output[0]=pointer(0x31C00);
    if(current->mutation==7 && address==0x31804)*(void **)pointer(0x30064)=pointer(0x31C00);
    return radius;
}
u16 *func_001CEA80(const GeorgeMathVec3 *point,GeorgeGoalRoad **road,u8 *vertex,s32 *mode,float radius)
{
    ++query_count;
    event(13);event(bits(point->x));event(bits(point->y));event(bits(point->z));event(bits(radius));
    if(current->query) {
        *road=pointer(0x30D00);*vertex=0;*mode=(s32)current->query-1;
        if(current->mutation==4)*(u8 *)pointer(0x30042)=5;
        {
            u16 *record=pointer(0x30E00+((current->mutation==5 || (current->mutation==8 && query_count>1))?0x34:0));
            if(arena.words[0xE00/4]&0xFFFF0000u)
                check((*(u32 *)record>>16)==0x8000u,"controlled query preserves full route ID",query_count);
            return record;
        }
    }
    return NULL;
}

static void translate_cell(unsigned offset)
{
    u32 v=arena.words[offset/4];
    if(v>=BUFFER&&v<BUFFER+sizeof(arena))arena.words[offset/4]=(u32)pointer(v);
}
static void translate_pointers(void)
{
    static const unsigned cells[]={0,0x64,0x110,0xC08,0xC50,0xCAC,0xD38,0xD48,0xD3C,0xD40,0x1804,0x1A04,0x1C04,0x2104,0x2020};
    unsigned i,j;
    for(i=0;i<sizeof(cells)/sizeof(cells[0]);++i)translate_cell(cells[i]);
    for(i=0;i<256;++i)translate_cell(0x6C+i*4);
    for(i=0;i<16;++i)for(j=0;j<3;++j)translate_cell(0x900+i*32+0x14+j*4);
    arena.words[0x1E2C/4]=(u32)position_call;
    arena.words[0x1E44/4]=(u32)radius_call;
}
static u32 normalized(u32 actual)
{
    if(actual>=(u32)&arena&&actual<(u32)&arena+sizeof(arena))return BUFFER+actual-(u32)&arena;
    if(actual==(u32)position_call)return 0x60000000;
    if(actual==(u32)radius_call)return 0x60000004;
    return actual;
}
static int pointer_cell(unsigned offset)
{
    static const unsigned cells[]={0,0x64,0x110,0xC08,0xC50,0xCAC,0xD38,0xD48,0xD3C,0xD40,0x1804,0x1A04,0x1C04,0x2104,0x2020,0x1E2C,0x1E44};
    unsigned i;
    for(i=0;i<sizeof(cells)/sizeof(cells[0]);++i)if(offset==cells[i])return 1;
    if(offset>=0x6C&&offset<0x6C+256*4)return 1;
    if(offset>=0x900&&offset<0x900+16*32) {
        unsigned member=(offset-0x900)%32;
        return member==0x14||member==0x18||member==0x1C;
    }
    return 0;
}
int main(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(collision_golden)/sizeof(collision_golden[0]);++i) {
        const struct CollisionGolden *g=&collision_golden[i];
        u32 expected[WORDS],pool[7],result=0,initial_route_key;
        GeorgeActorCollisionNode *node;
        current=g;current_fixture=i;event_count=position_count=radius_count=query_count=0;
        memset(&arena,0,sizeof(arena));memset(expected,0,sizeof(expected));
        for(j=0;j<g->initial_count;++j) {
            check(g->initial[j].index<WORDS,"initial sparse extent",j);
            arena.words[g->initial[j].index]=expected[g->initial[j].index]=g->initial[j].value;
        }
        for(j=0;j<g->changed_count;++j)expected[g->changed[j].index]=g->changed[j].value;
        initial_route_key=arena.words[0x14/4];
        translate_pointers();
        memset(&D_0045C6A0,0,sizeof(D_0045C6A0));
        D_0045C6A0.capacity=16;D_0045C6A0.stride=32;D_0045C6A0.data=pointer(0x30900);D_0045C6A0.free_count=16;
        D_003F8C28=pointer(0x30C00);D_003F8B58=pointer(0x32100);
        if(g->routine==1)func_001E52E8(pointer(g->args[0]),pointer(0x30900));
        else {
            node=func_001E2EC8(pointer(g->args[0]),pointer(g->args[1]),pointer(g->args[2]),
                 g->args[3],(s32)g->args[4],g->args[5],(s32)g->args[6],(s32)g->args[7],value(g->scalar[0]),value(g->scalar[1]));
            result=node?guest(node):0;
        }
        if(g->query && (arena.words[0xE00/4]&0xFFFF0000u)) {
            check(query_count==2,"both full-route-ID sides execute",query_count);
            check(arena.words[0x14/4]==0x80000000u+(g->mutation==5||g->mutation==8),
                  "full route key publication",0x14/4);
            if(initial_route_key==0x80000000u && g->mutation==0)
                check((arena.words[0x18/4]&255u)==(expected[0x18/4]&255u),
                      "matching full route key preserves orientation",0x18/4);
        }
        check(result==g->result,"original return",WORDS);
        for(j=0;j<WORDS;++j) {
            u32 actual=pointer_cell(j*4)?normalized(arena.words[j]):arena.words[j];
            if(actual!=expected[j])fprintf(stderr,"actual %08X expected %08X\n",actual,expected[j]);
            check(actual==expected[j],"original complete arena",j);
        }
        memcpy(pool,&D_0045C6A0,sizeof(pool));pool[2]=normalized(pool[2]);
        for(j=0;j<7;++j)check(pool[j]==g->pool[j],"original complete pool prefix",j);
        check(event_count==g->event_count,"original event extent",event_count);
        for(j=0;j<event_count;++j) {
            if(events[j]!=g->events[j])fprintf(stderr,"event actual %08X expected %08X\n",events[j],g->events[j]);
            check(events[j]==g->events[j],"original callback/soft operands",j);
        }
    }
    printf("actor collision: %u checks passed\n",checks);
    return 0;
}
