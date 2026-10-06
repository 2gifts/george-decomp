#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/path_sampling.h"
#include "path_sampling_golden.h"

/* The scalar inverse runs its existing production C as a separate translation
 * unit. The VU transform and lower curve calls have authored finite contracts,
 * rather than reproducing their complete runtime or exceptional behavior. */
static union { double alignment[2]; u32 words[512]; } storage;
static unsigned long checks;
static unsigned int fixture_index;
static u32 actual_base, mode, events[64], event_count, matrix_calls;

#define PTR(address) ((u8 *)storage.words + ((address) - 0x20000U))
#define FIELD(address,type) (*(type *)PTR(address))
#define PATH_OWNER FIELD(0x2004CU, void *)
#define PATH_DATA FIELD(0x20074U, void *)
#define PATH_TIME FIELD(0x20084U, float)
#define PATH_FOUND FIELD(0x20088U, float)

static u32 bits(float value) { union { float f; u32 u; } v; v.f=value; return v.u; }
static float value(u32 word) { union { float f; u32 u; } v; v.u=word; return v.f; }
static u32 physical(u32 word)
{
    if (word>=0x20000U && word<0x20800U) return actual_base+(word-0x20000U);
    return word;
}
static u32 canonical(u32 word)
{
    if (word>=actual_base && word-actual_base<sizeof(storage.words)) return 0x20000U+(word-actual_base);
    return word;
}
static void equal(u32 actual,u32 expected,const char *what,u32 offset)
{
    ++checks;
    if (actual!=expected) {
        fprintf(stderr,"path fixture%u %s +%x: %08x != %08x\n",fixture_index,what,offset,actual,expected);
        exit(1);
    }
}
static void event(u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h)
{
    const u32 input[8]={a,b,c,d,e,f,g,h}; unsigned int i;
    if (event_count+8>sizeof(events)/sizeof(events[0])) { fprintf(stderr,"path event bound\n");exit(1); }
    for (i=0;i<8;++i) events[event_count++]=input[i];
}

void func_002A1C60(const void *input_matrix,const GeorgeMathVec3 *input,GeorgeMathVec3 *output)
{
    float m[16],p[3],r[3]; unsigned int i;
    p[0]=input->x;p[1]=input->y;p[2]=input->z;
    memcpy(m,input_matrix,sizeof(m));
    event(0,bits(p[0]),bits(p[1]),bits(p[2]),bits(m[12]),bits(m[13]),bits(m[14]),canonical((u32)PATH_OWNER));
    for (i=0;i<3;++i) {
        volatile float a=m[i]*p[0],b=m[4+i]*p[1],c=m[8+i]*p[2];
        volatile float ab=a+b,abc=ab+c;
        r[i]=abc+m[12+i];
    }
    output->z=r[2];output->x=r[0];output->y=r[1];
    ++matrix_calls;
    if (matrix_calls==1 && mode==2) PATH_DATA=PTR(0x20600U);
    if (matrix_calls==1 && mode==6) PATH_OWNER=PTR(0x20300U);
}

u32 func_002966F0(void *data,const GeorgeMathVec3 *reference,u16 *index,s32 count,float *position,float *distance)
{
    float x=reference->x,y=reference->y,z=reference->z;
    equal((u32)index,0,"projection optional index",0);
    equal((u32)count,0,"projection count",0);
    event(1,bits(x),bits(y),bits(z),canonical((u32)data),(u32)index,(u32)count,bits(PATH_TIME));
    *distance=100000000.0f;*position=0.0f;
    *position=25.0f;*distance=3.5f;
    if (mode==3) PATH_TIME=-1.0f;
    if (mode==4) { PATH_DATA=PTR(0x20600U);PATH_OWNER=PTR(0x20300U); }
    if (mode==5) { PATH_FOUND=50.0f;PATH_TIME=7.0f; }
    return mode!=7;
}

