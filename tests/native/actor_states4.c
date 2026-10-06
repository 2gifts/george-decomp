/* Asset-free call-boundary tests. Expected state outcomes and callback
 * mutations are independent specifications; no retail instructions are used. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_states4.c"

enum { A0=1, WORD, VOID38, GATE, V78, VB0, PREDICATE, CONTROL_INT,
       REQUEST, DURATION, CANCEL, SET_FLAG, CLEAR_FLAG, ANGLE, NORMALIZE,
       QUERY, MOTION, OWNER, ROUTE_MODE, ROUTE_CLEAR, ROUTE_ADVANCE,
       ROUTE_STOP, ROUTE_UPDATE, ROUTE_STATUS, SAMPLE, LENGTH, DETACH,
       EXIT_STATE, ENTER_STATE, HANDLE_START, HANDLE_STOP, HANDLE_FREE,
       ALLOCATE, CONSTRUCT_EFFECT, HASH, EMIT, UPPER_BOUND, INSERT, AT_EXIT,
       GENERIC, FIND_REFERENCE, CONSTRUCT_OBJECT, MATRIX_COPY, REFERENCE_WORD,
       RELEASE, BALLISTIC_CLEAR, BALLISTIC_MATRIX, BALLISTIC_RECORD,
       SOFT_CONVERT, SOFT_CMP, SOFT_SUB, SOFT_MUL, SOFT_ADD, SOFT_FLOAT };
typedef struct Event { int kind; void *object, *argument; u32 word, other; float value; } Event;
static Event events[256];
static int event_count, checks;
static u8 entity_storage[0xA00] __attribute__((aligned(16)));
static u8 data_storage[0x300], alternate_data[0x300], primary[64], companion[64], replacement[64];
static u8 object_storage[0x400], handle[64], replacement_handle[64], reference[0x100];
static u8 map[0x400], alternate_map[0x400], parent[0x200], alternate_parent[0x200];
static u8 point[0x100], alternate_point[0x100], reference_table[0x40];
static u8 control_table[0x100];
static struct { GeorgeActorControlObject object; u32 pad[16]; } control;
static GeorgeGoalEntity *actor = (GeorgeGoalEntity *)entity_storage;
static void (*hook)(Event *);
static s32 request_result, gate_result, predicate_result, control_result, route_result;
static float duration_result, length_result, angles[8];
static int angle_override, angle_count, allocate_count;
static void *query_result, *reference_result;
static GeorgeMathVec3 sampled, last_vector, last_motion, emitted;
static void *last_enter;
static void *registry[8];
static void **upper_result;
static GeorgeActorEffectRecord effect_record;
void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[]={0}, D_0042D8A8[]={1}, D_0042D8C0[]={2};
u8 D_003F83F0[42*28] __attribute__((aligned(8)));

static void check(int value,const char *label,int line)
{ ++checks; if(!value){fprintf(stderr,"actor_states4:%d: %s\n",line,label);exit(1);} }
#define CHECK(c) check((c),#c,__LINE__)
static u32 bits(float value) { union {float f;u32 u;} b;b.f=value;return b.u; }
static float from_bits(u32 word) { union {float f;u32 u;} b;b.u=word;return b.f; }
static void near_value(float value,float expected) {if(!(fabsf(value-expected)<0.00005f))fprintf(stderr,"actual=%g expected=%g\n",value,expected);CHECK(fabsf(value-expected)<0.00005f);}
static Event *emit(int kind,void *object,void *argument,u32 word,u32 other,float value)
{
    Event *e;CHECK(event_count<256);e=&events[event_count++];
    e->kind=kind;e->object=object;e->argument=argument;e->word=word;e->other=other;e->value=value;
    if(hook)hook(e);return e;
}
static int count(int kind) {int i,n=0;for(i=0;i<event_count;++i)if(events[i].kind==kind)++n;return n;}
static Event *nth(int kind,int index) {int i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&index--==0)return &events[i];CHECK(0);return 0;}
static u32 self_offset(void *self) {return (u32)self-(u32)&control.object;}
static void mock_void(void *self) {emit(self_offset(self)==4?A0:VOID38,self,0,0,0,0);}
static void mock_word(void *self,u32 word) {emit(WORD,self,0,word,self_offset(self),0);}
static s32 mock_gate(void *self,u32 word) {emit(GATE,self,0,word,0,0);return gate_result;}
static s32 mock_predicate(void *self,float first,float second)
{CHECK(bits(first)==0x3E4CCCCDu&&bits(second)==0x3E800000u);emit(PREDICATE,self,0,0,0,0);return predicate_result;}
static s32 mock_int(void *self) {emit(CONTROL_INT,self,0,0,0,0);return control_result;}
static void mock_vector(void *self,const GeorgeMathVec3 *vector)
{last_vector=*vector;emit(self_offset(self)==20?V78:VB0,self,(void *)vector,0,0,0);}
static void mock_reference(void *self,u32 word) {emit(REFERENCE_WORD,self,0,word,0,0);}
static void mock_enter(void *self) {last_enter=self;emit(ENTER_STATE,self,0,0,0,0);}
static void reset(void)
{
    memset(entity_storage,0,sizeof(entity_storage));memset(data_storage,0,sizeof(data_storage));
    memset(alternate_data,0,sizeof(alternate_data));memset(object_storage,0,sizeof(object_storage));
    memset(map,0,sizeof(map));memset(alternate_map,0,sizeof(alternate_map));
    memset(parent,0,sizeof(parent));memset(alternate_parent,0,sizeof(alternate_parent));
    memset(point,0,sizeof(point));memset(alternate_point,0,sizeof(alternate_point));
    memset(control_table,0,sizeof(control_table));memset(reference_table,0,sizeof(reference_table));
    memset(D_003F83F0,0,sizeof(D_003F83F0));memset(events,0,sizeof(events));memset(angles,0,sizeof(angles));
    event_count=0;hook=0;request_result=1;gate_result=1;predicate_result=1;control_result=0;route_result=1;
    duration_result=4800.0f;length_result=1.0f;angle_override=1;angle_count=0;allocate_count=0;last_enter=0;
    sampled.x=2;sampled.y=3;sampled.z=4;query_result=point;reference_result=0;
    actor->field18=(GeorgeGoalEntityData *)data_storage;
    FIELD(actor,0x20,GeorgeActorControlObject *)=&control.object;control.object.field00=control_table;
    FIELD(actor,0x1B0,void *)=primary;FIELD(actor,0x1B4,void *)=companion;FIELD(actor,0x368,u32)=0x88776655;
    FIELD(actor,0x35C,float)=0.25f;FIELD(actor,0x14,u32)=99;FIELD(actor,0x824,void *)=map;FIELD(actor,0x84C,void *)=map;
    FIELD(map,0x0C,void *)=parent;FIELD(alternate_map,0x0C,void *)=alternate_parent;
    FIELD(map,0x48,float)=2;FIELD(parent,0x88,float)=1;FIELD(map,0x11C,float)=1;
    D_003F2D40=object_storage;D_0046A0F0.field00=registry;D_0046A0F0.field04=registry;D_0046A0F0.field08=registry+8;upper_result=registry;
    ((GeorgeGoalVirtualVoid *)(control_table+0xA0))->adjustment=4;
    ((GeorgeGoalVirtualVoid *)(control_table+0xA0))->invoke=mock_void;
    ((GeorgeGoalVirtualVoid *)(control_table+0x38))->adjustment=12;
    ((GeorgeGoalVirtualVoid *)(control_table+0x38))->invoke=mock_void;
    ((GeorgeGoalVirtualWord *)(control_table+0x30))->adjustment=8;
    ((GeorgeGoalVirtualWord *)(control_table+0x30))->invoke=mock_word;
    ((GeorgeGoalVirtualWord *)(control_table+0xB8))->adjustment=28;
    ((GeorgeGoalVirtualWord *)(control_table+0xB8))->invoke=mock_word;
    ((GeorgeActorVirtualWordResult *)(control_table+0x50))->adjustment=16;
    ((GeorgeActorVirtualWordResult *)(control_table+0x50))->invoke=mock_gate;
    ((GeorgeActorVirtualVectorInput *)(control_table+0x78))->adjustment=20;
    ((GeorgeActorVirtualVectorInput *)(control_table+0x78))->invoke=mock_vector;
    ((GeorgeActorVirtualVectorInput *)(control_table+0xB0))->adjustment=24;
    ((GeorgeActorVirtualVectorInput *)(control_table+0xB0))->invoke=mock_vector;
    ((GeorgeActorVirtualPredicate *)(control_table+0xD0))->adjustment=32;
    ((GeorgeActorVirtualPredicate *)(control_table+0xD0))->invoke=mock_predicate;
    ((GeorgeGoalVirtualInt *)(control_table+0xD8))->adjustment=36;
    ((GeorgeGoalVirtualInt *)(control_table+0xD8))->invoke=mock_int;
    FIELD(reference,0x20,void *)=reference_table;
    ((GeorgeGoalVirtualWord *)(reference_table+0x10))->adjustment=-4;
    ((GeorgeGoalVirtualWord *)(reference_table+0x10))->invoke=mock_reference;
}

s32 func_00270510(void *object,u32 word,u32 mode0,u32 mode1,u32 owner_word,
                 GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,
                 u32 invoke_word,u32 callback_word,float time)
{
    CHECK(mode0==0&&mode1==0&&invoke_word==0&&callback_word==0);
    CHECK(owner_word==FIELD(actor,0x368,u32)&&context==(callback?actor:0));
    emit(REQUEST,object,(void *)callback,word,owner_word,time);return request_result;
}
void func_002727D8(void *object) {emit(CANCEL,object,0,0,0,0);}
float func_002A6E60(void *source) {emit(DURATION,source,0,0,0,0);return duration_result;}
float func_0029B940(float first,float second)
{float result=angle_override?angles[angle_count]:atan2f(first,second);CHECK(angle_count<8);++angle_count;emit(ANGLE,0,0,bits(second),0,first);return result;}
float func_002A3538(GeorgeMathVec3 *vector)
{float length=sqrtf((vector->x*vector->x+vector->y*vector->y)+vector->z*vector->z);emit(NORMALIZE,vector,0,0,0,length);if(length!=0){vector->x/=length;vector->y/=length;vector->z/=length;}return length;}
void *func_00131578(void *object,const GeorgeMathVec3 *position)
{void *result=query_result;emit(QUERY,object,(void *)position,0,0,0);return result;}
void func_00131798(void *object,const GeorgeMathVec3 *position,const GeorgeMathVec3 *motion)
{CHECK(position==VECTOR(actor,0x40));last_motion=*motion;emit(MOTION,object,(void *)motion,0,0,0);}
void func_00132BB0(void *object,GeorgeGoalEntity *entity) {emit(OWNER,object,entity,0,0,0);}
void func_00132C18(void *object) {emit(ROUTE_CLEAR,object,0,0,0,0);}
void func_00132C60(void *object,u32 word) {emit(ROUTE_MODE,object,0,word,0,0);}
void func_00132CC8(void *object) {emit(ROUTE_ADVANCE,object,0,0,0,0);}
void func_00132CF8(void *object) {emit(ROUTE_STOP,object,0,0,0,0);}
void func_00132BC8(GeorgeMathScaled16C *object) {emit(ROUTE_UPDATE,object,0,0,0,0);}
s32 func_00132950(void *object,u32 word) {CHECK(word==8);emit(ROUTE_STATUS,object,0,word,0,0);return route_result;}
void func_0014B940(void *object,GeorgeMathVec3 *output,u32 mode,float elapsed)
{CHECK(mode==0);*output=sampled;emit(SAMPLE,object,output,mode,0,elapsed);}
float func_0014B8D8(void *object) {emit(LENGTH,object,0,0,0,0);return length_result;}
void func_0014B9C8(void *object,GeorgeGoalEntity *entity) {CHECK(entity==actor);emit(DETACH,object,entity,0,0,0);}
void func_00170538(GeorgeGoalEntity *entity) {CHECK(entity==actor);emit(EXIT_STATE,entity,0,0,0,0);}
void func_0017D908(GeorgeGoalEntity *entity) {CHECK(entity==actor);emit(BALLISTIC_CLEAR,entity,0,0,0,0);}
void func_0017EBF0(GeorgeGoalEntity *entity) {CHECK(entity==actor);emit(BALLISTIC_CLEAR,entity,0,1,0,0);}
void func_0017ED18(GeorgeGoalEntity *entity,s32 mode) {CHECK(entity==actor&&mode==2);emit(BALLISTIC_MATRIX,entity,0,(u32)mode,0,0);}
void func_0017EE30(GeorgeGoalEntity *entity,s32 mode) {CHECK(entity==actor&&mode==2);emit(BALLISTIC_RECORD,entity,0,(u32)mode,0,0);}
void func_0018FD40(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) {CHECK(entity==actor);emit(SET_FLAG,entity,0,(u32)mask,0,0);}
void func_0018FD30(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) {CHECK(entity==actor);emit(CLEAR_FLAG,entity,0,(u32)mask,0,0);}
void *func_002AEE60(u32 size) {++allocate_count;emit(ALLOCATE,0,0,size,0,0);return size==12?(void *)&effect_record:(void *)replacement;}
void *func_002481F0(void *object) {emit(CONSTRUCT_EFFECT,object,0,0,0,0);return object_storage;}
s32 func_00100AA8(const void *a,const void *b) {(void)a;(void)b;return 0;}
void **func_00100C30(void **begin,void **end,void *const *value,GeorgeStartupCompare compare)
{CHECK(begin==D_0046A0F0.field00&&end==D_0046A0F0.field04&&*value==&effect_record&&compare==func_00100AA8);emit(UPPER_BOUND,0,0,0,0,0);return upper_result;}
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value)
{CHECK(range==&D_0046A0F0&&position==upper_result&&*value==&effect_record);emit(INSERT,range,position,0,0,0);}
void func_002BD340(void) {}
s32 func_00396260(void (*callback)(void)) {CHECK(callback==func_002BD340);emit(AT_EXIT,0,0,0,0,0);return 0;}
u32 *func_002BEBA0(u32 *output,const u8 *text) {*output=0x12345678;emit(HASH,(void *)text,output,*output,0,0);return output;}
void *func_00251B58(void *object,u32 key,const GeorgeMathVec3 *position) {CHECK(key==0x12345678);emitted=*position;emit(EMIT,object,(void *)position,key,0,0);return handle;}
void func_002455C0(void *object) {emit(HANDLE_START,object,0,0,0,0);}
void func_002457D8(void *object) {emit(HANDLE_STOP,object,0,0,0,0);}
void func_00245B08(void *object) {emit(HANDLE_FREE,object,0,0,0,0);}
void func_001B67C0(void *context,GeorgeGoalEntity *entity,u32 w0,u32 w1,u32 w2,u32 w3)
{CHECK(context==ADDRESS(actor,0x40)&&entity==FIELD(actor,0x3B8,GeorgeGoalEntity *)&&w0==FIELD(actor,0x3B4,u32)&&w2==0&&w3==0);emit(GENERIC,entity,context,w1,0,0);}
void *func_00210078(u32 word,u32 mode0,u32 mode1) {CHECK(mode0==0&&mode1==0);emit(FIND_REFERENCE,0,0,word,0,0);return reference_result;}
void *func_00236CB8(const void *matrix,void *ref,u32 mode,u32 w0,u32 w1) {CHECK(mode==1&&w0==0&&w1==0);emit(CONSTRUCT_OBJECT,ref,(void *)matrix,0,0,0);return object_storage;}
void func_002A1C08(void *output,const void *input) {memcpy(output,input,64);emit(MATRIX_COPY,output,(void *)input,0,0,0);}
void func_002393F8(u32 word) {emit(RELEASE,(void *)word,0,0,0,0);}

static double unpack(GeorgeActorBits64 bits64) {union{double d;GeorgeActorBits64 u;}v;v.u=bits64;return v.d;}
static GeorgeActorBits64 pack(double value) {union{double d;GeorgeActorBits64 u;}v;v.d=value;return v.u;}
GeorgeActorBits64 func_00374848(float value) {GeorgeActorBits64 r=pack(value);emit(SOFT_CONVERT,0,0,0,0,value);return r;}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a,GeorgeActorBits64 b) {GeorgeActorBits64 r=pack(unpack(a)-unpack(b));emit(SOFT_SUB,0,0,0,0,0);return r;}
GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a,GeorgeActorBits64 b) {GeorgeActorBits64 r=pack(unpack(a)*unpack(b));emit(SOFT_MUL,0,0,0,0,0);return r;}
GeorgeActorBits64 func_00372C68(GeorgeActorBits64 a,GeorgeActorBits64 b) {GeorgeActorBits64 r=pack(unpack(a)+unpack(b));emit(SOFT_ADD,0,0,0,0,0);return r;}
s32 func_00373250(GeorgeActorBits64 a,GeorgeActorBits64 b) {double x=unpack(a),y=unpack(b);s32 r=x<y?-1:x>y?1:0;emit(SOFT_CMP,0,0,0,0,0);return r;}
float func_003734F8(GeorgeActorBits64 value) {float r=(float)unpack(value);emit(SOFT_FLOAT,0,0,0,0,r);return r;}

static u32 watched_phase, watched_timer, seen_phase;
static int phase_half;
static void duration_hook(Event *e)
{if(e->kind==DURATION){seen_phase=phase_half?FIELD(actor,watched_phase,u16):FIELD(actor,watched_phase,u32);if(phase_half)FIELD(actor,watched_phase,u16)=77;else FIELD(actor,watched_phase,u32)=77;}}
static void test_callbacks(void)
{
    GeorgeActorRequestCallback functions[]={func_00195978,func_00195B30,func_00195BE8,func_00195CC0};
    u32 phases[]={0x7F4,0x818,0x828,0x854},timers[]={0x7F0,0x81C,0x82C,0x858};
    u32 initial[]={0,1,0x7FFFFFFFu,0xFFFFFFFFu};int i,j;
    for(i=0;i<4;++i)for(j=0;j<4;++j){reset();watched_phase=phases[i];watched_timer=timers[i];phase_half=i==3;hook=duration_hook;if(phase_half)FIELD(actor,watched_phase,u16)=(u16)initial[j];else FIELD(actor,watched_phase,u32)=initial[j];functions[i](0,actor,reference);CHECK(seen_phase==(phase_half?(u16)(initial[j]+1u):initial[j]+1u));CHECK((phase_half?FIELD(actor,watched_phase,u16):FIELD(actor,watched_phase,u32))==77);CHECK(bits(FIELD(actor,watched_timer,float))==0x3F800000u);CHECK(nth(DURATION,0)->object==reference);}
    reset();FIELD(actor,0x810,u32)=0xFFFFFFFFu;FIELD(actor,0x804,float)=2;FIELD(actor,0x808,float)=3;FIELD(actor,0x80C,float)=4;FIELD(actor->field18,0x1B0,float)=2;func_00195A88(0,actor,reference);CHECK(FIELD(actor,0x810,u32)==0);near_value(FIELD(actor,0x804,float),4);near_value(FIELD(actor,0x808,float),6);near_value(FIELD(actor,0x80C,float),8);
}
static void request_hook(Event *e)
{if(e->kind==REQUEST&&count(REQUEST)==1){FIELD(actor,0x1B0,void *)=replacement;FIELD(actor,0x368,u32)=5;}}
static void state20_control_hook(Event *e)
{if(e->kind==A0){FIELD(actor,0x7F4,u32)=1;FIELD(actor,0x7F0,float)=0.25f;}}
static void test_state20(void)
{
    int i,j,k;u32 phases[]={0,1,2,0xFFFFFFFFu};u32 flags[]={0,0x80,0x80000,0x80080};float timers[]={-1,0,0.25f,1,0};timers[4]=from_bits(0x7FC01234);
    for(i=0;i<4;++i)for(j=0;j<4;++j)for(k=0;k<5;++k){reset();FIELD(actor,0x7F4,u32)=phases[i];FIELD(actor,0x7F0,float)=timers[k];FIELD(actor,0x190,GeorgeActorBits64)=flags[j];func_00187648(actor);CHECK(count(A0)==1);CHECK(count(PREDICATE)==(phases[i]==1&&timers[k]-0.25f<=0&&flags[j]==0));CHECK(FIELD(actor,0x14,u32)==((phases[i]==1&&timers[k]-0.25f<=0)?(flags[j]==0?3u:0u):99u));}
    reset();hook=state20_control_hook;func_00187648(actor);CHECK(FIELD(actor,0x14,u32)==3&&FIELD(actor,0x7F0,float)==0);
    reset();hook=request_hook;func_001959C0(actor);CHECK(nth(REQUEST,0)->object==companion&&nth(REQUEST,1)->object==replacement);CHECK(nth(REQUEST,1)->argument==(void *)func_00195978);CHECK(FIELD(actor,0x7F4,u32)==0);reset();request_result=0;func_001959C0(actor);CHECK(FIELD(actor,0x7F4,u32)==1);reset();FIELD(actor,0x1B0,void *)=0;func_001959C0(actor);CHECK(count(REQUEST)==0&&FIELD(actor,0x7F4,u32)==1);
}
static void test_state21(void)
{
    int bit,gate;float expected;
    for(bit=0;bit<2;++bit)for(gate=0;gate<2;++gate){reset();FIELD(actor,0x190,GeorgeActorBits64)=bit?0x20000u:0;gate_result=gate;request_result=0;FIELD(actor,0x7F8,float)=2;FIELD(actor,0x7FC,float)=3;FIELD(actor,0x800,float)=4;func_00187990(actor);CHECK(nth(REQUEST,0)->word==(bit&&!gate?0x53u:0x51u));CHECK(count(GATE)==bit&&count(SET_FLAG)==(bit&&gate));CHECK(FIELD(actor,0x810,u32)==1&&bits(FIELD(actor,0x814,float))==0);CHECK(FIELD(actor,0x804,float)==2&&FIELD(actor,0x808,float)==3&&FIELD(actor,0x80C,float)==4);}
    reset();FIELD(actor,0x810,u32)=0;func_00187BA0(actor);CHECK(count(A0)==1&&count(ANGLE)==0);
    reset();FIELD(actor,0x810,u32)=1;FIELD(actor,0x814,float)=1;angles[0]=angles[1]=angles[2]=0.05f;func_00187BA0(actor);CHECK(FIELD(actor,0x58,float)==0&&count(VB0)==1);
    reset();FIELD(actor,0x810,u32)=1;FIELD(actor,0x814,float)=0;angles[0]=angles[1]=angles[2]=0.2f;expected=0.2f*(0.25f*5.0f);func_00187BA0(actor);near_value(FIELD(actor,0x58,float),expected);CHECK(FIELD(actor,0x14,u32)==0);
    reset();FIELD(actor,0x810,u32)=1;FIELD(actor,0x190,GeorgeActorBits64)=0x20000;gate_result=0;func_00187BA0(actor);CHECK(FIELD(actor,0x14,u32)==0x1D);
}
static void handle_hook(Event *e) {if(e->kind==HANDLE_STOP)FIELD(actor,0x820,void *)=replacement_handle;}
static void test_state22(void)
{
    int i;u32 types[]={0,3,4,5,0xFFFFFFFFu};
    for(i=0;i<5;++i){reset();FIELD(actor,0x10,u32)=types[i];func_00187EA8(actor);CHECK(count(HASH)==1&&nth(HASH,0)->object==(void *)D_0042D8C0);CHECK(FIELD(actor,0x820,void *)==handle&&count(HANDLE_START)==1);CHECK(FIELD(actor,0x818,u32)==((types[i]-3u<2u)?0u:100u));CHECK(count(REQUEST)==((types[i]-3u<2u)?0:2));}
    reset();D_003F2D40=0;FIELD(actor,0x190,GeorgeActorBits64)=0x200000;func_00187EA8(actor);CHECK(allocate_count==2&&count(AT_EXIT)==1);CHECK(registry[0]==&effect_record&&D_0046A0F0.field04==registry+1);CHECK(effect_record.field00==9&&effect_record.field08==object_storage);CHECK(nth(HASH,0)->object==(void *)D_0042D8A8);
    reset();D_003F2D40=0;D_0046A0F0.field08=registry;func_00187EA8(actor);CHECK(count(INSERT)==1);
    reset();FIELD(actor->field18,0xEC,u32)=0x4455;reference_result=reference;func_00187EA8(actor);CHECK(FIELD(actor,0x234,void *)==object_storage&&FIELD(object_storage,0xA0,u32)==0x100);CHECK(nth(REFERENCE_WORD,0)->object==ADDRESS(reference,-4));
    reset();FIELD(actor,0x818,u32)=101;FIELD(actor,0x81C,float)=0;func_00188290(actor);CHECK(FIELD(actor,0x14,u32)==0&&count(REQUEST)==0&&FIELD(actor,0x74,u32)==0);
    reset();FIELD(actor,0x818,u32)=0;FIELD(actor,0x50,float)=-2;FIELD(actor,0x3C4,float)=1;FIELD(actor->field18,0x98,float)=1;control_result=1;FIELD(actor,0x820,void *)=handle;hook=handle_hook;request_result=0;func_00188290(actor);CHECK(count(BALLISTIC_MATRIX)==1&&nth(REQUEST,3)->word==0x69);CHECK(nth(HANDLE_FREE,0)->object==replacement_handle&&FIELD(actor,0x820,void *)==0);CHECK(FIELD(actor,0x818,u32)==101);
    reset();FIELD(actor,0x818,u32)=0;FIELD(actor,0x3C4,float)=1;FIELD(actor->field18,0x98,float)=1;control_result=1;func_00188290(actor);CHECK(count(BALLISTIC_MATRIX)==0&&nth(REQUEST,3)->word==0x68);
    reset();FIELD(actor,0x234,void *)=ADDRESS(actor,-0x40);FIELD(actor,0x818,u32)=100;FIELD(actor,0x40,float)=2;FIELD(actor,0x44,float)=3;FIELD(actor,0x48,float)=4;func_00188290(actor);CHECK(FIELD(actor,0x40,float)==2&&FIELD(actor,0x44,float)==3&&FIELD(actor,0x48,float)==4&&FIELD(actor,0x4C,float)==1);
    reset();FIELD(actor,0x234,void *)=ADDRESS(actor,-0x44);FIELD(actor,0x818,u32)=100;FIELD(actor,0x40,float)=2;FIELD(actor,0x44,float)=3;FIELD(actor,0x48,float)=4;func_00188290(actor);CHECK(FIELD(actor,0x3C,float)==2&&FIELD(actor,0x40,float)==3&&FIELD(actor,0x44,float)==4&&FIELD(actor,0x48,float)==1);
    reset();FIELD(actor,0x234,void *)=ADDRESS(actor,-0x3C);FIELD(actor,0x818,u32)=100;FIELD(actor,0x40,float)=2;FIELD(actor,0x44,float)=3;FIELD(actor,0x48,float)=4;func_00188290(actor);CHECK(FIELD(actor,0x44,float)==2&&FIELD(actor,0x48,float)==2&&FIELD(actor,0x4C,float)==2&&FIELD(actor,0x50,float)==1);
    reset();FIELD(actor,0x1B0,void *)=0;FIELD(actor,0x820,void *)=handle;FIELD(actor,0x234,void *)=object_storage;hook=handle_hook;func_00195B78(actor);CHECK(nth(CANCEL,0)->object==0&&nth(HANDLE_FREE,0)->object==replacement_handle);CHECK(FIELD(actor,0x234,void *)==0&&FIELD(actor,0x820,void *)==0);
    reset();FIELD(actor,0x1B0,void *)=0;func_00195B08(actor);CHECK(count(CANCEL)==0);FIELD(actor,0x1B0,void *)=primary;func_00195B08(actor);CHECK(count(CANCEL)==1);
}

static void selector_hook(Event *e)
{
    if(e->kind==MOTION){FIELD(actor,0x824,void *)=alternate_map;FIELD(alternate_parent,0x88,float)=1;FIELD(alternate_map,0x11C,float)=1;}
    if(e->kind==QUERY)FIELD(alternate_map,0x0C,void *)=parent;
}
static void test_selector(void)
{
    int i,j;float axis[4][2]={{0,1},{-1,0},{0,-1},{1,0}};
    reset();CHECK(func_00188590(actor)==4);CHECK(count(MOTION)==1&&count(QUERY)==0&&last_motion.x==0&&last_motion.y==0&&last_motion.z==0);
    for(i=0;i<4;++i){reset();FIELD(actor,0x68,float)=1;FIELD(map,0x114,float)=axis[i][0];FIELD(map,0x11C,float)=axis[i][1];CHECK(func_00188590(actor)==i);CHECK(count(NORMALIZE)==2);}
    for(i=-3;i<=3;++i)for(j=-3;j<=3;++j)if(i||j){s32 expected;reset();FIELD(actor,0x68,float)=1;FIELD(map,0x114,float)=(float)i;FIELD(map,0x11C,float)=(float)j;expected=abs(j)>abs(i)?(j>0?0:2):(i<0?1:3);CHECK(func_00188590(actor)==expected);}
    reset();FIELD(actor,0x68,float)=1;hook=selector_hook;CHECK(func_00188590(actor)==0);CHECK(nth(QUERY,0)->object==alternate_map);CHECK(FIELD(alternate_map,0x0C,void *)==parent);
}
static void query_hook(Event *e)
{if(e->kind==QUERY&&count(QUERY)==1){FIELD(actor,0x824,void *)=alternate_map;FIELD(point,0x28,float)=2;query_result=alternate_point;}else if(e->kind==QUERY&&count(QUERY)==2){FIELD(point,0x28,float)=99;FIELD(alternate_point,0x20,float)=3;}}
static void owner_hook(Event *e) {if(e->kind==OWNER)FIELD(actor,0x190,GeorgeActorBits64)=0x8000000000000001ULL;}
static void test_state23_init(void)
{
    int mode;for(mode=0;mode<3;++mode){reset();angles[0]=mode==0?4:0;angles[1]=mode==1?-4:1;angles[2]=2;func_00188968(actor);CHECK(count(QUERY)==(mode==0?4:6));near_value(FIELD(actor,0x848,float),mode==0?1-6.28318548202514648f:mode==1?2+6.28318548202514648f:2);CHECK(FIELD(actor,0x828,u32)==100&&FIELD(actor,0x834,u32)==0x84&&FIELD(actor,0x844,u32)==0x83);}
    reset();hook=query_hook;func_00188968(actor);CHECK(nth(QUERY,0)->object==map&&nth(QUERY,1)->object==alternate_map);CHECK(nth(ANGLE,0)->value==2&&nth(ANGLE,0)->word==bits(3));
    reset();hook=owner_hook;func_00188968(actor);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==0x8000000000040001ULL);
}
static void state23_mutation(Event *e)
{if(e->kind==REQUEST&&e->argument){FIELD(actor,0x828,u32)=999;FIELD(actor,0x824,void *)=alternate_map;}if(e->kind==ROUTE_UPDATE)FIELD(actor,0x824,void *)=map;}
static void cleanup_route_hook(Event *e)
{if(e->kind==ROUTE_CLEAR)FIELD(actor,0x824,void *)=alternate_map;else if(e->kind==ROUTE_MODE)FIELD(actor,0x824,void *)=map;}
static void test_state23(void)
{
    s32 phases[]={-1,0,100,101,102,200,201,202,300,301,302};int i,j;
    for(i=0;i<11;++i)for(j=0;j<2;++j){reset();FIELD(actor,0x828,s32)=phases[i];FIELD(actor,0x82C,float)=j?1:0;FIELD(actor,0x830+16,u32)=0x83;FIELD(actor,0x844,u32)=0x83;func_00188B10(actor);CHECK(count(A0)==1&&count(MOTION)==2);if(phases[i]==101){CHECK(count(V78)==1&&count(ROUTE_ADVANCE)==1&&count(REQUEST)==0);CHECK(FIELD(actor,0x828,u32)==(j?101u:200u));}else if(phases[i]==200||phases[i]==201){CHECK(count(VB0)==1&&count(ROUTE_UPDATE)==1);CHECK(count(REQUEST)==(phases[i]==200?2:0));}else if(phases[i]==301){CHECK(count(REQUEST)==0&&count(EXIT_STATE)==(!j));}else{CHECK(count(REQUEST)==2&&nth(REQUEST,1)->word==(phases[i]==300?0x88u:0x82u));}}
    reset();FIELD(actor,0x828,u32)=201;FIELD(actor,0x844,u32)=0x83;FIELD(actor,0x840,u32)=0x87;func_00188B10(actor);CHECK(FIELD(actor,0x844,u32)==0x87&&nth(REQUEST,1)->word==0x87&&count(VB0)==1);
    reset();FIELD(actor,0x828,u32)=200;hook=state23_mutation;route_result=0;func_00188B10(actor);CHECK(count(VB0)==1&&nth(QUERY,0)->object==alternate_map&&nth(ROUTE_STATUS,0)->object==map);CHECK(FIELD(actor,0x828,u32)==300);
    reset();FIELD(actor,0x828,u32)=301;((GeorgeGoalMember *)D_003F83F0)->selector=-1;((GeorgeGoalMember *)D_003F83F0)->adjustment=-12;((GeorgeGoalMember *)D_003F83F0)->target.direct=mock_enter;func_00188B10(actor);CHECK(actor->field0C==0&&last_enter==ADDRESS(actor,-12));
    reset();FIELD(actor,0x58,float)=3;FIELD(actor,0x848,float)=-3;FIELD(actor,0x35C,float)=0.1f;func_00188B10(actor);near_value(FIELD(actor,0x58,float),3-(float)(0.5*(6-6.28318548202514648)));
    reset();FIELD(actor,0x58,float)=-3;FIELD(actor,0x848,float)=3;FIELD(actor,0x35C,float)=0.1f;func_00188B10(actor);near_value(FIELD(actor,0x58,float),-3+(float)(0.5*(6-6.28318548202514648)));
    reset();FIELD(actor,0x190,GeorgeActorBits64)=0x8000000000044001ULL;hook=cleanup_route_hook;func_00195C30(actor);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==0x8000000000004001ULL&&FIELD(actor,0x824,void *)==0);CHECK(nth(ROUTE_MODE,0)->object==alternate_map&&nth(OWNER,0)->object==map);
}

static GeorgeMathVec3 *retained_sample;
static void state24_hook(Event *e)
{
    if(e->kind==SAMPLE)retained_sample=e->argument;
    if(e->kind==V78&&count(V78)==1){retained_sample->x=9;FIELD(actor,0x40,float)=50;}
    if(e->kind==LENGTH)FIELD(actor,0x85C,float)=2;
}
static void detach_hook(Event *e) {if(e->kind==DETACH)FIELD(actor,0x1B0,void *)=replacement;}
static void test_state24(void)
{
    s32 phases[]={-32768,-1,0,1,99,100,101,102,103,199,200,201,202,203,32767};int i,j;
    for(i=0;i<15;++i)for(j=0;j<2;++j){reset();FIELD(actor,0x854,s16)=(s16)phases[i];FIELD(actor,0x858,float)=j?1:0;func_00189310(actor);CHECK(count(A0)==1&&FIELD(actor,0x85C,float)==0.5f);if(phases[i]==0||phases[i]==100||phases[i]==200){CHECK(count(REQUEST)==2&&FIELD(actor,0x854,u16)==(phases[i]==200?201:101));}else if(phases[i]==102){CHECK(count(SAMPLE)==1&&count(V78)==2);CHECK(FIELD(actor,0x854,u16)==(j?102:200));}else if(phases[i]==202){CHECK(count(SAMPLE)==1&&count(V78)==1&&count(LENGTH)==1);}else CHECK(count(REQUEST)==0&&count(SAMPLE)==0&&FIELD(actor,0x854,s16)==phases[i]);}
    reset();FIELD(actor,0x854,u16)=100;request_result=0;func_00189310(actor);CHECK(FIELD(actor,0x854,u16)==102);reset();FIELD(actor,0x854,u16)=200;FIELD(actor,0x1B0,void *)=0;func_00189310(actor);CHECK(FIELD(actor,0x854,u16)==202&&count(REQUEST)==0);
    reset();FIELD(actor,0x854,u16)=102;FIELD(actor,0x44,float)=1;FIELD(actor,0x860,float)=2;hook=state24_hook;func_00189310(actor);CHECK(nth(SAMPLE,0)->value==0.5f);near_value(last_vector.x,90);near_value(last_vector.y,0);near_value(last_vector.z,40);CHECK(FIELD(actor,0x40,float)==50);
    reset();FIELD(actor,0x854,u16)=202;hook=state24_hook;((GeorgeGoalMember *)(D_003F83F0+3*28))->selector=-1;((GeorgeGoalMember *)(D_003F83F0+3*28))->adjustment=16;((GeorgeGoalMember *)(D_003F83F0+3*28))->target.direct=mock_enter;func_00189310(actor);CHECK(actor->field0C==3&&last_enter==ADDRESS(actor,16));
    reset();FIELD(actor,0x854,u16)=102;FIELD(actor,0x858,float)=from_bits(0x7FC01234);func_00189310(actor);CHECK(FIELD(actor,0x854,u16)==102);
    reset();FIELD(actor,0x854,u16)=100;FIELD(actor,0x856,u16)=99;func_00195D08(actor);CHECK(count(WORD)==1&&FIELD(actor,0x854,u16)==0&&FIELD(actor,0x856,u16)==0);
    reset();hook=detach_hook;func_00195D50(actor);CHECK(nth(DETACH,0)->object==map&&nth(CANCEL,0)->object==replacement&&FIELD(actor,0x84C,void *)==map);
}
static void duration_data_hook(Event *e)
{if(e->kind==DURATION){actor->field18=(GeorgeGoalEntityData *)alternate_data;FIELD(alternate_data,0x1B0,float)=3;FIELD(actor,0x804,float)=4;FIELD(actor,0x808,float)=5;FIELD(actor,0x80C,float)=6;}}
static void vector_timer_hook(Event *e)
{if(e->kind==VB0){FIELD(actor,0x814,float)=0.5f;FIELD(actor,0x35C,float)=0.5f;}}
static void soft23_hook(Event *e)
{
    if(e->kind==SOFT_CMP&&count(SOFT_CMP)==2)FIELD(actor,0x848,float)=8;
    if(e->kind==SOFT_CMP&&count(SOFT_CMP)==3){FIELD(actor,0x848,float)=10;FIELD(actor,0x58,float)=1;}
    if(e->kind==SOFT_SUB){FIELD(actor,0x848,float)=-100;FIELD(actor,0x58,float)=100;}
}
static void soft24_hook(Event *e)
{
    if(e->kind==SOFT_CMP&&count(SOFT_CMP)==1)FIELD(actor,0x58,float)=2;
    if(e->kind==SOFT_SUB&&count(SOFT_SUB)==1)FIELD(actor,0x58,float)=99;
}
static void control24_hook(Event *e)
{if(e->kind==A0){FIELD(actor,0x84C,void *)=alternate_map;FIELD(alternate_map,0x48,float)=4;FIELD(actor,0x35C,float)=0.5f;FIELD(actor,0x854,u16)=102;FIELD(actor,0x40,float)=50;}}
static void fresh_route_hook(Event *e)
{if(e->kind==V78){FIELD(actor,0x82C,float)=-1;FIELD(actor,0x824,void *)=alternate_map;}}
static void member_virtual_hook(Event *e)
{if(e->kind==EXIT_STATE)FIELD(actor,0x20,void *)=control_table;}
static void test_captures(void)
{
    reset();hook=duration_data_hook;func_00195A88(0,actor,reference);near_value(FIELD(actor,0x804,float),12);near_value(FIELD(actor,0x808,float),15);near_value(FIELD(actor,0x80C,float),18);CHECK(actor->field18==(GeorgeGoalEntityData *)alternate_data);
    reset();actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x664);FIELD(actor,0x804,float)=4;FIELD(actor,0x808,float)=5;FIELD(actor,0x80C,float)=6;func_00195A88(0,actor,reference);near_value(FIELD(actor,0x804,float),4);near_value(FIELD(actor,0x808,float),5);near_value(FIELD(actor,0x80C,float),6);
    reset();FIELD(actor,0x810,u32)=1;FIELD(actor,0x814,float)=100;hook=vector_timer_hook;func_00187BA0(actor);CHECK(FIELD(actor,0x14,u32)==0&&FIELD(actor,0x814,float)==0);
    reset();FIELD(actor,0x848,float)=6;hook=soft23_hook;func_00188B10(actor);near_value(FIELD(actor,0x58,float),(float)((8.0-6.28318548202514648)*1.25));CHECK(count(SOFT_CMP)==3);
    reset();FIELD(actor,0x854,u16)=202;FIELD(actor,0x58,float)=3;angles[0]=angles[1]=angles[2]=-3;hook=soft24_hook;func_00189310(actor);near_value(FIELD(actor,0x58,float),(float)(2+(6.28318548202514648-5)*1.25));
    reset();FIELD(actor,0x854,u16)=1;hook=control24_hook;func_00189310(actor);CHECK(nth(SAMPLE,0)->object==alternate_map&&nth(SAMPLE,0)->value==2);near_value(last_vector.x,20);CHECK(FIELD(actor,0x854,u16)==200);
    reset();FIELD(actor,0x828,u32)=101;FIELD(actor,0x82C,float)=1;hook=fresh_route_hook;func_00188B10(actor);CHECK(FIELD(actor,0x828,u32)==200&&nth(ROUTE_ADVANCE,0)->object==alternate_map);
    reset();FIELD(actor,0x828,u32)=301;hook=member_virtual_hook;
    ((GeorgeGoalMember *)D_003F83F0)->selector=1;
    ((GeorgeGoalMember *)D_003F83F0)->adjustment=30000;
    ((GeorgeGoalMember *)D_003F83F0)->target.vtable_offset=0x20;
    ((GeorgeGoalVirtualVoid *)control_table)->adjustment=10000;
    ((GeorgeGoalVirtualVoid *)control_table)->invoke=mock_enter;
    func_00188B10(actor);CHECK(last_enter==ADDRESS(actor,40000));
}
int main(void)
{
    CHECK(sizeof(void *)==4);test_callbacks();test_state20();test_state21();test_state22();
    test_selector();test_state23_init();test_state23();test_state24();test_captures();
    printf("actor_states4: %d checks passed\n",checks);return 0;
}
