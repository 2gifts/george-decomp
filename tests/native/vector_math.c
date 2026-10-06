#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/vector_math.h"

static unsigned checks, failures, sine_calls, cosine_calls, acos_calls, atan_calls;
static float sine_results[3], sine_arguments[3], cosine_argument, acos_argument;
static float acos_result, atan_result, atan_y, atan_x;
static GeorgeMathVec3 *mutation_current, *mutation_target;
static int mutate_sine, mutate_cosine, mutate_atan;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)
static u32 bits(float value) { union { float scalar; u32 word; } v; v.scalar=value; return v.word; }
static float scalar(u32 value) { union { float scalar; u32 word; } v; v.word=value; return v.scalar; }
static void close_value(float actual, float expected) { CHECK(fabsf(actual-expected)<0.000001f); }

float func_0029B940(float y, float x)
{
    ++atan_calls; atan_y=y; atan_x=x;
    if (mutate_atan) { mutation_current->x=6.0f; mutation_current->y=8.0f; mutation_current->z=10.0f; }
    return atan_result;
}
float func_0029C090(float angle)
{
    unsigned index=sine_calls++;
    CHECK(index<3); sine_arguments[index<3?index:2]=angle;
    if (mutate_sine && index==2) {
        mutation_current->x=7.0f; mutation_current->y=11.0f; mutation_current->z=13.0f;
        mutation_target->x=17.0f; mutation_target->y=19.0f; mutation_target->z=23.0f;
    }
    return sine_results[index<3?index:2];
}
float func_0029C168(float angle)
{
    ++cosine_calls; cosine_argument=angle;
    if (mutate_cosine) mutation_current->x=99.0f;
    return 0.625f;
}
float func_0029C230(float cosine) { ++acos_calls; acos_argument=cosine; return acos_result; }

static void reset(void)
{
    sine_calls=cosine_calls=acos_calls=atan_calls=0;
    sine_results[0]=4.0f; sine_results[1]=8.0f; sine_results[2]=12.0f;
    acos_result=2.0f; atan_result=4.0f;
    mutate_sine=mutate_cosine=mutate_atan=0;
    mutation_current=mutation_target=NULL;
}

static void normalization(void)
{
    GeorgeMathVec3 value, output; float data[5], length;
    unsigned i;
    const float samples[][3]={{3,4,0},{0,0,-5},{-2,0,0},{1,2,2},{0,0,0}};
    for (i=0;i<5;++i) {
        value.x=samples[i][0]; value.y=samples[i][1]; value.z=samples[i][2];
        length=func_002A3538(&value);
        CHECK(length==sqrtf((samples[i][0]*samples[i][0]+samples[i][1]*samples[i][1])+samples[i][2]*samples[i][2]));
        if (length==0) CHECK(value.x==1 && bits(value.y)==0 && bits(value.z)==0);
        else { close_value(value.x,samples[i][0]/length); close_value(value.y,samples[i][1]/length); close_value(value.z,samples[i][2]/length); }
    }
    value.x=value.y=value.z=scalar(0x80000000u);
    CHECK(bits(func_002A3538(&value))==0 && value.x==1 && bits(value.y)==0 && bits(value.z)==0);
    value.x=3;value.y=4;value.z=0;
    CHECK(func_002A35C0(&output,&value,10)==5 && output.x==6 && output.y==8 && output.z==0);
    CHECK(func_002A35C0(&value,&value,-10)==5 && value.x==-6 && value.y==-8 && bits(value.z)==0x80000000u);
    data[0]=3;data[1]=4;data[2]=0;data[3]=9;data[4]=123;
    CHECK(func_002A35C0((GeorgeMathVec3 *)(data+1),(GeorgeMathVec3 *)data,10)==5);
    CHECK(data[0]==3 && data[1]==6 && data[2]==12 && data[3]==24 && data[4]==123);
    value.x=value.y=value.z=0;
    CHECK(func_002A35C0(&output,&value,scalar(0xFFC12345u))==0);
    CHECK(bits(output.x)==0xFFC12345u && bits(output.y)==0 && bits(output.z)==0);
}