float func_00297178(void *data,u16 *index,float *position,GeorgeMathVec3 *output)
{
    float captured=*position;u16 before=*index;u8 format=*(u8 *)data;
    event(2,bits(captured),before,canonical((u32)data),canonical((u32)PATH_OWNER),format,bits(PATH_TIME),bits(PATH_FOUND));
    *index=(u16)(before+3);
    *position=captured+0.25f;
    output->x=captured;output->y=(float)((before&7)-3);output->z=format==16?3.0f:4.0f;
    if (mode==1) PATH_OWNER=PTR(0x20300U);
    return 0.125f;
}

static u32 invoke(u32 function,const u32 *args,float fraction)
{
    switch (function) {
    case 0:func_00135D10((void *)args[0],(GeorgeMathVec3 *)args[1],(const GeorgeMathVec3 *)args[2]);return 0;
    case 1:return bits(func_00135D88((void *)args[0],(const GeorgeMathVec3 *)args[1]));
    case 2:func_00135E88((void *)args[0],(GeorgeMathVec3 *)args[1],fraction);return 0;
    case 3:return func_00135F90((void *)args[0],(const GeorgeMathVec3 *)args[1],(float *)args[2],(float *)args[3]);
    case 4:func_00136020((void *)args[0],(float *)args[1],(u16 *)args[2],(GeorgeMathVec3 *)args[3]);return 0;
    default:fprintf(stderr,"unowned path fixture\n");exit(1);
    }
}

static void host_sentinel_cases(void)
{
    static const u32 current[]={0x7FC12345U,0xFFC23456U,0x7F800000U,0xFF800000U,0x80000000U,0};
    const struct PathGolden *g=0;
    unsigned int i,j;
    for (i=0;i<sizeof(path_golden)/sizeof(path_golden[0]);++i) {
        if (path_golden[i].function==2 && path_golden[i].mode==0) { g=&path_golden[i];break; }
    }
    if (g==0) { fprintf(stderr,"missing path sentinel input\n");exit(1); }
    /* Only the host equality gate is inspected for these encodings. They are
     * never used as numerical curve/matrix input; no EE FCR/special claim. */
    for (i=0;i<sizeof(current)/sizeof(current[0]);++i) {
        for (j=0;j<512;++j) storage.words[j]=physical(g->initial[j]);
        FIELD(0x20084U,u32)=current[i];PATH_FOUND=12.0f;
        mode=event_count=matrix_calls=0;
        func_00135E88(PTR(0x20040U),(GeorgeMathVec3 *)PTR(0x20090U),0.5f);
        equal(FIELD(0x20084U,u32),current[i],"host non-sentinel preserved",0);
        equal(bits(PATH_FOUND),bits(12.0f),"host found-time preserved",0);
        for (j=0;j<3;++j) equal(FIELD(0x20090U+4*j,u32),g->expected[(0x90U+4*j)/4],"finite output independent of current encoding",j);
    }
}

int main(void)
{
    unsigned int i;
    actual_base=(u32)storage.words;
    if (sizeof(void *)!=4 || actual_base>=0x80000000U || actual_base+sizeof(storage.words)>0x80000000U) {
        fprintf(stderr,"path harness needs complete low32 storage\n");return 1;
    }
    for (fixture_index=0;fixture_index<sizeof(path_golden)/sizeof(path_golden[0]);++fixture_index) {
        const struct PathGolden *g=&path_golden[fixture_index];u32 args[4],result;
        for (i=0;i<512;++i) storage.words[i]=physical(g->initial[i]);
        for (i=0;i<4;++i) args[i]=physical(g->args[i]);
        mode=g->mode;event_count=matrix_calls=0;
        result=invoke(g->function,args,value(g->fraction));
        equal(result,g->result,"result",0);
        equal(event_count,g->event_count,"event count",0);
        for (i=0;i<event_count;++i) equal(events[i],g->events[i],"event",i);
        for (i=0;i<512;++i) equal(canonical(storage.words[i]),g->expected[i],"memory",i*4);
    }
    host_sentinel_cases();
    printf("path_sampling: %lu checks passed\n",checks);
    return 0;
}

#undef PATH_FOUND
#undef PATH_TIME
#undef PATH_DATA
#undef PATH_OWNER
#undef FIELD
#undef PTR
