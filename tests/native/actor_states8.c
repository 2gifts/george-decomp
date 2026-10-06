/* Asset-free call-boundary tests. Expected state outcomes and callback
 * mutations are independent specifications; no retail instructions are used. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_states8.c"

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
static u8 data_storage[0x600], alternate_data[0x600], primary[0x800], companion[0x800], replacement[0x800];
static u8 object_storage[0x400], handle[0x400], reference[0x100];
static u8 map[0x400], alternate_map[0x400], parent[0x200], alternate_parent[0x200];
static u8 point[0x100], alternate_point[0x100], reference_table[0xB0];
static u8 control_table[0x110];
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
{ ++checks; if(!value){fprintf(stderr,"actor_states8:%d: %s\n",line,label);exit(1);} }
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
void func_002A1C08(void *output,const void *input) {memcpy(output,input,64);emit(MATRIX_COPY,output,(void *)input,0,0,0);}
void func_002393F8(u32 word) {emit(RELEASE,(void *)word,0,0,0,0);}

enum { FRAME=100, MATRIX_MULTIPLY, KEY_MODE, RECORD_ALLOC, RECORD_BUILD,
       RECORD_CALLBACK, MANAGER, RECORD_ADD, RECORD_REMOVE, RECORD_RELEASE,
       OBJECT_RELEASE, COSINE, SINE, SCALE_VECTOR, PROJECT, SET_ANGLE,
       UPDATE_BOUNDS, BUILD_RECORD, HANDLE_FINISH, ANGULAR_SAMPLE,
       NORMAL_SAMPLE, DIRECTION_SAMPLE, OBJECT_RESET };
const u8 D_0042DA60[]={4},D_0042DA48[]={5},D_0042E780[]={6};
void *D_004961F4;
static u8 record[0x100], alternate_record[0x100], callback_table[0x20];
static GeorgeActorInteractionCallback callback_object;
static GeorgeMathVec3 returned_vectors[2], normal_result, direction_result;
static GeorgeGeometryFrame frame_result, last_frame;
static GeorgeMathVec4 last_bounds[2];
static float samples[8];
static int sample_count, vector_queries;
static GeorgeMathVec3 *retained_direction6, *retained_orientation, *retained_normal6;
static s32 project_result;
static float cosine_result, sine_result, last_scale;

static void mock_frame(void *self,const GeorgeMathVec4 *basis,const GeorgeMathVec4 *position)
{memcpy(last_frame.axis,basis,48);last_frame.position=*position;emit(FRAME,self,(void *)basis,0,0,0);}
static const GeorgeMathVec3 *mock_vector_result(void *self)
{const GeorgeMathVec3 *p=&returned_vectors[vector_queries%2];++vector_queries;emit(QUERY,self,(void *)p,vector_queries,0,0);return p;}
static void mock_callback_word(void *self,u32 word)
{emit(REFERENCE_WORD,self,0,word,0,0);}
static void reset7(void)
{
 int i;reset();memset(primary,0,sizeof(primary));memset(companion,0,sizeof(companion));memset(replacement,0,sizeof(replacement));
 memset(record,0,sizeof(record));memset(alternate_record,0,sizeof(alternate_record));memset(&callback_object,0,sizeof(callback_object));memset(callback_table,0,sizeof(callback_table));
 memset(&frame_result,0,sizeof(frame_result));for(i=0;i<3;++i)((float *)&frame_result.axis[i])[i]=1;
 frame_result.position.x=10;frame_result.position.y=20;frame_result.position.z=30;frame_result.position.w=1;
 returned_vectors[0].x=1;returned_vectors[0].y=2;returned_vectors[0].z=3;returned_vectors[1]=returned_vectors[0];
 normal_result.x=0;normal_result.y=1;normal_result.z=0;direction_result.x=3;direction_result.y=4;direction_result.z=0;
 for(i=0;i<8;++i)samples[i]=0.25f;sample_count=vector_queries=0;project_result=1;cosine_result=1;sine_result=0;retained_direction6=retained_orientation=retained_normal6=0;
 D_004961F4=parent;FIELD(actor,0x424,void *)=object_storage;FIELD(actor,0x240,void *)=object_storage;FIELD(actor,0x300,void *)=record;
 FIELD(actor,0x2EC,void *)=handle;FIELD(actor,0x3DC,u32)=0x99887766;
 FIELD(data_storage,0x0C,float)=2;FIELD(data_storage,0x08,float)=5;FIELD(data_storage,0x18,float)=2;
 FIELD(data_storage,0x24C,float)=6;FIELD(data_storage,0x250,float)=8;FIELD(data_storage,0x400,float)=7;
 FIELD(primary,0x3D8,void *)=&frame_result;
 ((GeorgeGoalVirtualVoid *)(control_table+0x88))->adjustment=40;((GeorgeGoalVirtualVoid *)(control_table+0x88))->invoke=mock_void;
 ((GeorgeGoalVirtualVector *)(control_table+0x58))->adjustment=44;((GeorgeGoalVirtualVector *)(control_table+0x58))->invoke=mock_vector_result;
 ((GeorgeActorVirtualVectorInput *)(control_table+0xB8))->adjustment=28;((GeorgeActorVirtualVectorInput *)(control_table+0xB8))->invoke=mock_vector;
 ((GeorgeActorVirtualVectorInput *)(control_table+0x98))->adjustment=52;((GeorgeActorVirtualVectorInput *)(control_table+0x98))->invoke=mock_vector;
 ((GeorgeActorVirtualFrameParts *)(control_table+0x100))->adjustment=-4;((GeorgeActorVirtualFrameParts *)(control_table+0x100))->invoke=mock_frame;
 callback_object.field00=callback_table;callback_object.field04=actor;FIELD(actor,0x2F8,void *)=&callback_object;
 ((GeorgeGoalVirtualWord *)(callback_table+0x18))->adjustment=-8;((GeorgeGoalVirtualWord *)(callback_table+0x18))->invoke=mock_callback_word;
}
void func_00235CD8(void *object,u32 key,u32 word){emit(KEY_MODE,object,0,key,word,0);}
void func_00251CC8(void *object,u32 key){emit(EMIT,object,0,key,0,0);}
void func_00272390(void *object,u32 word,u32 mode0,u32 mode1,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,u32 invoke_word,u32 callback_word,u32 mode2,float time,float scale)
{CHECK(mode0==0&&mode1==0&&invoke_word==0&&callback_word==0&&mode2==0&&context==actor&&callback==func_001A7698);emit(REQUEST,object,(void *)callback,word,bits(scale),time);}
void *func_002E2BB0(void *manager,u32 kind,u32 group){CHECK(kind==0xA0&&group==0x2F);emit(RECORD_ALLOC,manager,0,kind,group,0);return record;}
void *func_0030C368(void *object,const GeorgeMathVec4 *bounds,u32 count_value){memcpy(last_bounds,bounds,32);emit(RECORD_BUILD,object,(void *)bounds,count_value,0,0);return alternate_record;}
void func_0030E018(void *object,GeorgeActorInteractionCallback *callback){emit(RECORD_CALLBACK,object,callback,0,0,0);}
void *func_0022C1E0(void){emit(MANAGER,0,0,0,0,0);return alternate_parent;}
void func_00312688(void *manager,void *object){emit(RECORD_ADD,manager,object,0,0,0);}
void func_00312C00(void *manager,void *object){emit(RECORD_REMOVE,manager,object,0,0,0);}
void func_00317910(void *object){emit(RECORD_RELEASE,object,0,0,0,0);}
void func_001413A8(void *object){emit(OBJECT_RELEASE,object,0,0,0,0);}
void *func_00251A88(void *object,u32 key){emit(EMIT,object,0,key,0,0);return handle;}
void func_002458E8(void *object){emit(HANDLE_FINISH,object,0,0,0,0);}
float func_0029C168(float angle){emit(COSINE,0,0,0,0,angle);return cosine_result;}
float func_0029C090(float angle){emit(SINE,0,0,0,0,angle);return sine_result;}
float func_002A35C0(GeorgeMathVec3 *output,const GeorgeMathVec3 *input,float scale)
{float length=sqrtf((input->x*input->x+input->y*input->y)+input->z*input->z);float reciprocal=length==0?1:scale/length;float x=input->x*reciprocal,y=input->y*reciprocal,z=input->z*reciprocal;last_scale=scale;output->x=x;output->y=y;output->z=z;emit(SCALE_VECTOR,output,(void *)input,0,0,scale);return length;}
s32 func_00177E48(GeorgeGoalEntity *entity,const GeorgeMathVec3 *input){CHECK(entity==actor);last_motion=*input;emit(PROJECT,entity,(void *)input,0,0,0);return project_result;}
void func_001784D0(GeorgeGoalEntity *entity,u32 mode,float angle){CHECK(entity==actor&&mode==1);emit(SET_ANGLE,entity,0,mode,0,angle);}
void func_0030C440(void *object,const GeorgeMathVec4 *bounds){memcpy(last_bounds,bounds,32);emit(UPDATE_BOUNDS,object,(void *)bounds,0,0,0);}
u32 func_00236A10(const GeorgeRotationMatrix *matrix,u32 word,u32 mode0,u32 mode1,u32 mode2,u32 mode3){CHECK(matrix==(const GeorgeRotationMatrix *)ADDRESS(actor,0xB0)&&mode0==0&&mode1==0&&mode2==0&&mode3==0);emit(BUILD_RECORD,0,(void *)matrix,word,0,0);return 0x12345678;}
static const GeorgeRotationMatrix *last_matrix_input;
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *first,const GeorgeRotationMatrix *second){last_matrix_input=first;memcpy(output,first?first:(const GeorgeRotationMatrix *)&frame_result,64);emit(MATRIX_MULTIPLY,output,(void *)second,0,0,0);}
float func_00141590(const GeorgeMathAngularAC *object){float value=samples[sample_count];CHECK(sample_count<8);++sample_count;emit(ANGULAR_SAMPLE,(void *)object,0,sample_count,0,value);return value;}
void func_00141638(void *object,GeorgeMathVec3 *output){*output=normal_result;retained_normal6=output;emit(NORMAL_SAMPLE,object,output,0,0,0);}
void func_00141678(void *object,GeorgeMathVec3 *output){*output=direction_result;retained_direction6=output;emit(DIRECTION_SAMPLE,object,output,0,0,0);}
void func_00141348(void *object){emit(OBJECT_RESET,object,0,0,0,0);}

enum { REQUEST_WRAPPER=140, OBJECT_MODE, ADJUSTMENT, OBJECT_CHECK, OBJECT_PREDICATE,
       ORIENTATION, OBJECT_STATE, MOTION_CHECK, ANIMATION_BITS, ARRAY_COUNT,
       ARRAY_RECORDS, LOOKUP_WORD, CREATE_WORD, CLEAR_HANDLES,
       SOFT_COMPARE, SOFT_SUBTRACT, SOFT_MULTIPLY };
const u8 D_0042D618[]={7}, D_0042D628[]={8};
static u32 animation_bits;
static u8 animation_records[8*32];
static GeorgeGeometryFrame animation_matrices[8];
static s32 animation_count, object_check, object_predicate, motion_check;
static s32 wrapped_request_result;
static int created_count, mutation_mode;
static float last_adjustment;
static void reset_all(void)
{
 int i;reset7();memset(animation_records,0,sizeof(animation_records));memset(animation_matrices,0,sizeof(animation_matrices));
 animation_count=2;animation_bits=0;object_check=0;object_predicate=1;motion_check=1;wrapped_request_result=1;created_count=0;mutation_mode=0;
 FIELD(actor,0x990,void *)=object_storage;FIELD(actor,0x980,void *)=&returned_vectors[0];FIELD(actor,0x9C8,void *)=&returned_vectors[0];
 FIELD(primary,0x378,void *)=reference;FIELD(primary,0x380,void *)=reference;FIELD(primary,0x3E8,void *)=animation_matrices;FIELD(primary,0x3F0,void *)=animation_matrices;
 FIELD(replacement,0x378,void *)=reference;FIELD(replacement,0x380,void *)=reference;FIELD(replacement,0x3E8,void *)=animation_matrices;FIELD(replacement,0x3F0,void *)=animation_matrices;
 FIELD(actor,0x9B4,float)=2;FIELD(actor,0x9B0,float)=1;FIELD(actor,0x9A8,float)=0;FIELD(actor,0x9C0,float)=1;
 FIELD(data_storage,0x3DC,float)=1;FIELD(data_storage,0x3E0,float)=.125f;FIELD(data_storage,0x37C,float)=1;FIELD(data_storage,0x380,float)=5;FIELD(data_storage,0x384,float)=2;
 FIELD(data_storage,0x2EC,s32)=2;FIELD(actor,0x268,void *)=object_storage;FIELD(actor,0x26C,void *)=replacement;
 FIELD(object_storage,0x20,void *)=reference_table;((GeorgeGoalVirtualWord *)(reference_table+0x10))->adjustment=-4;((GeorgeGoalVirtualWord *)(reference_table+0x10))->invoke=mock_reference;
 for(i=0;i<8;++i){animation_matrices[i]=frame_result;animation_matrices[i].position.x=(float)i;}
}
u32 func_00272C10(void *object){emit(ANIMATION_BITS,object,0,animation_bits,0,0);return animation_bits;}
u32 func_002A6460(const void *object){u32 result=(u32)animation_count;emit(ARRAY_COUNT,(void *)object,0,result,0,0);return result;}
void *func_002A6468(const void *object,s32 index){CHECK(index==0);emit(ARRAY_RECORDS,(void *)object,0,(u32)index,0,0);return animation_records;}
void func_00192078(GeorgeGoalEntity *entity,u32 key,float adjustment){CHECK(entity==actor);last_adjustment=adjustment;emit(ADJUSTMENT,entity,0,key,0,adjustment);}
void func_0013E4C8(void *object,u32 mode){emit(OBJECT_MODE,object,0,mode,0,0);}
s32 func_00192A18(void *object){s32 result=object_check;emit(OBJECT_CHECK,object,0,0,0,0);return result;}
s32 func_00176E10(GeorgeGoalEntity *entity){s32 result=motion_check;emit(MOTION_CHECK,entity,0,0,0,0);return result;}
void func_00190F70(GeorgeGoalEntity *entity){emit(CLEAR_HANDLES,entity,0,0,0,0);FIELD(entity,0x28C,u32)=FIELD(entity,0x290,u32)=0;}
void *func_00210078(u32 word,u32 mode0,u32 mode1){CHECK(mode0==0&&mode1==0);emit(LOOKUP_WORD,0,0,word,0,0);return object_storage;}
void *func_00236CB8(const void *matrix,void *object,u32 mode,u32 word0,u32 word1){CHECK(mode==0&&word0==0&&word1==0);++created_count;emit(CREATE_WORD,object,(void *)matrix,0,0,0);return created_count==1?handle:point;}
void func_0018E5A8(GeorgeGoalEntity *entity,const GeorgeMathVec3 *lower,const GeorgeMathVec3 *upper,u32 word,u32 count_value)
{last_bounds[0].x=lower->x;last_bounds[0].y=lower->y;last_bounds[0].z=lower->z;last_bounds[1].x=upper->x;last_bounds[1].y=upper->y;last_bounds[1].z=upper->z;emit(RECORD_BUILD,entity,(void *)lower,word,count_value,0);FIELD(entity,0x300,void *)=record;}
void func_00196980(GeorgeGoalEntity *entity){emit(RECORD_REMOVE,entity,0,0,0,0);FIELD(entity,0x300,void *)=0;}
static double unpack(GeorgeActorBits64 bits64){union{double d;GeorgeActorBits64 u;}v;v.u=bits64;return v.d;}
static GeorgeActorBits64 pack(double value){union{double d;GeorgeActorBits64 u;}v;v.d=value;return v.u;}
GeorgeActorBits64 func_00374848(float value){GeorgeActorBits64 r=pack(value);emit(SOFT_CONVERT,0,0,0,0,value);return r;}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a,GeorgeActorBits64 b){GeorgeActorBits64 r=pack(unpack(a)-unpack(b));emit(SOFT_SUBTRACT,0,0,0,0,0);return r;}
GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a,GeorgeActorBits64 b){GeorgeActorBits64 r=pack(unpack(a)*unpack(b));emit(SOFT_MULTIPLY,0,0,0,0,0);return r;}
GeorgeActorBits64 func_00372C68(GeorgeActorBits64 a,GeorgeActorBits64 b){return pack(unpack(a)+unpack(b));}
s32 func_00373250(GeorgeActorBits64 a,GeorgeActorBits64 b){double x=unpack(a),y=unpack(b);s32 r=x<y?-1:x>y?1:0;emit(SOFT_COMPARE,0,0,0,0,0);return r;}
float func_003734F8(GeorgeActorBits64 value){float r=(float)unpack(value);emit(SOFT_FLOAT,0,0,0,0,r);return r;}

enum { SPECIAL_REQUEST=190, CHANGE_STATE, REFRESH, MASK_QUERY, IDENTIFIER,
       COMPONENT, COMPONENT_POINTER, SET_PROPERTY, PREPARE_OBJECT,
       MASS_SCALE, VECTOR_PAIR, WALK_RECORD };
const u8 D_0042D650[]={9};
static u8 component_storage[0x200], physical_storage[3][0x400], component_table[0x110], physical_tables[2][0x110];
static int pointer_count, predicate_count, mutation8;
static u32 mask_result, identifier_result;
static float mass_scale;
static GeorgeMathVec4 sent_start,sent_displacement;
static GeorgeGoalVirtualObject *mock_component(void *self)
{GeorgeGoalVirtualObject *result=(GeorgeGoalVirtualObject *)physical_storage[pointer_count%3];++pointer_count;emit(COMPONENT_POINTER,self,result,pointer_count,0,0);return result;}
static void mock_pair(void *self,const GeorgeMathVec4 *start,const GeorgeMathVec4 *displacement)
{sent_start=*start;sent_displacement=*displacement;emit(VECTOR_PAIR,self,(void *)start,0,0,0);}
static void reset8(void)
{
 int i;reset_all();memset(component_storage,0,sizeof(component_storage));memset(physical_storage,0,sizeof(physical_storage));memset(component_table,0,sizeof(component_table));memset(physical_tables,0,sizeof(physical_tables));
 pointer_count=predicate_count=mutation8=0;mask_result=1;identifier_result=0x29;mass_scale=.5f;
 ((GeorgeGoalVirtualWord *)(control_table+0xB8))->adjustment=28;((GeorgeGoalVirtualWord *)(control_table+0xB8))->invoke=mock_word;
 FIELD(component_storage,4,void *)=component_table;
 ((GeorgeGoalVirtualPointer *)(component_table+0x98))->adjustment=-8;((GeorgeGoalVirtualPointer *)(component_table+0x98))->invoke=mock_component;
 for(i=0;i<2;++i){((GeorgeActorVirtualFrameParts *)(physical_tables[i]+0x98))->adjustment=(s16)(i?20:-12);((GeorgeActorVirtualFrameParts *)(physical_tables[i]+0x98))->invoke=mock_pair;}
 for(i=0;i<3;++i)FIELD(physical_storage[i],0xA0,void *)=physical_tables[0];
 FIELD(actor,0xA30,void *)=object_storage;FIELD(actor,0xA34,u32)=123;FIELD(actor,0xA40,float)=5;
 FIELD(actor,0xA44,float)=3;FIELD(actor,0xA48,float)=4;FIELD(actor,0xA4C,float)=4;
 FIELD(data_storage,0x178,float)=2;FIELD(data_storage,0x180,float)=5;
 FIELD(data_storage,0x3C4,u32)=11;FIELD(data_storage,0x3C8,u32)=22;FIELD(data_storage,0x3CC,u32)=33;
 FIELD(data_storage,0x3D0,float)=2;FIELD(data_storage,0x3D4,float)=3;animation_records[0x1D]=0x29;
}
void func_00185EA0(GeorgeGoalEntity *entity,void *reference,u32 first,u32 second,const GeorgeMathVec3 *vector)
{CHECK(entity==actor&&first==1&&second==0);emit(SPECIAL_REQUEST,entity,(void *)vector,(u32)reference,0,0);}
void func_00190E00(GeorgeGoalEntity *entity,u32 state){CHECK(entity==actor&&state==39);emit(CHANGE_STATE,entity,0,state,0,0);}
void func_00194FD8(GeorgeGoalEntity *entity){CHECK(entity==actor);emit(REFRESH,entity,0,0,0,0);}
s32 func_0018B710(GeorgeGoalEntity *entity,const GeorgeMathVec3 *direction,const GeorgeMathVec3 *position)
{CHECK(entity==actor);last_vector=*direction;last_motion=*position;emit(WALK_RECORD,entity,(void *)direction,0,0,0);return 0; }
u32 func_00297640(u32 word){u32 result=mask_result;emit(MASK_QUERY,0,0,word,0,0);return result;}
u32 func_002A7418(u32 word){u32 result=identifier_result;emit(IDENTIFIER,0,0,word,0,0);return result;}
void *func_00238BA0(void *object,u32 key){CHECK(key==0x0B6C8F2B);emit(COMPONENT,object,0,key,0,0);return component_storage;}
void func_0022C2C8(void *object,u32 key,void *reference){CHECK(key==0x007269B7);emit(SET_PROPERTY,object,reference,key,0,0);}
float func_0030A6A0(void *object){float result=mass_scale;emit(MASS_SCALE,object,0,0,0,result);return result;}
void func_00307850(void *object){emit(PREPARE_OBJECT,object,0,0,0,0);}
static void mutate(Event *e)
{
 if(mutation8==1&&e->kind==WORD){FIELD(actor,0x1B0,void *)=replacement;FIELD(actor,0x9E4,u8)=77;}
 if(mutation8==2&&e->kind==CANCEL)FIELD(actor,0xA30,u32)=77;
 if(mutation8==3&&e->kind==DURATION){FIELD(actor,0xA08,u16)=99;FIELD(actor,0xA38,u32)=10;FIELD(actor,0x9E5,u8)=2;}
 if(mutation8==4&&e->kind==SPECIAL_REQUEST)FIELD(actor,0x9E5,u8)=200;
 if(mutation8==5&&e->kind==SINE){actor->field18=(GeorgeGoalEntityData *)alternate_data;FIELD(actor,0x9CC,float)=.5f;FIELD(alternate_data,0x3D4,float)=8;FIELD(alternate_data,0x3D0,float)=10;}
 if(mutation8==6&&e->kind==REQUEST){FIELD(actor,0x1B0,void *)=replacement;FIELD(actor,0x9E5,u8)=77;}
 if(mutation8==7&&e->kind==REFRESH)FIELD(actor,0x9D4,float)=2;
 if(mutation8==8&&e->kind==V78){FIELD(actor,0xA0C,float)=1;FIELD(actor,0xA10,u32)=2;}
 if(mutation8==9&&e->kind==REFERENCE_WORD)FIELD(actor,0xA10,u32)=1;
 if(mutation8==10&&e->kind==REQUEST)FIELD(data_storage,0x180,float)=10;
 if(mutation8==11&&e->kind==V78){FIELD(actor,0x300,void *)=alternate_record;FIELD(actor,0x40,float)=10;}
 if(mutation8==12&&e->kind==HANDLE_STOP)FIELD(actor,0x2EC,void *)=replacement;
 if(mutation8==13&&e->kind==HANDLE_FREE)FIELD(actor,0x1B0,void *)=replacement;
 if(mutation8==14&&e->kind==PREDICATE){++predicate_count;predicate_result=predicate_count==1?0:1;}
 if(mutation8==15&&e->kind==EXIT_STATE)FIELD(actor,0x190,GeorgeActorBits64)=0x80;
 if(mutation8==16&&e->kind==PREPARE_OBJECT)FIELD(e->object,0xA0,void *)=physical_tables[1];
 if(mutation8==17&&e->kind==SET_PROPERTY){FIELD(actor,0xA30,void *)=replacement;FIELD(replacement,0x40,float)=1;}
 if(mutation8==18&&e->kind==MASS_SCALE){FIELD(actor,0xA30,void *)=replacement;FIELD(replacement,0x40,float)=9;}
 if(mutation8==19&&e->kind==ARRAY_COUNT)FIELD(actor,0x1B0,void *)=replacement;
 if(mutation8==20&&e->kind==REQUEST)FIELD(actor,0xA38,u32)=10;
 if(mutation8==21&&e->kind==CLEAR_HANDLES)FIELD(data_storage,0x110,u32)=77;
 if(mutation8==22&&e->kind==HASH)D_003F2D40=replacement;
 if(mutation8==23&&e->kind==VECTOR_PAIR)FIELD(actor,0xA38,u32)=100;
 if(mutation8==24&&e->kind==VB0&&self_offset(e->object)==52){GeorgeMathVec3 *v=e->argument;v->x=3;v->y=77;v->z=4;}
 if(mutation8==25&&e->kind==ANIMATION_BITS)FIELD(data_storage,0x418,u32)=77;
 if(mutation8==26&&e->kind==IDENTIFIER)FIELD(actor,0x1B0,void *)=replacement;
 if(mutation8==27&&e->kind==LOOKUP_WORD)FIELD(actor,0xA38,u32)=99;
 if(mutation8==28&&e->kind==RELEASE){FIELD(actor,0xA30,u32)=77;FIELD(actor,0x14,u32)=888;}
}
static void direct_member(u32 state,s16 adjustment){GeorgeGoalMember *m=(GeorgeGoalMember *)(D_003F83F0+state*28);m->selector=-1;m->adjustment=adjustment;m->target.direct=mock_enter;}

static void test_lifecycle(void)
{
 reset8();FIELD(actor,0x190,GeorgeActorBits64)=0x80000000001ULL;FIELD(data_storage,0x1D8,u32)=0x45A78000;FIELD(&control.object,0x34,u32)=77;mutation8=1;hook=mutate;func_001A7708(actor);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==0x80000040001ULL&&FIELD(&control.object,0x34,u32)==0&&FIELD(actor,0x9E4,u8)==0&&nth(WORD,0)->word==0);
 reset8();FIELD(actor,0x190,GeorgeActorBits64)=0x80000040001ULL;FIELD(data_storage,0x1D8,u32)=0x45A78000;FIELD(&control.object,0x34,u32)=77;mutation8=1;hook=mutate;func_001A7810(actor);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==0x80000000001ULL&&FIELD(&control.object,0x34,u32)==1&&FIELD(replacement,0x434,u32)==0&&nth(CANCEL,0)->object==replacement);
 reset8();FIELD(data_storage,0x1D8,u32)=0x45A78001;FIELD(&control.object,0x34,u32)=77;func_001A7708(actor);CHECK(FIELD(&control.object,0x34,u32)==77);
 reset8();FIELD(actor,0xA0C,u32)=99;FIELD(actor,0xA08,u16)=99;mutation8=22;hook=mutate;func_00180458(actor);CHECK(FIELD(actor,0xA0C,u32)==0&&FIELD(actor,0xA08,u16)==0&&FIELD(actor,0x2EC,void *)==handle&&nth(EMIT,0)->object==object_storage&&D_003F2D40==replacement);
 reset8();mutation8=12;hook=mutate;func_00194A00(actor);CHECK(nth(HANDLE_STOP,0)->object==handle&&nth(HANDLE_FREE,0)->object==replacement&&FIELD(actor,0x2EC,u32)==0&&nth(CANCEL,0)->object==primary);
 reset8();mutation8=13;hook=mutate;func_00194A00(actor);CHECK(nth(CANCEL,0)->object==replacement);
 reset8();FIELD(actor,0xA3C,u32)=99;FIELD(actor,0xA30,u32)=99;FIELD(actor,0xA38,u32)=99;func_00196920(actor);CHECK(FIELD(actor,0xA3C,u32)==0&&FIELD(actor,0xA30,u32)==0&&FIELD(actor,0xA38,u32)==0);
 reset8();mutation8=2;hook=mutate;func_00196930(actor);CHECK(nth(RELEASE,0)->object==(void *)77&&FIELD(actor,0xA30,u32)==0);
 reset8();FIELD(actor,0x1B0,u32)=0;FIELD(actor,0xA30,u32)=0;func_00196930(actor);CHECK(count(CANCEL)==0&&count(RELEASE)==0);
 reset8();FIELD(actor,0xA08,u16)=65535;func_001949B8(0,actor,point);CHECK(FIELD(actor,0xA08,u16)==0&&FIELD(actor,0xA0C,float)==1);
 reset8();FIELD(actor,0xA38,u32)=0xFFFFFFFF;func_001968D8(0,actor,point);CHECK(FIELD(actor,0xA38,u32)==0&&FIELD(actor,0xA3C,float)==1);
 reset8();mutation8=3;hook=mutate;func_001949B8(0,actor,point);CHECK(FIELD(actor,0xA08,u16)==99);func_001968D8(0,actor,point);CHECK(FIELD(actor,0xA38,u32)==11);
 reset8();FIELD(actor,0x9E5,u8)=1;func_001A7698(0,actor,point);CHECK(FIELD(actor,0x9D0,float)==1&&FIELD(actor,0x9E4,u8)==1&&count(SPECIAL_REQUEST)==0);
 reset8();FIELD(actor,0x9E5,u8)=2;FIELD(actor,0x9E8,void *)=point;mutation8=4;hook=mutate;func_001A7698(0,actor,point);CHECK(count(SPECIAL_REQUEST)==1&&nth(SPECIAL_REQUEST,0)->word==(u32)point&&nth(SPECIAL_REQUEST,0)->argument==data_storage+0x3E4&&FIELD(actor,0x9E4,u8)==200);
}
static void test_state39(void)
{
 int p,n;float blends[]={-1,-0.0f,0,.5f,1,0};blends[5]=from_bits(0x7FC00001);
 for(p=-128;p<128;++p){reset8();FIELD(actor,0x9E4,signed char)=(signed char)p;FIELD(actor,0x9CC,float)=.5f;func_001A7778(actor);CHECK(count(WORD)==(p==4)&&FIELD(primary,0x428,float)==(p==2?.5f:0));}
 for(n=0;n<6;++n){reset8();FIELD(actor,0x9E4,u8)=2;FIELD(actor,0x9CC,float)=blends[n];func_001A7778(actor);if(n==1)CHECK(bits(FIELD(primary,0x428,float))==0x80000000);else near_value(FIELD(primary,0x428,float),n==3?.5f:n==4?.99000000953674316f:0);}
 for(p=31;p<41;++p)for(n=-1;n<6;++n){reset8();actor->field0C=(u32)p;FIELD(actor,0x9E4,signed char)=(signed char)n;CHECK(func_001A7560(actor)==(p==39)&&func_001A7570(actor)==(p==39&&n==4));}
 for(p=31;p<34;++p)for(n=-2;n<3;++n){reset8();actor->field0C=(u32)p;FIELD(actor,0x180,signed char)=(signed char)n;FIELD(actor,0x9CC,float)=.5f;CHECK(func_001A7598(actor,123)==(p==32&&n==1));CHECK(count(CHANGE_STATE)==(p==32&&n==1));if(p==32&&n==1)CHECK(FIELD(actor,0x9E8,u32)==123&&FIELD(actor,0x9CC,u32)==0&&count(EXIT_STATE)==1);}
 reset8();FIELD(actor,0x9CC,float)=2;cosine_result=.6f;sine_result=.8f;func_001A7600(actor,1.25f);near_value(FIELD(actor,0x9D8,float),3.6f);near_value(FIELD(actor,0x9DC,float),4);near_value(FIELD(actor,0x9E0,float),4.8f);CHECK(FIELD(actor,0x9D4,float)==1.25f);
 reset8();cosine_result=.6f;sine_result=.8f;mutation8=5;hook=mutate;func_001A7600(actor,2);near_value(FIELD(actor,0x9D8,float),2.4f);near_value(FIELD(actor,0x9DC,float),5);near_value(FIELD(actor,0x9E0,float),3.2f);
 reset8();actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x60C);FIELD(actor,0x9CC,float)=1;FIELD(actor,0x9DC,float)=8;FIELD(actor,0x9E0,float)=3;cosine_result=.6f;sine_result=.8f;func_001A7600(actor,1);CHECK(FIELD(actor,0x9DC,float)==0);near_value(FIELD(actor,0x9E0,float),2.4f);
 for(p=-2;p<5;++p){reset8();mutation8=6;hook=mutate;func_001A7378(actor,p);CHECK(count(REQUEST)==(p>=0&&p<=2));if(p==0||p==1)CHECK(nth(REQUEST,0)->word==(p==0?11:22)&&FIELD(actor,0x9E5,u8)==(p==0?1:2));if(p==1)CHECK(FIELD(replacement,0x434,u32)==1);if(p==2)CHECK(FIELD(replacement,0x434,u32)==0&&FIELD(actor,0x9E4,u8)==4&&FIELD(actor,0x9E5,u8)==4&&count(REFRESH)==1);}
 reset8();mutation8=7;hook=mutate;func_001A7378(actor,2);CHECK(FIELD(actor,0x64,float)==2&&FIELD(actor,0x58,float)==2);
 reset8();mutation8=24;hook=mutate;func_001A7378(actor,2);CHECK(FIELD(actor,0x9D8,float)==3&&FIELD(actor,0x9DC,float)==0&&FIELD(actor,0x9E0,float)==4&&count(SCALE_VECTOR)==1);
}
static void prepare40(s32 phase)
{reset8();FIELD(actor,0xA08,s16)=(s16)phase;FIELD(actor,0xA0C,float)=1;direct_member(0,-8);direct_member(3,12);}
static void test_state40(void)
{
 int p,i,n;int phases[]={-1,0,1,100,200,400,500,501,502};float timers[]={-1,0,.25f,1,0};timers[4]=from_bits(0x7FC00001);
 for(p=0;p<9;++p)for(n=-1;n<3;++n){prepare40(phases[p]);FIELD(actor,0xA10,s32)=n;func_00180580(actor);if(phases[p]==1)CHECK(count(V78)==1&&count(REQUEST)==0);else if(phases[p]==100||phases[p]==200)CHECK(count(REQUEST)==2&&count(V78)==1&&count(UPDATE_BOUNDS)==1);else if(phases[p]==400)CHECK(count(WALK_RECORD)==1&&count(RECORD_REMOVE)==1);else if(phases[p]==500)CHECK(count(RECORD_REMOVE)==1&&nth(REQUEST,0)->word==(n==1?0xAE:0xB2));else if(phases[p]==501)CHECK(count(PREDICATE)==1&&count(EXIT_STATE)==0);else CHECK(nth(REQUEST,0)->word==(n==1?0xAC:0xB0));}
 for(i=0;i<5;++i){prepare40(1);FIELD(actor,0xA0C,float)=timers[i];FIELD(actor,0xA10,u32)=1;func_00180580(actor);CHECK(count(HANDLE_START)==(timers[i]-.25f<=0)&&FIELD(actor,0xA08,u16)==(timers[i]-.25f<=0?100:1));}
 prepare40(1);FIELD(actor,0xA0C,float)=0;mutation8=8;hook=mutate;func_00180580(actor);CHECK(count(HANDLE_START)==0&&FIELD(actor,0xA0C,float)==1);
 prepare40(1);FIELD(actor,0xA0C,float)=0;FIELD(data_storage,0x110,u32)=123;FIELD(actor,0x28C,u32)=1;mutation8=21;hook=mutate;func_00180580(actor);CHECK(nth(LOOKUP_WORD,0)->word==123&&count(CREATE_WORD)==2&&FIELD(actor,0xA08,u16)==200);
 prepare40(1);FIELD(actor,0xA0C,float)=0;FIELD(data_storage,0x110,u32)=123;mutation8=9;hook=mutate;func_00180580(actor);CHECK(FIELD(actor,0xA08,u16)==100);
 prepare40(100);func_00180580(actor);CHECK(last_vector.x==0&&last_vector.y==-5&&last_vector.z==0);near_value(last_bounds[0].x,-2);near_value(last_bounds[0].y,-.05f);near_value(last_bounds[1].y,1);CHECK(last_bounds[0].w==0&&last_bounds[1].w==0);
 prepare40(100);FIELD(actor,0x300,u32)=0;func_00180580(actor);CHECK(count(RECORD_BUILD)==1&&nth(RECORD_BUILD,0)->word==8&&last_bounds[0].y==0&&last_bounds[1].y==1.5f);
 prepare40(200);FIELD(actor,0xD0,float)=3;FIELD(actor,0xD4,float)=0;FIELD(actor,0xD8,float)=4;func_00180580(actor);CHECK(last_vector.x==3&&last_vector.y==-5&&last_vector.z==4);near_value(last_bounds[0].x,-1.94f);near_value(last_bounds[0].y,-.1f);near_value(last_bounds[0].z,-1.92f);near_value(last_bounds[1].x,2.06f);near_value(last_bounds[1].y,.9f);near_value(last_bounds[1].z,2.08f);
 prepare40(200);FIELD(actor,0x300,u32)=0;FIELD(actor,0xD0,float)=3;FIELD(actor,0xD8,float)=4;func_00180580(actor);near_value(last_bounds[1].y,1.4f);CHECK(count(RECORD_BUILD)==1);
 prepare40(100);mutation8=10;hook=mutate;func_00180580(actor);CHECK(last_vector.y==-10);
 prepare40(100);mutation8=11;hook=mutate;func_00180580(actor);CHECK(nth(UPDATE_BOUNDS,0)->object==alternate_record);near_value(last_bounds[0].x,8);
 prepare40(400);FIELD(actor,0xD0,float)=2;FIELD(actor,0xD4,float)=3;FIELD(actor,0xD8,float)=4;FIELD(actor,0x4C,float)=3;FIELD(actor,0x50,float)=4;FIELD(actor,0x54,float)=0;func_00180580(actor);CHECK(last_vector.x==-2&&last_vector.y==-4&&last_vector.z==-4);near_value(last_motion.x,1.2f);near_value(last_motion.y,1.6f);CHECK(count(RECORD_REMOVE)==1);
 for(i=0;i<5;++i){prepare40(501);FIELD(actor,0xA0C,float)=timers[i];func_00180580(actor);CHECK(count(EXIT_STATE)==(timers[i]-.25f<=0)&&count(PREDICATE)==(timers[i]-.25f<=0?2:1));if(timers[i]-.25f<=0)CHECK(actor->field0C==3&&last_enter==ADDRESS(actor,12));}
 prepare40(501);FIELD(actor,0xA0C,float)=0;mutation8=14;hook=mutate;func_00180580(actor);CHECK(count(WORD)==0&&count(PREDICATE)==2&&actor->field0C==3);
 prepare40(501);FIELD(actor,0xA0C,float)=0;mutation8=15;hook=mutate;func_00180580(actor);CHECK(count(PREDICATE)==1&&actor->field0C==0&&last_enter==ADDRESS(actor,-8));
}
static void test_state41(void)
{
 int i,p;float timers[]={-1,0,.25f,1,0};u32 phases[]={0,1,2,3,4,5,0x80000000u,0xFFFFFFFFu};timers[4]=from_bits(0x7FC00001);
 for(p=0;p<8;++p)for(i=0;i<5;++i){reset8();FIELD(actor,0xA38,u32)=phases[p];FIELD(actor,0xA3C,float)=timers[i];FIELD(actor,0xA30,u32)=0;func_0018E0E8(actor);CHECK(count(A0)==1);if(phases[p]==1)CHECK(FIELD(actor,0xA38,u32)==2&&bits(FIELD(actor,0xA3C,float))==bits(timers[i]));else if(phases[p]==2)CHECK(FIELD(actor,0xA38,u32)==2&&FIELD(actor,0x14,u32)==(timers[i]-.25f<=0?0:99));else if(phases[p]==3)CHECK(FIELD(actor,0xA38,u32)==4&&count(COMPONENT)==0);else if(phases[p]==4)CHECK(count(RELEASE)==0&&FIELD(actor,0x14,u32)==(timers[i]-.25f<=0?0:99));else CHECK(count(REQUEST)==2&&nth(REQUEST,0)->word==0xC7&&FIELD(actor,0xA38,u32)==phases[p]);}
 reset8();mutation8=20;hook=mutate;func_0018E0E8(actor);CHECK(FIELD(actor,0xA38,u32)==10);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;func_0018E0E8(actor);CHECK(FIELD(actor,0xA38,u32)==3&&FIELD(actor,0xA30,void *)==handle&&count(CREATE_WORD)==1&&count(MATRIX_COPY)==1);CHECK(last_matrix_input==(const GeorgeRotationMatrix *)&animation_matrices[0]&&nth(MATRIX_MULTIPLY,0)->argument==ADDRESS(actor,0xF0));CHECK((FIELD(handle,0xA0,u32)&0x100)!=0&&count(REFERENCE_WORD)==1);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;FIELD(actor,0xA34,u32)=0;animation_bits=1;func_0018E0E8(actor);CHECK(FIELD(actor,0x14,u32)==0&&count(CREATE_WORD)==1&&FIELD(actor,0xA38,u32)==3);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=.25f;animation_bits=1;func_0018E0E8(actor);CHECK(count(CREATE_WORD)==1&&FIELD(actor,0xA30,u32)==0&&FIELD(actor,0x14,u32)==0&&count(RELEASE)==0);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;mask_result=2;func_0018E0E8(actor);CHECK(count(IDENTIFIER)==0&&count(CREATE_WORD)==0&&FIELD(actor,0xA38,u32)==2);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;identifier_result=0x100;animation_records[0x1D]=0;func_0018E0E8(actor);CHECK(last_matrix_input==0&&count(CREATE_WORD)==1&&FIELD(actor,0xA38,u32)==3);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;mutation8=25;hook=mutate;func_0018E0E8(actor);CHECK(nth(MASK_QUERY,0)->word==77);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;FIELD(primary,0x378,void *)=point;mutation8=26;hook=mutate;func_0018E0E8(actor);CHECK(nth(ARRAY_COUNT,0)->object==reference&&nth(ARRAY_RECORDS,0)->object==reference);
 reset8();FIELD(actor,0xA38,u32)=2;FIELD(actor,0xA3C,float)=1;animation_bits=1;mutation8=27;hook=mutate;func_0018E0E8(actor);CHECK(FIELD(actor,0xA38,u32)==100);
 reset8();FIELD(actor,0xA38,u32)=3;FIELD(actor,0xA3C,float)=1;func_0018E0E8(actor);CHECK(pointer_count==3&&FIELD(actor,0xA38,u32)==4&&FIELD(actor,0xA3C,float)==.75f);CHECK(nth(SET_PROPERTY,0)->object==physical_storage[0]&&nth(SET_PROPERTY,0)->argument==object_storage&&nth(MASS_SCALE,0)->object==physical_storage[1]+0xA0&&nth(PREPARE_OBJECT,0)->object==physical_storage[2]);CHECK(nth(VECTOR_PAIR,0)->object==physical_storage[2]+0x94);near_value(sent_start.x,0);near_value(sent_start.y,0);near_value(sent_start.z,0);near_value(sent_displacement.x,1.5f);near_value(sent_displacement.y,4.45f);near_value(sent_displacement.z,2);CHECK(sent_start.w==0&&sent_displacement.w==0);
 reset8();FIELD(actor,0xA38,u32)=3;mutation8=16;hook=mutate;func_0018E0E8(actor);CHECK(nth(VECTOR_PAIR,0)->object==physical_storage[2]+0xB4);
 reset8();FIELD(actor,0xA38,u32)=3;mutation8=17;hook=mutate;func_0018E0E8(actor);near_value(sent_start.x,1);near_value(sent_displacement.x,1.118033988749895f);near_value(sent_displacement.z,2.23606797749979f);
 reset8();FIELD(actor,0xA38,u32)=3;mutation8=18;hook=mutate;func_0018E0E8(actor);near_value(sent_start.x,9);near_value(sent_displacement.x,1.5f);
 reset8();FIELD(actor,0xA38,u32)=3;mutation8=23;hook=mutate;func_0018E0E8(actor);CHECK(FIELD(actor,0xA38,u32)==101);
 for(i=0;i<5;++i){reset8();FIELD(actor,0xA38,u32)=4;FIELD(actor,0xA3C,float)=timers[i];func_0018E0E8(actor);CHECK(count(RELEASE)==(timers[i]-.25f<=0));if(timers[i]-.25f<=0)CHECK(FIELD(actor,0xA30,u32)==0&&FIELD(actor,0x14,u32)==0);}
 reset8();FIELD(actor,0xA38,u32)=4;mutation8=28;hook=mutate;func_0018E0E8(actor);CHECK(FIELD(actor,0xA30,u32)==0&&FIELD(actor,0x14,u32)==0);
}
int main(void){CHECK(sizeof(void *)==4);test_lifecycle();test_state39();test_state40();test_state41();printf("actor_states8: %d checks passed\n",checks);return 0;}
