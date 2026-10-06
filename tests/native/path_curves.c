#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "george/path_curves.h"
#include "path_curves_golden.h"

/* Selected bodies and vector helpers are separate real production TUs.
 * Callback/query/runtime functions below are controlled ABI observations,
 * not substitutes claimed to reproduce the original curve algorithms. */
static union { double align[2]; u32 words[512]; } memory;
static unsigned long checks;
static unsigned fixture_index, event_count, callback_calls;
static u32 actual_base, mode, events[128];
#define PTR(a) ((u8 *)memory.words + ((a)-0x20000U))
#define FIELD(a,type) (*(type *)PTR(a))

static u32 bits(float f) { union { float f; u32 u; } v;v.f=f;return v.u; }
static float value(u32 u) { union { float f; u32 u; } v;v.u=u;return v.f; }
static u32 physical(u32 u)
{ return u>=0x20000U && u<0x20800U ? actual_base+u-0x20000U : u; }
static u32 canonical(u32 u)
{ return u>=actual_base && u-actual_base<sizeof(memory.words) ? 0x20000U+u-actual_base : u; }
static void equal(u32 actual,u32 expected,const char *what,u32 at)
{
    ++checks;
    if (actual!=expected) {
        fprintf(stderr,"curve fixture%u %s +%x: %08x != %08x\n",fixture_index,what,at,actual,expected);
        exit(1);
    }
}
static void event(u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h)
{
    const u32 input[8]={a,b,c,d,e,f,g,h};unsigned i;
    if (event_count+8>sizeof(events)/sizeof(events[0])) { fprintf(stderr,"curve event bound\n");exit(1); }
    for(i=0;i<8;++i) events[event_count++]=input[i];
}
static void curve_callback(u32 column,const void *first,const void *second,void *output,float fraction)
{
    const float *p=first;float *out=output;
    u32 a=bits(p[0]),b=bits(p[1]),c=bits(p[2]);
    event(9+column,canonical((u32)first),canonical((u32)second),canonical((u32)output),bits(fraction),a,b,c);
    out[0]=fraction;out[1]=value(a);out[2]=value(b);
    if(mode==5) FIELD(0x20308U,u16)=55;
}
static void curve0(const void *a,const void *b,void *c,float d) { curve_callback(0,a,b,c,d); }
static void curve1(const void *a,const void *b,void *c,float d) { curve_callback(1,a,b,c,d); }
static void curve2(const void *a,const void *b,void *c,float d) { curve_callback(2,a,b,c,d); }
GeorgePathCurveCallback func_002C24B8(u32 kind) { event(0,kind,0,0,0,0,0,0);return curve0; }
GeorgePathCurveCallback func_002C24D8(u32 kind) { event(1,kind,0,0,0,0,0,0);return curve1; }
GeorgePathCurveCallback func_002C24F8(u32 kind) { event(2,kind,0,0,0,0,0,0);return curve2; }
static void projection(const GeorgeMathVec3 *reference,const void *first,const void *second,float *distance,float *position)
{
    float left=*(const float *)first,right=*(const float *)second;
    event(12,canonical((u32)first),canonical((u32)second),bits(left),bits(right),bits(reference->x),bits(reference->y),bits(reference->z));
    ++callback_calls;
    if(mode!=4) {
        *distance=mode==3 ? 100000000.0f : 10.0f-(float)callback_calls;
        *position=(mode==1 || mode==5 || mode==6) ? left : mode==2 ? right : (left+right)*0.5f;
    }
    if(mode==5) FIELD(0x20280U,float)=-8.0f;
    if(mode==6) FIELD(0x20081U,u8)=0;
}
GeorgePathProjectionCallback func_002C2518(u32 kind) { event(3,kind,0,0,0,0,0,0);return projection; }
float func_0037B238(float left,float right)
{
    event(4,bits(left),bits(right),0,0,0,0,0);
    if(mode==7) FIELD(0x20082U,u8)=0x20;
    return (float)fmod((double)left,(double)right);
}
u32 func_0029DD60(const GeorgeMathVec4 *query,const GeorgePathRay *ray)
{
    (void)query;
    event(5,bits(ray->field00.x),bits(ray->field00.y),bits(ray->field00.z),
            bits(ray->field0C.x),bits(ray->field0C.y),bits(ray->field0C.z),bits(ray->field18));
    ++callback_calls;
    if(mode==5) FIELD(0x20088U,float)=33.0f;
    return mode!=3 && (mode!=2 || callback_calls==2);
}
float func_0029CF28(const GeorgeMathVec3 *first,const GeorgeMathVec3 *second,const GeorgeMathVec3 *query,float *fraction)
{
    (void)second;
    event(6,bits(first->x),bits(first->y),bits(first->z),bits(query->x),bits(query->y),bits(query->z),mode);
    *fraction=0.25f;
    if(mode==6) FIELD(0x20088U,float)=44.0f;
    return 3.5f;
}
float func_0029C230(float cosine)
{
    event(7,bits(cosine),0,0,0,0,0,0);
    return cosine==1.0f ? 0.0f : cosine==-1.0f ? value(0x40490FDBU) : 1.0f;
}
float func_0029C090(float angle)
{ event(8,bits(angle),0,0,0,0,0,0);return angle==1.0f ? 1.0f : 0.5f; }
float func_0029B940(float y,float x)
{ (void)y;(void)x;fprintf(stderr,"unowned atan call\n");exit(1);return 0; }
float func_0029C168(float angle)
{ (void)angle;fprintf(stderr,"unowned cosine call\n");exit(1);return 0; }

