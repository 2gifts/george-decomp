/* Algorithm/ABI harness: target pad/math callees are controlled substitutes.
 * No original game bytes or tables are needed. EE arithmetic/cause flags are
 * not independently established by the host square-root/angle model. */
#include "george/pad_input.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

GeorgeInputState *D_00468388[4];
GeorgeInputState *D_00468398, *D_0046839C, *D_004683A0, *D_004683A4;
GeorgeInputState *D_004683A8;
GeorgeKeyState *D_004683AC;
GeorgePadDevice D_00468400[4];

static GeorgeInputState states[3][5];
static GeorgeKeyState keys;
static unsigned checks, failures;
static unsigned alloc_calls, init_calls, read_calls, delta_calls, frame_calls, motor_calls, angle_calls;
static unsigned mutation;
static int events[64], event_count;
static float *watched_magnitude;
static float magnitude_at_angle;
static struct Fixture {
    s32 kind, connection;
    u32 buttons;
    signed char axes[4];
    u8 pressure[12];
} fixture[4];
static struct Frame {
    GeorgeInputState *state;
    s32 mode;
    u32 buttons;
    float axes[4], pressure[12], elapsed, outputs[4];
    const u8 *byte_pressure;
} frame[4];
static struct Motor { int small, large; } motors[4];

