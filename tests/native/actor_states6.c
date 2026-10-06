/* Asset-free call-boundary tests. Expected state outcomes and callback
 * mutations are independent specifications; no retail instructions are used. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_states6.c"

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
static u8 object_storage[0x400], handle[64], reference[0x100];
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
{ ++checks; if(!value){fprintf(stderr,"actor_states6:%d: %s\n",line,label);exit(1);} }
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
static void reset6(void)
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
{CHECK(mode0==0&&mode1==0&&invoke_word==0&&callback_word==0&&mode2==0&&context==actor&&callback==func_00196420);emit(REQUEST,object,(void *)callback,word,bits(scale),time);}
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
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *first,const GeorgeRotationMatrix *second){memcpy(output,first,64);emit(MATRIX_MULTIPLY,output,(void *)second,0,0,0);}
float func_00141590(const GeorgeMathAngularAC *object){float value=samples[sample_count];CHECK(sample_count<8);++sample_count;emit(ANGULAR_SAMPLE,(void *)object,0,sample_count,0,value);return value;}
void func_00141638(void *object,GeorgeMathVec3 *output){*output=normal_result;retained_normal6=output;emit(NORMAL_SAMPLE,object,output,0,0,0);}
void func_00141678(void *object,GeorgeMathVec3 *output){*output=direction_result;retained_direction6=output;emit(DIRECTION_SAMPLE,object,output,0,0,0);}
void func_00141348(void *object){emit(OBJECT_RESET,object,0,0,0,0);}

static int mutation_mode;
static void mutate6(Event *e)
{
 if(mutation_mode==1&&e->kind==DURATION){FIELD(actor,0x908,u32)=0xFFFFFFFF;FIELD(actor,0x924,u32)=0xFFFFFFFF;FIELD(actor,0x9F0,u16)=400;}
 if(mutation_mode==2&&e->kind==HANDLE_STOP){FIELD(actor,0x2EC,void *)=replacement;FIELD(actor,0x2F4,void *)=replacement;FIELD(actor,0x2F0,void *)=replacement;}
 if(mutation_mode==3&&e->kind==CANCEL)FIELD(actor,0x24,void *)=replacement;
 if(mutation_mode==4&&e->kind==KEY_MODE)FIELD(actor,0x1B0,void *)=replacement;
 if(mutation_mode==5&&e->kind==FRAME){FIELD(actor,0x424,void *)=replacement;FIELD(actor,0x2F4,void *)=0;}
 if(mutation_mode==6&&e->kind==RECORD_REMOVE)FIELD(actor,0x300,void *)=alternate_record;
 if(mutation_mode==7&&e->kind==RECORD_RELEASE)FIELD(actor,0x2F8,void *)=&callback_object;
 if(mutation_mode==8&&e->kind==RECORD_ALLOC){FIELD(actor,0x40,float)=99;D_004961F4=replacement;}
 if(mutation_mode==9&&e->kind==ALLOCATE&&e->word==0x38)FIELD(actor,0x300,void *)=record;
 if(mutation_mode==10&&e->kind==RECORD_CALLBACK)FIELD(actor,0x300,void *)=record;
 if(mutation_mode==11&&e->kind==HANDLE_START)FIELD(actor,0x908,u32)=1000;
 if(mutation_mode==12&&e->kind==COSINE)FIELD(actor,0x58,float)=2;
 if(mutation_mode==13&&e->kind==QUERY&&vector_queries==2)returned_vectors[0].x=9;
 if(mutation_mode==14&&e->kind==PROJECT){FIELD(actor,0x58,float)=7;FIELD(actor,0x300,void *)=alternate_record;FIELD(actor,0x40,float)=10;actor->field18=(GeorgeGoalEntityData *)alternate_data;FIELD(alternate_data,0x0C,float)=1;}
 if(mutation_mode==15&&e->kind==VB0){FIELD(actor,0x910,u32)=200;FIELD(actor,0x90C,u32)=100;FIELD(actor,0x3DC,u32)=0;}
 if(mutation_mode==16&&e->kind==HANDLE_FINISH)FIELD(actor,0x908,u32)=0xFFFFFFFF;
 if(mutation_mode==17&&e->kind==REQUEST&&count(REQUEST)==1){FIELD(actor,0x1B0,void *)=replacement;FIELD(actor,0x368,u32)=55;FIELD(actor,0x92C,u16)=777;}
 if(mutation_mode==18&&e->kind==VOID38){FIELD(actor,0x930,float)=-1;FIELD(actor,0x92E,u16)=1;}
 if(mutation_mode==19&&e->kind==ANGULAR_SAMPLE){FIELD(actor,0x424,void *)=replacement;if(sample_count==3)FIELD(actor,0x1B0,void *)=replacement;}
 if(mutation_mode==20&&e->kind==NORMALIZE){if(count(NORMALIZE)==1)retained_orientation=(GeorgeMathVec3 *)e->object;else if(count(NORMALIZE)==2)retained_orientation->x=10;}
 if(mutation_mode==21&&e->kind==OBJECT_RESET){retained_direction6->x=0;retained_direction6->y=2;retained_direction6->z=5;}
 if(mutation_mode==22&&e->kind==ANGLE){retained_direction6->y=-3;FIELD(actor,0x548,float)=0.25f;}
 if(mutation_mode==23&&e->kind==RECORD_BUILD)FIELD(actor,0x300,void *)=record;
 if(mutation_mode==24&&e->kind==MATRIX_COPY&&count(MATRIX_COPY)==1)FIELD(actor,0x240,void *)=replacement;
 if(mutation_mode==25&&e->kind==VOID38)FIELD(actor,0x1B0,void *)=replacement;
 if(mutation_mode==26&&e->kind==NORMALIZE&&count(NORMALIZE)==1)FIELD(actor,0x540,float)=8;
 if(mutation_mode==27&&e->kind==ENTER_STATE)FIELD(actor,0x9F0,u16)=1234;
}

static void direct_member(u32 state,s16 adjustment)
{GeorgeGoalMember *member=(GeorgeGoalMember *)(D_003F83F0+state*28);member->selector=-1;member->adjustment=adjustment;member->target.direct=mock_enter;}
static void test_durations(void)
{
 float values[]={0,4800,-2400,9600};int i;
 for(i=0;i<4;++i){reset6();duration_result=values[i];FIELD(actor,0x908,u32)=0xFFFFFFFF;func_00196280(0,actor,point);CHECK(FIELD(actor,0x908,u32)==0);near_value(FIELD(actor,0x918,float),values[i]*0.000208333338377997279f);if(values[i])near_value(FIELD(actor,0x91C,float),4800/values[i]);else CHECK(isinf(FIELD(actor,0x91C,float)));}
 reset6();mutation_mode=1;hook=mutate6;func_00196280(0,actor,point);CHECK(FIELD(actor,0x908,u32)==0);
 reset6();mutation_mode=1;hook=mutate6;func_00196420(0,actor,point);CHECK(FIELD(actor,0x924,u32)==0&&FIELD(actor,0x928,float)==1);
 reset6();FIELD(actor,0x92C,u16)=777;func_00196680(0,actor,point);CHECK(FIELD(actor,0x92C,u16)==777&&FIELD(actor,0x930,float)==1);
 reset6();FIELD(actor,0x9F0,u16)=65535;func_00196700(0,actor,point);CHECK(FIELD(actor,0x9F0,u16)==0&&FIELD(actor,0x9F8,float)==1);
 reset6();mutation_mode=1;hook=mutate6;func_00196700(0,actor,point);CHECK(FIELD(actor,0x9F0,u16)==400);
}
static void test_lifecycle(void)
{
 int i;for(i=0;i<4;++i){reset6();FIELD(actor,0x190,GeorgeActorBits64)=i&1?0x20000ULL:0;gate_result=i&2;func_001962E0(actor);CHECK(FIELD(actor,0x908,u32)==100&&FIELD(actor,0x90C,u32)==20&&FIELD(actor,0x918,u32)==0);CHECK(count(GATE)==(!(i&1)));CHECK(count(CLEAR_FLAG)==(!(i&1)&&!!(i&2)));}
 reset6();FIELD(actor,0x190,GeorgeActorBits64)=0x20000;mutation_mode=2;hook=mutate6;func_00196368(actor);CHECK(nth(HANDLE_FREE,0)->object==replacement&&FIELD(actor,0x2EC,void *)==0);CHECK(count(RECORD_REMOVE)==1&&count(REFERENCE_WORD)==1&&FIELD(actor,0x4FC,float)==7);CHECK(count(SET_FLAG)==1&&count(CANCEL)==1);
 reset6();FIELD(actor,0x2EC,void *)=0;FIELD(actor,0x300,void *)=0;FIELD(actor,0x1B0,void *)=0;func_00196368(actor);CHECK(count(HANDLE_STOP)==0&&count(RECORD_REMOVE)==0&&nth(CANCEL,0)->object==0);
 reset6();func_00196748(actor);CHECK(count(VOID38)==1&&FIELD(actor,0x9F0,u16)==0);
 reset6();FIELD(actor,0x40,float)=1;FIELD(actor,0x44,float)=2;FIELD(actor,0x48,float)=3;FIELD(data_storage,0x118,float)=4;func_00196468(actor);CHECK(FIELD(actor,0x924,u32)==100&&(FIELD(actor,0x190,GeorgeActorBits64)&0x40000));near_value(FIELD(object_storage,0x40,float),1);near_value(FIELD(object_storage,0x44,float),6);near_value(FIELD(object_storage,0x48,float),3);CHECK(FIELD(object_storage,0x4C,float)==1&&(FIELD(object_storage,0xA0,u32)&0x100));
 reset6();FIELD(actor,0x240,void *)=0;func_00196468(actor);CHECK(count(MATRIX_COPY)==0);
 reset6();FIELD(data_storage,0x194,u32)=1;FIELD(actor,0x1CC,void *)=handle;mutation_mode=4;hook=mutate6;func_00196608(actor);CHECK(count(KEY_MODE)==2&&nth(KEY_MODE,0)->word==0xB95616B6&&nth(KEY_MODE,1)->other==0&&nth(CANCEL,0)->object==replacement);
 reset6();func_001966C0(actor);CHECK(count(VOID38)==1&&count(CANCEL)==1);
 reset6();FIELD(actor,0x2F4,void *)=handle;FIELD(actor,0x2F0,void *)=point;mutation_mode=2;hook=mutate6;func_0018D648(actor);CHECK(count(FRAME)==1&&count(HANDLE_STOP)==2&&nth(HANDLE_FREE,0)->object==replacement&&nth(HANDLE_FREE,1)->object==replacement);CHECK(FIELD(actor,0x424,void *)==0&&FIELD(actor,0x9EC,void *)==object_storage&&nth(OBJECT_RELEASE,0)->object==object_storage);
 reset6();mutation_mode=5;hook=mutate6;func_0018D648(actor);CHECK(FIELD(actor,0x9EC,void *)==replacement&&nth(OBJECT_RELEASE,0)->object==replacement);
}
static void test_records(void)
{
 GeorgeMathVec3 lower={1,2,3},upper={4,5,6};int i;
 reset6();func_0018E5A8(actor,&lower,&upper,0xAABBCCDD,7);CHECK(nth(RECORD_ALLOC,0)->object==parent&&FIELD(record,4,u16)==0xA0);CHECK(last_bounds[0].x==1&&last_bounds[1].z==6&&last_bounds[0].w==0&&last_bounds[1].w==0);CHECK(nth(RECORD_BUILD,0)->word==7&&FIELD(actor,0x300,void *)==alternate_record);CHECK(FIELD(actor,0x2F8,void *)==replacement&&FIELD(replacement,0,u32)==(u32)D_0042E780&&FIELD(replacement,4,void *)==actor&&FIELD(replacement,8,u32)==0xAABBCCDD&&FIELD(replacement,12,u32)==0);CHECK(nth(RECORD_CALLBACK,0)->object==alternate_record&&nth(RECORD_ADD,0)->argument==alternate_record);
 reset6();mutation_mode=9;hook=mutate6;func_0018E5A8(actor,&lower,&upper,1,2);CHECK(nth(RECORD_CALLBACK,0)->object==record);
 reset6();mutation_mode=10;hook=mutate6;func_0018E5A8(actor,&lower,&upper,1,2);CHECK(nth(RECORD_ADD,0)->argument==record);
 for(i=0;i<4;++i){reset6();if(!(i&1))FIELD(actor,0x300,void *)=0;if(!(i&2))FIELD(actor,0x2F8,void *)=0;func_00196980(actor);CHECK(count(RECORD_REMOVE)==(i==3));CHECK(count(REFERENCE_WORD)==(i==3));if(i!=3)CHECK(FIELD(actor,0x300,void *)==(i&1?record:0)&&FIELD(actor,0x2F8,void *)==(i&2?&callback_object:0));}
 reset6();mutation_mode=6;hook=mutate6;func_00196980(actor);CHECK(nth(RECORD_REMOVE,0)->argument==record&&nth(RECORD_RELEASE,0)->object==alternate_record&&FIELD(actor,0x300,void *)==0&&FIELD(actor,0x2F8,void *)==0);CHECK(nth(REFERENCE_WORD,0)->object==ADDRESS(&callback_object,-8)&&nth(REFERENCE_WORD,0)->word==3);
}
static void test_state32(void)
{
 s32 phases[]={-2147483647-1,-1,0,99,100,101,102,103,104,2147483647};float timers[]={-1,0,.25f,1,0};int i,j;timers[4]=from_bits(0x7FC00001);
 for(i=0;i<10;++i)for(j=0;j<5;++j){reset6();FIELD(actor,0x924,s32)=phases[i];FIELD(actor,0x17C,float)=timers[j];FIELD(actor,0x184,u32)=77;FIELD(actor,0x188,float)=2;func_00196500(actor);if(phases[i]==100)CHECK(count(REQUEST)==1&&count(KEY_MODE)==1&&nth(REQUEST,0)->word==77&&nth(REQUEST,0)->other==bits(2));else if(phases[i]==101)CHECK(FIELD(actor,0x924,u32)==102);else if(phases[i]==102)CHECK(FIELD(actor,0x924,u32)==(timers[j]-.25f>0?102u:103u)&&count(A0)==(timers[j]-.25f>0));else if(phases[i]==103)CHECK(FIELD(actor,0x14,u32)==0);else CHECK(count(REQUEST)==0&&FIELD(actor,0x924,s32)==phases[i]);}
}
static void test_state30(void)
{
 s32 phases[]={-2147483647-1,-1,0,99,100,101,102,103,104,2147483647};int i,k;
 for(i=0;i<10;++i){reset6();FIELD(actor,0x908,s32)=phases[i];FIELD(actor,0x918,float)=1;FIELD(actor,0x920,float)=5;func_0018C0C8(actor);if(phases[i]==100)CHECK(count(REQUEST)==2&&FIELD(actor,0x908,u32)==101&&count(RECORD_BUILD)==1&&last_bounds[0].x==-2&&last_bounds[1].y==5);else if(phases[i]==102)CHECK(count(PROJECT)==1&&count(UPDATE_BOUNDS)==1&&count(QUERY)==2&&FIELD(actor,0x918,float)==.75f&&FIELD(actor,0x920,float)==5.25f);else if(phases[i]==103)CHECK(actor->field0C==3&&count(EXIT_STATE)==1);else CHECK(event_count==0&&FIELD(actor,0x908,s32)==phases[i]);}
 reset6();FIELD(actor,0x908,u32)=100;request_result=0;func_0018C0C8(actor);CHECK(FIELD(actor,0x908,u32)==103&&count(RECORD_BUILD)==0);
 reset6();FIELD(actor,0x908,u32)=100;mutation_mode=11;hook=mutate6;func_0018C0C8(actor);CHECK(FIELD(actor,0x908,u32)==1001);
 reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=1;mutation_mode=12;hook=mutate6;func_0018C0C8(actor);CHECK(nth(COSINE,0)->value==0&&nth(SINE,0)->value==2);
 reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=1;mutation_mode=13;hook=mutate6;func_0018C0C8(actor);near_value(last_motion.x,18/sqrtf(94));
 reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=1;mutation_mode=14;hook=mutate6;func_0018C0C8(actor);CHECK(nth(SET_ANGLE,0)->value==7&&nth(UPDATE_BOUNDS,0)->object==alternate_record);CHECK(last_bounds[0].x==9&&last_bounds[1].x==11);
 for(k=0;k<3;++k){reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=1;control_result=1;FIELD(actor,0x910,s32)=k==0?-1:20;FIELD(actor,0x90C,s32)=20;if(k==2)FIELD(actor,0x3DC,u32)=0;func_0018C0C8(actor);CHECK(FIELD(actor,0x910,s32)==(k==2?20:0));CHECK(count(BUILD_RECORD)==(k==1));}
 reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=1;control_result=1;mutation_mode=15;hook=mutate6;func_0018C0C8(actor);CHECK(FIELD(actor,0x910,u32)==200&&count(BUILD_RECORD)==0);
 for(k=0;k<3;++k){reset6();FIELD(actor,0x908,u32)=102;FIELD(actor,0x918,float)=k==0?.25f:k==1?-1:from_bits(0x7FC00001);mutation_mode=16;hook=mutate6;func_0018C0C8(actor);CHECK(count(PROJECT)==0&&count(HANDLE_FINISH)==1&&FIELD(actor,0x908,u32)==0);}
 reset6();FIELD(actor,0x908,u32)=103;predicate_result=0;direct_member(29,-4);func_0018C0C8(actor);CHECK(actor->field0C==29&&last_enter==ADDRESS(actor,-4));
}
static void test_state33(void)
{
 s16 phases[]={-32768,-1,0,99,100,200,210,300,400,500,501,32767};int i,k;
 for(i=0;i<12;++i){reset6();FIELD(actor,0x92C,s16)=phases[i];FIELD(actor,0x930,float)=1;FIELD(data_storage,0x54,float)=100;func_0018C8A0(actor);CHECK(FIELD(actor,0x930,float)==.75f);if(phases[i]==100)CHECK(count(REQUEST)==2&&FIELD(actor,0x92C,u16)==200);else if(phases[i]==200)CHECK(count(VOID38)==1&&FIELD(actor,0x92C,u16)==200);else if(phases[i]==210)CHECK(count(REQUEST)==2&&FIELD(actor,0x92C,u16)==300);else if(phases[i]==300)CHECK(FIELD(actor,0x934,float)==.25f&&count(REQUEST)==0);else if(phases[i]==400)CHECK(actor->field0C==4&&count(EXIT_STATE)==1&&(FIELD(actor,0x190,GeorgeActorBits64)&0x40000));else if(phases[i]==500)CHECK(count(VB0)==1&&count(EXIT_STATE)==0);else CHECK(event_count==0);}
 reset6();FIELD(actor,0x92C,u16)=100;request_result=0;func_0018C8A0(actor);CHECK(FIELD(actor,0x92C,u16)==200);
 reset6();FIELD(actor,0x92C,u16)=100;mutation_mode=17;hook=mutate6;func_0018C8A0(actor);CHECK(nth(REQUEST,1)->object==replacement&&nth(REQUEST,1)->other==55&&FIELD(actor,0x92C,u16)==200);
 reset6();FIELD(actor,0x92C,u16)=200;FIELD(actor,0x930,float)=1;mutation_mode=18;hook=mutate6;func_0018C8A0(actor);CHECK(FIELD(actor,0x92C,u16)==210);
 for(k=0;k<3;++k){reset6();FIELD(actor,0x92C,u16)=210;FIELD(actor,0x92E,u16)=1;FIELD(actor,0x42C,float)=k==0?1:k==1?-1:from_bits(0x7FC00001);func_0018C8A0(actor);CHECK(FIELD(actor,0x92C,u16)==400&&count(ANGLE)==1);CHECK(nth(ANGLE,0)->value==(k==0?3:-3));}
 reset6();FIELD(actor,0x92C,u16)=300;FIELD(data_storage,0x54,float)=.25f;func_0018C8A0(actor);CHECK(FIELD(actor,0x92C,u16)==500&&count(REQUEST)==2&&(FIELD(actor,0x190,GeorgeActorBits64)&0x40000));
 reset6();FIELD(actor,0x92C,u16)=500;FIELD(actor,0x930,float)=0;direct_member(3,7);func_0018C8A0(actor);CHECK(actor->field0C==3&&last_enter==ADDRESS(actor,7)&&!(FIELD(actor,0x190,GeorgeActorBits64)&0x40000));
 reset6();FIELD(actor,0x42C,float)=1;func_0018C708(actor);CHECK(FIELD(actor,0x92C,u16)==100&&FIELD(actor,0x92E,u16)==0&&count(HASH)==1&&nth(ANGLE,0)->value==-3);
}
static void test_state31(void)
{
 s16 phases[]={-32768,-1,0,1,99,100,200,300,500,600,700,32767};int i,k;
 for(i=0;i<12;++i){reset6();FIELD(actor,0x9F0,s16)=phases[i];FIELD(actor,0x9F8,float)=1;func_0018CE18(actor);CHECK(FIELD(actor,0x9F8,float)==.75f);if(phases[i]==1)CHECK(event_count==0&&FIELD(actor,0x9F0,u16)==1);else if(phases[i]==100||phases[i]==200||phases[i]==300)CHECK(count(REQUEST)==2&&FIELD(actor,0x9F0,u16)==500&&FIELD(primary,0x434,u32)==0);else if(phases[i]==500)CHECK(count(FRAME)==1&&count(NORMALIZE)==3&&sample_count==3&&FIELD(primary,0x434,u32)==1&&FIELD(primary,0x428,float)==.25f);else if(phases[i]==700)CHECK(actor->field0C==4&&count(ANGULAR_SAMPLE)==2&&count(OBJECT_RESET)==1);else CHECK(count(REQUEST)==2&&nth(REQUEST,1)->word==0xA6&&nth(REQUEST,1)->argument==(void *)func_00196700);}
 for(k=0;k<3;++k){reset6();FIELD(actor,0x9F0,u16)=1;FIELD(actor,0x9F8,float)=k==0?.25f:k==1?0:from_bits(0x7FC00001);func_0018CE18(actor);CHECK(FIELD(actor,0x9F0,u16)==(k<2?300:1));}
 reset6();FIELD(actor,0x9F0,u16)=500;func_0018CE18(actor);near_value(last_frame.position.x,10);near_value(last_frame.position.y,20.3f);near_value(last_frame.position.z,29.485f);CHECK(last_frame.axis[0].x==0&&last_frame.axis[0].y==0&&last_frame.axis[0].z==0&&last_frame.axis[2].y==1&&last_frame.position.w==0);
 for(k=0;k<5;++k){reset6();FIELD(actor,0x9F0,u16)=500;samples[0]=k==0?-1:k==1?from_bits(0x7FC00001):.25f;samples[1]=k==2?1:k==3?from_bits(0x7FC00001):.5f;samples[2]=2;func_0018CE18(actor);CHECK(sample_count==(k<2?1:k<4?2:3));near_value(FIELD(primary,0x428,float),k<2?0:k<4?.99f:2);}
 reset6();FIELD(actor,0x9F0,u16)=500;mutation_mode=19;hook=mutate6;FIELD(replacement,0x3D8,void *)=&frame_result;func_0018CE18(actor);CHECK(nth(ANGULAR_SAMPLE,0)->object==object_storage&&nth(ANGULAR_SAMPLE,1)->object==replacement&&FIELD(replacement,0x428,float)==.25f&&FIELD(primary,0x428,float)==0);
 reset6();FIELD(actor,0x9F0,u16)=500;frame_result.axis[1].x=0;frame_result.axis[1].y=0;frame_result.axis[1].z=1;mutation_mode=20;hook=mutate6;func_0018CE18(actor);CHECK(last_frame.axis[2].x==0&&last_frame.axis[2].z==1);CHECK(last_frame.axis[1].y>0&&last_frame.axis[0].x==1);
 reset6();FIELD(actor,0x9F0,u16)=700;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),9);near_value(FIELD(actor,0x534,float),10);CHECK(FIELD(actor,0x538,u32)==13&&FIELD(actor,0x53C,u32)==14&&FIELD(actor,0x558,u32)==1&&(FIELD(actor,0x190,GeorgeActorBits64)&0x40000));
 reset6();FIELD(actor,0x9F0,u16)=700;direction_result.x=0;direction_result.y=-4;direction_result.z=0;func_0018CE18(actor);CHECK(count(QUERY)==0&&count(ANGLE)==0);near_value(FIELD(actor,0x530,float),8);near_value(FIELD(actor,0x534,float),6);
 reset6();FIELD(actor,0x9F0,u16)=700;mutation_mode=21;hook=mutate6;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),sqrtf(29)+4);near_value(FIELD(actor,0x534,float),8);
 reset6();FIELD(actor,0x9F0,u16)=700;mutation_mode=22;hook=mutate6;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),sqrtf(18)+4);near_value(FIELD(actor,0x534,float),6);
 reset6();FIELD(actor,0x9F0,u16)=700;direct_member(4,30000);((GeorgeGoalMember *)(D_003F83F0+4*28))->selector=1;((GeorgeGoalMember *)(D_003F83F0+4*28))->target.vtable_offset=0x3A0;FIELD(actor,0x3A0,void *)=reference_table;((GeorgeGoalVirtualVoid *)reference_table)->adjustment=10000;((GeorgeGoalVirtualVoid *)reference_table)->invoke=mock_enter;func_0018CE18(actor);CHECK(last_enter==ADDRESS(actor,40000));
}
static void test_additional_captures(void)
{
 int i,j;float timers[]={-1,0,.25f,.25000003f,1,0};timers[5]=from_bits(0x7FC00001);
 for(i=0;i<6;++i)for(j=0;j<2;++j){reset6();FIELD(actor,0x92C,u16)=j?500:200;FIELD(actor,0x930,float)=timers[i];func_0018C8A0(actor);if(j)CHECK(count(EXIT_STATE)==(timers[i]-.25f<0));else CHECK(FIELD(actor,0x92C,u16)==(timers[i]-.25f<0?210:200));}
 reset6();FIELD(actor,0x9F0,u16)=700;actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x2E4);FIELD(actor,0x534,float)=8;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),9);near_value(FIELD(actor,0x534,float),13);
 reset6();FIELD(actor,0x9F0,u16)=700;direction_result.y=from_bits(0x7FC00001);func_0018CE18(actor);CHECK(isnan(FIELD(actor,0x530,float)));near_value(FIELD(actor,0x534,float),6);
 reset6();FIELD(actor,0x9F0,u16)=700;direct_member(4,0);mutation_mode=27;hook=mutate6;func_0018CE18(actor);CHECK(FIELD(actor,0x9F0,u16)==1234&&count(ENTER_STATE)==1);
 reset6();FIELD(actor,0x9F0,u16)=0;FIELD(actor,0x1B0,void *)=ADDRESS(actor,-0x284);func_0018CE18(actor);CHECK(FIELD(actor,0x1B0,void *)==0&&count(REQUEST)==0&&FIELD(actor,0x4E4,u32)==0xA6);
 reset6();FIELD(actor,0x9F0,u16)=100;FIELD(actor,0x1B0,void *)=ADDRESS(actor,-0x284);func_0018CE18(actor);CHECK(count(REQUEST)==0&&FIELD(actor,0x9F0,u16)==500);
 reset6();FIELD(actor,0x92C,u16)=400;actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x4EC);FIELD(actor,0x42C,float)=2;FIELD(actor,0x430,float)=3;FIELD(actor,0x434,float)=4;FIELD(actor,0x534,float)=7;func_0018C8A0(actor);near_value(FIELD(actor,0x534,float),FIELD(actor,0x530,float));
 reset6();mutation_mode=24;hook=mutate6;func_00196468(actor);CHECK(nth(MATRIX_COPY,1)->object==ADDRESS(replacement,0x10)&&(FIELD(replacement,0xA0,u32)&0x100)&&!(FIELD(object_storage,0xA0,u32)&0x100));
 reset6();mutation_mode=25;hook=mutate6;func_001966C0(actor);CHECK(nth(CANCEL,0)->object==replacement);
 reset6();FIELD(actor,0x9F0,u16)=700;samples[0]=.25f;samples[1]=.75f;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),1);
 reset6();FIELD(actor,0x9F0,u16)=700;samples[0]=.75f;samples[1]=.25f;func_0018CE18(actor);near_value(FIELD(actor,0x530,float),1);
 reset6();FIELD(actor,0x908,u32)=100;D_003F2D40=0;func_0018C0C8(actor);CHECK(count(CONSTRUCT_EFFECT)==1&&count(UPPER_BOUND)==1&&count(AT_EXIT)==1&&count(ALLOCATE)==3);
 reset6();D_003F2D40=0;func_0018C708(actor);CHECK(count(CONSTRUCT_EFFECT)==1&&count(UPPER_BOUND)==1&&count(AT_EXIT)==1&&count(ALLOCATE)==2);
}
int main(void)
{CHECK(sizeof(void *)==4);test_durations();test_lifecycle();test_records();test_state32();test_state30();test_state33();test_state31();test_additional_captures();printf("actor_states6: %d checks passed\n",checks);return 0;}
