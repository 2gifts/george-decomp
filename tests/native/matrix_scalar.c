#include <stdio.h>
#include <string.h>
#include "george/matrix_scalar.h"

static unsigned checks, failures, sine_count, cosine_count, call_count;
static char call_kinds[8];
static float sine_results[4], cosine_results[4], sine_arguments[4], cosine_arguments[4];
static int mutate;
static GeorgeRotationMatrix *watch;
static GeorgeMathVec4 *change_axis;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)
static u32 bits(float value) { union { float scalar; u32 word; } v; v.scalar=value; return v.word; }

float func_0029C168(float angle)
{
    unsigned index=cosine_count++, i;
    CHECK(index<4 && call_count<8);
    call_kinds[call_count++ % 8]='C';
    cosine_arguments[index % 4]=angle;
    if (watch) for(i=0;i<16;++i) CHECK(watch->element[i]==9.0f);
    if(mutate) change_axis->w=21.0f;
    return cosine_results[index % 4];
}

float func_0029C090(float angle)
{
    unsigned index=sine_count++, i;
    CHECK(index<4 && call_count<8);
    call_kinds[call_count++ % 8]='S';
    sine_arguments[index % 4]=angle;
    if(watch) for(i=0;i<16;++i) CHECK(watch->element[i]==9.0f);
    if(mutate) { GeorgeMathVec4 changed={2,3,4,99}; *change_axis=changed; }
    return sine_results[index % 4];
}

static void reset(void)
{
    unsigned i;
    sine_count=cosine_count=call_count=0;
    watch=NULL; change_axis=NULL; mutate=0;
    memset(call_kinds,0,sizeof(call_kinds));
    for(i=0;i<4;++i) { sine_results[i]=2; cosine_results[i]=3; }
}

#include "matrix_scalar_golden.h"
static void golden_cases(void)
{
    unsigned c,i;
    for(c=0;c<sizeof(matrix_golden)/sizeof(matrix_golden[0]);++c) {
        const struct MatrixGolden *g=&matrix_golden[c];
        union { float values[48]; u32 words[48]; } buffer;
        memcpy(buffer.words,g->initial,sizeof(buffer.words));
        if(g->routine==0) func_002A1CE0((GeorgeRotationMatrix *)(buffer.values+g->matrix),
            (GeorgeMathVec3 *)(buffer.values+g->input),(GeorgeMathVec3 *)(buffer.values+g->output));
        if(g->routine==1) func_002A2278((GeorgeRotationMatrix *)(buffer.values+g->output),0.25f,-0.5f,2.0f);
        if(g->routine==2) func_002A2330((GeorgeRotationMatrix *)(buffer.values+g->output),
            (GeorgeRotationMatrix *)(buffer.values+g->matrix));
        for(i=0;i<48;++i) CHECK(buffer.words[i]==g->expected[i]);
    }
}

static void callback_cases(void)
{
    unsigned i;
    GeorgeRotationMatrix output;
    GeorgeMathVec4 axis={7,8,9,10};
    const float expected[16]={2.5f,4,3.25f,0,2,5,6.5f,0,4.75f,5.5f,8.5f,0,0,0,0,1};
    reset();
    for(i=0;i<16;++i) output.element[i]=9;
    watch=&output;change_axis=&axis;mutate=1;
    cosine_results[0]=0.5f;sine_results[0]=0.25f;
    func_002A1FD8(&output,&axis);
    CHECK(cosine_arguments[0]==10 && sine_arguments[0]==21);
    CHECK(call_count==2 && call_kinds[0]=='C' && call_kinds[1]=='S');
    for(i=0;i<16;++i) CHECK(output.element[i]==expected[i]);
    reset();
    for(i=0;i<16;++i) output.element[i]=9;
    output.element[2]=1;output.element[3]=2;output.element[4]=3;output.element[5]=4;
    func_002A1FD8(&output,(GeorgeMathVec4 *)(output.element+2));
    CHECK(cosine_arguments[0]==4 && sine_arguments[0]==4);
    CHECK(output.element[0]==1 && output.element[1]==2 && output.element[2]==-10);
    CHECK(output.element[4]==-10 && output.element[5]==-5 && output.element[6]==-10);
    CHECK(output.element[8]==-2 && output.element[9]==-14 && output.element[10]==-15);
    reset(); axis.x=axis.y=axis.z=0;axis.w=0;
    func_002A20D8(&output,&axis,(GeorgeMathVec3 *)(output.element+13));
    CHECK(output.element[12]==0 && output.element[13]==0 && output.element[14]==1);
    CHECK(output.element[15]==1);
}

static void builders(void)
{
    unsigned i,which;
    GeorgeRotationMatrix output;
    const float expected[3][16]={
        {3,2,0,0,-2,3,0,0,0,0,1,0,0,0,0,1},
        {3,0,-2,0,0,1,0,0,2,0,3,0,0,0,0,1},
        {1,0,0,0,0,3,2,0,0,-2,3,0,0,0,0,1}};
    for(which=0;which<3;++which) {
        reset();
        for(i=0;i<16;++i) output.element[i]=9;
        watch=&output;
        if(which==0) func_002A2370(&output,7);
        if(which==1) func_002A2400(&output,7);
        if(which==2) func_002A2490(&output,7);
        CHECK(cosine_count==1 && sine_count==1);
        CHECK(cosine_arguments[0]==7 && sine_arguments[0]==7);
        CHECK(call_kinds[0]=='C' && call_kinds[1]=='S');
        for(i=0;i<16;++i) CHECK(bits(output.element[i])==bits(expected[which][i]));
    }
    reset();
    for(i=0;i<16;++i) output.element[i]=9;
    watch=&output;
    cosine_results[0]=2;cosine_results[1]=3;cosine_results[2]=5;
    sine_results[0]=7;sine_results[1]=11;sine_results[2]=13;
    func_002A2520(&output,17,19,23);
    CHECK(call_count==6 && memcmp(call_kinds,"CSCSCS",6)==0);
    CHECK(cosine_arguments[0]==17 && sine_arguments[0]==17);
    CHECK(cosine_arguments[1]==19 && sine_arguments[1]==19);
    CHECK(cosine_arguments[2]==23 && sine_arguments[2]==23);
    {
        const float euler[16]={15,39,-11,0,359,1011,21,0,201,251,6,0,0,0,0,1};
        for(i=0;i<16;++i) CHECK(output.element[i]==euler[i]);
    }
    reset(); for(i=0;i<4;++i) { cosine_results[i]=1;sine_results[i]=0; }
    func_002A2520(&output,0,0,0);
    for(i=0;i<16;++i) CHECK(output.element[i]==((i==0||i==5||i==10||i==15)?1.0f:0.0f));
}

int main(void)
{
    golden_cases(); callback_cases(); builders();
    printf("matrix_scalar: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
