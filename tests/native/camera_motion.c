#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/camera_motion.h"
#include "camera_motion_golden.h"

static union { GeorgeCameraMotionTransform object; u32 words[16]; } transform, second_transform;
static GeorgeCameraMotion motion, allocated;
static GeorgeInputState input, second_input;
static GeorgeRotationMatrix matrix;
static GeorgeCameraMotionTransform *captured_transform;
static u32 captured_word;
static unsigned checks, failures, mutation, matrix_calls, normalize_calls, allocations, allocation_fail;
static GeorgeMathVec3 *first_vector;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures < 20) printf("failure line %u mutation %u\n",__LINE__,mutation); ++failures; } } while (0)

static float scalar(u32 value)
{ union {u32 bits;float real;} storage;storage.bits=value;return storage.real; }
static u32 word(float value)
{ union {u32 bits;float real;} storage;storage.real=value;return storage.bits; }
static void check_float(float actual,float expected)
{
    CHECK(fabsf(actual-expected) <= 0.000003f * (1.0f+fabsf(expected)));
}

void func_0029A308(GeorgeCameraMotionTransform *captured,
                  GeorgeRotationMatrix *inverse, GeorgeRotationMatrix *forward)
{
    unsigned index;
    ++matrix_calls;
    CHECK(captured == captured_transform);
    CHECK(inverse != forward);
    CHECK(captured->field00 == captured_word);
    for (index=0;index<16;++index) {
        forward->element[index]=matrix.element[index];
        inverse->element[index]=-matrix.element[index];
    }
    if (mutation == 1) motion.input=&second_input;
    if (mutation == 2) motion.transform=&second_transform.object;
}

float func_002A3538(GeorgeMathVec3 *value)
{
    float x=value->x,y=value->y,z=value->z;
    float length=sqrtf((x*x+y*y)+z*z);
    ++normalize_calls;
    if (length == 0.0f) { value->z=0;value->x=1;value->y=0; }
    else { float inverse=1.0f/length;value->z=z*inverse;value->x=x*inverse;value->y=y*inverse; }
    if (normalize_calls == 1) first_vector=value;
    if (normalize_calls == 2) {
        if (mutation == 3) motion.input=&second_input;
        if (mutation == 4) motion.transform=&second_transform.object;
        if (mutation == 5) { first_vector->x=2;first_vector->y=-3;first_vector->z=4; }
    }
    return length;
}

void *func_002AEC28(u32 size)
{
    ++allocations;
    CHECK(size == 12);
    return allocation_fail ? NULL : &allocated;
}

static void prepare(const struct CameraMotionGolden *fixture)
{
    static const float axes[2][4]={{-0.75f,0.5f,0.25f,-0.5f},{1,-1,0.75f,-0.25f}};
    unsigned index,side;
    memcpy(transform.words,fixture->initial,sizeof transform.words);
    memcpy(second_transform.words,fixture->initial,sizeof second_transform.words);
    memset(&input,0x5A,sizeof input);memset(&second_input,0x5A,sizeof second_input);
    for (side=0;side<2;++side) {
        GeorgeInputState *state=side ? &second_input : &input;
        state->held=fixture->held;
        for (index=0;index<4;++index) {
            state->axes[index/2][index%2].current=axes[side][index];
            state->axes[index/2][index%2].delta=axes[side][index];
        }
    }
    for (index=0;index<16;++index) matrix.element[index]=scalar(fixture->matrix[index]);
    motion.mode=fixture->variant;motion.transform=&transform.object;motion.input=&input;
    captured_transform=&transform.object;
    captured_word=transform.object.field00;
    mutation=fixture->mutation;matrix_calls=normalize_calls=0;first_vector=NULL;
}

