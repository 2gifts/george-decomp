#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/camera_basis.h"
#include "camera_basis_golden.h"

typedef unsigned long long Bits64;
static union { u32 words[64];float scalars[64]; } buffer;
static GeorgeCameraPose *pose;
static GeorgeMathVec3 *direction_pointer,*up_pointer;
static const struct CameraBasisGolden *fixture;
static u32 calls[8],events[128],event_count,trig_calls;
static unsigned checks,failures;
#define CHECK(expression) do { ++checks;if (!(expression)) { \
    if (failures<20) printf("line%u routine%u mode%u mutation%u direction%u out%u inverse%u\n", \
        __LINE__,fixture->routine,fixture->mode,fixture->mutation,fixture->direction,fixture->output,fixture->inverse); \
    ++failures; } } while (0)

static u32 word(float value)
{ union {float value;u32 bits;} result;result.value=value;return result.bits; }
static float scalar(u32 bits)
{ union {float value;u32 bits;} result;result.bits=bits;return result.value; }
static Bits64 double_bits(double value)
{ union {double value;Bits64 bits;} result;result.value=value;return result.bits; }
static double double_value(Bits64 bits)
{ union {double value;Bits64 bits;} result;result.bits=bits;return result.value; }
static int float_equal(u32 actual,u32 expected)
{
    float a=scalar(actual),e=scalar(expected);
    if (actual==expected) return 1;
    if (!isfinite(a)||!isfinite(e)||e==0.0f) return 0;
    return fabsf(a-e)<=3e-6f*(1.0f+fabsf(e));
}
static void event(u32 value)
{ CHECK(event_count<128);if (event_count<128) events[event_count++]=value; }
static void vector(GeorgeMathVec3 *value,float x,float y,float z)
{ value->x=x;value->y=y;value->z=z; }

float func_002A3538(GeorgeMathVec3 *value)
{
    float first,second,square,length,inverse;
    u32 n=++calls[0];
    event(0);event(word(value->x));event(word(value->y));event(word(value->z));
    first=value->x*value->x;second=value->y*value->y;
    square=first+second;second=value->z*value->z;square=square+second;
    length=sqrtf(square);
    if (length==0.0f) vector(value,1,0,0);
    else {
        inverse=1.0f/length;
        value->x=value->x*inverse;value->y=value->y*inverse;value->z=value->z*inverse;
    }
    if (n==1) {
        direction_pointer=value;
        if (fixture->direction>=3&&fixture->direction<=5)
            value->y=scalar(0x3F7D70A3u+fixture->direction-3);
    }
    if (n==2) up_pointer=value;
    if (fixture->mutation==2) {
        if (n==1) {
            pose->field00=2;vector(&pose->field10,4,5,6);pose->field2C=0.5f;
        } else if (n==2) vector(direction_pointer,0.25f,-0.5f,0.75f);
        else if (n==3) vector(up_pointer,2,-3,4);
    } else if (fixture->mutation==3&&n==4) {
        pose->field00=2;vector(&pose->field10,7,8,9);
    } else if (fixture->mutation==5) length=(float)(10+n);
    return length;
}

static float trig(float angle,u32 index)
{
    float result;
    ++calls[index];++trig_calls;event(index);event(word(angle));
    /* Controlled finite output, deliberately not an imported trig algorithm. */
    result=index==1?(float)(0.8-(double)angle*0.05):(float)(0.6+(double)angle*0.1);
    if (fixture->mutation==1) {
        if (trig_calls==1) { pose->field38=7.0f;pose->field2C=1.75f; }
        else if (trig_calls==2) pose->field34=-0.5f;
        else if (trig_calls==4) {
            pose->field30=3.0f;vector(&pose->field04,8,-9,10);pose->field00=3;
        }
    }
    return result;
}
float func_0029C168(float angle) { return trig(angle,1); }
float func_0029C090(float angle) { return trig(angle,2); }

