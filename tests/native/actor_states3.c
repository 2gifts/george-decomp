/* Asset-free tests: host mocks observe the recovered engine call boundaries.
 * Expected phase transitions and callback mutations are specified separately;
 * no original executable or instruction bytes are needed to run this file. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_states3.c"

enum { REQUEST=1, CONTROL_A0, CONTROL_88, CONTROL_38, CONTROL_VECTOR,
       NORMALIZE, HASH, START_OBJECT, STOP_OBJECT, SCRIPT_VALUE, SCRIPT_CALL,
       LOOKUP, ALLOCATE, CONSTRUCT_EFFECT, UPPER_BOUND, INSERT_RANGE, AT_EXIT,
       EMIT, HANDLE_START, HANDLE_STOP, HANDLE_FREE, STATUS, DURATION,
       NOTIFY, RELEASE, SET_FLAG, COUNT, RECORDS, MULTIPLY, COPY_MATRIX,
       ATTACH, CANCEL_ATTACH, RELEASE_COMPANION, FIND_REFERENCE, CONSTRUCT_COMPANION,
       STOP_REQUEST_CALL, ANGLE, GENERIC_EFFECT };
typedef struct Event { int kind; void *object, *argument; u32 word, other; float value; } Event;
static Event events[256];
static int event_count, checks;
static unsigned char entity_storage[0xA00] __attribute__((aligned(16)));
static unsigned char data_storage[0x300], primary_storage[0x800], companion_storage[0x800];
static unsigned char replacement_storage[0x800], object_storage[0x200], handle_storage[0x100];
static unsigned char attachment_storage[0x200], replacement_attachment[0x200];
static unsigned char reference_storage[0x100], map_storage[0x100], secondary_storage[0x100];
static unsigned char control_table[0x100] __attribute__((aligned(16)));
static struct { GeorgeActorControlObject object; u32 padding[4]; } control;
static GeorgeGoalEntity *actor=(GeorgeGoalEntity *)entity_storage;
static void (*hook)(Event *);
static s32 request_result;
static u32 status_result, count_result;
static void *lookup_result;
static GeorgeDeimosValue values[4], alternate_values[4];
static GeorgeDeimosPoolNode nodes[3];
static GeorgeMathVec3 last_vector, emitted_position;
static float normalized_length, duration_result, angle_results[4];
static int normalize_override, angle_count;
static unsigned char records_storage[4*0x20];
static GeorgeRotationMatrix matrices_storage[4], multiply_result;
static GeorgeActorEffectRecord allocated_record;
static void *registry_values[8];
static void **upper_result;
static int allocate_count;
GeorgeDeimosValue *D_00474F48=values;
void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[]={0},D_0042D678[]={1},D_0042D690[]={2},D_0042C3E8[]={3};

static void check(int condition,const char *label,int line)
{ ++checks; if(!condition){fprintf(stderr,"actor_states3:%d: %s\n",line,label);exit(1);} }
#define CHECK(c) check((c),#c,__LINE__)
static u32 float_bits(float value) { union {float f;u32 u;} bits;bits.f=value;return bits.u; }
static float from_bits(u32 word) { union {float f;u32 u;} bits;bits.u=word;return bits.f; }
static void near_value(float actual,float expected) { CHECK(fabsf(actual-expected)<0.00001f); }
static Event *event(int kind,void *object,u32 word,float value)
{
    Event *e; CHECK(event_count<256);e=&events[event_count++];
    e->kind=kind;e->object=object;e->word=word;e->value=value;
    if(hook)hook(e);
    return e;
}
static int count_events(int kind) {int i,n=0;for(i=0;i<event_count;++i)if(events[i].kind==kind)++n;return n;}
static Event *nth_event(int kind,int index) {int i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&index--==0)return &events[i];CHECK(0);return 0;}
static void mock_control(void *self)
{
    u32 off=(u32)self-(u32)&control.object;
    event(off==4?CONTROL_A0:off==8?CONTROL_88:CONTROL_38,self,off,0);
}
static void mock_vector(void *self,const GeorgeMathVec3 *vector)
{last_vector=*vector;event(CONTROL_VECTOR,self,0,0);}
static void reset(void)
{
    GeorgeGoalVirtualVoid *pair;
    memset(entity_storage,0,sizeof(entity_storage));memset(data_storage,0,sizeof(data_storage));
    memset(primary_storage,0,sizeof(primary_storage));memset(companion_storage,0,sizeof(companion_storage));
    memset(replacement_storage,0,sizeof(replacement_storage));memset(object_storage,0,sizeof(object_storage));
    memset(handle_storage,0,sizeof(handle_storage));memset(attachment_storage,0,sizeof(attachment_storage));
    memset(replacement_attachment,0,sizeof(replacement_attachment));memset(control_table,0,sizeof(control_table));
    memset(events,0,sizeof(events));memset(values,0,sizeof(values));memset(alternate_values,0,sizeof(alternate_values));
    memset(records_storage,0,sizeof(records_storage));memset(matrices_storage,0,sizeof(matrices_storage));
    memset(&multiply_result,0,sizeof(multiply_result));memset(map_storage,0,sizeof(map_storage));
    event_count=0;hook=0;request_result=1;status_result=0;count_result=0;lookup_result=0;
    normalize_override=0;normalized_length=0;duration_result=4800.0f;angle_count=0;
    angle_results[0]=angle_results[1]=angle_results[2]=0;
    D_00474F48=values;D_003F2D40=object_storage;
    D_0046A0F0.field00=registry_values;D_0046A0F0.field04=registry_values;
    D_0046A0F0.field08=registry_values+8;upper_result=registry_values;allocate_count=0;
    actor->field18=(GeorgeGoalEntityData *)data_storage;
    FIELD(actor,0x20,GeorgeActorControlObject *)=&control.object;control.object.field00=control_table;
    FIELD(actor,0x1B0,void *)=primary_storage;FIELD(actor,0x1B4,void *)=companion_storage;
    FIELD(actor,0x2D4,void *)=object_storage;FIELD(actor,0x7BC,void *)=handle_storage;
    FIELD(actor,0x35C,float)=0.25f;FIELD(actor,0x14,u32)=99;
    FIELD(actor,0x368,u32)=0x88776655;
    pair=(GeorgeGoalVirtualVoid *)(control_table+0xA0);pair->adjustment=4;pair->invoke=mock_control;
    pair=(GeorgeGoalVirtualVoid *)(control_table+0x88);pair->adjustment=8;pair->invoke=mock_control;
    pair=(GeorgeGoalVirtualVoid *)(control_table+0x38);pair->adjustment=12;pair->invoke=mock_control;
    ((GeorgeActorVirtualVectorInput *)(control_table+0x78))->adjustment=-4;
    ((GeorgeActorVirtualVectorInput *)(control_table+0x78))->invoke=mock_vector;
}

s32 func_00270510(void *object,u32 word,u32 mode0,u32 mode1,u32 owner_word,
                 GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,
                 u32 invoke_word,u32 callback_word,float time)
{
    Event *e;CHECK(mode0==0&&mode1==0&&invoke_word==0&&callback_word==0);
    CHECK(owner_word==FIELD(actor,0x368,u32));CHECK(context==(callback?actor:0));
    e=event(REQUEST,object,word,time);e->argument=(void *)callback;return request_result;
}
float func_002A3538(GeorgeMathVec3 *vector)
{
    float length=sqrtf(vector->x*vector->x+vector->y*vector->y+vector->z*vector->z);
    last_vector=*vector;event(NORMALIZE,vector,0,length);
    if(length!=0){vector->x/=length;vector->y/=length;vector->z/=length;}
    return normalize_override?normalized_length:length;
}
float func_0029B940(float first,float second)
{Event *e=event(ANGLE,0,0,first);e->other=float_bits(second);return angle_results[angle_count++];}
u32 *func_002BEBA0(u32 *output,const u8 *text)
{*output=0x12345678;event(HASH,(void *)text,*output,0);return output;}
void func_001F1758(void *object,void *reference,u32 key,float time)
{CHECK(reference==ADDRESS(actor,0x760));CHECK(key==0x12345678);event(START_OBJECT,object,key,time);}
void func_001F2020(void *object,u32 word,float time)
{CHECK(word==0);event(STOP_OBJECT,object,word,time);}
GeorgeDeimosPoolNode *func_002D0790(GeorgeScriptObject *object)
{event(SCRIPT_VALUE,object,0,0);return object==(GeorgeScriptObject *)actor?&nodes[0]:&nodes[1];}
void func_002D02A8(GeorgeDeimosPoolNode *callable,s32 count,s32 offset)
{CHECK(offset==0);event(SCRIPT_CALL,callable,(u32)count,0);}
void *func_00238BA0(void *object,u32 key)
{event(LOOKUP,object,key,0);return lookup_result;}
void *func_002AEE60(u32 size)
{event(ALLOCATE,0,size,0);++allocate_count;return size==12?(void *)&allocated_record:(void *)replacement_storage;}
void *func_002481F0(void *object) {event(CONSTRUCT_EFFECT,object,0,0);return object_storage;}
s32 func_00100AA8(const void *first,const void *second) {(void)first;(void)second;return 0;}
void **func_00100C30(void **begin,void **end,void *const *key,GeorgeStartupCompare compare)
{CHECK(begin==D_0046A0F0.field00&&end==D_0046A0F0.field04);CHECK(*(void *const *)key==&allocated_record);CHECK(compare==func_00100AA8);event(UPPER_BOUND,0,0,0);return upper_result;}
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value)
{CHECK(range==&D_0046A0F0&&position==upper_result&&*value==&allocated_record);event(INSERT_RANGE,0,0,0);}
void func_002BD340(void) {}
s32 func_00396260(void (*callback)(void)) {CHECK(callback==func_002BD340);event(AT_EXIT,0,0,0);return 0;}
void *func_00251B58(void *object,u32 key,const GeorgeMathVec3 *position)
{CHECK(key==0x12345678);emitted_position=*position;event(EMIT,object,key,0);return handle_storage;}
void func_002455C0(void *object) {event(HANDLE_START,object,0,0);}
void func_002457D8(void *object) {event(HANDLE_STOP,object,0,0);}
void func_00245B08(void *object) {event(HANDLE_FREE,object,0,0);}
void func_001B67C0(void *context,GeorgeGoalEntity *entity,u32 word0,u32 word1,u32 word2,u32 word3)
{CHECK(context==ADDRESS(actor,0x40)&&entity==FIELD(actor,0x3B8,GeorgeGoalEntity *));CHECK(word0==FIELD(actor,0x3B4,u32)&&word2==0&&word3==0);event(GENERIC_EFFECT,context,word1,0);}
u32 func_00272C10(void *object) {event(STATUS,object,0,0);return status_result;}
float func_002A6E60(void *source) {event(DURATION,source,0,0);return duration_result;}
void func_002727D8(void *object) {event(STOP_REQUEST_CALL,object,0,0);}
void func_00235CD8(void *object,u32 key,u32 word)
{Event *e=event(NOTIFY,object,key,0);e->other=word;}
void func_002393F8(u32 word) {event(RELEASE,(void *)word,0,0);}
void func_0018FD30(GeorgeGoalEntity *entity,GeorgeActorBits64 mask)
{CHECK(entity==actor&&mask==0x4000);event(SET_FLAG,entity,(u32)mask,0);}
u32 func_002A6460(const void *array) {event(COUNT,(void *)array,0,0);return count_result;}
void *func_002A6468(const void *array,s32 index) {CHECK(index==0);event(RECORDS,(void *)array,0,0);return records_storage;}
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *first,const GeorgeRotationMatrix *second)
{Event *e;CHECK(((u32)output&15)==0);*output=multiply_result;e=event(MULTIPLY,(void *)first,0,0);e->argument=(void *)second;}
void func_002A1C08(void *output,const void *input)
{memcpy(output,input,64);event(COPY_MATRIX,output,0,0);}
void *func_00239E40(u32 word,void *output,u32 mode,GeorgeActorAttachmentCallback callback,
                  GeorgeGoalEntity *context,u32 word0,u32 word1)
{CHECK(((u32)output&15)==0&&mode==0&&word0==0&&word1==0&&context==actor&&callback==func_00195168);multiply_result=*(GeorgeRotationMatrix *)output;event(ATTACH,context,word,0);return 0;}
void func_00237608(GeorgeGoalEntity *entity,GeorgeActorAttachmentCallback callback)
{CHECK(entity==actor&&callback==func_00195168);event(CANCEL_ATTACH,entity,0,0);}
void func_0026F390(void *object,u32 mode) {CHECK(mode==3);event(RELEASE_COMPANION,object,mode,0);}
void *func_0023C230(void *reference,u32 key)
{event(FIND_REFERENCE,reference,key,0);return key==0xFFFFFFFFu?(void *)map_storage:(void *)secondary_storage;}
void *func_0026EC98(void *storage,void *reference,u32 word,const u8 *text,u32 word0,u32 word1,s32 word2)
{CHECK(storage==replacement_storage&&reference==secondary_storage&&text==D_0042C3E8&&word0==0&&word1==0&&word2==-1);event(CONSTRUCT_COMPANION,reference,word,0);return companion_storage;}

static u32 watched_phase, watched_timer, seen_phase;
static void duration_mutation(Event *e)
{if(e->kind==DURATION){seen_phase=FIELD(actor,watched_phase,u32);FIELD(actor,watched_phase,u32)=100;}}
static void test_duration(void)
{
    GeorgeActorRequestCallback callbacks[]={func_00194F08,func_00195068,func_001950E8,func_001957D0,func_001952D8};
    u32 phases[]={0x7AC,0x7C0,0x7D8,0x7E0,0x7E8},timers[]={0x7B4,0x7C8,0x7DC,0x7E4,0x7EC};
    int i,j;u32 initial[]={0,1,0x7FFFFFFFu,0xFFFFFFFFu};
    for(i=0;i<5;++i)for(j=0;j<4;++j){reset();FIELD(actor,phases[i],u32)=initial[j];callbacks[i](123,actor,reference_storage);CHECK(FIELD(actor,phases[i],u32)==initial[j]+1u);near_value(FIELD(actor,timers[i],float),1);CHECK(nth_event(DURATION,0)->object==reference_storage);}
    for(i=0;i<5;++i){reset();watched_phase=phases[i];watched_timer=timers[i];hook=duration_mutation;FIELD(actor,watched_phase,u32)=0xFFFFFFFFu;callbacks[i](0,actor,reference_storage);CHECK(seen_phase==(i==0?0xFFFFFFFFu:0u));CHECK(FIELD(actor,watched_phase,u32)==(i==0?101u:100u));CHECK(float_bits(FIELD(actor,watched_timer,float))==0x3F800000u);}
}
static void request_mutation(Event *e)
{if(e->kind==REQUEST&&count_events(REQUEST)==1){FIELD(actor,0x1B0,void *)=replacement_storage;FIELD(actor,0x368,u32)=5;}}
static void timer_mutation(Event *e)
{if(e->kind==CONTROL_A0){FIELD(actor,watched_phase,u32)=2;FIELD(actor,watched_timer,float)=0.25f;}}
static void test_timed_requests(void)
{
    void (*functions[])(GeorgeGoalEntity *)={func_001867D8,func_00186AC0};
    u32 phases[]={0x7D8,0x7E8},timers[]={0x7DC,0x7EC},words[]={0x39,0x3A};
    s32 initial[]={-2147483647-1,-1,0,1,2,3,2147483647};
    float times[]={-1,0,0.25f,1,0};int i,j,k;times[4]=from_bits(0x7FC01234);
    for(i=0;i<2;++i)for(j=0;j<7;++j)for(k=0;k<5;++k){reset();FIELD(actor,phases[i],s32)=initial[j];FIELD(actor,timers[i],float)=times[k];functions[i](actor);CHECK(count_events(CONTROL_A0)==1);CHECK(count_events(REQUEST)==((initial[j]!=1&&initial[j]!=2)?2:0));if(initial[j]==2)CHECK(FIELD(actor,0x14,u32)==((times[k]-0.25f<=0)?0u:99u));else if(initial[j]==1)CHECK(FIELD(actor,phases[i],u32)==1);else{CHECK(FIELD(actor,phases[i],u32)==1);CHECK(FIELD(actor,0x4E4,u32)==words[i]);CHECK(nth_event(REQUEST,0)->argument==0);CHECK(nth_event(REQUEST,1)->argument==(void *)(i?func_001952D8:func_001950E8));}}
    for(i=0;i<2;++i){reset();hook=request_mutation;functions[i](actor);CHECK(nth_event(REQUEST,1)->object==replacement_storage);reset();request_result=0;functions[i](actor);CHECK(FIELD(actor,phases[i],u32)==2);reset();FIELD(actor,0x1B0,void *)=0;functions[i](actor);CHECK(count_events(REQUEST)==0&&FIELD(actor,phases[i],u32)==2);reset();watched_phase=phases[i];watched_timer=timers[i];hook=timer_mutation;functions[i](actor);CHECK(count_events(REQUEST)==0&&FIELD(actor,0x14,u32)==0);}
}
static void notify_timer_mutation(Event *e)
{if(e->kind==NOTIFY&&count_events(NOTIFY)==2)FIELD(actor,0x7C8,float)=-1;}
static void test_state16(void)
{
    s32 phases[]={-1,0,1,2,100,101,102,200,201,202,203};int i;
    for(i=0;i<11;++i){reset();FIELD(actor,0x7C0,s32)=phases[i];FIELD(actor,0x7C8,float)=1;func_00186328(actor);CHECK(count_events(CONTROL_A0)==1);if(phases[i]==1||phases[i]==101||phases[i]==201){CHECK(count_events(REQUEST)==0&&FIELD(actor,0x7C0,s32)==phases[i]);}else if(phases[i]==102){CHECK(FIELD(actor,0x7C0,u32)==102);}else if(phases[i]==2||phases[i]==202){CHECK(count_events(REQUEST)==0);}else{u32 next=phases[i]==100?101:phases[i]==200?201:1;u32 word=phases[i]==100?0x37:phases[i]==200?0x38:0x36;CHECK(FIELD(actor,0x7C0,u32)==next);CHECK(nth_event(REQUEST,1)->word==word);CHECK(nth_event(REQUEST,1)->argument==(void *)func_00195068);}}
    {u32 gates[]={0,1,0x80000000u,0xFFFFFFFFu};for(i=0;i<4;++i){reset();FIELD(actor,0x7C0,u32)=102;FIELD(actor,0x7C4,u32)=gates[i];func_00186328(actor);CHECK(FIELD(actor,0x7C0,u32)==(gates[i]?200u:102u));CHECK(count_events(REQUEST)==0&&count_events(NOTIFY)==0);}}
    reset();FIELD(actor,0x7C0,u32)=2;FIELD(actor,0x7C8,float)=1;FIELD(actor,0x214,void *)=reference_storage;FIELD(actor,0x218,void *)=map_storage;FIELD(actor,0x21C,void *)=secondary_storage;FIELD(actor,0x220,void *)=attachment_storage;hook=notify_timer_mutation;func_00186328(actor);CHECK(count_events(NOTIFY)==4&&FIELD(actor,0x7C0,u32)==100);CHECK(nth_event(NOTIFY,2)->object==secondary_storage);CHECK(nth_event(NOTIFY,0)->word==0xB95616B6u);
    reset();FIELD(actor,0x7C0,u32)=202;FIELD(actor,0x7C8,float)=0;FIELD(actor,0x214,void *)=reference_storage;FIELD(actor,0x218,void *)=map_storage;FIELD(actor,0x21C,void *)=secondary_storage;FIELD(actor,0x220,void *)=attachment_storage;func_00186328(actor);CHECK(count_events(NOTIFY)==4&&FIELD(actor,0x14,u32)==0);CHECK(nth_event(NOTIFY,1)->object==secondary_storage&&nth_event(NOTIFY,2)->object==attachment_storage&&nth_event(NOTIFY,3)->object==map_storage);
    reset();FIELD(actor,0x7C0,u32)=100;request_result=0;func_00186328(actor);CHECK(FIELD(actor,0x7C0,u32)==102);
    reset();FIELD(actor,0x7C0,u32)=2;FIELD(actor,0x7C8,float)=from_bits(0x7FC00000);func_00186328(actor);CHECK(FIELD(actor,0x7C0,u32)==2);
}

static void initializer_mutation(Event *e)
{if(e->kind==ANGLE){FIELD(actor,0x40,float)=1000;FIELD(actor,0x48,float)=2000;}}
static void test_state15_initializer(void)
{
    int mode;
    for(mode=0;mode<4;++mode){
        reset();FIELD(actor,0x790,float)=5;FIELD(actor,0x794,float)=10;FIELD(actor,0x798,float)=8;
        FIELD(actor,0x40,float)=2;FIELD(actor,0x44,float)=4;FIELD(actor,0x48,float)=1;
        if(mode==0)angle_results[0]=1,angle_results[1]=2;
        if(mode==1)angle_results[0]=-1,angle_results[1]=-10,angle_results[2]=-9;
        if(mode==2)angle_results[0]=-1,angle_results[1]=-1,angle_results[2]=-2;
        if(mode==3)angle_results[0]=from_bits(0x7FC00001),angle_results[1]=from_bits(0x7FC00002),angle_results[2]=from_bits(0x7FC00003);
        hook=initializer_mutation;func_00185178(actor);
        CHECK(FIELD(actor,0x7AC,u32)==0&&FIELD(actor,0x7B4,u32)==0&&FIELD(actor,0x7BC,void *)==0);
        CHECK(FIELD(actor,0x7A0,float)==5&&FIELD(actor,0x7A4,float)==8&&FIELD(actor,0x7A8,float)==8);
        CHECK(angle_count==(mode==0?2:3));CHECK(nth_event(ANGLE,0)->value==3&&nth_event(ANGLE,0)->other==float_bits(-7));
        CHECK(nth_event(ANGLE,angle_count-1)->value==3&&nth_event(ANGLE,angle_count-1)->other==float_bits(-7));
        if(mode==0)near_value(FIELD(actor,0x58,float),(2+3.14159274101257324f)-6.28318548202514648f);
        if(mode==1)near_value(FIELD(actor,0x58,float),(-9+3.14159274101257324f)+6.28318548202514648f);
        if(mode==2)near_value(FIELD(actor,0x58,float),-2+3.14159274101257324f);
        if(mode==3)CHECK(isnan(FIELD(actor,0x58,float)));
    }
}
static void start_mutation(Event *e)
{if(e->kind==HASH)FIELD(actor,0x2D4,void *)=replacement_attachment;if(e->kind==START_OBJECT)FIELD(actor,0x7AC,u32)=0xFFFFFFFFu;}
static void lookup_mutation(Event *e)
{if(e->kind==LOOKUP)FIELD(actor,0x7B4,float)=3;}
static void script_mutation(Event *e)
{if(e->kind==SCRIPT_VALUE&&count_events(SCRIPT_VALUE)==1){D_00474F48=alternate_values;FIELD(actor,0x7B8,void *)=replacement_storage;}}
static void release_handle_mutation(Event *e)
{if(e->kind==HANDLE_STOP)FIELD(actor,0x7BC,void *)=replacement_attachment;}
static void effect_mutation(Event *e)
{if(e->kind==HASH){FIELD(actor,0x40,float)=7;FIELD(actor,0x44,float)=8;FIELD(actor,0x48,float)=9;}if(e->kind==HANDLE_START)FIELD(actor->field18,0xD0,u32)=33;}
static void movement_mutation(Event *e)
{if(e->kind==CONTROL_VECTOR)FIELD(actor,0x7B4,float)=100;}
static void test_state15(void)
{
    s32 inactive[]={-2147483647-1,-1,2,99,101,199,204,2147483647};int i;
    for(i=0;i<8;++i){reset();FIELD(actor,0x7AC,s32)=inactive[i];func_001852C0(actor);CHECK(event_count==0&&FIELD(actor,0x14,u32)==99);}
    reset();FIELD(actor,0x2D4,void *)=0;FIELD(actor,0x7A0,float)=3;FIELD(actor,0x7A4,float)=4;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==100);near_value(FIELD(actor,0x7B0,float),0.5f);CHECK(count_events(START_OBJECT)==0);
    reset();FIELD(actor,0x7A8,float)=10;FIELD(actor,0x794,float)=5;hook=start_mutation;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==0);CHECK(nth_event(START_OBJECT,0)->object==replacement_attachment);near_value(FIELD(actor,0x794,float),4.74f);CHECK(nth_event(START_OBJECT,0)->value==30);
    reset();FIELD(actor,0x7AC,u32)=1;FIELD(object_storage,0x104,float)=2;FIELD(object_storage,0x100,float)=1;func_001852C0(actor);CHECK(count_events(LOOKUP)==0&&FIELD(actor,0x7AC,u32)==1);
    reset();FIELD(actor,0x7AC,u32)=1;FIELD(object_storage,0x104,float)=from_bits(0x7FC00000);func_001852C0(actor);CHECK(count_events(LOOKUP)==0);
    reset();FIELD(actor,0x7AC,u32)=1;FIELD(actor,0x3AC,GeorgeDeimosPoolNode *)=&nodes[2];FIELD(actor,0x7B8,void *)=reference_storage;lookup_result=map_storage;FIELD(map_storage,0x40,u32)=1;hook=script_mutation;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==200);CHECK(alternate_values[0].tag==4&&alternate_values[0].subtype==0&&alternate_values[0].payload.pointer==&nodes[0]);CHECK(alternate_values[1].tag==4&&alternate_values[1].subtype==0&&alternate_values[1].payload.pointer==&nodes[1]);CHECK(nth_event(SCRIPT_VALUE,1)->object==replacement_storage);CHECK(nth_event(SCRIPT_CALL,0)->object==&nodes[2]&&nth_event(SCRIPT_CALL,0)->word==2);
    reset();FIELD(actor,0x7AC,u32)=1;hook=effect_mutation;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==100);CHECK(FIELD(actor,0x7BC,void *)==handle_storage);CHECK(emitted_position.x==7&&emitted_position.y==8&&emitted_position.z==9);CHECK(count_events(GENERIC_EFFECT)==1&&nth_event(GENERIC_EFFECT,0)->word==33);
    reset();FIELD(actor,0x7AC,u32)=1;D_003F2D40=0;func_001852C0(actor);CHECK(allocate_count==2&&count_events(AT_EXIT)==1&&count_events(INSERT_RANGE)==0);CHECK(D_0046A0F0.field04==registry_values+1&&registry_values[0]==&allocated_record);CHECK(allocated_record.field00==9&&allocated_record.field04==D_00421160&&allocated_record.field08==object_storage);
    reset();FIELD(actor,0x7AC,u32)=1;D_003F2D40=0;D_0046A0F0.field08=registry_values;func_001852C0(actor);CHECK(count_events(INSERT_RANGE)==1);
    reset();FIELD(actor,0x7AC,u32)=100;FIELD(actor,0x40,float)=1;FIELD(actor,0x44,float)=2;FIELD(actor,0x48,float)=3;FIELD(actor,0x7A0,float)=1;FIELD(actor,0x7A4,float)=2;FIELD(actor,0x7A8,float)=8;FIELD(actor,0x7B0,float)=10;func_001852C0(actor);CHECK(FIELD(actor,0x14,u32)==99);CHECK(FIELD(handle_storage,0x14,float)==1&&FIELD(handle_storage,0x18,float)==2&&FIELD(handle_storage,0x1C,float)==3);near_value(last_vector.z,10);CHECK(count_events(HANDLE_STOP)==0);
    /* Handle+0x14 aliases the target vector. All three source components are
     * captured before either output store; movement reloads the changed target. */
    reset();FIELD(actor,0x7AC,u32)=100;FIELD(actor,0x40,float)=1;FIELD(actor,0x44,float)=2;FIELD(actor,0x48,float)=3;FIELD(actor,0x7BC,void *)=ADDRESS(actor,0x7A0-0x14);FIELD(actor,0x7A0,float)=10;FIELD(actor,0x7A4,float)=20;FIELD(actor,0x7A8,float)=30;hook=release_handle_mutation;func_001852C0(actor);CHECK(last_vector.x==0&&last_vector.y==0&&last_vector.z==0);CHECK(FIELD(actor,0x7A0,float)==1&&FIELD(actor,0x7A4,float)==2&&FIELD(actor,0x7A8,float)==3);CHECK(nth_event(HANDLE_FREE,0)->object==replacement_attachment&&FIELD(actor,0x7BC,void *)==0&&FIELD(actor,0x14,u32)==3);
    reset();FIELD(actor,0x7AC,u32)=100;FIELD(actor,0x7A0,float)=10;FIELD(actor,0x7B0,float)=50;hook=movement_mutation;func_001852C0(actor);CHECK(FIELD(actor,0x14,u32)==3&&count_events(HANDLE_STOP)==1);
    reset();FIELD(actor,0x7AC,u32)=200;FIELD(actor,0x7B4,float)=0;lookup_result=map_storage;FIELD(map_storage,0x44,float)=2;hook=lookup_mutation;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==201&&FIELD(actor,0x4E4,u32)==0x4A);CHECK(nth_event(REQUEST,1)->argument==(void *)func_00194F08);
    reset();FIELD(actor,0x7AC,u32)=200;FIELD(actor,0x7B4,float)=0;FIELD(actor,0x1B0,void *)=0;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==201&&count_events(REQUEST)==0);
    reset();FIELD(actor,0x7AC,u32)=201;func_001852C0(actor);CHECK(count_events(CONTROL_A0)==1&&event_count==1);
    reset();FIELD(actor,0x7AC,u32)=202;FIELD(actor,0x7B4,float)=0.25f;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==203&&count_events(STATUS)==0&&count_events(STOP_OBJECT)==1);
    reset();FIELD(actor,0x7AC,u32)=202;FIELD(actor,0x7B4,float)=1;FIELD(actor,0x1B0,void *)=0;status_result=0x400000;func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==203&&nth_event(STATUS,0)->object==0);
    reset();FIELD(actor,0x7AC,u32)=202;FIELD(actor,0x7B4,float)=from_bits(0x7FC00000);func_001852C0(actor);CHECK(FIELD(actor,0x7AC,u32)==202&&count_events(STATUS)==1);
    reset();FIELD(actor,0x7AC,u32)=203;FIELD(object_storage,0x104,float)=1;func_001852C0(actor);CHECK(FIELD(actor,0x14,u32)==99);
    reset();FIELD(actor,0x7AC,u32)=203;FIELD(object_storage,0x104,float)=from_bits(0x7FC00000);FIELD(actor,0x3B0,GeorgeDeimosPoolNode *)=&nodes[2];func_001852C0(actor);CHECK(FIELD(actor,0x14,u32)==0&&count_events(SCRIPT_CALL)==1);
}