static void golden_cases(void)
{
    unsigned index,repeat,part;
    for (index=0;index<sizeof camera_motion_golden/sizeof camera_motion_golden[0];++index) {
        const struct CameraMotionGolden *fixture=camera_motion_golden+index;
        for (repeat=0;repeat<2;++repeat) {
            GeorgeInputState expected_input,expected_second;
            prepare(fixture);
            memcpy(&expected_input,&input,sizeof input);memcpy(&expected_second,&second_input,sizeof second_input);
            if (repeat) func_002B5F18(&motion,scalar(fixture->elapsed));
            else if (fixture->variant) func_002B5D08(&motion,scalar(fixture->elapsed));
            else func_002B59F8(&motion,scalar(fixture->elapsed));
            for (part=0;part<16;++part) {
                if ((part>=1 && part<=3)||(part>=12 && part<=14))
                    check_float(scalar(transform.words[part]),scalar(fixture->expected[part]));
                else CHECK(transform.words[part]==fixture->expected[part]);
                CHECK(second_transform.words[part]==fixture->initial[part]);
            }
            CHECK(motion.transform==(fixture->transform_changed ? &second_transform.object : &transform.object));
            CHECK(motion.input==(fixture->input_changed ? &second_input : &input));
            CHECK(matrix_calls==fixture->matrix_calls);CHECK(normalize_calls==fixture->normalize_calls);
            CHECK(memcmp(&input,&expected_input,sizeof input)==0);
            CHECK(memcmp(&second_input,&expected_second,sizeof second_input)==0);
        }
    }
}

static void allocation_and_modes(void)
{
    unsigned index;
    for (allocation_fail=0;allocation_fail<2;++allocation_fail) {
        u8 untouched[sizeof allocated];
        memset(&allocated,0x7E,sizeof allocated);memset(untouched,0x7E,sizeof untouched);allocations=0;
        CHECK(func_002B5EA8(&transform.object,&input)==(allocation_fail ? NULL : &allocated));
        CHECK(allocations==1);
        if (allocation_fail) CHECK(memcmp(&allocated,untouched,sizeof allocated)==0);
        else { CHECK(allocated.mode==0);CHECK(allocated.transform==&transform.object);CHECK(allocated.input==&input); }
    }
    for (index=0;index<4;++index) {
        static const s32 modes[]={0,1,-1,0x12345678};
        GeorgeCameraMotion expected;
        memset(&motion,0x7E,sizeof motion);memcpy(&expected,&motion,sizeof motion);
        expected.mode=modes[index];func_002B5F10(&motion,modes[index]);
        CHECK(memcmp(&motion,&expected,sizeof motion)==0);
    }
    /* Every mode other than exactly one takes the clamped first controller. */
    for (index=0;index<3;++index) {
        static const s32 modes[]={-1,2,0x12345678};
        const struct CameraMotionGolden *fixture=&camera_motion_golden[0];
        unsigned part;
        prepare(fixture);motion.mode=modes[index];func_002B5F18(&motion,scalar(fixture->elapsed));
        for (part=0;part<16;++part) {
            if ((part>=1&&part<=3)||(part>=12&&part<=14)) check_float(scalar(transform.words[part]),scalar(fixture->expected[part]));
            else CHECK(transform.words[part]==fixture->expected[part]);
        }
        CHECK(matrix_calls==1);CHECK(normalize_calls==0);
    }
}

static void timestep_and_angle_boundaries(void)
{
    static const float times[]={-1,0,0.01f,0.016666667535901069f,0.125f};
    static const float angles[]={-20,-0.25f,-0.0f,0,6.2831854820251465f,7,20};
    unsigned time,angle;
    for (time=0;time<sizeof times/sizeof times[0];++time)
    for (angle=0;angle<sizeof angles/sizeof angles[0];++angle) {
        float elapsed=times[time] < 0.016666667535901069f ? 0.016666667535901069f : times[time];
        float expected_angle=angles[angle]+0.75f*elapsed;
        prepare(&camera_motion_golden[0]);input.held=0x00800000;
        input.axes[0][0].delta=0.5f;input.axes[0][1].delta=-0.25f;
        transform.object.field34=angles[angle];
        if (expected_angle<0) expected_angle+=6.2831854820251465f;
        else if (!(expected_angle<=6.2831854820251465f)) expected_angle-=6.2831854820251465f;
        func_002B59F8(&motion,times[time]);
        check_float(transform.object.field34,expected_angle);
        check_float(transform.object.field38,0.75f+0.375f*elapsed);
        CHECK(transform.words[12]==word(8));
        CHECK(transform.words[0]==0);CHECK(normalize_calls==0);
        check_float(transform.object.field04.x,3);check_float(transform.object.field04.y,-2);check_float(transform.object.field04.z,5);
    }
    /* Mode one does not clamp negative or zero time and does not wrap its angle. */
    for (time=0;time<3;++time) {
        float elapsed=times[time];
        prepare(&camera_motion_golden[0]);transform.object.field34=20;
        func_002B5D08(&motion,elapsed);
        check_float(transform.object.field04.x,3-0.075f*elapsed);
        check_float(transform.object.field04.z,5+0.05f*elapsed);
        check_float(transform.object.field34,20-0.5f*elapsed);
        check_float(transform.object.field38,0.75f+elapsed);
        CHECK(transform.words[12]==word(0.10000000149011612f));
    }
}