Bits64 func_00374848(float value)
{ ++calls[3];event(3);event(word(value));return double_bits((double)value); }
static void double_event(u32 index,Bits64 first,Bits64 second)
{
    ++calls[index];event(index);event((u32)first);event((u32)(first>>32));
    event((u32)second);event((u32)(second>>32));
}
s32 func_00373250(Bits64 first,Bits64 second)
{
    double a=double_value(first),b=double_value(second);
    double_event(4,first,second);return (a>b)-(a<b);
}
Bits64 func_00372CC0(Bits64 first,Bits64 second)
{
    double a=double_value(first),b=double_value(second);
    double_event(5,first,second);return double_bits(a-b);
}
float func_0029B940(float first,float second)
{
    float product=second*0.25f,result=first+product;
    ++calls[6];event(6);event(word(first));event(word(second));
    if (fixture->mutation==4) {
        if (calls[6]==1) vector(up_pointer,0.3f,0.4f,0.5f);
        else pose->field34=-0.75f;
    }
    return result;
}
void func_002A1098(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *input)
{
    float captured[16];u32 i;
    CHECK((void *)output==(void *)((u8 *)&buffer+fixture->inverse));
    CHECK((void *)input==(void *)((u8 *)&buffer+fixture->output));
    ++calls[7];event(7);
    for (i=0;i<16;++i) { captured[i]=input->element[i];event(word(captured[i])); }
    for (i=0;i<16;++i) output->element[i]=-captured[i];
}

static void check_events(void)
{
    u32 i=0,index,count,j;
    CHECK(event_count==fixture->event_count);
    while (i<event_count&&i<fixture->event_count) {
        index=events[i];CHECK(index==fixture->events[i]);++i;
        count=index==0?3:index==1||index==2||index==3?1:index==4||index==5?4:index==6?2:16;
        CHECK(i+count<=event_count&&i+count<=fixture->event_count);
        if (i+count>event_count||i+count>fixture->event_count) return;
        if (index==4||index==5) {
            for (j=0;j<4;j+=2) {
                Bits64 a=(Bits64)events[i+j]|((Bits64)events[i+j+1]<<32);
                Bits64 e=(Bits64)fixture->events[i+j]|((Bits64)fixture->events[i+j+1]<<32);
                /* Original constants and zero operands retain all64 bits. The
                 * first converted finite direction can differ by host rounding. */
                if (e==0ULL||e==0x3FEFAE1480000000ULL) CHECK(a==e);
                else CHECK(fabs(double_value(a)-double_value(e))<=3e-6*(1+fabs(double_value(e))));
            }
        } else for (j=0;j<count;++j) CHECK(float_equal(events[i+j],fixture->events[i+j]));
        i+=count;
    }
    CHECK(i==event_count&&i==fixture->event_count);
}

int main(void)
{
    unsigned c,i;u32 result;
    pose=(GeorgeCameraPose *)((u8 *)&buffer+64);
    for (c=0;c<sizeof(camera_basis_golden)/sizeof(camera_basis_golden[0]);++c) {
        fixture=&camera_basis_golden[c];memcpy(buffer.words,fixture->initial,sizeof(buffer));
        memset(calls,0,sizeof(calls));event_count=trig_calls=0;
        direction_pointer=up_pointer=NULL;
        if (fixture->routine==0) {
            result=(u32)func_00299BE0((GeorgeCameraMotionTransform *)pose);
            CHECK(result==fixture->result);
        } else func_00299D68((GeorgeCameraMotionTransform *)pose,
                  (GeorgeRotationMatrix *)((u8 *)&buffer+fixture->inverse),
                  (GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));
        for (i=0;i<64;++i) {
            u32 e=fixture->expected[i];
            /* Untouched words, integer modes and all zero/one stores are exact. */
            if (e==fixture->initial[i]||e<=5||e==0x80000000u||e==0x3F800000u)
                CHECK(buffer.words[i]==e);
            else CHECK(float_equal(buffer.words[i],e));
        }
        for (i=0;i<8;++i) CHECK(calls[i]==fixture->calls[i]);
        check_events();
    }
    printf("camera_basis: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
