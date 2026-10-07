#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/plane_intersection.h"
#include "plane_intersection_golden.h"

typedef unsigned long long Bits64;
static unsigned checks, fixture, calls[4], event_count;
static u32 events[60];
static float *published[2];

static u32 bits(float value) { union { float f; u32 u; } v; v.f=value; return v.u; }
static void check(int condition,const char *message,unsigned index)
{
    ++checks;
    if(!condition) {
        fprintf(stderr,"plane fixture%u word%u: %s\n",fixture,index,message);
        exit(1);
    }
}
static void event(unsigned index,u32 a,u32 b,u32 c,u32 d,u32 e)
{
    unsigned i,j;
    check(event_count+12<=60,"call event bound",event_count);
    ++calls[index];events[event_count++]=index;
    events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
    events[event_count++]=d;events[event_count++]=e;
    for(i=0;i<2;++i) for(j=0;j<3;++j)
        events[event_count++]=published[i] ? bits(published[i][j]) : 0;
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
void *func_003936A0(void *destination,s32 fill,u32 count)
{
    check(fill==0 && count==12,"actual reviewed zero-fill lanes",0);
    event(3,0,count,0,0,0);
    return memset(destination,fill,count);
}
static void clear_calls(void)
{
    event_count=0;memset(calls,0,sizeof(calls));memset(events,0,sizeof(events));
}
static void golden_checks(void)
{
    unsigned i,j;
    for(i=0;i<sizeof(plane_golden)/sizeof(plane_golden[0]);++i) {
        const struct PlaneGolden *g=&plane_golden[i];
        union { u32 words[64];float value[64]; } memory;
        u32 result;
        fixture=i;memcpy(memory.words,g->initial,sizeof(memory.words));clear_calls();
        published[0]=memory.value+g->offsets[g->routine==0 ? 2 : 3];
        published[1]=g->routine==0 ? memory.value+g->offsets[3] : 0;
        if(g->routine==0) result=func_0029F370((GeorgeMathVec4 *)(memory.value+g->offsets[0]),
                (GeorgeMathVec4 *)(memory.value+g->offsets[1]),(GeorgeMathVec3 *)published[0],
                (GeorgeMathVec3 *)published[1]);
        else result=func_0029F630((GeorgeMathVec3 *)(memory.value+g->offsets[0]),
                (GeorgeMathVec3 *)(memory.value+g->offsets[1]),(GeorgeMathVec4 *)(memory.value+g->offsets[2]),
                (GeorgeMathVec3 *)published[0]);
        check(result==g->result,"return differs from original finite trace",64);
        for(j=0;j<64;++j) check(memory.words[j]==g->expected[j],"memory differs from original finite trace",j);
        for(j=0;j<4;++j) check(calls[j]==g->calls[j],"actual call count",j);
        check(event_count==g->event_count,"call event extent",65);
        for(j=0;j<event_count;++j) check(events[j]==g->events[j],"soft/zero-fill arguments and publication",j);
    }
}
static void independent_geometry(void)
{
    GeorgeMathVec4 a,b;
    GeorgeMathVec3 point,end,first={1,2,-3},second={1,2,5},output;
    unsigned axis;int sign,offset;
    published[0]=published[1]=0;
    for(axis=0;axis<3;++axis) for(sign=-1;sign<=1;sign+=2) for(offset=-6;offset<=6;++offset) {
        float *normal_a=(float *)&a,*normal_b=(float *)&b;
        float *p=(float *)&point,*e=(float *)&end;
        memset(&a,0,sizeof(a));memset(&b,0,sizeof(b));
        normal_a[axis]=2;normal_b[(axis+1)%3]=3*sign;a.w=offset;b.w=3;
        clear_calls();check(func_0029F370(&a,&b,&point,&end)==1,"independent orthogonal planes",0);
        check(2*p[axis]==a.w && 3*sign*p[(axis+1)%3]==b.w,"point satisfies both planes",1);
        check(2*e[axis]==a.w && 3*sign*e[(axis+1)%3]==b.w,"endpoint satisfies both planes",2);
        check(e[(axis+2)%3]-p[(axis+2)%3]==6*sign,"raw cross remains nonunit",3);
    }
    a.x=a.y=0;a.z=2;
    for(offset=-20;offset<=24;++offset) {
        a.w=offset;clear_calls();check(func_0029F630(&first,&second,&a,&output)==1,"infinite line accepted",0);
        check(output.x==1 && output.y==2 && output.z==offset/2.0f,"independent line-plane solution",1);
    }
    /* A parallel rejection leaves the complete output untouched. */
    a.x=1;a.y=a.z=0;a.w=2;output.x=9;output.y=8;output.z=7;
    clear_calls();check(func_0029F630(&first,&second,&a,&output)==0,"parallel denominator rejects",0);
    check(output.x==9 && output.y==8 && output.z==7,"rejection output untouched",1);
}
int main(void)
{
    golden_checks();independent_geometry();
    printf("plane_intersection: %u checks passed\n",checks);return 0;
}
