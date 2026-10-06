#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/rotation.h"

static unsigned checks, failures, sine_calls, cosine_calls, atan_calls, acos_calls;
static float sine_results[16], cosine_results[16], sine_arguments[16], cosine_arguments[16];
static float atan_arguments[4][2], atan_result = 4, acos_argument;
static int mutation, axis_mode;
static GeorgeMathVec4 *watch, *mutate_left, *mutate_right;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n",__LINE__); } } while (0)
static u32 bits(float value) { union { float scalar; u32 word; } v; v.scalar=value; return v.word; }
static void close_value(float actual,float expected) { CHECK(fabsf(actual-expected)<0.000002f*(1+fabsf(expected))); }

float func_0029C090(float angle)
{
    unsigned index = sine_calls++;
    CHECK(index < 16);
    sine_arguments[index % 16] = angle;
    if (axis_mode) {
        if (axis_mode == 1) { CHECK(watch->x==0 && watch->y==0 && watch->z==9); watch->x=77; }
        if (axis_mode == 2) { CHECK(watch->x==0 && watch->y==9 && watch->z==9); watch->z=77; }
        if (axis_mode == 3) { CHECK(watch->x==9 && watch->y==9 && watch->z==9); watch->y=77; }
    }
    if (mutation == 1 && index == 2) {
        GeorgeMathVec4 left = {7,11,13,17}, right = {19,23,29,31};
        *mutate_left=left; *mutate_right=right;
    }
    if (mutation == 2) { GeorgeMathVec4 value={3,4,5,100}; *mutate_left=value; }
    return sine_results[index % 16];
}

float func_0029C168(float angle)
{
    unsigned index = cosine_calls++;
    CHECK(index < 16);
    cosine_arguments[index % 16] = angle;
    if (axis_mode == 1) CHECK(watch->x==77 && watch->y==0 && watch->z==2);
    if (axis_mode == 2) CHECK(watch->x==0 && watch->y==2 && watch->z==0);
    if (axis_mode == 3) CHECK(watch->x==2 && watch->y==0 && watch->z==0);
    if (mutation == 2) CHECK(watch->x==6 && watch->y==8 && watch->z==10);
    if (mutation == 3) { GeorgeMathVec4 value={0,0,0,2}; *mutate_left=value; }
    return cosine_results[index % 16];
}

float func_0029B940(float y,float x)
{
    unsigned index=atan_calls++;
    CHECK(index<4);
    atan_arguments[index%4][0]=y; atan_arguments[index%4][1]=x;
    if (mutation == 3 && index == 0) {
        GeorgeMathVec4 current={3,4,0,2}, next={2,3,0,5};
        *mutate_left=current; *mutate_right=next;
    }
    return atan_result;
}

float func_0029C230(float cosine) { ++acos_calls; acos_argument=cosine; return 2; }

static void reset(void)
{
    unsigned index;
    sine_calls=cosine_calls=atan_calls=acos_calls=0;
    mutation=axis_mode=0; watch=mutate_left=mutate_right=NULL;
    for(index=0;index<16;++index) { sine_results[index]=2; cosine_results[index]=3; }
}

#include "rotation_golden.h"

static void original_alias_fixtures(void)
{
    unsigned index, component;
    for(index=0;index<sizeof(rotation_golden)/sizeof(rotation_golden[0]);++index) {
        const struct RotationGolden *fixture=rotation_golden+index;
        float data[32];
        memcpy(data,fixture->initial,sizeof data);
        reset();
        if(fixture->routine==0)
            func_002A2658((GeorgeRotationMatrix *)(data+fixture->output),(GeorgeMathVec3 *)(data+fixture->left));
        else if(fixture->routine==1)
            func_002A26C8((GeorgeMathVec4 *)(data+fixture->output),(GeorgeMathVec4 *)(data+fixture->left),(GeorgeMathVec4 *)(data+fixture->right));
        else
            func_002A2F68((GeorgeMathVec4 *)(data+fixture->output),(GeorgeMathVec4 *)(data+fixture->left),(GeorgeMathVec4 *)(data+fixture->right),0.25f);
        for(component=0;component<32;++component) CHECK(bits(data[component])==fixture->expected[component]);
        CHECK(sine_calls==0 && cosine_calls==0 && atan_calls==0 && acos_calls==0);
    }
}

static void products_and_rotation(void)
{
    GeorgeMathVec4 value={1,2,3,4}, identity={0,0,0,1}, result;
    GeorgeMathVec4 q={0,0,0.707106769084930419921875f,0.707106769084930419921875f};
    GeorgeMathVec3 point={1,0,0}, rotated;
    float alias[5]={1,0,0,91,92};
    reset(); func_002A26C8(&result,&identity,&value);
    CHECK(memcmp(&result,&value,sizeof value)==0);
    func_002A26C8(&result,&value,&identity);
    CHECK(memcmp(&result,&value,sizeof value)==0);
    func_002A2C48(&q,&point,&rotated);
    close_value(rotated.x,0); close_value(rotated.y,1); close_value(rotated.z,0);
    func_002A2C48(&q,(GeorgeMathVec3 *)alias,(GeorgeMathVec3 *)(alias+1));
    CHECK(alias[0]==1 && alias[4]==92);
    close_value(alias[1],0); close_value(alias[2],1); close_value(alias[3],0);
}