static void shifted_input_aliases(void)
{
    static const unsigned offsets[]={0x54,0x64};
    unsigned alias,variant,index;
    for (alias=0;alias<2;++alias) for (variant=0;variant<2;++variant) {
        unsigned offset=offsets[alias];
        GeorgeInputState expected;
        GeorgeCameraMotionTransform *overlap;
        float x,y,z,angle,pitch,speed,elapsed=0.125f;
        prepare(&camera_motion_golden[0]);
        overlap=(GeorgeCameraMotionTransform *)((u8 *)&input+offset);
        memcpy(overlap,transform.words,sizeof transform.words);
        /* These writes intentionally alter overlapping transform words. */
        input.held=0x00800000;
        input.axes[0][0].current=-0.75f;input.axes[0][0].delta=0.5f;
        input.axes[0][1].current=0.5f;input.axes[0][1].delta=-0.25f;
        input.axes[1][0].current=0.25f;input.axes[1][1].current=-0.5f;
        x=overlap->field04.x;y=overlap->field04.y;z=overlap->field04.z;
        angle=overlap->field34;pitch=overlap->field38;speed=overlap->field30;
        if (variant) {
            x+=-0.075f*elapsed;z+=0.05f*elapsed;
            angle+=-0.5f*elapsed;pitch+=elapsed;speed=0.10000000149011612f;
        } else {
            angle+=0.75f*elapsed;pitch+=0.375f*elapsed;
            if (angle<0) angle+=6.2831854820251465f;
            else if (!(angle<=6.2831854820251465f)) angle-=6.2831854820251465f;
        }
        memcpy(&expected,&input,sizeof input);
        memset((u8 *)&expected+offset,0,4);
        memcpy((u8 *)&expected+offset+4,&x,4);memcpy((u8 *)&expected+offset+8,&y,4);
        memcpy((u8 *)&expected+offset+12,&z,4);memcpy((u8 *)&expected+offset+0x30,&speed,4);
        memcpy((u8 *)&expected+offset+0x34,&angle,4);memcpy((u8 *)&expected+offset+0x38,&pitch,4);
        captured_transform=overlap;motion.transform=overlap;
        captured_word=overlap->field00;
        if (variant) func_002B5D08(&motion,elapsed);
        else func_002B59F8(&motion,elapsed);
        for (index=0;index<sizeof input;++index) {
            unsigned relative=index-offset;
            if (relative==4||relative==8||relative==12||relative==0x30||relative==0x34||relative==0x38) {
                u32 actual_word,expected_word;
                memcpy(&actual_word,(u8 *)&input+index,4);memcpy(&expected_word,(u8 *)&expected+index,4);
                check_float(scalar(actual_word),scalar(expected_word));index+=3;
            } else CHECK(((u8 *)&input)[index]==((u8 *)&expected)[index]);
        }
        CHECK(matrix_calls==1);CHECK(normalize_calls==(variant ? 2u : 0u));
    }
}

int main(void)
{
    golden_cases();allocation_and_modes();timestep_and_angle_boundaries();shifted_input_aliases();
    printf("camera_motion: %u checks, %u failures\n",checks,failures);
    return failures ? 1 : 0;
}
