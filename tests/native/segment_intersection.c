#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/segment_intersection.h"
#include "segment_intersection_golden.h"

typedef unsigned long long Bits64;
static unsigned checks, fixture, calls[4], event_count;
static u32 events[108];
static float *published;
static unsigned output_size;

static u32 bits(float value) { union { float f; u32 u; } v; v.f=value; return v.u; }
static void check(int condition,const char *message,unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr,"intersection fixture%u word%u: %s\n",fixture,index,message);
        exit(1);
    }
}
static void event(unsigned index,u32 a,u32 b,u32 c,u32 d,u32 e)
{
    unsigned i;
    check(event_count+9<=108,"call event bound",event_count);
    ++calls[index];
    events[event_count++]=index;
    events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
    events[event_count++]=d;events[event_count++]=e;
    for(i=0;i<3;++i) events[event_count++]=published && i<output_size ? bits(published[i]) : 0;
}
Bits64 func_00374848(float value)
{
    union { double f; Bits64 u; } v;
    event(0,0,0,0,0,bits(value));v.f=(double)value;return v.u;
}
s32 func_00373250(Bits64 first,Bits64 second)
{
    union { double f; Bits64 u; } a,b;
    event(1,(u32)first,(u32)(first>>32),(u32)second,(u32)(second>>32),0);
    a.u=first;b.u=second;return (a.f>b.f)-(a.f<b.f);
}
Bits64 func_00372CC0(Bits64 first,Bits64 second)
{
    union { double f; Bits64 u; } a,b;
    event(2,(u32)first,(u32)(first>>32),(u32)second,(u32)(second>>32),0);
    a.u=first;b.u=second;a.f-=b.f;return a.u;
}

/* Native-only symbol bridge logs the actual scalar arguments, then executes
 * the unchanged published vector_math.c normalization as a separate TU. */
extern float intersection_native_normalize(GeorgeMathVec3 *);
float func_002A3538(GeorgeMathVec3 *value)
{
    event(3,bits(value->x),bits(value->y),bits(value->z),0,0);
    return intersection_native_normalize(value);
}
static float unexpected_math(void) { check(0,"unselected trig body executed",0);return 0; }
float func_0029B940(float y,float x) { (void)y;(void)x;return unexpected_math(); }
float func_0029C090(float value) { (void)value;return unexpected_math(); }
float func_0029C168(float value) { (void)value;return unexpected_math(); }
float func_0029C230(float value) { (void)value;return unexpected_math(); }

static void clear_calls(void) { event_count=0;memset(calls,0,sizeof(calls));memset(events,0,sizeof(events)); }
static void golden_checks(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(intersection_golden)/sizeof(intersection_golden[0]);++i) {
        const struct IntersectionGolden *g=&intersection_golden[i];
        union { u32 words[64];float value[64]; } memory;
        u32 result;
        fixture=i;memcpy(memory.words,g->initial,sizeof(memory.words));clear_calls();
        output_size=g->routine==0 ? 3 : 1;
        published=g->offsets[g->routine==0 ? 5 : 2]<0 ? 0 : memory.value+g->offsets[g->routine==0 ? 5 : 2];
        if(g->routine==0) result=func_0029EB98((GeorgeMathVec3 *)(memory.value+g->offsets[0]),
                (GeorgeMathVec3 *)(memory.value+g->offsets[1]),(GeorgeMathVec3 *)(memory.value+g->offsets[2]),
                (GeorgeMathVec3 *)(memory.value+g->offsets[3]),(GeorgeMathVec3 *)(memory.value+g->offsets[4]),
                (GeorgeMathVec3 *)published);
        else result=func_0029F080((GeorgePathRay *)(memory.value+g->offsets[0]),
                                 (GeorgeMathVec4 *)(memory.value+g->offsets[1]),published);
        check(result==g->result,"return differs from original finite trace",64);
        for(j=0;j<64;++j) check(memory.words[j]==g->expected[j],"memory differs from original finite trace",j);
        for(j=0;j<4;++j) check(calls[j]==g->calls[j],"actual call count",j);
        check(event_count==g->event_count,"call event extent",65);
        for(j=0;j<event_count;++j) check(events[j]==g->events[j],"soft/actual normalization arguments and publication",j);
    }
}
static void geometry_invariants(void)
{
    GeorgeMathVec3 a={0,0,0},b={4,0,0},c={0,4,0},first,second,output;
    GeorgeMathVec4 plane={0,0,1,0};GeorgePathRay ray;
    int x,y;float fraction;u32 result;
    published=0;output_size=0;
    /* Interior/outside points away from the unusual zero-vector fallback
     * boundaries have a separate axis-plane barycentric inequality model. */
    for(x=-3;x<=7;++x) for(y=-3;y<=7;++y) {
        first.x=second.x=x/2.0f;first.y=second.y=y/2.0f;first.z=-2;second.z=2;
        clear_calls();result=func_0029EB98(&first,&second,&a,&b,&c,&output);
        check(result==(u32)(x>=0 && y>=0 && x+y<=8),"independent triangle halfspace model",0);
        check(output.x==first.x && output.y==first.y && output.z==0,"axis-plane intersection point",1);
        check(calls[3]==7,"all seven actual normalizations",2);
    }
    memset(&ray,0,sizeof(ray));ray.field0C.z=1;ray.field18=2;
    for(x=-8;x<=12;++x) {
        plane.w=x/4.0f;clear_calls();result=func_0029F080(&ray,&plane,&fraction);
        check(fraction==plane.w/2,"independent axis-plane fraction",3);
        check(result==(u32)(0<plane.w && plane.w/2<1.00100004673004150390625f),"independent ray interval model",4);
    }
}
int main(void)
{
    golden_checks();geometry_invariants();
    printf("segment_intersection: %u checks passed\n",checks);return 0;
}