#define CHECK(condition) do { ++checks; if (!(condition)) { \
    if (failures < 20) printf("line %d: %s\n", __LINE__, #condition); ++failures; } } while (0)

static float scalar(u32 bits) { union { u32 bits; float scalar; } v; v.bits=bits; return v.scalar; }
static u32 encoding(float scalar_value) { union { u32 bits; float scalar; } v; v.scalar=scalar_value; return v.bits; }
static void phase(GeorgeInputState *state, float value) { state->reservedD4 = encoding(value); }
static float phase_value(GeorgeInputState *state) { return scalar(state->reservedD4); }
static int close_float(float a, float b)
{
    if (isnan(a) || isnan(b)) return isnan(a) && isnan(b);
    if (isinf(a) || isinf(b)) return a == b;
    return fabsf(a-b) <= 0.00002f * (1.0f + fabsf(b));
}
static void event(int code) { if (event_count < 64) events[event_count++] = code; }
static int device_index(GeorgePadDevice *pad) { return (int)(pad - D_00468400); }

GeorgeInputState *func_002AC628(s32 mode, u32 device)
{
    CHECK(alloc_calls < 5);
    CHECK(mode == (alloc_calls < 4 ? 1 : 2));
    CHECK(device == alloc_calls);
    if (alloc_calls == 4) {
        CHECK(D_00468398 == D_00468388[0]);
        CHECK(D_0046839C == D_00468388[1]);
        CHECK(D_004683A0 == D_00468388[2]);
        CHECK(D_004683A4 == D_00468388[3]);
        if (mutation) D_00468388[0] = &states[1][0];
    }
    return &states[0][alloc_calls++];
}
GeorgeKeyState *func_002AC700(void)
{
    CHECK(D_004683A8 == &states[0][4]);
    CHECK(states[0][4].flags == (0xABCDEFFFU & ~0x18U));
    return &keys;
}
s32 func_002B2DA8(GeorgePadDevice *pad, s32 port, s32 slot, s32 word)
{
    CHECK(device_index(pad) == (int)init_calls);
    CHECK(port == (int)init_calls && slot == 0 && word == 0);
    CHECK(D_004683AC == &keys);
    ++init_calls;
    return -1;
}
void *func_003936A0(void *output, s32 value, u32 size)
{
    CHECK(value == 0 && (size == 16 || size == 48));
    return memset(output, value, size);
}
s32 func_002B2800(GeorgePadDevice *pad, u32 *buttons, signed char *lx,
    signed char *ly, signed char *rx, signed char *ry, u8 *pressure, s16 *delta)
{
    int i, index = device_index(pad);
    ++read_calls; event(100 + index);
    *buttons = fixture[index].buttons;
    *lx = fixture[index].axes[0]; *ly = fixture[index].axes[1];
    *rx = fixture[index].axes[2]; *ry = fixture[index].axes[3];
    memcpy(pressure, fixture[index].pressure, 12);
    for (i=0;i<12;++i) delta[i] = (s16)(i - 6);
    pad->connection_state = fixture[index].connection;
    if (mutation && index == 0) D_00468388[0] = &states[1][0];
    return fixture[index].kind;
}
void func_002B2ED8(GeorgePadDevice *pad, u16 *lx, u16 *ly, u16 *rx, u16 *ry)
{
    ++delta_calls; event(200 + device_index(pad));
    *lx=0xFFFF; *ly=0x8000; *rx=7; *ry=21;
}
float func_0029B940(float y, float x)
{
    ++angle_calls; event(300);
    if (watched_magnitude) magnitude_at_angle = *watched_magnitude;
    if (mutation && angle_calls == 1) D_00468388[0] = &states[2][0];
    return atan2f(y, x);
}
float func_0037B238(float numerator, float denominator)
{
    event(400);
    CHECK(denominator == scalar(0x40C90FDBU));
    return fmodf(numerator, denominator);
}
void func_002AC310(GeorgeInputState *state, s32 mode, const float *axes,
    const u8 *byte_pressure, u32 buttons, const float *pressure,
    float elapsed, float a, float b, float c, float d)
{
    struct Frame *out = &frame[frame_calls];
    event(500 + (int)frame_calls);
    out->state=state; out->mode=mode; out->buttons=buttons;
    memcpy(out->axes,axes,16); memcpy(out->pressure,pressure,48);
    out->byte_pressure=byte_pressure; out->elapsed=elapsed;
    out->outputs[0]=a; out->outputs[1]=b; out->outputs[2]=c; out->outputs[3]=d;
    if (mutation && frame_calls == 0) D_00468388[0] = &states[1][0];
    ++frame_calls;
}
void func_002B2D60(GeorgePadDevice *pad, s32 small, s32 large)
{
    int index=device_index(pad);
    ++motor_calls; event(600 + index);
    motors[index].small=small; motors[index].large=large;
    if (mutation && index == 0) {
        phase(D_00468388[0],10.0f);
        D_00468388[0]=&states[2][0];
    }
}

static void reset(void)
{
    unsigned i,bank;
    memset(states,0,sizeof(states)); memset(fixture,0,sizeof(fixture));
    memset(frame,0,sizeof(frame)); memset(motors,0,sizeof(motors));
    for (bank=0;bank<3;++bank) for (i=0;i<5;++i) states[bank][i].threshold=0.16f;
    for (i=0;i<4;++i) { D_00468388[i]=&states[0][i]; fixture[i].connection=99; }
    alloc_calls=init_calls=read_calls=delta_calls=frame_calls=motor_calls=angle_calls=0;
    mutation=0; event_count=0; watched_magnitude=0;
}

static void test_startup(void)
{
    reset(); mutation=1; states[0][4].flags=0xABCDEFFFU;
    func_00295080();
    CHECK(alloc_calls==5 && init_calls==4);
    CHECK(D_00468388[0]==&states[1][0] && D_00468398==&states[0][0]);
    CHECK(D_0046839C==&states[0][1] && D_004683A0==&states[0][2]);
    CHECK(D_004683A4==&states[0][3] && D_004683A8==&states[0][4]);
}

static void test_selection(void)
{
    static const u32 values[] = {0,0x80000000U,1,0x80000001U,0x007FFFFFU,0x807FFFFFU,
        0x00800000U,0x80800000U,0x3F000000U,0xBF000000U,0x3F800000U,0xBF800000U,
        0x7F7FFFFFU,0xFF7FFFFFU,0x7F800000U,0xFF800000U,0x7FC00001U,0xFFC00001U};
    unsigned i,j;
    for (i=0;i<sizeof(values)/sizeof(*values);++i) for (j=0;j<sizeof(values)/sizeof(*values);++j) {
        u32 a=values[i],b=values[j];
        /* Monotone unsigned representation independently defines encoding order. */
        u32 ka=(a&0x80000000U)?~a:(a^0x80000000U);
        u32 kb=(b&0x80000000U)?~b:(b^0x80000000U);
        CHECK(encoding(george_ee_minimum(scalar(a),scalar(b)))==(ka<kb?a:b));
        CHECK(encoding(george_ee_maximum(scalar(a),scalar(b)))==(ka>kb?a:b));
    }
}

static void test_scalar(void)
{
    int i;
    for (i=-1200;i<=1200;++i) {
        float value=i*0.25f, expected;
        if (value>127.0f) value=127.0f;
        if (value<-127.0f) value=-127.0f;
        if (value>20.0f) expected=(value-20.0f)*scalar(0x3F97ECDCU);
        else if (value<-20.0f) expected=(value+20.0f)*scalar(0x3F97ECDCU);
        else expected=0.0f;
        CHECK(close_float(func_00295B18(i*0.25f),expected));
    }
    CHECK(encoding(func_00295B18(-0.0f))==0x80000000U);
    CHECK(isnan(func_00295B18(scalar(0x7FC00123U))));
}

static void radial_reference(float x,float y,float threshold,float *out)
{
    float length=sqrtf(x*x+y*y), scale=(length*scalar(0x3C010204U)-threshold)/(1.0f-threshold);
    if (scale<0.0f) scale=0.0f;
    out[2]=scale>1.0f?1.0f:scale;
    out[3]=fmodf(scalar(0x40C90FDBU)-atan2f(y,x),scalar(0x40C90FDBU));
    if (length>0.0f) {
        out[0]=(scale*x)/length; out[1]=(scale*y)/length;
        if (out[0]<-1.0f) out[0]=-1.0f; if (out[0]>1.0f) out[0]=1.0f;
        if (out[1]<-1.0f) out[1]=-1.0f; if (out[1]>1.0f) out[1]=1.0f;
    } else out[0]=out[1]=0.0f;
}
static void test_radial(void)
{
    int x,y,t,i,a,b,c,d;
    float out[4], expected[4];
    static const float thresholds[]={0.0f,0.16f,0.5f,0.99f};
    for (t=0;t<4;++t) for (x=-128;x<=128;x+=16) for (y=-128;y<=128;y+=16) {
        reset();
        func_00295BD0(&out[0],&out[1],&out[2],&out[3],(float)x,(float)y,thresholds[t]);
        radial_reference((float)x,(float)y,thresholds[t],expected);
        for (i=0;i<4;++i) CHECK(close_float(out[i],expected[i]));
        CHECK(angle_calls==1 && event_count==2 && events[0]==300 && events[1]==400);
    }
    for (a=0;a<4;++a) for (b=0;b<4;++b) for (c=0;c<4;++c) for (d=0;d<4;++d) {
        float actual[4]={9,9,9,9}, model[4]={9,9,9,9};
        reset(); radial_reference(64.0f,-32.0f,0.16f,expected);
        model[c]=expected[2]; model[d]=expected[3]; model[a]=expected[0]; model[b]=expected[1];
        watched_magnitude=&actual[c];
        func_00295BD0(&actual[a],&actual[b],&actual[c],&actual[d],64.0f,-32.0f,0.16f);
        CHECK(close_float(magnitude_at_angle,expected[2]));
        for (i=0;i<4;++i) CHECK(close_float(actual[i],model[i]));
    }
}

static u32 mapped_buttons(u32 raw)
{
    /* Original pad bit positions paired with game bit positions, independently
     * expressed as a permutation rather than the recovered branch sequence. */
    static const unsigned positions[16]={13,16,14,17,20,18,19,21,26,12,15,27,30,29,28,31};
    u32 result=0; unsigned bit;
    for (bit=0;bit<16;++bit) if (raw&(1U<<bit)) result|=1U<<positions[bit];
    return result;
}
static void test_buttons_and_kinds(void)
{
    u32 raw; int kind,connection,i;
    reset(); fixture[0].kind=1;
    for (raw=0;raw<65536;++raw) {
        fixture[0].buttons=raw|0xFFFF0000U;
        read_calls=delta_calls=frame_calls=motor_calls=angle_calls=0; event_count=0;
        func_00295170(0.01f);
        CHECK(frame[0].buttons==mapped_buttons(raw));
    }
    for (connection=98;connection<=100;++connection) for (kind=-1;kind<=4;++kind) {
        reset(); fixture[0].kind=kind; fixture[0].connection=connection; fixture[0].buttons=0xFFFF;
        for (i=0;i<4;++i) fixture[0].axes[i]=(signed char)(32*(i-2));
        for (i=0;i<12;++i) fixture[0].pressure[i]=(u8)(120+i);
        func_00295170(0.02f);
        CHECK(read_calls==4 && frame_calls==4 && motor_calls==4);
        CHECK(delta_calls==(connection==99?4:3));
        CHECK(frame[0].byte_pressure==0 && frame[0].elapsed==0.02f);
        CHECK(frame[0].mode==(connection==99&&kind>=1&&kind<=3));
        CHECK(frame[0].buttons==(frame[0].mode?mapped_buttons(0xFFFF):0));
        CHECK(angle_calls==(connection==99&&(kind==2||kind==3)?2:0));
        for (i=0;i<12;++i) CHECK(close_float(frame[0].pressure[i],
            connection==99&&kind==3?(float)(signed char)fixture[0].pressure[i]*scalar(0x3B808081U):0.0f));
        if (!(connection==99&&(kind==2||kind==3)))
            for (i=0;i<4;++i) CHECK(frame[0].axes[i]==0.0f && frame[0].outputs[i]==0.0f);
    }
}

static void test_rumble(void)
{
    static const float timers[]={0.0f,-0.0f,-1.0f,1.0f};
    static const float values[]={0.0f,0.1f,1.0f,2.0f};
    static const float phases[]={0.0f,0.5f,0.99f,1.0f,3.0f};
    static const float elapsed[]={0.0f,0.001f,0.02f,0.1f,-0.05f};
    int enabled,t,v,p,e;
    for (enabled=0;enabled<2;++enabled) for (t=0;t<4;++t) for (v=0;v<4;++v)
    for (p=0;p<5;++p) for (e=0;e<5;++e) {
        float expected; int active;
        reset(); states[0][0].flags=enabled?2:0;
        states[0][0].lock_timer=timers[t]; states[0][0].lock_value=values[v]; phase(&states[0][0],phases[p]);
        if (!enabled || timers[t]==0.0f) expected=0.5f;
        else expected=phases[p]+((values[v]*30.0f)*elapsed[e]);
        active=enabled && timers[t]!=0.0f && !(expected<1.0f);
        if (active) expected-=30.0f*elapsed[e];
        func_00295170(elapsed[e]);
        CHECK(motors[0].small==active && motors[0].large==(active?96:0));
        CHECK(close_float(phase_value(&states[0][0]),expected));
        CHECK(states[0][0].lock_timer==(enabled?timers[t]:0.0f));
    }
}

static void test_callback_reloads(void)
{
    float left[4],right[4]; int i;
    reset(); mutation=1; fixture[0].kind=2;
    for (i=0;i<4;++i) fixture[0].axes[i]=64;
    states[1][0].threshold=0.25f; states[2][0].threshold=0.5f;
    states[1][0].flags=2; states[1][0].lock_timer=1.0f; states[1][0].lock_value=1.0f;
    phase(&states[1][0],1.0f); phase(&states[2][0],77.0f);
    func_00295170(0.1f);
    radial_reference(64.0f,64.0f,0.25f,left); radial_reference(64.0f,64.0f,0.5f,right);
    CHECK(frame[0].state==&states[2][0]);
    CHECK(close_float(frame[0].axes[0],left[0]) && close_float(frame[0].axes[1],left[1]));
    CHECK(close_float(frame[0].axes[2],right[0]) && close_float(frame[0].axes[3],right[1]));
    CHECK(close_float(frame[0].outputs[1],left[2]) && close_float(frame[0].outputs[3],right[2]));
    CHECK(motors[0].small==1 && motors[0].large==96);
    CHECK(phase_value(&states[1][0])==7.0f && phase_value(&states[2][0])==77.0f);
    CHECK(D_00468388[0]==&states[2][0]);
    CHECK(events[0]==100 && events[1]==200 && events[2]==300 && events[3]==400);
    CHECK(events[4]==300 && events[5]==400 && events[6]==500 && events[7]==600);
}

int main(void)
{
    test_startup(); test_selection(); test_scalar(); test_radial();
    test_buttons_and_kinds(); test_rumble(); test_callback_reloads();
    printf("pad_input: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