static u32 invoke(u32 function,const u32 *a,float time)
{
    switch(function) {
    case 0:return func_00295EB8((void *)a[0],a[1],(float *)a[2],(float *)a[3],(void **)a[4],(void **)a[5]);
    case 1:return func_00296560((void *)a[0],(GeorgeMathVec4 *)a[1],(float *)a[2],(float *)a[3]);
    case 2:return func_002966F0((void *)a[0],(GeorgeMathVec3 *)a[1],(u16 *)a[2],(s32)a[3],(float *)a[4],(float *)a[5]);
    case 3:return bits(func_00297178((void *)a[0],(u16 *)a[1],(float *)a[2],(GeorgeMathVec3 *)a[3]));
    case 4:return bits(func_00297208((void *)a[0],(u16 *)a[1],(void *)a[2],time));
    case 5:return bits(func_00297290((void *)a[0],(u16 *)a[1],(void *)a[2],time));
    case 6:return bits(func_00297318((void *)a[0],(u16 *)a[1],(GeorgeMathVec3 *)a[2],time));
    default:fprintf(stderr,"unowned curve entry\n");exit(1);return 0;
    }
}
int main(void)
{
    unsigned i;
    actual_base=(u32)memory.words;
    if(sizeof(void *)!=4 || actual_base>=0x80000000U || actual_base+sizeof(memory.words)>=0x80000000U) {
        fprintf(stderr,"curve harness requires complete low32 storage\n");return 1;
    }
    for(fixture_index=0;fixture_index<sizeof(curve_golden)/sizeof(curve_golden[0]);++fixture_index) {
        const struct CurveGolden *g=&curve_golden[fixture_index];u32 a[6],result;
        for(i=0;i<512;++i) memory.words[i]=physical(g->initial[i]);
        for(i=0;i<6;++i) a[i]=physical(g->args[i]);
        mode=g->mode;callback_calls=event_count=0;
        result=invoke(g->function,a,value(g->time));
        equal(result,g->result,"result",0);equal(event_count,g->event_count,"event count",0);
        for(i=0;i<event_count;++i) equal(events[i],g->events[i],"event",i);
        for(i=0;i<512;++i) equal(canonical(memory.words[i]),g->expected[i],"memory",i*4);
    }
    printf("path_curves: %lu checks passed\n",checks);
    return 0;
}
#undef FIELD
#undef PTR