static void scan_mutation(Event *e)
{
    if(e->kind==COUNT)FIELD(actor,0x1B0,void *)=replacement_storage;
    if(e->kind==RECORDS){FIELD(actor,0x1B0,void *)=companion_storage;FIELD(companion_storage,0x3F0,void *)=matrices_storage;}
    if(e->kind==MULTIPLY)FIELD(actor,0x298,void *)=replacement_attachment;
}
static void copy_mutation(Event *e)
{if(e->kind==COPY_MATRIX)FIELD(actor,0x298,void *)=replacement_attachment;}
static void notify_phase_mutation(Event *e)
{if(e->kind==NOTIFY){FIELD(actor,0x7E0,u32)=0xFFFFFFFFu;FIELD(actor,0x7E4,float)=0;}}
static void request_phase_mutation(Event *e)
{if(e->kind==REQUEST&&count_events(REQUEST)==2)FIELD(actor,0x7E0,u32)=300;}
static void test_state18(void)
{
    s32 phases[]={-1,0,100,101,102,103,104};int i,j;
    for(i=0;i<7;++i){reset();FIELD(actor,0x7E0,s32)=phases[i];FIELD(actor,0x7E4,float)=1;FIELD(actor,0x3C4,float)=4;func_00186DF8(actor);CHECK(FIELD(actor,0x3C4,u32)==0);if(phases[i]==101){CHECK(FIELD(actor,0x7E0,u32)==101&&count_events(REQUEST)==0);}else if(phases[i]==103){CHECK(FIELD(actor,0x14,u32)==0&&count_events(REQUEST)==0);}else if(phases[i]==102){CHECK(FIELD(actor,0x7E0,u32)==102&&count_events(REQUEST)==0);}else{CHECK(FIELD(actor,0x7E0,u32)==(u32)phases[i]+1u);CHECK(nth_event(REQUEST,0)->value==0&&nth_event(REQUEST,1)->argument==(void *)func_001957D0);}}
    reset();request_result=0;func_00186DF8(actor);CHECK(FIELD(actor,0x7E0,u32)==102);
    reset();hook=request_phase_mutation;func_00186DF8(actor);CHECK(FIELD(actor,0x7E0,u32)==301);
    reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=0.25f;FIELD(actor,0x1B0,void *)=0;func_00186DF8(actor);CHECK(FIELD(actor,0x7E0,u32)==103&&count_events(STATUS)==0);
    /* Actual byte-count getter may return 0..255. Exercise both no-record and
     * positive-record paths, plus full signed command to wrapped32 offsets. */
    for(i=0;i<4;++i){s32 commands[]={0,3,-1,0x40000001};u32 off=(u32)(commands[i]==3?2:commands[i])<<2;
        reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=1;status_result=0x2000;FIELD(actor,0x298,void *)=attachment_storage;FIELD(primary_storage,0x0C,s32)=commands[i];FIELD(primary_storage,0x378u+off,void *)=reference_storage;FIELD(primary_storage,0x3E8u+off,void *)=matrices_storage;func_00186DF8(actor);CHECK(nth_event(COUNT,0)->object==reference_storage&&count_events(RECORDS)==0&&count_events(COPY_MATRIX)==1);CHECK(FIELD(attachment_storage,0xA0,u32)==0x100);CHECK(nth_event(NOTIFY,0)->object==attachment_storage&&nth_event(NOTIFY,0)->other==1);
    }
    for(j=0;j<4;++j){reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=1;status_result=0x2000;FIELD(actor,0x298,void *)=attachment_storage;FIELD(primary_storage,0x0C,s32)=3;FIELD(primary_storage,0x380,void *)=reference_storage;FIELD(primary_storage,0x3F0,void *)=matrices_storage;count_result=4;records_storage[j*0x20+0x1D]=0x29;multiply_result.element[12]=1;multiply_result.element[13]=2;multiply_result.element[14]=3;func_00186DF8(actor);CHECK(nth_event(MULTIPLY,0)->object==&matrices_storage[j]&&nth_event(MULTIPLY,0)->argument==ADDRESS(actor,0xB0));CHECK(FIELD(attachment_storage,0x40,float)==1&&FIELD(attachment_storage,0x44,float)==2&&FIELD(attachment_storage,0x48,float)==3&&FIELD(attachment_storage,0x4C,float)==1);CHECK(count_events(COPY_MATRIX)==0);}
    reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=1;status_result=0x2000;FIELD(actor,0x298,void *)=attachment_storage;FIELD(primary_storage,0x0C,s32)=3;FIELD(primary_storage,0x380,void *)=reference_storage;FIELD(replacement_storage,0x380,void *)=secondary_storage;count_result=2;records_storage[0x3D]=0x29;multiply_result.element[12]=11;hook=scan_mutation;func_00186DF8(actor);CHECK(nth_event(RECORDS,0)->object==secondary_storage);CHECK(nth_event(MULTIPLY,0)->object==&matrices_storage[1]);CHECK(FIELD(replacement_attachment,0x40,float)==11&&FIELD(attachment_storage,0x40,float)==0);CHECK(nth_event(NOTIFY,0)->object==replacement_attachment);
    reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=1;status_result=0x2000;FIELD(actor,0x298,void *)=attachment_storage;count_result=2;hook=copy_mutation;func_00186DF8(actor);CHECK(FIELD(attachment_storage,0xA0,u32)==0x100&&FIELD(replacement_attachment,0xA0,u32)==0);CHECK(nth_event(NOTIFY,0)->object==replacement_attachment);
    reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=1;status_result=0x2000;FIELD(actor,0x298,void *)=attachment_storage;hook=notify_phase_mutation;func_00186DF8(actor);CHECK(FIELD(actor,0x7E0,u32)==0);
    reset();FIELD(actor,0x7E0,u32)=102;FIELD(actor,0x7E4,float)=from_bits(0x7FC00000);func_00186DF8(actor);CHECK(FIELD(actor,0x7E0,u32)==102);
}

