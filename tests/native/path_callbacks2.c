#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/path_callbacks2.h"
#include "path_callbacks2_golden.h"

/* Real selected callback and rotation sources execute as separate TUs.
 * Projection/trigonometric definitions are authored observation contracts. */
static union { double align[2]; u32 words[128]; } memory;
static unsigned long checks;
static unsigned fixture_index, event_count;
static u32 actual_base, mode, events[64];
#define PTR(a) ((u8 *)memory.words + ((a)-0x20000U))
#define FIELD(a,type) (*(type *)PTR(a))

static u32 bits(float f) { union { float f; u32 u; } v;v.f=f;return v.u; }
static float value(u32 u) { union { float f; u32 u; } v;v.u=u;return v.f; }
static u32 physical(u32 u)
{ return u>=0x20000U && u<0x20200U ? actual_base+u-0x20000U : u; }
static u32 canonical(u32 u)
{ return u>=actual_base && u-actual_base<sizeof(memory.words) ? 0x20000U+u-actual_base : u; }
static void equal(u32 actual,u32 expected,const char *what,u32 at)
{
    ++checks;
    if (actual!=expected) {
        fprintf(stderr,"callback fixture%u %s +%x: %08x != %08x\n",fixture_index,what,at,actual,expected);
        exit(1);
    }
}
static void event(u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h)
{
    const u32 input[8]={a,b,c,d,e,f,g,h};unsigned i;
    if (event_count+8>sizeof(events)/sizeof(events[0])) { fprintf(stderr,"callback event bound\n");exit(1); }
    for(i=0;i<8;++i) events[event_count++]=input[i];
}
float func_0029C230(float cosine)
{
    event(0,bits(cosine),0,0,0,0,0,0);
    if(mode==1) { unsigned j;for(j=0;j<7;++j) { FIELD(0x20044U+4*j,float)=(float)(2+(int)j);FIELD(0x200C4U+4*j,float)=(float)(3-(int)j); } }
    return cosine==1.0f ? 0.0f : cosine==-1.0f ? value(0x40490FDBU) : 1.0f;
}
float func_0029C090(float angle)
{
    event(1,bits(angle),0,0,0,0,0,0);
    if(mode==1) { unsigned j;for(j=0;j<7;++j) { FIELD(0x20044U+4*j,float)=(float)(2+(int)j);FIELD(0x200C4U+4*j,float)=(float)(3-(int)j); } }
    return angle==1.0f ? 1.0f : 0.5f;
}
float func_0029CF28(const GeorgeMathVec3 *first,const GeorgeMathVec3 *second,const GeorgeMathVec3 *query,float *fraction)
{
    event(2,bits(first->x),bits(first->y),bits(first->z),bits(second->x),bits(second->y),bits(second->z),mode);
    event(3,bits(query->x),bits(query->y),bits(query->z),canonical((u32)fraction),0,0,0);
    *fraction=0.25f;
    if(mode==2) { FIELD(0x20040U,float)=20.0f;FIELD(0x200C0U,float)=40.0f; }
    return 3.5f;
}
float func_0029B940(float y,float x)
{ (void)y;(void)x;fprintf(stderr,"unowned atan call\n");exit(1);return 0; }
float func_0029C168(float angle)
{ (void)angle;fprintf(stderr,"unowned cosine call\n");exit(1);return 0; }

static void invoke(u32 function,const u32 *a,float time)
{
    switch(function) {
    case 0:func_002C19F0((const void *)a[0],(const void *)a[1],(void *)a[2],time);break;
    case 1:func_002C1CA0((const void *)a[0],(const void *)a[1],(void *)a[2],time);break;
    case 2:func_002C1D60((const void *)a[0],(const void *)a[1],(void *)a[2],time);break;
    case 3:func_002C2180((const void *)a[0],(const void *)a[1],(void *)a[2],time);break;
    case 4:func_002C2390((const void *)a[0],(const void *)a[1],(void *)a[2],time);break;
    default:fprintf(stderr,"unowned accumulator entry\n");exit(1);
    }
}
static void endpoint_checks(void)
{
    static const float a[3]={3.0f,-2.0f,5.0f};
    static const float b[3]={10.0f,4.0f,7.0f};
    static const float c[3]={5.0f,10.0f,-1.0f};
    static const float d[3]={8.0f,3.0f,4.0f};
    unsigned i,case_index;
    u32 args[5]={0,0,0,0,0};
    args[0]=physical(0x20040U);args[1]=physical(0x200C0U);args[2]=physical(0x20140U);
    FIELD(0x20040U,float)=10.0f;FIELD(0x200C0U,float)=22.0f;
    for(i=0;i<3;++i) {
        FIELD(0x20044U+4*i,float)=a[i];FIELD(0x2005CU+4*i,float)=b[i];
        FIELD(0x200D0U+4*i,float)=c[i];FIELD(0x200C4U+4*i,float)=d[i];
        FIELD(0x200DCU+4*i,float)=d[i];
    }
    mode=0;event_count=0;
    for(case_index=0;case_index<6;++case_index) {
        unsigned function=case_index/2;float time=(float)(case_index%2);
        invoke(function,args,time);
        for(i=0;i<3;++i) {
            float expected;
            if(function==0) expected=time==0.0f ? b[i] : d[i];
            else if(function==1) expected=time==0.0f ? a[i] : d[i];
            else expected=time==0.0f ? 3.0f*(b[i]-a[i]) : 3.0f*(d[i]-c[i]);
            equal(bits(FIELD(0x20140U+4*i,float)),bits(expected),"closed endpoint",i*4);
        }
    }
}
int main(void)
{
    unsigned i;
    actual_base=(u32)memory.words;
    if(sizeof(void *)!=4 || actual_base>=0x80000000U || actual_base+sizeof(memory.words)>=0x80000000U) {
        fprintf(stderr,"callback harness requires complete low32 storage\n");return 1;
    }
    for(fixture_index=0;fixture_index<sizeof(accumulator_golden)/sizeof(accumulator_golden[0]);++fixture_index) {
        const struct AccumulatorGolden *g=&accumulator_golden[fixture_index];u32 a[5];
        for(i=0;i<128;++i) memory.words[i]=g->initial[i];
        for(i=0;i<5;++i) a[i]=physical(g->args[i]);
        mode=g->mode;event_count=0;
        invoke(g->function,a,value(g->time));
        equal(event_count,g->event_count,"event count",0);
        for(i=0;i<event_count;++i) equal(events[i],g->events[i],"event",i);
        for(i=0;i<128;++i) equal(memory.words[i],g->expected[i],"memory",i*4);
    }
    endpoint_checks();
    printf("path_callbacks2: %lu checks passed\n",checks);
    return 0;
}
#undef FIELD
#undef PTR