static void spherical(void)
{
    GeorgeMathVec4 left={1,0,0,0},right={0,1,0,0},out;
    reset(); sine_results[0]=4; sine_results[1]=8; sine_results[2]=12;
    func_002A2780(&out,&left,&right,0.25f);
    CHECK(acos_calls==1 && acos_argument==0 && sine_calls==3);
    CHECK(sine_arguments[0]==2 && sine_arguments[1]==1.5f && sine_arguments[2]==0.5f);
    CHECK(out.x==2 && out.y==3 && out.z==0 && out.w==0);
    reset(); sine_results[0]=4; sine_results[1]=8; sine_results[2]=12;
    mutation=1; mutate_left=&left; mutate_right=&right;
    func_002A2780(&out,&left,&right,0.25f);
    CHECK(out.x==71 && out.y==91 && out.z==113 && out.w==127);
    left.x=1; left.y=2; left.z=3; left.w=4;
    right.x=-1; right.y=-2; right.z=-3; right.w=-4;
    reset(); sine_results[0]=2; sine_results[1]=3;
    func_002A2780(&out,&left,&right,0.25f);
    CHECK(acos_calls==0 && sine_calls==2 && out.x==-4 && out.y==7 && out.z==-6 && out.w==3);
    close_value(sine_arguments[0],1.1780972778797149658203125f);
    close_value(sine_arguments[1],0.3926990926265716552734375f);
    reset(); sine_results[0]=2; sine_results[1]=3;
    func_002A2780(&left,&left,&right,0.25f);
    CHECK(left.x==-4 && left.y==-8 && left.z==-6 && left.w==-6);
    left.x=1; left.y=left.z=left.w=0; right=left;
    reset(); func_002A2780(&out,&left,&right,0.75f);
    CHECK(acos_calls==0 && sine_calls==0 && out.x==1 && out.w==0);
    right.x=-1; reset(); func_002A2780(&out,&left,&right,0.5f);
    CHECK(acos_calls==0 && sine_calls==2);
}

static void conversions(void)
{
    GeorgeMathVec4 out={9,9,9,9},axis={1,2,3,2};
    void (*methods[3])(GeorgeMathVec4 *,float)={func_002A2D70,func_002A2DC8,func_002A2E20};
    unsigned index;
    for(index=0;index<3;++index) {
        out.x=out.y=out.z=out.w=9;
        reset(); axis_mode=(int)index+1; watch=&out;
        methods[index](&out,2);
        CHECK(sine_calls==1 && cosine_calls==1 && sine_arguments[0]==1 && cosine_arguments[0]==1 && out.w==3);
    }
    reset(); mutation=2; mutate_left=&axis; watch=&out;
    func_002A2CF0(&out,&axis);
    CHECK(out.x==6 && out.y==8 && out.z==10 && out.w==3);
    CHECK(sine_arguments[0]==1 && cosine_arguments[0]==1);
    reset(); cosine_results[0]=2;cosine_results[1]=3;cosine_results[2]=5;
    sine_results[0]=7;sine_results[1]=11;sine_results[2]=13;
    func_002A2E78(&out,4,6,10);
    CHECK(sine_calls==3 && cosine_calls==3);
    CHECK(sine_arguments[0]==2 && sine_arguments[1]==3 && sine_arguments[2]==5);
    CHECK(cosine_arguments[0]==2 && cosine_arguments[1]==3 && cosine_arguments[2]==5);
    CHECK(out.x==-307 && out.y==303 && out.z==27 && out.w==1031);
    reset(); sine_results[0]=sine_results[1]=sine_results[2]=0;
    cosine_results[0]=cosine_results[1]=cosine_results[2]=1;
    func_002A2E78(&out,0,0,0);
    CHECK(out.x==0 && out.y==1 && out.z==0 && out.w==1);
}

static void connected_curves(void)
{
    GeorgeMathVec4 previous={0,0,0,1},current={0,0,0,1},next={0,0,0,1},out;
    GeorgeMathVec4 first={2,0,0,0},second={3,0,0,0};
    reset(); cosine_results[0]=0.625f;
    func_002A2990(&out,&previous,&current,&next);
    CHECK(atan_calls==2 && sine_calls==0 && cosine_calls==1);
    CHECK(atan_arguments[0][0]==0 && atan_arguments[0][1]==1 && atan_arguments[1][0]==0 && atan_arguments[1][1]==1);
    CHECK(out.x==0 && out.y==0 && out.z==0 && out.w==0.625f);
    reset(); mutation=3; mutate_left=&current; mutate_right=&next;
    cosine_results[0]=0.625f;
    func_002A2990(&out,&previous,&current,&next);
    CHECK(atan_calls==2 && sine_calls==1 && cosine_calls==1);
    CHECK(atan_arguments[0][0]==0 && atan_arguments[0][1]==1);
    close_value(atan_arguments[1][0],sqrtf(180)); CHECK(atan_arguments[1][1]==10);
    close_value(sine_arguments[0],1); close_value(cosine_arguments[0],1);
    close_value(out.x,4/sqrtf(5)); close_value(out.y,8/sqrtf(5));
    CHECK(out.z==0 && out.w==1.25f);
    reset(); func_002A2FE0(&out,&first,&second,&second,&first,0.25f);
    CHECK(sine_calls==0 && cosine_calls==0 && acos_calls==0 && out.x==2.375f && out.y==0 && out.z==0 && out.w==0);
}

int main(void)
{
    original_alias_fixtures(); products_and_rotation(); spherical(); conversions(); connected_curves();
    printf("rotation: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