static void cleanup_mutation(Event *e)
{
    if(e->kind==HANDLE_STOP)FIELD(actor,0x7BC,void *)=replacement_attachment;
    if(e->kind==HANDLE_FREE)FIELD(actor,0x2D4,void *)=replacement_storage;
    if(e->kind==NOTIFY)FIELD(actor,0x4E8,void *)=replacement_attachment;
    if(e->kind==SET_FLAG)FIELD(actor,0x190,GeorgeActorBits64)=~(GeorgeActorBits64)0;
}
static void attachment_mutation(Event *e)
{
    if(e->kind==ATTACH)FIELD(actor,0x190,GeorgeActorBits64)=0x1234;
    if(e->kind==NOTIFY)FIELD(actor,0x190,GeorgeActorBits64)=~(GeorgeActorBits64)0;
}
static void companion_mutation(Event *e)
{if(e->kind==ALLOCATE){FIELD(actor,0x1C,void *)=replacement_attachment;FIELD(replacement_attachment,4,u32)=123;}}
static void test_lifecycle(void)
{
    void (*stops[])(GeorgeGoalEntity *)={func_001950C0,func_00195140,func_00195330,func_00195828};
    int i;float threshold[]={-1,0,1,0};threshold[3]=from_bits(0x7FC00000);
    for(i=0;i<4;++i){reset();stops[i](actor);CHECK(count_events(STOP_REQUEST_CALL)==1&&nth_event(STOP_REQUEST_CALL,0)->object==primary_storage);reset();FIELD(actor,0x1B0,void *)=0;stops[i](actor);CHECK(event_count==0);}
    for(i=0;i<4;++i){reset();FIELD(object_storage,0x104,float)=threshold[i];func_00194F50(actor);CHECK(count_events(HANDLE_STOP)==1&&count_events(HANDLE_FREE)==1&&FIELD(actor,0x7BC,void *)==0);CHECK(count_events(STOP_OBJECT)==(i==2));}
    reset();FIELD(replacement_storage,0x104,float)=1;hook=cleanup_mutation;func_00194F50(actor);CHECK(nth_event(HANDLE_FREE,0)->object==replacement_attachment&&nth_event(STOP_OBJECT,0)->object==replacement_storage);
    reset();FIELD(actor,0x7BC,void *)=0;FIELD(actor,0x2D4,void *)=0;func_00194F50(actor);CHECK(event_count==0);
    reset();FIELD(actor,0x190,GeorgeActorBits64)=~(GeorgeActorBits64)0;FIELD(actor,0x60,u32)=3;FIELD(actor,0x484,u32)=4;func_00194FD8(actor);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(~(GeorgeActorBits64)0&~((GeorgeActorBits64)1<<19)&~((GeorgeActorBits64)1<<41)&~((GeorgeActorBits64)1<<40)));CHECK(FIELD(actor,0x60,u32)==0&&FIELD(actor,0x484,u32)==0&&count_events(CONTROL_38)==1);
    reset();hook=cleanup_mutation;func_00194FD8(actor);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&((GeorgeActorBits64)1<<19))!=0);
    reset();hook=attachment_mutation;func_00195168(actor,attachment_storage);CHECK(FIELD(actor,0x4E8,void *)==attachment_storage);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(~(GeorgeActorBits64)0&~((GeorgeActorBits64)1<<39)));
    reset();for(i=0;i<16;++i)FIELD(actor,0xB0+i*4,float)=(float)i;hook=attachment_mutation;func_001951C0(actor,77,5);CHECK(FIELD(actor,0x4EC,float)==5&&multiply_result.element[12]==12&&multiply_result.element[13]==18&&multiply_result.element[14]==14);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(0x1234|((GeorgeActorBits64)1<<39)));CHECK(nth_event(ATTACH,0)->word==77);
    reset();FIELD(actor,0x4E8,void *)=attachment_storage;FIELD(actor,0x190,GeorgeActorBits64)=(GeorgeActorBits64)1<<39;hook=cleanup_mutation;func_00195260(actor);CHECK(nth_event(RELEASE,0)->object==replacement_attachment&&FIELD(actor,0x4E8,void *)==0&&count_events(CANCEL_ATTACH)==1);
    reset();func_00195260(actor);CHECK(event_count==0);
    reset();FIELD(map_storage,0,void *)=reference_storage;FIELD(map_storage,4,u32)=10;hook=companion_mutation;func_00195358(actor,secondary_storage);CHECK(nth_event(RELEASE_COMPANION,0)->object==companion_storage);CHECK(nth_event(FIND_REFERENCE,0)->object==secondary_storage&&nth_event(FIND_REFERENCE,1)->object==reference_storage);CHECK(nth_event(CONSTRUCT_COMPANION,0)->word==123&&FIELD(actor,0x1B4,void *)==companion_storage);
}

int main(void)
{
    CHECK(sizeof(void *)==4&&sizeof(GeorgeActorUnalignedPosition)==12);
    CHECK(float_bits(0.000208333338377997279f)==0x395A740Eu);
    CHECK(float_bits(-0.259999990463256836f)==0xBE851EB8u);
    test_duration();test_timed_requests();test_state16();test_state15_initializer();
    test_state15();test_state18();test_lifecycle();
    printf("actor_states3: %d checks passed\n",checks);return 0;
}
