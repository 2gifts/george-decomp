#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/camera_transform.h"
#include "camera_transform_golden.h"

static union { u32 words[64]; float scalars[64]; } buffer;
static GeorgeCameraTransform *transform;
static const struct CameraTransformGolden *fixture;
static u32 calls[7];
static unsigned checks,failures;
static float captured_angle;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures<20) printf("failure line%u routine%u mutation%u out%u inverse%u delta%u\n", \
        __LINE__,fixture->routine,fixture->mutation,fixture->output,fixture->inverse,fixture->delta); ++failures; } } while (0)

static float scalar(u32 value)
{ union {u32 bits;float real;} storage;storage.bits=value;return storage.real; }
static u32 word(float value)
{ union {u32 bits;float real;} storage;storage.real=value;return storage.bits; }

void *func_002AEC28(u32 size)
{
    ++calls[0];CHECK(size==0x68);
    return fixture->allocation_fail ? NULL : transform;
}

s32 func_00299BE0(GeorgeCameraMotionTransform *prefix)
{
    ++calls[1];CHECK(prefix==&transform->pose.prefix);
    if (fixture->routine==0||fixture->routine==1) {
        CHECK(prefix->field00==0);
        CHECK(word(prefix->field04.x)==word(3));CHECK(word(prefix->field04.y)==word(-2));
        CHECK(word(prefix->field04.z)==word(5));CHECK(word(prefix->field30)==word(1000));
        CHECK(word(transform->field3C)==word(1));CHECK(word(transform->field40)==word(4000));
        CHECK(word(transform->field44)==0x420A1062);CHECK(word(transform->field48)==0x3FAAAAA8);
        CHECK(memcmp((u8 *)transform+0x4C,(u8 *)fixture->initial+64+0x4C,0x1C)==0);
    }
    if (fixture->mutation==1) {
        transform->pose.fields.field10.x=7;transform->pose.fields.field10.y=8;transform->pose.fields.field10.z=9;
        transform->field50=transform->field64=0x12345678;
    }
    return (s32)fixture->rebuild;
}

void func_002A1E78(GeorgeRotationMatrix *output,const GeorgeMathVec4 *rotation)
{
    unsigned index;
    ++calls[2];CHECK(output==(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));
    CHECK(rotation==&transform->pose.fields.field1C);
    for (index=0;index<16;++index) output->element[index]=(float)(index+1);
}

void func_002A0E20(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *input)
{
    float values[16];unsigned index;
    ++calls[3];CHECK(output==(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->inverse));
    CHECK(input==(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));
    memcpy(values,input->element,sizeof values);
    for (index=0;index<16;++index) output->element[index]=-values[index];
}

void func_00299D68(GeorgeCameraMotionTransform *prefix,
                  GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward)
{
    unsigned index;
    ++calls[4];CHECK(prefix==&transform->pose.prefix);
    CHECK(inverse==(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->inverse));
    CHECK(forward==(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));
    for (index=0;index<16;++index) forward->element[index]=(float)(index+21);
    for (index=0;index<16;++index) inverse->element[index]=-(float)(index+21);
}

float func_0029C168(float angle)
{
    ++calls[5];captured_angle=angle;
    CHECK(fabsf(angle-34.51599884033203f*0.0087266471236944199f)<0.000001f);
    if (fixture->mutation==1) { transform->field3C=9;transform->field40=90;transform->field44=17; }
    return 1.0f;
}

float func_0029C090(float angle)
{
    ++calls[6];CHECK(calls[5]==1);CHECK(word(angle)==word(captured_angle));
    if (fixture->mutation==2) transform->field48=3;
    return 0.5f;
}

static void golden_cases(void)
{
    unsigned entry,index;
    for (entry=0;entry<sizeof camera_transform_golden/sizeof camera_transform_golden[0];++entry) {
        fixture=camera_transform_golden+entry;
        memcpy(buffer.words,fixture->initial,sizeof buffer.words);
        transform=(GeorgeCameraTransform *)((u8 *)&buffer+64);
        memset(calls,0,sizeof calls);
        switch (fixture->routine) {
        case 0: CHECK(func_0029A100(3,-2,5)==(fixture->allocation_fail ? NULL : transform));break;
        case 1: func_0029A188(transform,3,-2,5);break;
        case 2: func_0029A308(&transform->pose.prefix,
                             (GeorgeRotationMatrix *)((u8 *)&buffer+fixture->inverse),
                             (GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));break;
        case 3: func_0029A3A0(transform,(GeorgeRotationMatrix *)((u8 *)&buffer+fixture->output));break;
        case 4: func_0029A498(transform,(GeorgeMathVec3 *)((u8 *)transform+fixture->delta));break;
        default: CHECK(0);break;
        }
        for (index=0;index<64;++index) {
            unsigned relative=index-fixture->output/4;
            if (fixture->routine==3&&(relative==0||relative==5||relative==10||relative==14)) {
                float actual=scalar(buffer.words[index]),expected=scalar(fixture->expected[index]);
                CHECK(fabsf(actual-expected)<=0.000003f*(1+fabsf(expected)));
            } else CHECK(buffer.words[index]==fixture->expected[index]);
        }
        for (index=0;index<7;++index) CHECK(calls[index]==fixture->calls[index]);
    }
}

int main(void)
{
    golden_cases();
    printf("camera_transform: %u checks, %u failures\n",checks,failures);
    return failures ? 1 : 0;
}
