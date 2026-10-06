/* Engine wrapper semantics with controlled SDK entry points; no retail bytes. */
#include "george/pad_device.h"
#include <stdio.h>
#include <string.h>

u32 D_003FD248;
static GeorgePadDevice pad;
static unsigned checks, failures;
static int events[32], event_count, state_result, mode_result[2], request_result[2];
static int request_calls, main_result, actuator_result, align_result, pressure_result, info_result, read_result;
static int init_calls, open_calls, motor_calls, read_calls, mutate;
static u8 packet_data[32];
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    if (failures<20) printf("line %d: %s\n",__LINE__,#condition); ++failures; } } while (0)
static void event(int code) { if (event_count<32) events[event_count++]=code; }
static void args(s32 port,s32 slot)
{
    CHECK(port==pad.port && slot==pad.slot);
}
s32 func_0038A6B8(s32 port,s32 slot)
{
    args(port,slot); event(1);
    CHECK(pad.updates==0);
    return state_result;
}
s32 func_0038AAE0(s32 port,s32 slot,s32 mode,s32 index)
{
    args(port,slot); event(10+mode); CHECK(index==0 && (mode==1||mode==2));
    if (mutate) { pad.port+=1; pad.slot+=2; pad.connection_state=0x7FFFFFFF; }
    return mode_result[mode-1];
}
s32 func_0038AC18(s32 port,s32 slot,s32 mode,s32 lock)
{
    args(port,slot); event(20); CHECK(mode==1 && lock==3);
    if (mutate) { pad.connection_state=0x7FFFFFFF; pad.port+=1; pad.slot+=2; }
    return main_result;
}
s32 func_0038A820(s32 port,s32 slot)
{
    int index=request_calls++;
    args(port,slot); event(30); CHECK(index<2);
    if (mutate) { pad.connection_state=10+index*100; pad.port+=1; pad.slot+=2; }
    return request_result[index];
}
s32 func_0038A8B0(s32 port,s32 slot,s32 actuator,s32 term)
{
    args(port,slot); event(40); CHECK(actuator==-1 && term==0);
    if (mutate) { pad.port+=1; pad.slot+=2; }
    return actuator_result;
}
s32 func_0038AD98(s32 port,s32 slot,u8 *actuators)
{
    args(port,slot); event(41); CHECK(actuators==pad.actuators);
    if (mutate) { pad.connection_state=0x7FFFFFFF; pad.port+=1; pad.slot+=2; }
    return align_result;
}
s32 func_0038AFD0(s32 port,s32 slot) { args(port,slot);event(50);return pressure_result; }
s32 func_0038B050(s32 port,s32 slot)
{
    args(port,slot); event(51);
    if (mutate) pad.connection_state=0x7FFFFFFF;
    return info_result;
}
s32 func_0038A640(s32 port,s32 slot,u8 *packet)
{
    args(port,slot); event(60); ++read_calls;
    CHECK(pad.previous_buttons==0xABCDEFFFU);
    memcpy(packet,packet_data,32);
    if (mutate) { pad.kind=3; pad.axes[0]=-128; pad.pressure[0]=200; }
    return read_result;
}
s32 func_0038ACD0(s32 port,s32 slot,u8 *motors)
{
    args(port,slot);event(70);++motor_calls;CHECK(motors==pad.motors);return -1;
}
s32 func_00389D98(s32 mode)
{
    ++init_calls;event(80);CHECK(mode==0 && D_003FD248==0);
    CHECK(pad.connection_state==0 && pad.raw_buttons==0 && pad.updates==0 && pad.packet_type==0);
    CHECK(pad.axes[0]==(signed char)0xA5 && pad.axis_delta[0]==(s16)0xA5A5);
    if (mutate) {
        pad.port=9;pad.slot=8;pad.kind=7;pad.axes[0]=99;pad.axis_delta[0]=123;
        pad.buttons=0x12345678U;D_003FD248=99;
    }
    return -7;
}
s32 func_0038A1F0(s32 port,s32 slot,void *buffer)
{
    ++open_calls;event(81);CHECK(port==3 && slot==4 && buffer==&pad);
    CHECK(pad.axes[0]==0 && pad.axis_delta[0]==0);
    if (mutate) { CHECK(pad.port==9 && pad.slot==8);CHECK(pad.buttons==0x12345678U);pad.dma[0]=12;pad.axes[0]=13; }
    return 0;
}

static void reset(s32 connection)
{
    int i;
    memset(&pad,0,sizeof pad);memset(packet_data,0,sizeof packet_data);
    pad.port=3;pad.slot=4;pad.connection_state=connection;pad.kind=3;
    pad.updates=0xFFFFFFFFU;pad.buttons=0xABCDEFFFU;pad.raw_buttons=0x5555;
    pad.accumulated_buttons=0x80000000U;pad.toggled_buttons=0xF0000000U;
    for(i=0;i<4;++i)pad.axes[i]=(signed char)(i*30-45);
    for(i=0;i<12;++i){pad.pressure[i]=(u8)(200-i);packet_data[8+i]=(u8)(3+i*21);}
    packet_data[0]=0;packet_data[1]=0x79;packet_data[2]=0xED;packet_data[3]=0xCB;
    packet_data[4]=0;packet_data[5]=255;packet_data[6]=128;packet_data[7]=127;
    event_count=request_calls=init_calls=open_calls=motor_calls=read_calls=mutate=0;
    state_result=6;mode_result[0]=4;mode_result[1]=7;request_result[0]=2;request_result[1]=0;
    main_result=actuator_result=align_result=pressure_result=info_result=read_result=1;
}
static s32 invoke(u32 *buttons,signed char *axes,u8 *pressure,s16 *delta)
{
    return func_002B2800(&pad,buttons,axes?&axes[0]:0,axes?&axes[1]:0,
        axes?&axes[2]:0,axes?&axes[3]:0,pressure,delta);
}

static void test_initializer(void)
{
    int index,again;
    for(again=0;again<2;++again) {
        reset(99);memset(&pad,0xA5,sizeof pad);D_003FD248=again?1:0;mutate=!again;
        CHECK(func_002B2DA8(&pad,3,4,0x1234)==(again?0:-7));
        CHECK(init_calls==(again?0:1) && open_calls==1 && D_003FD248==1);
        CHECK(pad.kind==(again?(s32)0xA5A5A5A5U:7));
        CHECK(pad.port==(again?3:9) && pad.slot==(again?4:8));
        CHECK(pad.axes[0]==(again?0:13));
        CHECK(pad.field128==0x1234 && pad.buttons==0 && pad.accumulated_buttons==0);
        CHECK(pad.previous_buttons==0 && pad.toggled_buttons==0);
        CHECK(pad.untouched11A[0]==0xA5 && pad.untouched11A[1]==0xA5);
        for(index=0;index<6;++index) {
            CHECK(pad.motors[index]==0);
            CHECK(pad.actuators[index]==(index<2?index:0xFF));
        }
        for(index=0;index<12;++index)CHECK(pad.pressure[index]==0);
        for(index=0;index<0x2C;++index)CHECK(pad.untouched154[index]==0xA5);
        for(index=1;index<0x100;++index)CHECK(pad.dma[index]==0xA5);
    }
}

static void test_switch(void)
{
    s32 state;
    /* Disconnected status5 still executes all state cases, but never reads an
     * uninitialized packet. Derived expected transitions are semantic values. */
    for(state=-2;state<=101;++state) {
        s32 expected=state;int reads=0;
        reset(state);state_result=5;
        switch(state) {
        case 40:expected=42;break;
        case 41:expected=42;break;
        case 42:expected=0;reads=2;break;
        case 70:expected=71;reads=1;break;
        case 71:expected=72;reads=2;break;
        case 72:expected=76;break;
        case 76:expected=77;break;
        case 77:expected=99;reads=2;break;
        }
        CHECK(invoke(0,0,0,0)==0);
        CHECK(pad.connection_state==expected && request_calls==reads && read_calls==0);
        CHECK(pad.kind==(state==42?2:3));
    }
    reset(99);state_result=0;CHECK(invoke(0,0,0,0)==0 && pad.connection_state==0 && pad.kind==0);
    {
        static const s32 modes[]={-1,0,4,7,8};
        int a,b,k;
        for(a=0;a<5;++a)for(b=0;b<5;++b)for(k=1;k<=2;++k) {
            s32 mode=modes[b]>=1?modes[b]:modes[a],expected;
            reset(0);pad.kind=k;mode_result[0]=modes[a];mode_result[1]=modes[b];
            expected=modes[a]==0?0:mode==4?40:mode==7?70:99;
            invoke(0,0,0,0);
            CHECK(pad.connection_state==expected);
            CHECK(pad.kind==(modes[a]!=0&&mode==4?1:k));
        }
    }
    /* Original case0 with a zero kind becomes a deterministic kind1/read path
     * only for mode4/7; invalid-mode/zero-kind packet reads remain untested. */
    reset(0);pad.kind=0;mode_result[0]=7;mode_result[1]=0;
    CHECK(invoke(0,0,0,0)==1 && pad.connection_state==40);
    reset(40);state_result=5;mode_result[1]=0;invoke(0,0,0,0);CHECK(pad.connection_state==99);
    reset(40);state_result=5;main_result=-1;invoke(0,0,0,0);CHECK(pad.connection_state==41);
    reset(70);state_result=5;actuator_result=0;invoke(0,0,0,0);CHECK(pad.connection_state==99);
    reset(70);state_result=5;align_result=0;invoke(0,0,0,0);CHECK(pad.connection_state==70);
    reset(72);state_result=5;pressure_result=-1;invoke(0,0,0,0);CHECK(pad.connection_state==99);
    reset(76);state_result=5;info_result=0;invoke(0,0,0,0);CHECK(pad.connection_state==76);
    {
        static const s32 targets[]={42,71,77};int i,a,b;
        for(i=0;i<3;++i)for(a=0;a<=2;++a)for(b=0;b<=2;++b) {
            s32 expected=targets[i]-(a==1);
            reset(targets[i]);state_result=5;request_result[0]=a;request_result[1]=b;
            if(b==0)expected=targets[i]==42?0:targets[i]==71?expected+1:99;
            invoke(0,0,0,0);CHECK(pad.connection_state==expected && request_calls==2);
        }
    }
}

static void test_read(void)
{
    int kind,success,index;u32 buttons; signed char axes[4];u8 pressure[12];s16 delta[12];
    for(kind=1;kind<=3;++kind)for(success=0;success<=1;++success) {
        reset(99);pad.kind=kind;packet_data[0]=(u8)success;
        memset(pressure,0xA5,12);memset(delta,0xA5,sizeof delta);buttons=0;memset(axes,0xA5,4);
        CHECK(invoke(&buttons,axes,pressure,delta)==kind);
        CHECK(buttons==0x1234 && pad.raw_buttons==0x1234 && pad.buttons==0x1234);
        CHECK(pad.accumulated_buttons==0x80001234U);
        CHECK(pad.toggled_buttons==(0xF0000000U^(0x1234U&~0x5555U)));
        CHECK(pad.previous_buttons==0xABCDEFFFU && pad.packet_type==0x79);
        for(index=0;index<4;++index) {
            static const int raw[]={0,-1,-128,127};
            CHECK(axes[index]==(kind>=2?raw[index]:0));
            CHECK(pad.axes[index]==(kind>=2?raw[index]:index*30-45));
            CHECK(pad.axis_delta[index]==(kind>=2?raw[index]-(index*30-45):0));
        }
        for(index=0;index<12;++index) {
            int active=kind==3&&success==0;
            CHECK(pressure[index]==(active?packet_data[8+index]:0xA5));
            CHECK(delta[index]==(active?(int)packet_data[8+index]-(200-index):(s16)0xA5A5));
            CHECK(pad.pressure[index]==(active?packet_data[8+index]:200-index));
        }
    }
    reset(99);read_result=0;buttons=0xFACE;memset(axes,0xA5,4);memset(pressure,0xA5,12);memset(delta,0xA5,sizeof delta);
    CHECK(invoke(&buttons,axes,pressure,delta)==0 && read_calls==1);
    CHECK(buttons==0xFACE && pad.previous_buttons==0xABCDEFFFU && pad.packet_type==0);
    for(index=0;index<4;++index)CHECK((u8)axes[index]==0xA5 && pad.axes[index]==index*30-45);
    reset(99);state_result=5;buttons=0;memset(axes,0xA5,4);
    CHECK(invoke(&buttons,axes,0,0)==0 && buttons==0xABCDEFFFU && read_calls==0);
    for(index=0;index<4;++index)CHECK(axes[index]==0);
}

static void test_aliases(void)
{
    int index;u8 expected[12];s16 differences[12],actual[12];u16 out[4];
    reset(99);pad.kind=2;packet_data[2]=0xFF;packet_data[3]=0xFC;
    CHECK(invoke((u32*)&pad.kind,0,0,actual)==3);
    for(index=0;index<12;++index)CHECK(actual[index]==(int)packet_data[8+index]-(200-index));
    reset(99);packet_data[2]=0xFF;packet_data[3]=0xFE;
    CHECK(invoke((u32*)&pad.kind,0,0,actual)==1 && pad.pressure[0]==200);
    reset(99);memcpy(expected,pad.pressure,12);
    for(index=0;index<12;++index) {
        differences[index]=(s16)((int)packet_data[8+index]-expected[index]);
        if(index<11)expected[index+1]=packet_data[8+index];
        expected[index]=packet_data[8+index];
    }
    invoke(0,0,&pad.pressure[1],actual);
    CHECK(memcmp(expected,pad.pressure,12)==0 && memcmp(differences,actual,sizeof actual)==0);
    reset(99);
    /* Delta writes overlap future old pressure bytes; interleaving is observable. */
    /* Use a larger complete object as output storage; later delta stores lie
     * in motors/actuators and the untouched suffix, without leaving the object. */
    {
        GeorgePadDevice model=pad;u8 *bytes=(u8*)&model;
        for(index=0;index<12;++index) {
            s16 difference=(s16)((int)packet_data[8+index]-model.pressure[index]);
            bytes[0x13C+2*index]=(u8)difference;bytes[0x13D+2*index]=(u8)((u16)difference>>8);
            model.pressure[index]=packet_data[8+index];
        }
        invoke(0,0,0,(s16*)pad.pressure);
        CHECK(memcmp((u8*)&pad+0x13C,(u8*)&model+0x13C,24)==0);
    }
    for(index=0;index<16;++index) {
        reset(99);pad.axis_delta[0]=-255;pad.axis_delta[1]=-1;pad.axis_delta[2]=255;pad.axis_delta[3]=0;
        memset(out,0xA5,sizeof out);
        func_002B2ED8(&pad,index&1?&out[0]:0,index&2?&out[1]:0,index&4?&out[2]:0,index&8?&out[3]:0);
        CHECK(out[0]==(index&1?(u16)-255:0xA5A5));CHECK(out[1]==(index&2?0xFFFF:0xA5A5));
        CHECK(out[2]==(index&4?255:0xA5A5));CHECK(out[3]==(index&8?0:0xA5A5));
    }
    reset(99);pad.axis_delta[0]=1;pad.axis_delta[1]=2;pad.axis_delta[2]=3;pad.axis_delta[3]=4;
    func_002B2ED8(&pad,(u16*)&pad.axis_delta[1],(u16*)&pad.axis_delta[2],(u16*)&pad.axis_delta[3],&out[0]);
    CHECK(pad.axis_delta[1]==1 && pad.axis_delta[2]==1 && pad.axis_delta[3]==1 && out[0]==1);
}

static void test_mutation_and_motor(void)
{
    int kind,small,large;unsigned i;
    static const s32 cases[]={40,41,42,70,71,76,77};
    static const s32 expected[]={ (s32)0x80000000U,(s32)0x80000000U,0,10,111,(s32)0x80000000U,99 };
    for(i=0;i<sizeof cases/sizeof *cases;++i) {
        reset(cases[i]);state_result=5;mutate=1;
        invoke(0,0,0,0);CHECK(pad.connection_state==expected[i]);
    }
    reset(99);pad.kind=1;mutate=1;
    CHECK(invoke(0,0,0,0)==3 && pad.axis_delta[0]==128 && pad.pressure[0]==3);
    for(kind=-2;kind<=3;++kind)for(small=-2;small<=2;++small)for(large=-1;large<=257;large+=43) {
        reset(99);pad.kind=kind;memset(pad.motors,0xA5,6);
        func_002B2D60(&pad,small,large);
        CHECK(motor_calls==(kind>=2));
        CHECK(pad.motors[0]==(kind>=2?(small>0):0xA5));
        CHECK(pad.motors[1]==(kind>=2?(u8)large:0xA5));
        for(i=2;i<6;++i)CHECK(pad.motors[i]==0xA5);
    }
}

int main(void)
{
    test_initializer();test_switch();test_read();test_aliases();test_mutation_and_motor();
    printf("pad_device: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
