/* Independent asset-free scenarios and callback mutations; no original code. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_actions.c"

enum { VISIBILITY=1, CONTROL_CALL, PHYSICAL, VECTOR_CALL, POSE_REQUEST,
       MAIN_REQUEST, BLEND, MOTION, MATRIX, RAY, DURATION, RELEASE, NOTICE,
       SERIALIZE, LOOKUP, DETAIL_REQUEST, MESSAGE, ALLOCATE, CONSTRUCT,
       UPPER, INSERT, EXIT_CALLBACK, EFFECT, DIRECTION, ATAN, CONVERT,
       COMPARE, SUBTRACT, MULTIPLY, COLLISION };
typedef struct Event {
    int kind; void *object, *argument; u32 word, other;
    float first, second; GeorgeActorRequestCallback callback;
    GeorgeGoalEntity *context;
} Event;
static Event events[256];
static int checks, event_count;
static u8 storage[0x2400] __attribute__((aligned(16))), data0[0x600], data1[0x600];
static u8 main0[0x800], main1[0x800], companion0[0x800], companion1[0x800];
static u8 physical0[0x400], physical1[0x400], control_table[0x120], physical_table[0x100];
static u8 effect[0x300], effect1[0x300], detail[0x100], collision[0x100];
static GeorgeGoalEntity *actor = (GeorgeGoalEntity *)(storage+0x900);
static struct { GeorgeActorControlObject object; u8 unknown04[0x80]; } control_storage;
#define control control_storage.object
static GeorgeActorEffectRecord effect_record;
static GeorgeMathVec3 direction, collision_point;
static GeorgeMathVec4 zero_value, ray_start;
static GeorgeMathVec3 ray_end;
static GeorgeDeimosValue arguments0[4], arguments1[4];
static GeorgeDeimosPoolNode node0, node1;
static GeorgeRotationMatrix matrix_input[4];
static void *registry[8], **upper_result, *lookup_result;
static float duration_value, atan_value;
static void (*hook)(Event *);
static GeorgeActorBits64 last_left, last_right;
GeorgeDeimosValue *D_00474F48;
void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[]={0};

static void check(int okay,const char *message,int line)
{ ++checks; if(!okay){fprintf(stderr,"actor_actions:%d %s\n",line,message);exit(1);} }
#define CHECK(x) check((x),#x,__LINE__)
static u32 bits(float value){union{float f;u32 u;}v;v.f=value;return v.u;}
static float scalar(u32 value){union{float f;u32 u;}v;v.u=value;return v.f;}
static GeorgeActorBits64 pack(double value){union{double d;GeorgeActorBits64 u;}v;v.d=value;return v.u;}
static double unpack(GeorgeActorBits64 value){union{double d;GeorgeActorBits64 u;}v;v.u=value;return v.d;}
static void near_value(float value,float expected){CHECK(fabsf(value-expected)<0.0001f);}
static Event *emit(int kind,void *object,void *argument,u32 word,u32 other,float first,float second)
{ Event *e;CHECK(event_count<256);e=&events[event_count++];memset(e,0,sizeof(*e));e->kind=kind;e->object=object;e->argument=argument;e->word=word;e->other=other;e->first=first;e->second=second;if(hook)hook(e);return e; }
static int count(int kind){int i,n=0;for(i=0;i<event_count;++i)if(events[i].kind==kind)++n;return n;}
static Event *nth(int kind,int n){int i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&n--==0)return &events[i];CHECK(0);return 0;}
static void mock_control(void *object){emit(CONTROL_CALL,object,0,0,0,0,0);}
static void mock_vector(void *object,const GeorgeMathVec4 *vector){zero_value=*vector;emit(VECTOR_CALL,object,(void *)vector,0,0,0,0);}
static const GeorgeMathVec3 *mock_direction(void *object){emit(DIRECTION,object,0,0,0,0,0);return &direction;}
static void reset(void)
{
    memset(storage,0,sizeof(storage));memset(data0,0,sizeof(data0));memset(data1,0,sizeof(data1));
    memset(main0,0,sizeof(main0));memset(main1,0,sizeof(main1));memset(companion0,0,sizeof(companion0));memset(companion1,0,sizeof(companion1));
    memset(physical0,0,sizeof(physical0));memset(physical1,0,sizeof(physical1));memset(control_table,0,sizeof(control_table));memset(physical_table,0,sizeof(physical_table));
    memset(arguments0,0,sizeof(arguments0));memset(arguments1,0,sizeof(arguments1));memset(matrix_input,0,sizeof(matrix_input));
    hook=0;event_count=0;atan_value=0;duration_value=4800;lookup_result=detail;
    actor->field18=(GeorgeGoalEntityData *)data0;control.field00=control_table;
    FIELD(actor,0x20,GeorgeActorControlObject *)=&control;
    ((GeorgeGoalVirtualVoid *)(control_table+0xF0))->adjustment=-4;
    ((GeorgeGoalVirtualVoid *)(control_table+0xF0))->invoke=mock_control;
    ((GeorgeGoalVirtualVector *)(control_table+0x58))->adjustment=20;
    ((GeorgeGoalVirtualVector *)(control_table+0x58))->invoke=mock_direction;
    FIELD(physical0,0xA0,void *)=physical_table;
    ((GeorgeActorVirtualVec4Input *)(physical_table+0x80))->adjustment=-12;
    ((GeorgeActorVirtualVec4Input *)(physical_table+0x80))->invoke=mock_vector;
    FIELD(actor,0x1B0,void *)=main0;FIELD(actor,0x1B4,void *)=companion0;
    FIELD(actor,0x368,u32)=0x11223344;FIELD(actor,0x35C,float)=0.5f;
    FIELD(actor,0x384,void *)=detail;FIELD(actor,0x38C,void *)=&node0;
    FIELD(data0,0x404,float)=0.125f;FIELD(data1,0x404,float)=0.25f;
    FIELD(data0,0x198,float)=2;FIELD(data1,0x198,float)=3;
    D_00474F48=arguments0;D_003F2D40=effect;
    D_0046A0F0.field00=registry;D_0046A0F0.field04=registry;D_0046A0F0.field08=registry+8;upper_result=registry;
    FIELD(&control,0x48,void *)=collision;
    direction.x=2;direction.y=3;direction.z=4;
}

void func_00238E38(void *object,u32 visible){emit(VISIBILITY,object,0,visible,0,0,0);}
void func_00307850(void *object){emit(PHYSICAL,object,0,0,0,0,0);}
void func_00272390(void *object,u32 word,u32 zero0,u32 zero1,GeorgeActorRequestCallback callback,
                   GeorgeGoalEntity *context,u32 zero2,u32 zero3,u32 zero4,float time,float scale)
{ Event *e;CHECK(zero0==0&&zero1==0&&zero2==0&&zero3==0&&zero4==0);e=emit(POSE_REQUEST,object,0,word,0,time,scale);e->callback=callback;e->context=context; }
s32 func_00177C40(GeorgeGoalEntity *entity,u32 word,u32 zero0,u32 zero1,u32 one,u32 zero2,u32 zero3,float value)
{ CHECK(entity==actor&&zero0==0&&zero1==0&&one==1&&zero2==0&&zero3==0);emit(MAIN_REQUEST,entity,0,word,0,value,0);return 0; }
void func_0021BF28(void *object,const GeorgeMathVec3 *position,float blend){emit(BLEND,object,(void *)position,0,0,blend,0);}
void func_0026FE30(void *object,const GeorgeRotationMatrix *matrix,u32 zero,float time,float blend)
{ CHECK(zero==0);emit(MOTION,object,(void *)matrix,0,0,time,blend); }
u32 func_00195D88(GeorgeGoalEntity *entity,u32 word){CHECK(entity==actor);emit(LOOKUP,entity,0,word,0,0,0);return 0xAABBCCDD;}
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *first,const GeorgeRotationMatrix *second)
{ memset(output,0,sizeof(*output));output->element[12]=1;output->element[13]=2;output->element[14]=3;emit(MATRIX,(void *)first,(void *)second,0,0,0,0); }
void func_002B85D0(void *object,const GeorgeMathVec3 *end,const GeorgeMathVec4 *start,float scale)
{ ray_end=*end;ray_start=*start;emit(RAY,object,0,0,0,scale,0); }
float func_002A6E60(void *source){emit(DURATION,source,0,0,0,0,0);return duration_value;}
void func_002393F8(u32 object){emit(RELEASE,(void *)object,0,0,0,0,0);}
void func_002D02A8(GeorgeDeimosPoolNode *callable,s32 value_count,s32 offset)
{ CHECK(value_count==1&&offset==0);emit(NOTICE,callable,D_00474F48,D_00474F48[0].tag,D_00474F48[0].subtype,D_00474F48[0].payload.scalar,0); }
GeorgeDeimosPoolNode *func_002D0790(GeorgeScriptObject *entity){emit(SERIALIZE,entity,0,0,0,0,0);return &node1;}
void *func_0023C230(void *key,u32 type){void *result=lookup_result;emit(LOOKUP,key,0,type,0,0,0);return result;}
void func_00272970(void *object,u32 word,u32 zero0,u32 zero1,u32 owner,void *reference,
                  GeorgeActorRequestCallback callback,GeorgeGoalEntity *context)
{ Event *e;CHECK(zero0==0&&zero1==0);e=emit(DETAIL_REQUEST,object,reference,word,owner,0,0);e->callback=callback;e->context=context; }
void func_001B67C0(void *point,GeorgeGoalEntity *target,u32 word,u32 key,u32 zero0,u32 zero1)
{ CHECK(zero0==0&&zero1==0);emit(MESSAGE,point,target,word,key,0,0); }
void *func_002AEE60(u32 size){void *result;CHECK(size==0x1B4||size==12);result=size==12?(void *)&effect_record:effect;emit(ALLOCATE,result,0,size,0,0,0);return result;}
void *func_002481F0(void *object){emit(CONSTRUCT,object,0,0,0,0,0);return object;}
s32 func_00100AA8(const void *a,const void *b){(void)a;(void)b;return 0;}
void **func_00100C30(void **begin,void **end,void *const *value,s32(*compare)(const void *,const void *))
{ CHECK(begin==registry&&end==registry&&*value==&effect_record&&compare==func_00100AA8);emit(UPPER,begin,0,0,0,0,0);return upper_result;}
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value)
{ CHECK(range==&D_0046A0F0&&position==upper_result&&*value==&effect_record);emit(INSERT,range,0,0,0,0,0);}
void func_002BD340(void){}
s32 func_00396260(void(*function)(void)){CHECK(function==func_002BD340);emit(EXIT_CALLBACK,0,0,0,0,0,0);return 0;}
void func_00251DB0(void *object,u32 key,const GeorgeMathVec3 *point)
{ collision_point=*point;emit(EFFECT,object,0,key,0,0,0); }
float func_0029B940(float first,float second){emit(ATAN,0,0,bits(second),0,first,0);return atan_value;}
GeorgeActorBits64 func_00374848(float value){emit(CONVERT,0,0,0,0,value,0);return pack(value);}
s32 func_00373250(GeorgeActorBits64 first,GeorgeActorBits64 second)
{ double a=unpack(first),b=unpack(second);last_left=first;last_right=second;emit(COMPARE,0,0,0,0,(float)a,(float)b);return a<b?-1:(a>b?1:0);}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first,GeorgeActorBits64 second)
{ emit(SUBTRACT,0,0,0,0,(float)unpack(first),(float)unpack(second));return pack(unpack(first)-unpack(second));}
GeorgeActorBits64 func_00372D28(GeorgeActorBits64 first,GeorgeActorBits64 second)
{ emit(MULTIPLY,0,0,0,0,(float)unpack(first),(float)unpack(second));return pack(unpack(first)*unpack(second));}
void *func_0013D328(void *object,const GeorgeMathVec3 *point,GeorgeMathVec3 *output,GeorgeMathVec3 *normal,u32 mode,float threshold)
{ collision_point=*point;output->x=7;output->y=8;output->z=9;normal->x=10;normal->y=11;normal->z=12;emit(COLLISION,object,output,mode,0,threshold,0);return detail;}

static void visibility_hook(Event *e)
{ if(e->kind==VISIBILITY&&e->object==main0){FIELD(actor,0x29C,void *)=main1;FIELD(actor,0x2C0,s16)=1;}else if(e->kind==VISIBILITY&&e->object==companion0)FIELD(actor,0x2C0,s16)=0; }
static void test_visibility(void)
{
    unsigned i;u32 inputs[]={0,1,0xFFFFFFFFU};
    for(i=0;i<3;++i){reset();FIELD(actor,0x228,void *)=main0;FIELD(actor,0x29C,void *)=companion1;FIELD(actor,0x2B4,void *)=companion0;hook=visibility_hook;func_00178D10(actor,inputs[i]);CHECK(count(VISIBILITY)==3);CHECK(nth(VISIBILITY,1)->object==main1&&nth(VISIBILITY,2)->object==companion0);CHECK(nth(VISIBILITY,0)->word==(inputs[i]==0));CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(inputs[i]?1ULL<<33:0));}
    reset();FIELD(actor,0x2C0,s16)=-1;func_00178D10(actor,0);CHECK(count(VISIBILITY)==0);
    reset();FIELD(actor,0x2C0,s16)=1;FIELD(actor,0x2B4,void *)=0;func_00178D10(actor,7);CHECK(count(VISIBILITY)==1&&nth(VISIBILITY,0)->object==0);
}
static void pose_hook(Event *e)
{
    if(e->kind==PHYSICAL){FIELD(actor,0x1A0,void *)=physical1;((GeorgeActorVirtualVec4Input *)(physical_table+0x80))->adjustment=16;}
    if(e->kind==MAIN_REQUEST){FIELD(actor,0x1B0,void *)=main1;FIELD(actor,0x370,float)=0.75f;}
    if(e->kind==LOOKUP&&e->object==actor)FIELD(actor,0x1B4,void *)=companion1;
    if(e->kind==POSE_REQUEST&&e->object==companion1)FIELD(actor,0x370,float)=0.25f;
}
static void pose_alias_hook(Event *e)
{
    if(e->kind==MAIN_REQUEST){FIELD(actor,0x1B0,void *)=ADDRESS(actor,-0x278);FIELD(actor,0x370,float)=scalar((u32)main1);}
}
static void test_pose(void)
{
    reset();FIELD(actor,0x1A0,void *)=physical0;hook=pose_hook;func_00178E80(actor,0x76543210,0.5f);
    CHECK(nth(CONTROL_CALL,0)->object==ADJUST(&control,-4));CHECK(nth(PHYSICAL,0)->object==physical0);CHECK(nth(VECTOR_CALL,0)->object==physical0+0xB0);
    CHECK(zero_value.x==0&&zero_value.y==0&&zero_value.z==0&&zero_value.w==0);CHECK(count(POSE_REQUEST)==2);
    CHECK(nth(POSE_REQUEST,0)->word==0x76543210&&nth(POSE_REQUEST,0)->callback==func_00192BC0&&nth(POSE_REQUEST,0)->context==actor);
    CHECK(nth(POSE_REQUEST,1)->object==companion1&&nth(POSE_REQUEST,1)->word==0xAABBCCDD&&nth(POSE_REQUEST,1)->callback==0);
    CHECK(nth(POSE_REQUEST,0)->first==0&&nth(POSE_REQUEST,0)->second==5);CHECK(nth(MAIN_REQUEST,0)->first==5);
    CHECK(FIELD(main1,0x428,float)==0.75f&&FIELD(main1,0x434,u32)==1);CHECK(FIELD(companion1,0x428,float)==0.25f&&FIELD(companion1,0x434,u32)==1);
    CHECK(count(MOTION)==2&&nth(MOTION,0)->object==main1&&nth(MOTION,1)->object==companion1);CHECK(nth(MOTION,0)->argument==ADDRESS(actor,0xF0));
    CHECK(FIELD(actor,0x190,GeorgeActorBits64)==((1ULL<<32)|0x4000000ULL));
    reset();FIELD(actor,0x1B4,void *)=0;func_00178E80(actor,7,0);CHECK(count(POSE_REQUEST)==1&&count(PHYSICAL)==0&&count(MOTION)==1);
    reset();FIELD(actor,0x1A0,void *)=physical0;hook=pose_hook;func_00192B28(actor);CHECK(count(PHYSICAL)==1&&nth(VECTOR_CALL,0)->object==physical0+0xB0);CHECK(count(POSE_REQUEST)==0);
    reset();FIELD(actor,0x1B4,void *)=0;hook=pose_alias_hook;func_00178E80(actor,7,0.5f);CHECK(FIELD(actor,0x1B0,void *)==main1);CHECK(FIELD(main1,0x434,u32)==1&&nth(MOTION,0)->object==main1);
}
static void ray_hook(Event *e){if(e->kind==MATRIX)FIELD(actor,0x1F8,void *)=main1;if(e->kind==RAY)FIELD(actor,0x1FC,u32)=99;}
static void duration_hook(Event *e){if(e->kind==DURATION){FIELD(actor,0x370,float)=0.25f;FIELD(actor,0x2DC,u32)=99;FIELD(actor,0x710,u32)=88;}}
static void test_ray_duration(void)
{
    reset();func_00179168(actor);CHECK(count(MATRIX)==0);
    reset();FIELD(actor,0x1F4,u32)=2;FIELD(main0,0x0C,u32)=1;FIELD(main0,0x3DC,void *)=matrix_input;FIELD(actor,0x3F8,u32)=2;hook=ray_hook;func_00179168(actor);
    CHECK(nth(MATRIX,0)->object==matrix_input+2&&nth(MATRIX,0)->argument==ADDRESS(actor,0xB0));CHECK(nth(RAY,0)->object==main1&&nth(RAY,0)->first==2);
    CHECK(ray_start.x==1&&ray_start.y==3&&ray_start.z==3&&ray_start.w==2);CHECK(ray_end.x==1&&ray_end.y==3003&&ray_end.z==3);CHECK(FIELD(actor,0x1FC,u32)==1);
    reset();hook=duration_hook;func_00190FC0(123,actor,detail);CHECK(FIELD(actor,0x2DC,u32)==0);near_value(FIELD(actor,0x2E8,float),1);
    func_00192BC0(456,actor,main1);CHECK(FIELD(actor,0x710,u32)==102);near_value(FIELD(actor,0x714,float),0.75f);
}
static void health_hook(Event *e)
{ if(e->kind==RELEASE&&e->object==main0){FIELD(actor,0x290,void *)=companion1;actor->field18=(GeorgeGoalEntityData *)data1;}if(e->kind==NOTICE)FIELD(actor,0x3C0,u32)=0x9999; }
static void test_health(void)
{
    unsigned i;float values[]={-2,-0.0f,0,0.5f,1,2};
    for(i=0;i<6;++i){float old=0.5f,expected;reset();FIELD(actor,0x36C,float)=old;expected=old<values[i]?values[i]+0.125f:values[i]-0.125f;func_00191F78(actor,values[i]);near_value(FIELD(actor,0x36C,float),expected);CHECK(nth(NOTICE,0)->first==expected&&nth(NOTICE,0)->word==2&&nth(NOTICE,0)->other==0);}
    reset();FIELD(actor,0x36C,float)=0.75f;func_00191FF0(actor,0.5f);near_value(FIELD(actor,0x36C,float),1.125f);
    reset();FIELD(actor,0x36C,float)=0.5f;func_00191FF0(actor,-3);near_value(FIELD(actor,0x36C,float),-2.625f);
    reset();FIELD(actor,0x36C,float)=1;func_00191F78(actor,scalar(0x7FC01234));CHECK(isnan(FIELD(actor,0x36C,float)));
    reset();FIELD(actor,0x38C,void *)=0;func_00191F78(actor,0.5f);CHECK(count(NOTICE)==0);
    reset();FIELD(actor,0x36C,float)=0.5f;D_00474F48=(GeorgeDeimosValue *)ADDRESS(actor,0x388);func_00191F78(actor,0.25f);CHECK(nth(NOTICE,0)->object==(void *)bits(0.125f));
    reset();FIELD(actor,0x36C,float)=1;FIELD(actor,0x28C,void *)=main0;FIELD(actor,0x290,void *)=companion0;hook=health_hook;func_00192078(actor,0xABCDEF01,0.1f);
    CHECK(count(RELEASE)==2&&nth(RELEASE,1)->object==companion1);CHECK(FIELD(actor,0x28C,void *)==0&&FIELD(actor,0x290,void *)==0);near_value(FIELD(actor,0x36C,float),0.45f);CHECK(FIELD(actor,0x3C0,u32)==0xABCDEF01);
    for(i=0;i<2;++i){reset();FIELD(actor,0x190,GeorgeActorBits64)=i?0x200ULL:0x100ULL;FIELD(actor,0x36C,float)=1;FIELD(actor,0x3C0,u32)=99;func_00192078(actor,7,1);CHECK(event_count==0&&FIELD(actor,0x36C,float)==1&&FIELD(actor,0x3C0,u32)==99);}
}
static void serialized_hook(Event *e)
{ if(e->kind==SERIALIZE){D_00474F48=arguments1;FIELD(actor,0x3A0,void *)=&node1;}if(e->kind==NOTICE&&e->word==4)actor->field0C=37; }
static void test_damage(void)
{
    unsigned i;float skipped[]={-1,-0.0f,0};
    for(i=0;i<3;++i){reset();func_00191E88(actor,1,skipped[i]);CHECK(event_count==0);}
    reset();actor->field0C=13;func_00191E88(actor,1,1);CHECK(event_count==0);
    reset();FIELD(actor,0x36C,float)=1;FIELD(actor,0x3A0,void *)=&node0;hook=serialized_hook;func_00191E88(actor,1,0.1f);
    CHECK(count(SERIALIZE)==1&&nth(SERIALIZE,0)->object==actor);CHECK(arguments1[0].tag==4&&arguments1[0].subtype==0&&arguments1[0].payload.pointer==&node1);CHECK(nth(NOTICE,1)->object==&node1);CHECK(FIELD(actor,0x590,u32)==0);
    reset();FIELD(actor,0x3A0,void *)=&node0;func_00191E88(actor,0,0.5f);CHECK(count(SERIALIZE)==0&&FIELD(actor,0x590,u32)==0x51);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x8000000ULL)!=0);
    reset();FIELD(actor,0x190,GeorgeActorBits64)=0x100;FIELD(actor,0x3A0,void *)=0;func_00191E88(actor,0,scalar(0x7FC00001));CHECK(FIELD(actor,0x590,u32)==0x51);
}
static void detail_hook(Event *e)
{ if(e->kind==DETAIL_REQUEST&&e->object==main0){FIELD(actor,0x1B4,void *)=companion1;FIELD(actor,0x368,u32)=0x55667788;actor->field18=(GeorgeGoalEntityData *)data1;FIELD(data1,0x1B4,u32)=0xDEADBEEF;} }
static void test_keys_requests(void)
{
    u32 keys[]={0,0x260105ECU,0xEC789588U,0xCAD99652U,0xCB426EF9U,0x1111150CU,0x75188880U,0x3BAC798CU,0x728F0147U,0x794F22DAU,0x7F9000CFU,1,0xFFFFFFFFU};unsigned i,j;
    for(i=0;i<13;++i)for(j=0;j<4;++j){reset();FIELD(actor,0x36C,float)=1;FIELD(actor,0x3BC,u32)=99;FIELD(actor,0x190,GeorgeActorBits64)=j==1?0x100:(j==2?0x200:(j==3?0x20:0));func_00192180(actor,keys[i]);CHECK(FIELD(actor,0x3C0,u32)==keys[i]&&FIELD(actor,0x36C,u32)==0);CHECK(FIELD(actor,0x3BC,u32)==((j!=0||i>=11)?99:(i==0?0x728F0147U:keys[i])));}
    reset();FIELD(actor,0x3B4,u32)=7;FIELD(actor,0x3B8,void *)=physical0;hook=detail_hook;CHECK(func_00195850(actor,9,0x12345678,1)==1);
    CHECK(FIELD(actor,0x2D8,u32)==2&&FIELD(actor,0x2DC,u32)==1);CHECK(count(DETAIL_REQUEST)==2&&nth(DETAIL_REQUEST,0)->callback==func_00190FC0&&nth(DETAIL_REQUEST,0)->context==actor);CHECK(nth(DETAIL_REQUEST,1)->object==companion1&&nth(DETAIL_REQUEST,1)->other==0x55667788&&nth(DETAIL_REQUEST,1)->callback==0);CHECK(nth(MESSAGE,0)->other==0xDEADBEEF&&nth(MESSAGE,0)->argument==physical0);
    reset();lookup_result=0;FIELD(data0,0x1B4,u32)=7;CHECK(func_00195850(actor,1,2,0)==1&&count(DETAIL_REQUEST)==0&&count(MESSAGE)==0);
    reset();FIELD(actor,0x1B0,void *)=0;CHECK(func_00195850(actor,1,2,0)==1&&count(DETAIL_REQUEST)==0);
    reset();actor->field0C=13;CHECK(func_00195850(actor,1,2,1)==0&&event_count==0);
    reset();FIELD(actor,0x2D8,u32)=0x10000;CHECK(func_00195850(actor,1,2,1)==0&&event_count==0);
    reset();FIELD(data0,0x1B4,u32)=7;FIELD(actor,0x190,GeorgeActorBits64)=0x100000;CHECK(func_00195850(actor,1,2,1)==1&&count(MESSAGE)==0);
}
static void registry_hook(Event *e)
{ if(e->kind==EXIT_CALLBACK){actor->field18=(GeorgeGoalEntityData *)data1;FIELD(data1,0x1BC,u32)=0x99887766;D_003F2D40=effect1;} }
static void test_effects(void)
{
    u32 keys[]={0x728F0147U,0x260105ECU,0xEC789588U};unsigned i,j;
    for(i=0;i<3;++i)for(j=0;j<2;++j){u32 member=j?(i==0?0x1C4:0x1C8):(i==0?0x1B8:(i==1?0x1BC:0x1C0));int allowed=!(j&&i==2);reset();FIELD(actor,0x3BC,u32)=keys[i];FIELD(actor,0x190,GeorgeActorBits64)=j?0x200000:0;FIELD(data0,member,u32)=0xAABBCCDD;FIELD(actor,0x40,float)=1;FIELD(actor,0x44,float)=2;FIELD(actor,0x48,float)=3;func_001787B0(actor);CHECK(count(EFFECT)==allowed);if(allowed){CHECK(nth(EFFECT,0)->word==0xAABBCCDD&&nth(EFFECT,0)->object==effect);CHECK(collision_point.x==1&&collision_point.y==2&&collision_point.z==3);}}
    reset();FIELD(actor,0x3BC,u32)=0x260105EC;FIELD(data0,0x1BC,u32)=7;D_003F2D40=0;hook=registry_hook;func_001787B0(actor);CHECK(count(ALLOCATE)==2&&count(EXIT_CALLBACK)==1&&count(EFFECT)==1);CHECK(effect_record.field00==9&&effect_record.field04==D_00421160&&effect_record.field08==effect);CHECK(nth(EFFECT,0)->object==effect1&&nth(EFFECT,0)->word==0x99887766);CHECK(D_0046A0F0.field04==registry+1&&registry[0]==&effect_record);
    reset();FIELD(actor,0x3BC,u32)=0x260105EC;FIELD(data0,0x1BC,u32)=7;D_003F2D40=0;upper_result=registry+1;func_001787B0(actor);CHECK(count(INSERT)==1&&D_0046A0F0.field04==registry);
    reset();FIELD(actor,0x3BC,u32)=0x728F0147;func_001787B0(actor);CHECK(event_count==0);
}
static int angle_writes(u32 state){return state>=36||state==0||state==2||state==3||state==4||state==5||state==7||state==8||state==11||state==13||state==15||state==16||state==17||state==19||state==24||state==26||state==27||state==29||state==30||state==31||state==33||state==34;}
static void angle_hook(Event *e)
{ if(e->kind==CONVERT){actor->field0C=14;FIELD(actor,0x190,GeorgeActorBits64)=0x40000;FIELD(actor,0x58,float)=9;} }
static float expected_angle(float angle,float desired)
{ float difference=desired-angle;double magnitude=fabs((double)difference);if(magnitude>3.14159274101257324)magnitude=fabs(magnitude-6.28318548202514648);if(magnitude<1.57079637050628662)return desired;desired=desired-3.14159274101257324f;if(desired>3.14159274101257324f)desired-=6.28318548202514648f;else if(!(desired>=-3.14159274101257324f))desired+=6.28318548202514648f;return desired; }
static void test_angle_collision(void)
{
    u32 state;unsigned i,j;float inputs[]={-5,-3,-1,0,1,3,5},desired[]={-5,-2,0,2,5};
    for(state=0;state<39;++state){reset();actor->field0C=state;FIELD(actor,0x58,float)=9;FIELD(actor,0x504,float)=88;func_001784D0(actor,0,2);CHECK(FIELD(actor,0x64,float)==2);CHECK(FIELD(actor,0x58,float)==(angle_writes(state)?2:9));CHECK(FIELD(actor,0x504,float)==(state==0?9:88));CHECK(event_count==0);}
    for(i=0;i<7;++i)for(j=0;j<5;++j){reset();FIELD(data0,0x1D8,u32)=0x45A78000;atan_value=desired[j];func_001784D0(actor,1,inputs[i]);near_value(FIELD(actor,0x58,float),expected_angle(inputs[i],desired[j]));CHECK(nth(DIRECTION,0)->object==ADJUST(&control,20));CHECK(nth(ATAN,0)->first==4&&nth(ATAN,0)->word==bits(2));CHECK(last_right==0x3FF921FB60000000ULL);}
    reset();FIELD(actor,0x2D8,u32)=6;func_001784D0(actor,1,2);CHECK(FIELD(actor,0x64,u32)==0&&event_count==0);
    reset();FIELD(actor,0x190,GeorgeActorBits64)=0x40000;func_001784D0(actor,1,2);CHECK(FIELD(actor,0x64,u32)==0&&event_count==0);
    reset();FIELD(data0,0x1D8,u32)=0x45A78000;atan_value=scalar(0x7FC00000);func_001784D0(actor,1,0);CHECK(isnan(FIELD(actor,0x58,float)));
    reset();FIELD(data0,0x1D8,u32)=0x45A78000;hook=angle_hook;atan_value=2;func_001784D0(actor,1,0);CHECK(FIELD(actor,0x64,float)==0&&FIELD(actor,0x58,float)==9);
    {float boundary[]={-6.28318548202514648f,-3.14159274101257324f,-1.57079637050628662f,1.57079637050628662f,3.14159274101257324f,6.28318548202514648f};for(i=0;i<6;++i){reset();FIELD(data0,0x1D8,u32)=0x45A78000;atan_value=boundary[i];func_001784D0(actor,1,0);near_value(FIELD(actor,0x58,float),expected_angle(0,boundary[i]));}}
    for(i=0;i<2;++i){GeorgeMathVec3 point={0,0,0},normal={0,0,0};reset();FIELD(data0,0x1D8,u32)=0x45A78000;FIELD(data0,0x230,float)=5;FIELD(data0,0x234,float)=2;FIELD(actor,0x40,float)=10;FIELD(actor,0x44,float)=20;FIELD(actor,0x48,float)=30;FIELD(actor,0xD0,float)=3;FIELD(actor,0xD4,float)=4;FIELD(actor,0xD8,float)=5;CHECK(func_00195DC8(actor,&point,&normal,i,0.75f)==detail);CHECK(nth(COLLISION,0)->object==collision&&nth(COLLISION,0)->first==0.75f&&nth(COLLISION,0)->word==i);CHECK(collision_point.x==(i?16:10)&&collision_point.y==(i?33:20)&&collision_point.z==(i?40:30));CHECK(point.x==7&&normal.z==12);}
    {GeorgeMathVec3 a={1,2,3},b={4,5,6};reset();CHECK(func_00195DC8(actor,&a,&b,1,0)==0);CHECK(event_count==0&&a.x==1&&b.z==6);}
    {GeorgeMathVec3 normal;reset();FIELD(data0,0x1D8,u32)=0x45A78000;FIELD(data0,0x230,float)=5;FIELD(data0,0x234,float)=2;FIELD(actor,0x40,float)=10;FIELD(actor,0x44,float)=20;FIELD(actor,0x48,float)=30;FIELD(actor,0xD0,float)=3;FIELD(actor,0xD4,float)=4;FIELD(actor,0xD8,float)=5;CHECK(func_00195DC8(actor,VECTOR(actor,0x40),&normal,1,-0.0f)==detail);CHECK(collision_point.x==16&&collision_point.y==33&&collision_point.z==40);CHECK(FIELD(actor,0x40,float)==7&&bits(nth(COLLISION,0)->first)==0x80000000U);}
}
int main(void)
{ test_visibility();test_pose();test_ray_duration();test_health();test_damage();test_keys_requests();test_effects();test_angle_collision();printf("actor_actions native: %d checks passed\n",checks);return 0; }