static void spherical(void)
{
    GeorgeMathVec3 left={1,2,3},right={4,5,6},output;
    float data[4]; unsigned i;
    reset(); func_002A3390(&output,&left,&right,0.25f);
    CHECK(acos_calls==1 && acos_argument==1 && sine_calls==3);
    CHECK(sine_arguments[0]==2 && sine_arguments[1]==1.5f && sine_arguments[2]==0.5f);
    CHECK(output.x==14 && output.y==19 && output.z==24);
    left.x=-2;left.y=left.z=0;right.x=1;right.y=right.z=0;
    reset(); func_002A3390(&output,&left,&right,0.25f); CHECK(acos_argument==-1);
    left.x=scalar(0x7FC12345u); reset(); acos_result=0;
    func_002A3390(&output,&left,&right,0.25f); CHECK(acos_argument!=acos_argument && sine_calls==0);
    for(i=0;i<2;++i) {
        data[0]=1;data[1]=2;data[2]=3;data[3]=9;
        reset(); acos_result=i?3.1415927410125732421875f:0;
        func_002A3390((GeorgeMathVec3 *)(data+1),(GeorgeMathVec3 *)data,&right,0.75f);
        CHECK(sine_calls==0 && data[0]==1 && data[1]==1 && data[2]==1 && data[3]==1);
    }
    left.x=1;left.y=2;left.z=3;right.x=4;right.y=5;right.z=6;
    reset(); mutate_sine=1;mutation_current=&left;mutation_target=&right;
    func_002A3390(&output,&left,&right,0.25f);
    CHECK(output.x==65 && output.y==79 && output.z==95 && sine_calls==3);
    data[0]=1;data[1]=2;data[2]=3;data[3]=9;
    right.x=4;right.y=5;right.z=6;reset();
    func_002A3390((GeorgeMathVec3 *)(data+1),(GeorgeMathVec3 *)data,&right,0.25f);
    CHECK(data[0]==1 && data[1]==14 && data[2]==19 && data[3]==24);
}

static void quaternion_operations(void)
{
    GeorgeMathVec4 left={3,4,0,2},right={2,3,4,5},output,reference;
    float data[5]; unsigned i;
    reset();func_002A3080(&output,&left);
    CHECK(atan_calls==1 && atan_y==5 && atan_x==2 && bits(output.w)==0);
    close_value(output.x,2.4f);close_value(output.y,3.2f);CHECK(output.z==0);
    reset();mutate_atan=1;mutation_current=(GeorgeMathVec3 *)&left;
    func_002A3080(&output,&left);close_value(output.x,4.8f);close_value(output.y,6.4f);close_value(output.z,8);
    left.x=3;left.y=4;left.z=0;left.w=2;
    reset();func_002A3138(&output,&left,&right);
    close_value(atan_y,sqrtf(180));CHECK(atan_x==10 && output.w==0);
    close_value(output.x,-24/sqrtf(180));close_value(output.y,-48/sqrtf(180));CHECK(bits(output.z)==0x80000000u);
    reset();data[0]=3;data[1]=4;data[2]=0;data[3]=2;data[4]=123;
    func_002A3080((GeorgeMathVec4 *)(data+1),(GeorgeMathVec4 *)data);
    close_value(data[1],2.4f);close_value(data[2],1.92f);close_value(data[3],1.536f);CHECK(bits(data[4])==0);
    for(i=0;i<3;++i) {
        left.x=i==0?0:i==1?0.0000999999974737875163555145263671875f:0.25f;left.y=left.z=0;
        reset();func_002A3220(&output,(GeorgeMathVec3 *)&left);
        CHECK(sine_calls==(i==2) && cosine_calls==1 && output.w==0.625f);
        CHECK(cosine_argument==left.x);close_value(output.x,i==2?4:left.x);
    }
    left.x=3;left.y=4;left.z=0;reset();mutate_cosine=1;mutation_current=(GeorgeMathVec3 *)&output;
    func_002A3220(&output,(GeorgeMathVec3 *)&left);
    CHECK(sine_calls==1 && output.x==99 && output.w==0.625f);
    left.x=1;left.y=-2;left.z=3;left.w=-4;reference=left;
    func_002A32E0(&left,&reference);CHECK(left.x==1 && left.y==-2 && left.z==3 && left.w==-4);
    reference.x=-1;reference.y=2;reference.z=-3;reference.w=4;
    func_002A32E0(&left,&reference);CHECK(left.x==-1 && left.y==2 && left.z==-3 && left.w==4);
    reference.x=scalar(0x7FC00000u);output=left;
    func_002A32E0(&left,&reference);CHECK(memcmp(&left,&output,sizeof left)==0);
}

int main(void)
{
    normalization();spherical();quaternion_operations();
    printf("vector_math: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
