/* Asset-free call-boundary tests. Expected state outcomes and callback
 * mutations are independent specifications; no retail instructions are used. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_states5.c"

enum { A0=1, WORD, VOID38, GATE, V78, VB0, PREDICATE, CONTROL_INT,
       REQUEST, DURATION, CANCEL, SET_FLAG, CLEAR_FLAG, ANGLE, NORMALIZE,
       QUERY, MOTION, OWNER, ROUTE_MODE, ROUTE_CLEAR, ROUTE_ADVANCE,
       ROUTE_STOP, ROUTE_UPDATE, ROUTE_STATUS, SAMPLE, LENGTH, DETACH,
       EXIT_STATE, ENTER_STATE, HANDLE_START, HANDLE_STOP, HANDLE_FREE,
       ALLOCATE, CONSTRUCT_EFFECT, HASH, EMIT, UPPER_BOUND, INSERT, AT_EXIT,
       GENERIC, FIND_REFERENCE, CONSTRUCT_OBJECT, MATRIX_COPY, REFERENCE_WORD,
       RELEASE, BALLISTIC_CLEAR, BALLISTIC_MATRIX, BALLISTIC_RECORD,
       COLLISION, VECTOR_BUILD, CLASSIFY, FIRST_DIRECTION, SECOND_DIRECTION, DOT_DIRECTION, DISTANCE, DIRECTION_WORD, POINT_NORMAL, ZERO_MEMORY, VFLOAT, SOFT_CONVERT, SOFT_CMP, SOFT_SUB, SOFT_MUL, SOFT_ADD, SOFT_FLOAT };
typedef struct Event { int kind; void *object, *argument; u32 word, other; float value; } Event;
static Event events[256];
static int event_count, checks;
static u8 entity_storage[0xB00] __attribute__((aligned(16)));
static u8 data_storage[0x300], alternate_data[0x300], primary[64], companion[64], replacement[64];
static u8 object_storage[0x400], handle[64], reference[0x100];
static u8 map[0x400], alternate_map[0x400], parent[0x200], alternate_parent[0x200];
static u8 point[0x100], alternate_point[0x100], reference_table[0xB0];
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
const u8 D_00421160[]={0}, D_0042D8A8[]={1}, D_0042D8C0[]={2}, D_0042DA38[]={3};
const GeorgeActorUnalignedPosition D_0042D8D8={0x3F80000000000000ULL,0};
u8 D_003F83F0[42*28] __attribute__((aligned(8)));

static void check(int value,const char *label,int line)
{ ++checks; if(!value){fprintf(stderr,"actor_states5:%d: %s\n",line,label);exit(1);} }
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


static s32 collision_result,first_direction_result,second_direction_result,direction_value;
static float collision_weight,dot_result,distance_result;
static GeorgeMathVec3 collision_point,build_vector,query_point,query_normal;
static GeorgeMathVec3 *retained_query_point,*retained_normal;
static float *retained_weight;
static s32 *retained_direction;
static int point_query_count;
static s32 mock_collision(void *self,const GeorgeMathVec3 *input,GeorgeMathVec3 *output,float *weight)
{ CHECK(self==ADDRESS(object_storage,-8));CHECK(input==VECTOR(actor,0x40));*output=collision_point;*weight=collision_weight;retained_weight=weight;emit(COLLISION,self,output,0,0,0);return collision_result; }
static void mock_float(void *self,float value) {emit(VFLOAT,self,0,0,0,value);}
void func_00119888(void *object,GeorgeMathVec3 *output,float first,float second)
{*output=build_vector;emit(VECTOR_BUILD,object,output,bits(second),0,first);}
u32 func_00119A58(void *object,const GeorgeMathVec3 *vector)
{last_motion=*vector;emit(CLASSIFY,object,(void *)vector,0,0,0);return 0x91;}
s32 func_00119BC8(void *object,const GeorgeMathVec3 *input,s32 *direction)
{*direction=direction_value;retained_direction=direction;emit(FIRST_DIRECTION,object,(void *)input,0,0,0);return first_direction_result;}
s32 func_00119D58(void *object,const GeorgeMathVec3 *input,s32 *direction)
{*direction=direction_value;retained_direction=direction;emit(SECOND_DIRECTION,object,(void *)input,0,0,0);return second_direction_result;}
float func_00119EE8(const GeorgeMathDirectionsFC *object,s32 direction,const GeorgeMathVec3 *vector)
{last_motion=*vector;emit(DOT_DIRECTION,(void *)object,(void *)vector,(u32)direction,0,0);return dot_result;}
float func_0011A6E8(void *object,s32 direction,const GeorgeMathVec3 *input)
{last_motion=*input;emit(DISTANCE,object,(void *)input,(u32)direction,0,0);return distance_result;}
u32 func_0011A7A8(void *object,s32 direction)
{emit(DIRECTION_WORD,object,0,(u32)direction,0,0);return direction==2?0x8D:direction==3?0x8E:0;}
void func_0013D118(void *object,const GeorgeMathVec3 *input,GeorgeMathVec3 *output,GeorgeMathVec3 *normal,float unused)
{last_motion=*input;*output=query_point;*normal=query_normal;retained_query_point=output;retained_normal=normal;++point_query_count;emit(POINT_NORMAL,object,output,0,0,unused);}
void *func_003936A0(void *memory,s32 value,u32 size)
{memset(memory,value,size);emit(ZERO_MEMORY,memory,0,(u32)value,size,0);return memory;}
void func_001A5E90(void *object,void *value) {emit(DETACH,object,value,0,0,0);}
void func_00251CC8(void *object,u32 key) {emit(EMIT,object,0,key,0,0);}
float func_002A35C0(GeorgeMathVec3 *output,const GeorgeMathVec3 *input,float scale)
{GeorgeMathVec3 v=*input;float length=func_002A3538(&v);output->x=v.x*scale;output->y=v.y*scale;output->z=v.z*scale;return length;}
static void reset5(void)
{
 reset(); collision_result=1;first_direction_result=0;second_direction_result=0;direction_value=2;
 collision_weight=1;dot_result=1;distance_result=1;point_query_count=0;retained_query_point=0;retained_normal=0;retained_direction=0;retained_weight=0;
 build_vector.x=1;build_vector.y=2;build_vector.z=3;collision_point.x=3;collision_point.y=4;collision_point.z=5;
 query_point.x=10;query_point.y=20;query_point.z=30;query_normal.x=1;query_normal.y=0;query_normal.z=0;
 FIELD(actor,0x24,void *)=object_storage;FIELD(object_storage,4,void *)=reference_table;
 FIELD(actor,0x868,void *)=map;FIELD(data_storage,0x1C,float)=2;FIELD(data_storage,0x1B0,float)=2;
 FIELD(data_storage,0x230,float)=3;FIELD(data_storage,0x234,float)=2;FIELD(data_storage,0x238,float)=4;FIELD(data_storage,0x244,float)=0.5f;
 ((GeorgeActorVirtualPointVectorScalar *)(reference_table+0x98))->adjustment=-8;
 ((GeorgeActorVirtualPointVectorScalar *)(reference_table+0x98))->invoke=mock_collision;
 ((GeorgeGoalVirtualVoid *)(control_table+0x88))->adjustment=40;
 ((GeorgeGoalVirtualVoid *)(control_table+0x88))->invoke=mock_void;
 ((GeorgeGoalVirtualFloat *)(control_table+0xC0))->adjustment=44;
 ((GeorgeGoalVirtualFloat *)(control_table+0xC0))->invoke=mock_float;
 ((GeorgeActorVirtualVectorInput *)(control_table+0x98))->adjustment=20;
 ((GeorgeActorVirtualVectorInput *)(control_table+0x98))->invoke=mock_vector;
}

static u32 watched_phase, seen_phase;
static void duration_hook(Event *e)
{if(e->kind==DURATION){seen_phase=FIELD(actor,watched_phase,u32);FIELD(actor,watched_phase,u32)=77;}}
static void request_reload(Event *e)
{if(e->kind==REQUEST&&count(REQUEST)==1){FIELD(actor,0x1B0,void *)=replacement;FIELD(actor,0x368,u32)=7;}}
static void test_duration_and_lifecycle(void)
{
 GeorgeActorRequestCallback callbacks[]={func_00195E88,func_00195F68,func_00195FE8,func_00196088};
 u32 phases[]={0x880,0x8B8,0x8E0,0xA14},timers[]={0x86C,0x8BC,0x8F0,0xA18};
 u32 initial[]={0,1,0x7FFFFFFF,0xFFFFFFFF};int i,j;
 for(i=0;i<4;++i)for(j=0;j<4;++j){reset5();watched_phase=phases[i];FIELD(actor,watched_phase,u32)=initial[j];hook=duration_hook;callbacks[i](0,actor,reference);CHECK(seen_phase==(i==0?initial[j]+1u:initial[j]));CHECK(FIELD(actor,watched_phase,u32)==(i==0?77u:i==3?200u:78u));near_value(FIELD(actor,timers[i],float),1);if(i==0)near_value(FIELD(actor,0x870,float),1);if(i==3)near_value(FIELD(actor,0xA20,float),2);}
 reset5();actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x868);FIELD(actor,0xA18,float)=9;func_00196088(0,actor,reference);near_value(FIELD(actor,0xA20,float),1);
 reset5();FIELD(actor,0x9EC,u32)=99;func_00195ED8(actor);CHECK(FIELD(actor,0x9EC,u32)==0&&nth(WORD,0)->word==0);
 reset5();func_00195F10(actor);CHECK(count(CANCEL)==2&&nth(WORD,0)->word==1);reset5();FIELD(actor,0x1B0,void *)=0;func_00195F10(actor);CHECK(count(CANCEL)==1&&nth(CANCEL,0)->object==0);
 reset5();func_00195FB0(actor);CHECK(FIELD(actor,0x14,u32)==3&&FIELD(actor,0x24,void *)==0&&count(CANCEL)==1);
 reset5();FIELD(actor,0x8F4,float)=2;FIELD(actor,0x9EC,u32)=9;func_00196030(actor);CHECK(FIELD(actor,0x8E0,u32)==100&&FIELD(actor,0x58,float)==2&&FIELD(actor,0x9EC,u32)==0&&FIELD(actor,0x8F0,float)==0&&FIELD(actor,0x8EC,float)==0);
 reset5();FIELD(actor,0x8F8,float)=1;FIELD(actor,0x8FC,float)=2;FIELD(actor,0x900,float)=3;func_00196050(actor);CHECK(count(CANCEL)==1&&FIELD(actor,0x8F8,u32)==0&&FIELD(actor,0x8FC,u32)==0&&FIELD(actor,0x900,u32)==0);
 reset5();FIELD(actor,0xA14,u32)=999;func_001960E8(actor);CHECK(FIELD(actor,0xA14,u32)==100&&FIELD(actor,0xA18,u32)==0&&FIELD(actor,0xA1C,u32)==0&&count(CLEAR_FLAG)==1);
 reset5();func_00196120(actor);CHECK(count(VOID38)==1&&count(SET_FLAG)==1&&nth(SET_FLAG,0)->word==0x40000&&count(CANCEL)==1);
}
static void collision_mutation(Event *e)
{
 if(e->kind==SOFT_CMP&&count(SOFT_CMP)==1)FIELD(actor,0x34,float)=0.2f;
 if(e->kind==DISTANCE&&retained_direction)*retained_direction=3;
}
static void inactive_mutation(Event *e)
{if(e->kind==REQUEST){FIELD(actor,0x190,GeorgeActorBits64)|=(1ULL<<38)|0x100ULL;FIELD(actor,0x34,float)=5;FIELD(actor,0x38,float)=6;}}
static void test_state26(void)
{
 int i,j;float weights[]={1,0.49f,0.25f};
 reset5();FIELD(object_storage,0xFC,float)=1;FIELD(object_storage,0x100,float)=4;FIELD(object_storage,0x104,float)=0;angles[0]=angles[1]=angles[2]=0.4f;func_0018ACC8(actor);CHECK(count(NORMALIZE)==1&&count(ANGLE)==3&&FIELD(actor,0x8B8,u32)==100&&FIELD(actor,0x8BC,u32)==0);near_value(FIELD(actor,0x58,float),0.4f);CHECK(FIELD(actor,0x8C4,u32)==0&&FIELD(actor,0x8CC,u32)==0&&FIELD(actor,0x8D4,u32)==0);
 reset5();FIELD(actor,0x8B8,u32)=100;hook=request_reload;request_result=0;func_0018AE08(actor);CHECK(count(REQUEST)==2&&nth(REQUEST,1)->object==replacement&&nth(REQUEST,1)->argument==(void *)func_00195F68&&FIELD(actor,0x8B8,u32)==100);
 reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x24,void *)=0;FIELD(actor,0x34,float)=2;FIELD(actor,0x38,float)=3;FIELD(actor,0x190,GeorgeActorBits64)=(1ULL<<38)|0x100;func_0018AE08(actor);CHECK(count(COLLISION)==0&&FIELD(actor,0x14,u32)==3&&FIELD(actor,0x34,float)==2&&FIELD(actor,0x38,float)==3&&FIELD(actor,0x190,GeorgeActorBits64)==0x100);
 reset5();FIELD(actor,0x8B8,u32)=101;collision_result=0;FIELD(actor,0x34,float)=2;FIELD(actor,0x38,float)=3;FIELD(actor,0x190,GeorgeActorBits64)=(1ULL<<38)|0x100;func_0018AE08(actor);CHECK(FIELD(actor,0x24,void *)==0&&FIELD(actor,0x14,u32)==3&&FIELD(actor,0x34,u32)==0&&FIELD(actor,0x38,u32)==0&&FIELD(actor,0x190,GeorgeActorBits64)==0x100);
 for(i=0;i<3;++i){reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;collision_weight=weights[i];FIELD(object_storage,0xFC,float)=2;FIELD(object_storage,0x100,float)=4;FIELD(object_storage,0x104,float)=6;func_0018AE08(actor);near_value(last_vector.x,2+(weights[i]<0.49f?(0.5f-weights[i])*2:0));near_value(last_vector.y,4+(weights[i]<0.49f?(0.5f-weights[i])*4:0));near_value(last_vector.z,6+(weights[i]<0.49f?(0.5f-weights[i])*6:0));CHECK(count(CLASSIFY)==1&&count(REQUEST)==2&&nth(REQUEST,1)->word==0x91&&FIELD(actor,0x34,u32)==0&&FIELD(actor,0x38,u32)==0);}
 for(i=0;i<4;++i)for(j=0;j<2;++j){reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;FIELD(actor,0x190,GeorgeActorBits64)=(1ULL<<38)|0x100;first_direction_result=1;direction_value=i;distance_result=j?1:0;func_0018AE08(actor);CHECK(count(DOT_DIRECTION)==1&&count(SECOND_DIRECTION)==0);if(i>=2){CHECK(count(DISTANCE)==1);if(!j){CHECK(count(NORMALIZE)==1&&count(REQUEST)==0&&FIELD(actor,0x190,GeorgeActorBits64)==((1ULL<<38)|0x100));near_value(last_vector.x,0);near_value(FIELD(actor,0x8D0,float),-2/sqrtf(56));}else{CHECK(count(DIRECTION_WORD)==1&&nth(REQUEST,1)->word==(i==2?0x8D:0x8E)&&FIELD(actor,0x190,GeorgeActorBits64)==((1ULL<<38)|0x100));}}else CHECK(count(CLASSIFY)==1&&count(DISTANCE)==0&&FIELD(actor,0x190,GeorgeActorBits64)==0x100);}
 reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;second_direction_result=1;func_0018AE08(actor);CHECK(count(SECOND_DIRECTION)==1&&count(CLASSIFY)==0&&nth(REQUEST,1)->word==0x8C);near_value(last_vector.x,0);
 for(j=0;j<2;++j){reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x8D0,float)=7;FIELD(actor,0x8D4,float)=8;FIELD(actor,0x8D8,float)=9;first_direction_result=1;distance_result=j?0.19f:0.18f;hook=inactive_mutation;func_0018AE08(actor);CHECK(count(VECTOR_BUILD)==0&&count(DISTANCE)==1&&nth(REQUEST,1)->word==0x8C&&FIELD(actor,0x190,GeorgeActorBits64)==0x100&&FIELD(actor,0x34,u32)==0&&FIELD(actor,0x38,u32)==0);near_value(last_vector.x,j?0:7);}
 reset5();FIELD(actor,0x8B8,u32)=101;first_direction_result=1;hook=collision_mutation;func_0018AE08(actor);CHECK(count(VECTOR_BUILD)==1&&nth(DIRECTION_WORD,0)->word==3&&nth(REQUEST,1)->word==0x8E);
 reset5();FIELD(actor,0x8B8,u32)=102;FIELD(actor,0x8BC,float)=1;func_0018AE08(actor);CHECK(event_count==0&&FIELD(actor,0x8BC,float)==0.75f);
 reset5();angles[0]=angles[1]=4;func_0018ACC8(actor);CHECK(count(ANGLE)==2);near_value(FIELD(actor,0x58,float),4-6.28318548202514648f);
 reset5();angles[0]=0;angles[1]=angles[2]=-4;func_0018ACC8(actor);CHECK(count(ANGLE)==3);near_value(FIELD(actor,0x58,float),-4+6.28318548202514648f);
}
static void phase27_hook(Event *e)
{if(e->kind==CONTROL_INT){FIELD(actor,0x8E0,u32)=55;FIELD(actor,0x8EC,float)=1;}}
static void phase28_hook(Event *e)
{if(e->kind==VB0)FIELD(actor,0xA14,u32)=77;}
static void test_states27_28(void)
{
 s32 phases[]={-1,0,99,100,101,102,103,200,201,202,203};float timers[]={-1,0,0.25f,1,0};int i,j,k;timers[4]=from_bits(0x7FC01234);
 for(i=0;i<11;++i)for(j=0;j<5;++j){reset5();FIELD(actor,0x8E0,s32)=phases[i];FIELD(actor,0x8F0,float)=timers[j];FIELD(actor,0x8E8,float)=4;FIELD(actor,0x8EC,float)=2;FIELD(actor,0x8F4,float)=3;func_0018B528(actor);if(phases[i]==100){CHECK(count(REQUEST)==2&&count(V78)==1&&FIELD(actor,0x8E0,s32)==100);}else if(phases[i]==101){CHECK(FIELD(actor,0x8E0,u32)==(0<=timers[j]-0.25f?101u:102u));CHECK(FIELD(actor,0x8EC,float)==2.25f&&FIELD(actor,0x8E8,float)==3.75f&&FIELD(actor,0x58,float)==3);}else CHECK(count(REQUEST)==0&&count(CONTROL_INT)==0);if(phases[i]==102)CHECK(actor->field0C==3);}
 reset5();FIELD(actor,0x8E0,u32)=100;request_result=0;func_0018B528(actor);CHECK(FIELD(actor,0x8E0,u32)==102&&count(V78)==0);
 reset5();FIELD(actor,0x8E0,u32)=101;FIELD(actor,0x8F0,float)=1;FIELD(actor,0x3C4,float)=5;FIELD(actor,0x50,float)=2;control_result=1;hook=phase27_hook;func_0018B528(actor);CHECK(FIELD(actor,0x8E0,u32)==56&&FIELD(actor,0x14,u32)==5&&FIELD(actor,0x3C4,float)==2);
 for(i=0;i<11;++i)for(j=0;j<5;++j){reset5();FIELD(actor,0xA14,s32)=phases[i];FIELD(actor,0xA18,float)=timers[j];FIELD(actor,0x478,float)=3;FIELD(actor,0x480,float)=4;func_0018B7E8(actor);if(phases[i]==100){CHECK(count(REQUEST)==2&&FIELD(actor,0xA14,u32)==100);}else if(phases[i]==200){CHECK(count(VB0)==1&&FIELD(actor,0xA14,u32)==201);}else if(phases[i]==201){CHECK(count(VFLOAT)==(0.25f<timers[j]));CHECK(FIELD(actor,0xA14,u32)==(0.25f<timers[j]?201u:202u));if(count(VFLOAT))near_value(nth(VFLOAT,0)->value,0.25f);}else if(phases[i]==202){CHECK(count(EXIT_STATE)==1&&actor->field0C==3);}else CHECK(event_count==0);}
 reset5();FIELD(actor,0xA14,u32)=100;request_result=0;func_0018B7E8(actor);CHECK(FIELD(actor,0xA14,u32)==201);
 reset5();FIELD(actor,0xA14,u32)=200;FIELD(actor,0xA18,float)=2;FIELD(actor,0x478,float)=3;FIELD(actor,0x480,float)=4;hook=phase28_hook;func_0018B7E8(actor);near_value(FIELD(actor,0x478,float),0.6f);near_value(FIELD(actor,0x480,float),0.8f);CHECK(FIELD(actor,0xA14,u32)==78);
 for(k=0;k<4;++k){reset5();FIELD(actor,0xA14,u32)=202;FIELD(actor,0x190,GeorgeActorBits64)=k==1?0x80:k==2?0x80000:0;predicate_result=k!=3;func_0018B7E8(actor);CHECK(actor->field0C==(k==0?3u:0u)&&count(PREDICATE)==(k==0||k==3));}
}
static void test_state29(void)
{
 int i,j,k;u32 flags[]={0,0x20000};s32 phases[]={-1,100,101,200,201};
 for(i=0;i<2;++i)for(j=0;j<2;++j){reset5();FIELD(actor,0x190,GeorgeActorBits64)=flags[i];gate_result=j;func_0018BC28(actor);CHECK(count(HASH)==1&&count(EMIT)==1&&count(GATE)==!i&&count(CLEAR_FLAG)==(!i&&j)&&FIELD(actor,0x904,u32)==100);reset5();FIELD(actor,0x190,GeorgeActorBits64)=flags[i];gate_result=j;func_00196210(actor);CHECK(count(GATE)==i&&count(SET_FLAG)==(i&&j)&&count(CANCEL)==1);}
 reset5();FIELD(actor,0x904,u32)=101;func_0018BC28(actor);CHECK(count(HASH)==0&&FIELD(actor,0x904,u32)==100);
 reset5();D_003F2D40=0;func_0018BC28(actor);CHECK(allocate_count==2&&registry[0]==&effect_record&&count(AT_EXIT)==1);
 reset5();D_003F2D40=0;D_0046A0F0.field08=registry;func_0018BC28(actor);CHECK(count(INSERT)==1);
 for(i=0;i<5;++i)for(j=0;j<2;++j)for(k=0;k<2;++k){reset5();FIELD(actor,0x904,s32)=phases[i];request_result=j;predicate_result=k;func_0018BDA0(actor);CHECK(count(A0)==1);if(phases[i]==100)CHECK(FIELD(actor,0x904,u32)==(j?101u:200u)&&count(REQUEST)==2);else if(phases[i]==101)CHECK(FIELD(actor,0x904,u32)==(k?200u:101u)&&count(PREDICATE)==1);else if(phases[i]==200)CHECK(FIELD(actor,0x14,u32)==(k?3u:0u)&&count(PREDICATE)==1);else CHECK(count(PREDICATE)==0&&count(REQUEST)==0);}
}
static void state25_mutation(Event *e)
{
 if(e->kind==REQUEST&&nth(REQUEST,0)==e&&retained_query_point){retained_query_point->x=15;FIELD(actor,0x870,float)=2;}
 if(e->kind==A0){FIELD(actor,0x880,u32)=100;FIELD(actor,0x87C,float)=4;}
}
static void state25_state_switch(Event *e)
{if(e->kind==ENTER_STATE)control_result=1;}
static void test_state25(void)
{
 s32 phases[]={-2147483647-1,-1,0,99,100,101,102,200,201,250,300,301,400,401,402,2147483647};
 float timers[]={-1,0,0.25f,1,0};int i,j,k;timers[4]=from_bits(0x7FC01234);
 for(i=0;i<16;++i)for(j=0;j<5;++j){reset5();FIELD(actor,0x880,s32)=phases[i];FIELD(actor,0x86C,float)=timers[j];FIELD(actor,0x870,float)=1;FIELD(actor,0x87C,float)=2;((GeorgeActorVirtualVectorInput *)(control_table+0xB8))->invoke=mock_vector;func_00189E88(actor);CHECK(count(A0)==1);if(phases[i]==100)CHECK(count(REQUEST)==2&&nth(REQUEST,1)->word==0x9B&&FIELD(actor,0x58,float)==2&&FIELD(actor,0x880,u32)==100);else if(phases[i]==101||phases[i]==401)CHECK(FIELD(actor,0x880,u32)==(timers[j]-0.25f<=0?200u:(u32)phases[i]));else if(phases[i]==300)CHECK(point_query_count==2&&count(ZERO_MEMORY)==1&&nth(REQUEST,1)->word==0xA3);else if(phases[i]==400)CHECK(point_query_count==1&&count(ZERO_MEMORY)==1&&nth(REQUEST,1)->word==0xA4);else if(phases[i]==301)CHECK(count(DETACH)==(timers[j]-0.25f<0.25f));else if(phases[i]==200)CHECK(count(REQUEST)==2&&nth(REQUEST,1)->word==0x9A);else if(phases[i]==250)CHECK(count(CONTROL_INT)==1);else CHECK(point_query_count==0&&count(REQUEST)==0);}
 reset5();FIELD(actor,0x880,u32)=101;FIELD(actor,0x86C,float)=1;func_00189E88(actor);near_value(FIELD(actor,0x890,float),12);near_value(FIELD(actor,0x894,float),17);near_value(FIELD(actor,0x898,float),30);near_value(last_vector.x,120);near_value(last_vector.y,170);near_value(last_vector.z,300);near_value(last_motion.y,3);
 for(k=0;k<4;++k){reset5();FIELD(actor,0x880,u32)=200;FIELD(actor,0x190,GeorgeActorBits64)=k==1?0x8000:k==2?0x10000:k==3?0x18000:0;func_00189E88(actor);near_value(FIELD(actor,0x40,float),12);near_value(FIELD(actor,0x44,float),17);near_value(FIELD(actor,0x48,float),30);CHECK(FIELD(actor,0x880,u32)==(k==0?200u:k==1?250u:300u));CHECK(count(REQUEST)==(k==1?4:2));}
 reset5();FIELD(actor,0x880,u32)=300;FIELD(actor,0x870,float)=1;func_00189E88(actor);near_value(FIELD(actor,0x884,float),-4);near_value(FIELD(actor,0x888,float),3);near_value(FIELD(actor,0x40,float),-1);near_value(FIELD(actor,0x44,float),0.75f);CHECK(FIELD(actor,0x880,u32)==300);
 reset5();FIELD(actor,0x880,u32)=300;FIELD(actor,0x870,float)=1;hook=state25_mutation;func_00189E88(actor);CHECK(FIELD(actor,0x880,u32)==100&&nth(REQUEST,1)->word==0x9B);
 reset5();FIELD(actor,0x880,u32)=400;FIELD(actor,0x87C,float)=2;func_00189E88(actor);near_value(FIELD(actor,0x884,float),2);near_value(FIELD(actor,0x888,float),-3);near_value(FIELD(actor,0x890,float),10);near_value(FIELD(actor,0x58,float),2);
 for(k=0;k<2;++k){reset5();FIELD(actor,0x880,u32)=k?401:301;FIELD(actor,0x86C,float)=1;FIELD(actor,0x870,float)=2;FIELD(actor,0x40,float)=4;FIELD(actor,0x44,float)=5;FIELD(actor,0x48,float)=6;FIELD(actor,0x890,float)=1;FIELD(actor,0x894,float)=2;FIELD(actor,0x898,float)=3;FIELD(actor,0x884,float)=4;FIELD(actor,0x888,float)=8;FIELD(actor,0x88C,float)=12;func_00189E88(actor);near_value(FIELD(actor,0x40,float),13.5f);near_value(FIELD(actor,0x44,float),24);near_value(FIELD(actor,0x48,float),34.5f);CHECK(FIELD(actor,0x890,float)==10&&FIELD(actor,0x894,float)==20&&FIELD(actor,0x898,float)==30);}
 reset5();FIELD(actor,0x880,u32)=250;FIELD(actor,0x8A0,float)=10;((GeorgeActorVirtualVectorInput *)(control_table+0xB8))->invoke=mock_vector;((GeorgeGoalMember *)(D_003F83F0+3*28))->selector=-1;((GeorgeGoalMember *)(D_003F83F0+3*28))->target.direct=mock_enter;hook=state25_state_switch;func_00189E88(actor);CHECK(count(EXIT_STATE)==2&&actor->field0C==5&&count(CONTROL_INT)==1);
}
static int hook_mode;
static void capture_hook(Event *e)
{
 if(hook_mode==1&&e->kind==VECTOR_BUILD){*retained_weight=0.25f;FIELD(actor,0x24,void *)=replacement;FIELD(replacement,0xFC,float)=2;FIELD(replacement,0x100,float)=4;FIELD(replacement,0x104,float)=6;}
 if(hook_mode==2&&e->kind==DOT_DIRECTION)*retained_direction=1;
 if(hook_mode==3&&e->kind==REQUEST&&count(REQUEST)==1){retained_query_point->x=15;FIELD(actor,0x870,float)=2;}
 if(hook_mode==4&&e->kind==V78)FIELD(actor,0x86C,float)=-1;
 if(hook_mode==5&&e->kind==V78)FIELD(actor,0x190,GeorgeActorBits64)=0x18000;
 if(hook_mode==6&&e->kind==POINT_NORMAL){FIELD(actor,0x880,u32)=401;FIELD(actor,0x890,float)=5;FIELD(actor,0x884,float)=8;}
 if(hook_mode==7&&e->kind==CANCEL&&count(CANCEL)==1)FIELD(actor,0x1B0,void *)=replacement;
 if(hook_mode==8&&e->kind==WORD)FIELD(actor,0x1B0,void *)=replacement;
}
static void test_captures(void)
{
 int i;float inputs[]={0,0.1f,0.10000001f,-0.1f,-0.10000001f};
 for(i=0;i<5;++i){reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=inputs[i];func_0018AE08(actor);CHECK(count(VECTOR_BUILD)==(fabsf(inputs[i])>0.1f));}
 reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;hook_mode=1;hook=capture_hook;func_0018AE08(actor);near_value(last_vector.x,2.5f);near_value(last_vector.y,5);near_value(last_vector.z,7.5f);CHECK(nth(FIRST_DIRECTION,0)->object==replacement&&nth(CLASSIFY,0)->object==replacement);
 reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;first_direction_result=1;hook_mode=2;hook=capture_hook;func_0018AE08(actor);CHECK(count(DISTANCE)==0&&count(CLASSIFY)==1);
 reset5();FIELD(actor,0x8B8,u32)=101;FIELD(actor,0x38,float)=0.2f;build_vector.x=2;build_vector.y=3;build_vector.z=4;actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x8A8);func_0018AE08(actor);near_value(last_vector.x,4);near_value(last_vector.y,6);near_value(last_vector.z,8);
 reset5();FIELD(actor,0x880,u32)=300;FIELD(actor,0x870,float)=1;hook_mode=3;hook=capture_hook;func_00189E88(actor);near_value(FIELD(actor,0x40,float),-5.5f);near_value(FIELD(actor,0x44,float),0.375f);CHECK(FIELD(actor,0x890,float)==10&&point_query_count==2);
 reset5();FIELD(actor,0x880,u32)=101;FIELD(actor,0x86C,float)=100;hook_mode=4;hook=capture_hook;func_00189E88(actor);CHECK(FIELD(actor,0x880,u32)==200);
 reset5();FIELD(actor,0x880,u32)=200;hook_mode=5;hook=capture_hook;func_00189E88(actor);CHECK(FIELD(actor,0x880,u32)==300&&count(REQUEST)==2);
 reset5();FIELD(actor,0x880,u32)=301;FIELD(actor,0x870,float)=2;FIELD(actor,0x86C,float)=1;hook_mode=6;hook=capture_hook;func_00189E88(actor);near_value(FIELD(actor,0x40,float),6);CHECK(FIELD(actor,0x880,u32)==401&&count(DETACH)==0);
 reset5();hook_mode=7;hook=capture_hook;func_00195F10(actor);CHECK(nth(CANCEL,0)->object==primary&&nth(CANCEL,1)->object==replacement);
 reset5();hook_mode=8;hook=capture_hook;func_00195F10(actor);CHECK(nth(CANCEL,1)->object==replacement);
 reset5();FIELD(actor,0xA14,u32)=202;
 ((GeorgeGoalMember *)(D_003F83F0+3*28))->selector=1;
 ((GeorgeGoalMember *)(D_003F83F0+3*28))->adjustment=30000;
 ((GeorgeGoalMember *)(D_003F83F0+3*28))->target.vtable_offset=0x3A0;
 FIELD(actor,0x3A0,void *)=reference_table;
 ((GeorgeGoalVirtualVoid *)reference_table)->adjustment=10000;
 ((GeorgeGoalVirtualVoid *)reference_table)->invoke=mock_enter;
 func_0018B7E8(actor);CHECK(last_enter==ADDRESS(actor,40000));
}
int main(void)
{
 CHECK(sizeof(void *)==4);test_duration_and_lifecycle();test_state26();test_states27_28();test_state29();test_state25();test_captures();
 printf("actor_states5: %d checks passed\n",checks);return 0;
}
