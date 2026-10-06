/* Host callback and finite arithmetic models. No original files are needed. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/actor_states.c"

#define AT(p,n,t) (*(t *)((u8 *)(p)+(n)))
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
#define CLOSE(x,y) CHECK(fabsf((x)-(y)) < 0.00001f)

static unsigned checks, controls, predicates, vectors, stops, releases, requests;
static unsigned clears, duration_calls, refs, query_calls, notifications, matrices;
static union { GeorgeActorBits64 align; u8 bytes[0xA00]; } actor_bytes;
static union { GeorgeActorBits64 align; u8 bytes[0x600]; } data_bytes[2];
static GeorgeGoalEntity *actor;
static u8 control_bytes[2][0x80], control_tables[2][0xE0];
static GeorgeActorControlObject *control[2];
static u8 vehicle_bytes[2][0x80], vehicle_tables[2][0x1C0];
static GeorgeGoalVirtualObject *vehicles[2];
static u8 object_bytes[2][0xC0], reference[0x80], map_bytes[0x50], node_bytes[0x60];
static u8 primary[0x20], secondary[2][0x20];
static GeorgeDeimosValue output;
GeorgeDeimosValue *D_00474F48 = &output;
static s32 predicate_result, vector_mode, release_mode, request_mode, reference_mode;
static s32 duration_mode, compare_mode, position_result, query_result;
static s32 soft_compare_calls, soft_sub_calls, soft_mul_calls, soft_add_calls, soft_from_calls;
static u32 queried_flags[4], observed_word, reset_word, duration_offset;
static float duration_value;
static void *expected_control, *observed_release, *observed_stop, *observed_reference;
static void *expected_vehicle_self;
static void *observed_objects[8];
static u32 observed_words[8];
static float observed_scale[8];
static GeorgeActorRequestCallback observed_callbacks[8];
static GeorgeMathVec3 observed_vector;

static GeorgeActorBits64 bits(double value) { union { double value; GeorgeActorBits64 bits; } u; u.value=value;return u.bits; }
static double number64(GeorgeActorBits64 value) { union { double value; GeorgeActorBits64 bits; } u;u.bits=value;return u.value; }
static float nan32(void) { union { float value; u32 bits; } u;u.bits=0x7FC00001U;return u.value; }

static void on_void(void *self) {
    CHECK(self == expected_control); ++controls;
}
static void on_word(void *self,u32 word) {
    CHECK(self == expected_control); ++controls; observed_word=word;
    if (request_mode==8) AT(actor,0x704,u32)=0, AT(actor,0x73C,u32)=1;
    if (request_mode==9) AT(actor,0x20,void *)=control[1],expected_control=control_bytes[1]-4;
}
static void on_vector(void *self,const GeorgeMathVec3 *vector) {
    CHECK(self == expected_control); ++vectors; observed_vector=*vector;
    if(vector_mode==1) AT(actor,0x568,float)=-1.0f;
    if(vector_mode==2) AT(actor,0x568,float)=nan32();
    if(vector_mode==3) AT(actor,0x580,void *)=0;
}
static s32 on_predicate(void *self,float a,float b) {
    CHECK(self==expected_control);CLOSE(a,0.20000000298023224f);CLOSE(b,0.25f);
    ++predicates;return predicate_result;
}
static void on_entity(void *self,GeorgeGoalEntity *entity) {
    CHECK(self==expected_vehicle_self);CHECK(entity==actor);++controls;
    AT(actor,0x730,void *)=vehicles[0];
}
static void install_controls(unsigned index,s16 adjustment) {
    GeorgeGoalVirtualVoid *p;
    control[index]=(GeorgeActorControlObject *)control_bytes[index];
    control[index]->field00=control_tables[index];
    p=(GeorgeGoalVirtualVoid *)(control_tables[index]+0x88);p->adjustment=adjustment;p->invoke=on_void;
    p=(GeorgeGoalVirtualVoid *)(control_tables[index]+0xA0);p->adjustment=adjustment;p->invoke=on_void;
    { GeorgeGoalVirtualWord *w=(GeorgeGoalVirtualWord *)(control_tables[index]+0x30);w->adjustment=adjustment;w->invoke=on_word;
      w=(GeorgeGoalVirtualWord *)(control_tables[index]+0xB8);w->adjustment=adjustment;w->invoke=on_word; }
    { GeorgeActorVirtualVectorInput *v=(GeorgeActorVirtualVectorInput *)(control_tables[index]+0x78);v->adjustment=adjustment;v->invoke=on_vector;
      v=(GeorgeActorVirtualVectorInput *)(control_tables[index]+0xB0);v->adjustment=adjustment;v->invoke=on_vector; }
    { GeorgeActorVirtualPredicate *v=(GeorgeActorVirtualPredicate *)(control_tables[index]+0xD0);v->adjustment=adjustment;v->invoke=on_predicate; }
}
static void reset(void) {
    memset(&actor_bytes,0,sizeof(actor_bytes));memset(data_bytes,0,sizeof(data_bytes));
    memset(control_bytes,0,sizeof(control_bytes));memset(control_tables,0,sizeof(control_tables));
    memset(object_bytes,0,sizeof(object_bytes));memset(map_bytes,0,sizeof(map_bytes));memset(&output,0,sizeof(output));
    actor=(GeorgeGoalEntity *)actor_bytes.bytes;actor->field18=(GeorgeGoalEntityData *)data_bytes[0].bytes;
    install_controls(0,4);install_controls(1,-4);AT(actor,0x20,void *)=control[0];expected_control=control_bytes[0]+4;
    vehicles[0]=(GeorgeGoalVirtualObject *)vehicle_bytes[0];vehicles[1]=(GeorgeGoalVirtualObject *)vehicle_bytes[1];
    vehicles[0]->field04=vehicle_tables[0];vehicles[1]->field04=vehicle_tables[1];
    { GeorgeActorVirtualEntity *p=(GeorgeActorVirtualEntity *)(vehicle_tables[1]+0x1B0);p->adjustment=-8;p->invoke=on_entity; }
    { GeorgeActorVirtualEntity *p=(GeorgeActorVirtualEntity *)(vehicle_tables[0]+0x1B0);p->adjustment=8;p->invoke=on_entity; }
    expected_vehicle_self=vehicle_bytes[0]+8;
    AT(actor,0x1C,void *)=map_bytes;AT(map_bytes,8,s32)=1;AT(map_bytes,0xC,u32)=70;AT(map_bytes,0x14,u32)=170;
    AT(actor,0x14,u32)=99;AT(actor,0x74,float)=55.0f;AT(actor,0x78,float)=66.0f;
    AT(actor,0x580,void *)=reference;AT(actor,0x35C,float)=0.1f;AT(actor,0x3BC,u32)=0x3BAC798C;
    AT(actor,0x1B0,void *)=primary;AT(actor,0x1B4,void *)=secondary[0];
    AT(actor,0x700,u32)=70;AT(actor,0x704,u32)=70;AT(actor,0x708,u32)=70;AT(actor,0x720,float)=2.5f;
    AT(actor,0x56C,u32)=70;AT(actor,0x570,u32)=70;AT(actor,0x574,u32)=70;
    controls=predicates=vectors=stops=releases=requests=clears=duration_calls=refs=query_calls=notifications=matrices=0;
    predicate_result=vector_mode=release_mode=request_mode=reference_mode=duration_mode=compare_mode=position_result=query_result=0;
    soft_compare_calls=soft_sub_calls=soft_mul_calls=soft_add_calls=soft_from_calls=0;
    memset(queried_flags,0,sizeof(queried_flags));duration_value=4800.0f;duration_offset=0;
    observed_word=reset_word=0;observed_release=observed_stop=observed_reference=0;
    D_00474F48=&output;
}

s32 func_0018FCF0(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) {CHECK(entity==actor);CHECK(mask==0x10000000ULL);return query_result;}
void func_0018FD30(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) {CHECK(entity==actor);AT(entity,0x190,GeorgeActorBits64)|=mask;}
void func_0018FD40(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) {CHECK(entity==actor);AT(entity,0x190,GeorgeActorBits64)&=~mask;}
s32 func_001CE080(const GeorgeMathVec3 *a,const GeorgeMathVec3 *b) {CHECK(a==VECTOR(actor,0x40));CHECK(b==VECTOR(object_bytes[0],0x40));return position_result;}
void func_001CDD90(void *object,u32 word) {CHECK(object==object_bytes[0]+0xB4);reset_word=word;AT(object_bytes[0],0x40,u32)=12;}
void func_001C4158(void *object,u32 word) {CHECK(object==object_bytes[0]);observed_word=word;}
void func_0015F280(void *object,float a,float b) {CHECK(object==object_bytes[0]);CLOSE(a,.2f);CLOSE(b,.75f);++releases;}
void func_002393F8(u32 word) {
    CHECK(word==1 || word==2);++releases;
    if(release_mode==3 && word==1) {actor->field18=(GeorgeGoalEntityData *)data_bytes[1].bytes;AT(actor,0x290,u32)=2;}
}
void func_002727D8(void *object) {++stops;observed_stop=object;if(release_mode==1)AT(actor,0x730,void *)=vehicles[1];}
void func_001F6148(void *object) {
    ++releases;observed_release=object;
    if(release_mode==1){CHECK(AT(actor,0x740,u32)==0);AT(actor,0x730,void *)=vehicles[0];}
    if(release_mode==2)AT(actor,0x724,void *)=object_bytes[1];
}
float func_002A6E60(void *source) {CHECK(source==reference);++duration_calls;if(duration_mode)AT(actor,duration_offset,u32)=0xFFFFFFFFU;return duration_value;}
void func_00121A80(void *source,GeorgeMathVec3 *position,GeorgeMathVec3 *direction) {CHECK(source==reference);position->x=1;position->y=2;position->z=3;direction->x=4;direction->y=5;direction->z=6;}
void func_001219B8(void *object) {++refs;observed_reference=object;}
void func_00121988(void *object) {++refs;observed_reference=object;if(reference_mode)AT(actor,0x584,float)=0;}
void func_001219E8(void *object) {++refs;observed_reference=object;}
void func_00121A18(void *object) {++refs;observed_reference=object;}
GeorgeActorBits64 func_00374848(float x) {++soft_from_calls;return bits(x);}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_sub_calls;return bits(number64(a)-number64(b));}
GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_mul_calls;return bits(number64(a)*number64(b));}
GeorgeActorBits64 func_00372C68(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_add_calls;return bits(number64(a)+number64(b));}
s32 func_00373250(GeorgeActorBits64 a,GeorgeActorBits64 b) {
    ++soft_compare_calls;
    if(compare_mode==1 && soft_compare_calls==2)AT(actor,0x57C,float)=1.0f;
    if(compare_mode==2 && soft_compare_calls==3)AT(actor,0x57C,float)=5.0f,AT(actor,0x58,float)=6.0f;
    return number64(a)<number64(b)?-1:number64(a)>number64(b)?1:0;
}
float func_003734F8(GeorgeActorBits64 x) {return (float)number64(x);}
static void request_log(void *object,u32 word,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,float scale) {
    observed_objects[requests]=object;observed_words[requests]=word;observed_scale[requests]=scale;observed_callbacks[requests]=callback;++requests;
    if(request_mode==1 && context) {
        AT(actor,0x704,u32)=71;AT(actor,0x700,u32)=71;AT(actor,0x708,u32)=71;
        AT(actor,0x1B4,void *)=secondary[1];AT(actor,0x720,float)=7.0f;
        AT(map_bytes,0xC,u32)=71;AT(map_bytes,0x14,u32)=171;
    }
    if(request_mode==2 && context)callback(0,context,reference);
    if(request_mode==3 && !context)AT(actor,0x1B0,void *)=secondary[1],AT(actor,0x368,u32)=999;
}
s32 func_00270510(void *object,u32 word,u32 mode0,u32 mode1,u32 owner,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,u32 invoke,u32 argument,float time) {
    CHECK(mode0==0&&mode1==0&&invoke==0&&argument==0);CLOSE(time,0);
    if(request_mode==3&&context)CHECK(owner==999);
    request_log(object,word,callback,context,time);return 1;
}
void func_00272390(void *object,u32 word,u32 mode0,u32 mode1,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context,u32 reuse,u32 invoke,u32 argument,float time,float scale) {
    CHECK(mode0==0&&mode1==0&&reuse==0&&invoke==0&&argument==0);CLOSE(time,0);
    CHECK((callback!=0)==(context!=0));request_log(object,word,callback,context,scale);
}
u32 func_00272C10(void *object) {CHECK(object==primary);return queried_flags[query_calls++];}
void func_00270858(void *object) {++stops;observed_stop=object;if(request_mode==4)AT(actor,0x72C,u32)=99;}
void *func_00238BA0(void *object,u32 key) {CHECK(object==object_bytes[0]);CHECK(key==0x1A6B0F5D);return object_bytes[1];}
void func_00232BA0(void *object) {CHECK(object==object_bytes[1]);CHECK(AT(actor,0x72C,u32)==0);CLOSE(AT(actor,0x728,float),0);}
void *func_00239E40(u32 word,void *dest,u32 a,u32 b,u32 c,u32 d,u32 e) {CHECK(word==7);CHECK(dest==(u8 *)actor+0xB0);CHECK((a|b|c|d|e)==0);++refs;return dest;}
void func_001787B0(GeorgeGoalEntity *entity) {CHECK(entity==actor);++refs;}
void func_002A1C08(void *dest,const void *source) {
    CHECK(source==(u8 *)actor+0xF0);CHECK(dest==object_bytes[0]+0x10);++matrices;
    if(request_mode==5)AT(actor,0x288,void *)=object_bytes[1];
}
void func_002389E8(void *object,const void *matrix) {CHECK(matrix==(u8 *)actor+0xF0);CHECK(object==object_bytes[1]);++matrices;}
void func_00235CD8(void *object,u32 key,u32 word) {CHECK(key==0x9F79558F&&word==1);++notifications;observed_release=object;if(request_mode==6)AT(actor,0x1B0,void *)=0;}
void func_002D02A8(GeorgeDeimosPoolNode *node,s32 count,s32 offset) {
    CHECK(node==(GeorgeDeimosPoolNode *)node_bytes);CHECK(offset==0);++clears;
    if(count==1){CHECK(output.tag==2&&output.subtype==0);CLOSE(output.payload.scalar,AT(actor,0x36C,float));}
    else {CHECK(count==0);if(request_mode==4)AT(actor,0x72C,u32)=0xFFFFFFFFU;}
}

static void timers(void) {
    unsigned i;float gate[]={-.03f,-.02f,0,.02f,.03f};
    for(i=0;i<5;i++){reset();AT(actor,0x50,float)=gate[i];AT(actor,0x560,float)=0;func_0017D908(actor);CLOSE(AT(actor,0x560,float),i==2?.1f:0);CLOSE(AT(actor,0x50,float),i==2?-5:gate[i]);}
    reset();AT(actor,0x560,float)=3;AT(actor,0x190,GeorgeActorBits64)=1ULL<<55;func_0017D908(actor);CHECK(AT(actor,0x14,u32)==0);CHECK(AT(actor,0x190,GeorgeActorBits64)==((1ULL<<55)|0x80));
    reset();AT(actor,0x44,float)=.1f;AT(actor,0x55C,float)=.9f;func_0017D908(actor);CHECK(AT(actor,0x14,u32)==99);func_0017D908(actor);CHECK(AT(actor,0x14,u32)==5);
    reset();AT(actor,0x50,float)=nan32();AT(actor,0x44,float)=nan32();func_0017D908(actor);CLOSE(AT(actor,0x560,float),0);CLOSE(AT(actor,0x55C,float),.1f);
    for(i=0;i<4;i++){reset();AT(actor,0x568,float)=.2f;vector_mode=i==1?1:i==2?2:0;AT(actor,0x68,float)=i==3?1:0;func_0017F588(actor);CHECK(vectors==1);CLOSE(observed_vector.x,0);CLOSE(AT(actor,0x74,float),0);CHECK(AT(actor,0x14,u32)==(i==1?0:99));}
    reset();AT(actor,0x568,float)=0;AT(actor,0x68,float)=.3f;func_0017F588(actor);CHECK(AT(actor,0x14,u32)==2);CHECK(vectors==0);
    reset();AT(actor,0x568,float)=0;AT(actor,0x68,float)=nan32();func_0017F588(actor);CHECK(AT(actor,0x14,u32)==2);
    {void (*callbacks[])(u32,GeorgeGoalEntity *,void *)={func_00194B70,func_00194C98,func_00194D60,func_00194DE0};u32 state[]={0x588,0x710,0x72C,0x740},time[]={0x584,0x714,0x728,0x748};
    for(i=0;i<4;i++){reset();duration_mode=1;duration_offset=state[i];callbacks[i](123,actor,reference);CHECK(AT(actor,state[i],u32)==0);CLOSE(AT(actor,time[i],float),1);CHECK(duration_calls==1);}}
}
static void lifecycle(void) {
    reset();func_00194BB8(actor);CHECK(observed_word==1);CHECK(AT(actor,0x74,u32)==0&&AT(actor,0x584,u32)==0&&AT(actor,0x588,u32)==0&&AT(actor,0x58C,u32)==0);
    reset();request_mode=8;func_00194CE0(actor);CHECK(AT(actor,0x710,u32)==100&&AT(actor,0x718,u32)==0);
    reset();func_00194CE0(actor);CHECK(AT(actor,0x710,u32)==0);
    reset();AT(actor,0x1B0,void *)=0;func_00194C08(actor);func_00194D38(actor);CHECK(stops==2&&observed_stop==0);
    reset();AT(actor,0x724,void *)=object_bytes[0];release_mode=2;func_00194DA8(actor);CHECK(releases==1&&observed_release==object_bytes[0]&&AT(actor,0x724,void *)==0);
    reset();func_00194DA8(actor);CHECK(releases==0);
    reset();request_mode=8;func_00194E28(actor);CHECK(controls==2&&observed_word==0);CHECK(AT(actor,0x740,u32)==1000);CHECK(AT(actor,0x78,u32)==0&&AT(actor,0x74C,u32)==0&&AT(actor,0x750,u32)==0&&AT(actor,0x754,u32)==0);
    reset();request_mode=9;func_00194E28(actor);CHECK(controls==2);
    reset();AT(actor,0x730,void *)=vehicles[0];release_mode=1;func_00194EA8(actor);CHECK(stops==1&&releases==1&&controls==1);CHECK(observed_release==vehicles[1]&&AT(actor,0x730,void *)==0);
    reset();AT(map_bytes,8,s32)=-1;CHECK(func_00195D88(actor,70)==0xFFFFFFFFU);AT(map_bytes,8,s32)=0;CHECK(func_00195D88(actor,70)==0xFFFFFFFFU);AT(map_bytes,8,s32)=2;AT(map_bytes,0x18,u32)=70;AT(map_bytes,0x20,u32)=999;CHECK(func_00195D88(actor,70)==170);CHECK(func_00195D88(actor,1)==0xFFFFFFFFU);
}
static void scalar_release(void) {
    reset();AT(actor,0x364,void *)=object_bytes[0];AT(object_bytes[0],0x40,u32)=11;AT(actor,0x438,float)=1;AT(actor,0x190,GeorgeActorBits64)=~0ULL;AT(actor->field18,0x1D8,u32)=0x45A78000;func_0017EB20(actor);CHECK(reset_word==11&&observed_word==12);CHECK(AT(actor,0x190,GeorgeActorBits64)==0xFFFFFFFFFFFBFFFFULL);CHECK(AT(control[0],0x34,u32)==1);
    reset();query_result=1;AT(actor,0x438,float)=nan32();func_0017EB20(actor);CHECK(isnan(AT(actor,0x438,float)));CLOSE(AT(actor,0x388,float),0);
    reset();AT(actor,0x378,void *)=object_bytes[0];AT(actor,0x190,GeorgeActorBits64)=0x200;AT(actor,0x3C0,u32)=123;func_0017EBF0(actor);CHECK(releases==1&&AT(actor,0x3C0,u32)==123);
    reset();AT(actor->field18,0xA0,float)=2;AT(data_bytes[1].bytes,0xA0,float)=999;AT(data_bytes[1].bytes,0x198,float)=3;AT(data_bytes[1].bytes,0x404,float)=.5f;AT(actor,0x28C,u32)=1;AT(actor,0x36C,float)=10;AT(actor,0x38C,void *)=node_bytes;release_mode=3;func_0017EBF0(actor);CHECK(releases==2&&clears==1);CLOSE(AT(actor,0x36C,float),3.5f);CHECK(AT(actor,0x28C,u32)==0&&AT(actor,0x290,u32)==0&&AT(actor,0x3C0,u32)==0);
    reset();AT(actor->field18,0xA0,float)=-2;AT(actor->field18,0x198,float)=3;AT(actor->field18,0x404,float)=.5f;AT(actor,0x36C,float)=10;func_0017EBF0(actor);CLOSE(AT(actor,0x36C,float),16.5f);
}
static void state12(void) {
    unsigned i; s32 wait[]={1,101,201};
    for(i=0;i<3;i++){reset();AT(actor,0x710,s32)=wait[i];func_00183380(actor);CHECK(requests==0);CLOSE(AT(actor,0x74,float),0);}
    reset();AT(actor,0x710,s32)=2;AT(actor,0x714,float)=.1f;func_00183380(actor);CHECK(AT(actor,0x710,u32)==100);
    reset();AT(actor,0x710,s32)=2;AT(actor,0x714,float)=nan32();func_00183380(actor);CHECK(AT(actor,0x710,u32)==2);
    reset();AT(actor,0x710,s32)=100;request_mode=1;func_00183380(actor);CHECK(requests==2);CHECK(AT(actor,0x710,u32)==101);CHECK(observed_words[0]==70&&observed_words[1]==171);CHECK(observed_objects[1]==secondary[1]);CLOSE(observed_scale[0],2.5f);CLOSE(observed_scale[1],7);
    reset();AT(actor,0x710,u32)=0;request_mode=2;func_00183380(actor);CHECK(AT(actor,0x710,u32)==2);CLOSE(AT(actor,0x714,float),1);
    reset();AT(actor,0x710,u32)=100;AT(actor,0x1B0,void *)=0;func_00183380(actor);CHECK(AT(actor,0x710,u32)==200&&requests==0);
    for(i=0;i<3;i++){reset();AT(actor,0x710,u32)=102;AT(actor,0x70C,s32)=i==0?-1:i==1?1:2;func_00183380(actor);CHECK(AT(actor,0x710,u32)==(i==1?201:100));CHECK(requests==(i==1?2:0));}
    reset();AT(actor,0x710,u32)=102;AT(actor,0x68,float)=nan32();AT(actor,0x714,float)=1;func_00183380(actor);CHECK(requests==0&&AT(actor,0x710,u32)==102);
    reset();AT(actor,0x710,u32)=102;AT(actor,0x68,float)=1;func_00183380(actor);CHECK(requests==2&&AT(actor,0x710,u32)==201);
    reset();AT(actor,0x710,u32)=102;AT(actor,0x718,u32)=1;AT(actor,0x708,u32)=0;func_00183380(actor);CHECK(AT(actor,0x14,u32)==0&&requests==0);
    reset();AT(actor,0x710,u32)=200;AT(actor,0x1B0,void *)=0;func_00183380(actor);CHECK(AT(actor,0x14,u32)==0);
    reset();AT(actor,0x710,u32)=202;func_00183380(actor);CHECK(AT(actor,0x14,u32)==0);
    reset();AT(actor,0x710,u32)=202;AT(actor,0x714,float)=nan32();func_00183380(actor);CHECK(AT(actor,0x14,u32)==99);
    reset();AT(actor,0x710,u32)=200;AT(actor,0x708,u32)=0;func_00183380(actor);CHECK(AT(actor,0x14,u32)==0&&requests==0);
    reset();AT(actor,0x710,u32)=200;func_00183380(actor);CHECK(AT(actor,0x710,u32)==201&&requests==2);
    reset();AT(actor,0x710,u32)=0;AT(actor,0x704,u32)=0;func_00183380(actor);CHECK(AT(actor,0x710,u32)==100&&requests==0);
    reset();AT(actor,0x710,u32)=0xFFFFFFFFU;func_00183380(actor);CHECK(AT(actor,0x710,u32)==0);
    reset();AT(actor,0x190,GeorgeActorBits64)=0x80000;predicate_result=1;func_00183380(actor);CHECK(predicates==0&&AT(actor,0x14,u32)==99);
    reset();predicate_result=1;func_00183380(actor);CHECK(predicates==1&&AT(actor,0x14,u32)==3);
}
static void state13(void) {
    unsigned i;u32 keys[]={0x260105EC,0xCB426EF9,0xCAD99652,0xEC789588,0x1111150C,0x728F0147,0x75188880,0x794F22DA};
    for(i=0;i<8;i++){reset();AT(actor,0x1C8,void *)=object_bytes[0];AT(actor,0x3BC,u32)=keys[i];func_00183730(actor);CHECK(requests==2&&AT(actor,0x4E4,u32)==(i==0?77:78));CHECK(observed_callbacks[1]==func_00194D60&&observed_callbacks[0]==0);}
    reset();AT(actor,0x1C8,void *)=object_bytes[0];AT(actor,0x3BC,u32)=keys[1];AT(actor,0x190,GeorgeActorBits64)=1ULL<<45;request_mode=3;func_00183730(actor);CHECK(observed_words[0]==79&&observed_words[1]==79&&observed_objects[1]==secondary[1]);
    reset();AT(actor,0x1C8,void *)=object_bytes[0];AT(actor->field18,0x27C,u32)=7;func_00183730(actor);CHECK(AT(actor,0x72C,u32)==1&&requests==0&&refs==1);
    reset();AT(actor,0x72C,u32)=1;AT(actor,0x728,float)=1;func_001839D0(actor);CHECK(query_calls==3&&stops==0);CLOSE(AT(actor,0x74,float),66);CHECK((AT(actor,0x190,GeorgeActorBits64)&(1ULL<<43))!=0);
    reset();AT(actor,0x72C,u32)=1;AT(actor,0x728,float)=0;AT(actor,0x3A8,void *)=node_bytes;request_mode=4;func_001839D0(actor);CHECK(stops==2&&clears==1&&AT(actor,0x72C,u32)==0);
    reset();AT(actor,0x72C,u32)=1;AT(actor,0x288,void *)=object_bytes[0];queried_flags[0]=0x2000;request_mode=5;func_001839D0(actor);CHECK(matrices==2&&notifications==1&&observed_release==object_bytes[1]);CHECK(AT(object_bytes[0],0xA0,u32)==0x100);
    reset();AT(actor,0x72C,u32)=1;AT(actor,0x228,void *)=object_bytes[0];queried_flags[1]=0x8000;request_mode=6;func_001839D0(actor);CHECK(query_calls==2&&notifications==1&&stops==0);CHECK((AT(actor,0x190,GeorgeActorBits64)&(1ULL<<44))!=0);
    reset();AT(actor,0x72C,u32)=1;queried_flags[2]=0x4000;func_001839D0(actor);CHECK(observed_word==0&&controls==3);
    reset();AT(actor,0x72C,u32)=0;func_001839D0(actor);CHECK(query_calls==0);CLOSE(AT(actor,0x74,float),66);
    reset();AT(actor,0x72C,u32)=1;AT(actor,0x728,float)=nan32();func_001839D0(actor);CHECK(query_calls==3&&stops==0);
    reset();AT(actor,0x72C,u32)=1;predicate_result=1;func_001839D0(actor);CHECK(controls==2&&observed_word==0);
}
static void state6(void) {
    unsigned i;float target[]={0,.5f,-.5f,3.1415927410125732f,-3.1415927410125732f,4,-4,6,-6};
    for(i=0;i<9;i++){
        double difference=target[i], term;float expect;
        reset();AT(actor,0x588,u32)=1;AT(actor,0x57C,float)=target[i];
        if(fabs(difference)>number64(0x400921FB60000000ULL)){term=fabs(difference)-number64(0x401921FB60000000ULL);if(difference<0)term*=-(double).1f;else term*=(double).1f;}
        else term=difference*(double).1f;
        expect=(float)(term*5.0);func_00182B98(actor);CLOSE(AT(actor,0x58,float),expect);CHECK(requests==0&&soft_add_calls==1&&soft_from_calls==4);CLOSE(observed_vector.x,5);CLOSE(observed_vector.y,10);CLOSE(observed_vector.z,15);
    }
    reset();AT(actor,0x588,u32)=100;request_mode=1;func_00182B98(actor);CHECK(requests==2&&AT(actor,0x588,u32)==101);CLOSE(observed_scale[0],5);CLOSE(observed_scale[1],5);
    reset();AT(actor,0x588,u32)=2;func_00182B98(actor);CHECK(refs==1&&AT(actor,0x588,u32)==100);
    reset();AT(actor,0x588,u32)=102;AT(actor,0x578,u32)=0x80000000;AT(actor,0x584,float)=1;queried_flags[0]=0x0F000000;reference_mode=1;func_00182B98(actor);CHECK(refs==1&&AT(actor,0x578,u32)==0x7FFFFFFF&&AT(actor,0x588,u32)==100);
    reset();AT(actor,0x588,u32)=102;AT(actor,0x578,u32)=1;func_00182B98(actor);CHECK(refs==1&&AT(actor,0x588,u32)==200);
    reset();AT(actor,0x588,u32)=202;vector_mode=3;func_00182B98(actor);CHECK(AT(actor,0x14,u32)==0);
    reset();AT(actor,0x588,u32)=202;AT(actor,0x584,float)=nan32();func_00182B98(actor);CHECK(AT(actor,0x14,u32)==99&&refs==0);
    reset();AT(actor,0x588,u32)=200;func_00182B98(actor);CHECK(AT(actor,0x588,u32)==201&&requests==2);
    reset();AT(actor,0x588,u32)=200;AT(actor,0x574,u32)=0;func_00182B98(actor);CHECK(AT(actor,0x14,u32)==0&&requests==0);
    reset();AT(actor,0x588,u32)=0;AT(actor,0x56C,u32)=0;func_00182B98(actor);CHECK(AT(actor,0x588,u32)==100&&requests==0);
    reset();AT(actor,0x588,u32)=0x7FFFFFFFU;func_00182B98(actor);CHECK(AT(actor,0x588,u32)==0x80000000U);
    reset();AT(actor,0x588,u32)=1;AT(actor,0x57C,float)=4;compare_mode=1;func_00182B98(actor);CLOSE(AT(actor,0x58,float),(1.0f-6.2831854820251465f)*.5f);
    reset();AT(actor,0x588,u32)=1;AT(actor,0x57C,float)=4;compare_mode=2;func_00182B98(actor);CLOSE(AT(actor,0x58,float),(4.0f-6.2831854820251465f)*-.5f);
}
int main(void) {timers();lifecycle();scalar_release();state12();state13();state6();printf("actor_states: %u checks passed\n",checks);return 0;}
