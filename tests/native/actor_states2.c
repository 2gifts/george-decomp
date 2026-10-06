/* Independent callback models and finite arithmetic checks; no retail files. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/actor_states2.c"

#define AT(p,n,t) (*(t *)((u8 *)(p)+(n)))
#define CHECK(x) do { ++checks; if (!(x)) {fprintf(stderr,"line %d: %s\n",__LINE__,#x);exit(1);} } while (0)
#define CLOSE(x,y) CHECK(fabsf((x)-(y)) < 0.00002f)

static unsigned checks;
static union { GeorgeActorBits64 align; u8 bytes[0xA00]; } actor_storage;
static u8 data[2][0x200], controls[2][0x60], control_tables[2][0xE0];
static u8 vehicles[2][0x60], vehicle_tables[2][0x220], configuration[0x80];
static u8 primary[0x500], companion[0x500], replacement[0x500], effect_storage[0x1B4];
static u8 map_object[0x40], target_object[0x40];
static GeorgeGoalEntity *actor;
static GeorgeMathVec3 output_vectors[3], observed_vector, captured_vectors[8], *retained_first;
static float output_angles[3], *retained_angle;
static unsigned vectors, words, voids, predicates, outputs, command_calls;
static unsigned requests, effects, allocations, insertions, registrations, hashes, notifications;
static unsigned releases, matrix_calls, effect_creates, cleanup_calls, array_calls, clear_calls, stop_calls;
static unsigned flag_calls, soft_from, soft_sub, soft_mul, soft_add, soft_cmp;
static s32 predicate_result, moving_result, query_result, request_result, vehicle_end;
static s32 command118, command110, selected_command, selected_mode;
static s32 vector_mode, request_mode, command_mode, output_mode, registry_mode, release_mode, soft_mode;
static void *observed_self, *observed_effect, *observed_clear;
static void *request_objects[10];
static u32 request_words[10], observed_effect_word, released_words[10], word_value;
static float request_times[10];
static s32 request_commands[10];
static GeorgeActorRequestCallback request_callbacks[10];
static void *registry_slots[4];
static GeorgeActorEffectRecord record_storage;
static u32 array_values[3] = {1,0,2};
static s32 array_count;

void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[] = "registry";
const u8 D_0042D640[] = "effect";

static GeorgeActorBits64 bits(double x) {union {double f;GeorgeActorBits64 b;}u;u.f=x;return u.b;}
static double value(GeorgeActorBits64 x) {union {double f;GeorgeActorBits64 b;}u;u.b=x;return u.f;}
static float nan32(void) {union {float f;u32 b;}u;u.b=0x7FC00001U;return u.f;}
static void check_control(void *self) {
    void *object=AT(actor,0x20,void *);
    CHECK(self==(u8 *)object+(object==controls[0]?4:-4));
}
static void check_vehicle(void *self) {
    void *object=AT(actor,0x730,void *);
    CHECK(self==(u8 *)object+(object==vehicles[0]?8:-8));
}
static void on_word(void *self,u32 word) {check_control(self);++words;word_value=word;}
static void on_void(void *self) {check_control(self);++voids;}
static void on_vector(void *self,const GeorgeMathVec3 *vector) {
    check_control(self);CHECK(vectors<8);if(vector)observed_vector=*vector,captured_vectors[vectors]=*vector;++vectors;
    if(vector_mode==1) AT(actor,0x52F,signed char)=0;
    if(vector_mode==2) AT(actor,0x528,float)=2.0f;
    if(vector_mode==3) AT(actor,0x20,void *)=controls[1];
}
static s32 on_moving(void *self) {check_control(self);if(vector_mode==4)AT(actor,0x52F,signed char)=1;return moving_result;}
static const GeorgeMathVec3 *on_output(void *self) {check_control(self);return &output_vectors[0];}
static s32 on_predicate(void *self,float a,float b) {
    check_control(self);CLOSE(a,.2f);CLOSE(b,.25f);++predicates;return predicate_result;
}
static GeorgeGoalVirtualObject *on_configuration(void *self) {check_vehicle(self);return (GeorgeGoalVirtualObject *)configuration;}
static void on_vehicle_vector(void *self,s32 command,GeorgeMathVec3 *vector,float *scalar) {
    check_vehicle(self);CHECK(outputs<3);request_commands[outputs]=command;
    *vector=output_vectors[outputs];*scalar=output_angles[outputs];
    if(outputs==0)retained_first=vector,retained_angle=scalar;
    if(output_mode==1 && outputs==0)AT(actor,0x730,void *)=vehicles[1];
    if(output_mode==2 && outputs==2)retained_first->x=50.0f,*retained_angle=1.5f;
    ++outputs;
}
static s32 on118(void *self,s32 command) {
    check_vehicle(self);CHECK(command==AT(actor,0x734,s32));++command_calls;
    if(command_mode==1)AT(actor,0x734,s32)=-65536,AT(actor,0x730,void *)=vehicles[1];
    return command118;
}
static s32 on110(void *self,s32 command) {check_vehicle(self);CHECK(command==AT(actor,0x734,s32));++command_calls;return command110;}
static s32 on100(void *self,s32 command) {check_vehicle(self);CHECK(command==AT(actor,0x734,s32));++command_calls;return 0;}
static s32 on108(void *self,s32 command) {check_vehicle(self);CHECK(command==AT(actor,0x734,s32));++command_calls;return 0;}
static s32 on_selected(void *self) {check_vehicle(self);return selected_command;}
static s32 on_mode(void *self) {check_vehicle(self);return selected_mode;}
static s32 on_end(void *self) {check_vehicle(self);return vehicle_end;}
static float on_blend(void *self) {check_vehicle(self);return AT(configuration,0x60,float);}

static void reset(void) {
    unsigned i;
    memset(&actor_storage,0,sizeof(actor_storage));memset(data,0,sizeof(data));
    memset(configuration,0,sizeof(configuration));memset(primary,0,sizeof(primary));
    memset(controls,0,sizeof(controls));memset(vehicles,0,sizeof(vehicles));
    memset(control_tables,0,sizeof(control_tables));memset(vehicle_tables,0,sizeof(vehicle_tables));
    actor=(GeorgeGoalEntity *)actor_storage.bytes;actor->field18=(GeorgeGoalEntityData *)data[0];
    AT(actor,0x20,void *)=controls[0];AT(actor,0x730,void *)=vehicles[0];
    for(i=0;i<2;i++) {
        const s16 ca=i==0?4:-4,va=i==0?8:-8;
        u32 off;
        AT(controls[i],0,const u8 *)=control_tables[i];AT(vehicles[i],4,const u8 *)=vehicle_tables[i];
        {GeorgeGoalVirtualWord*p=(GeorgeGoalVirtualWord *)(control_tables[i]+0x30);p->adjustment=ca;p->invoke=on_word;}
        {GeorgeGoalVirtualVoid*p=(GeorgeGoalVirtualVoid *)(control_tables[i]+0x88);p->adjustment=ca;p->invoke=on_void;}
        for(off=0x78;off<=0xB8;off+=8)if(off==0x78||off==0x80||off==0x98||off==0xB8){GeorgeActorVirtualVectorInput*p=(GeorgeActorVirtualVectorInput *)(control_tables[i]+off);p->adjustment=ca;p->invoke=on_vector;}
        {GeorgeGoalVirtualVector*p=(GeorgeGoalVirtualVector *)(control_tables[i]+0x68);p->adjustment=ca;p->invoke=on_output;}
        {GeorgeActorVirtualPredicate*p=(GeorgeActorVirtualPredicate *)(control_tables[i]+0xD0);p->adjustment=ca;p->invoke=on_predicate;}
        {GeorgeGoalVirtualInt*p=(GeorgeGoalVirtualInt *)(control_tables[i]+0xD8);p->adjustment=ca;p->invoke=on_moving;}
        for(off=0xD8;off<=0xF0;off+=0x18){GeorgeActorVirtualCommandScalarVector*p=(GeorgeActorVirtualCommandScalarVector *)(vehicle_tables[i]+off);p->adjustment=va;p->invoke=on_vehicle_vector;}
        {GeorgeGoalVirtualPointer*p=(GeorgeGoalVirtualPointer *)(vehicle_tables[i]+0x200);p->adjustment=va;p->invoke=on_configuration;}
        {GeorgeGoalVirtualCommand*p=(GeorgeGoalVirtualCommand *)(vehicle_tables[i]+0x118);p->adjustment=va;p->invoke=on118;
         p=(GeorgeGoalVirtualCommand *)(vehicle_tables[i]+0x110);p->adjustment=va;p->invoke=on110;
         p=(GeorgeGoalVirtualCommand *)(vehicle_tables[i]+0x100);p->adjustment=va;p->invoke=on100;
         p=(GeorgeGoalVirtualCommand *)(vehicle_tables[i]+0x108);p->adjustment=va;p->invoke=on108;}
        {GeorgeGoalVirtualInt*p=(GeorgeGoalVirtualInt *)(vehicle_tables[i]+0xF8);p->adjustment=va;p->invoke=on_selected;
         p=(GeorgeGoalVirtualInt *)(vehicle_tables[i]+0x130);p->adjustment=va;p->invoke=on_mode;
         p=(GeorgeGoalVirtualInt *)(vehicle_tables[i]+0x168);p->adjustment=va;p->invoke=on_end;}
        {GeorgeGoalVirtualFloatResult*p=(GeorgeGoalVirtualFloatResult *)(vehicle_tables[i]+0x128);p->adjustment=va;p->invoke=on_blend;}
    }
    for(i=0;i<3;i++){output_vectors[i].x=10.0f+2*i;output_vectors[i].y=20.0f+4*i;output_vectors[i].z=30.0f+6*i;output_angles[i]=.2f+.4f*i;}
    for(i=0;i<=0x4C;i+=4)AT(configuration,i,u32)=100+i;
    AT(configuration,0x60,float)=0.0f;
    AT(actor,0x14,u32)=99;AT(actor,0x1B0,void *)=primary;AT(actor,0x1B4,void *)=companion;AT(actor,0x368,u32)=55;
    AT(actor,0x35C,float)=.1f;AT(actor,0x748,float)=1.0f;AT(actor,0x744,float)=0;
    AT(actor,0x78,float)=66;AT(actor,0x4C,float)=1;AT(actor,0x50,float)=-2;AT(actor,0x54,float)=3;
    AT(actor,0x68,float)=1;AT(actor,0x8C,float)=1;AT(actor,0x530,float)=.5f;AT(actor,0x534,float)=5;
    AT(actor,0x540,float)=3;AT(actor,0x548,float)=4;AT(actor,0x538,u32)=110;AT(actor,0x53C,u32)=111;
    AT(actor,0x514,float)=10;AT(actor,0x518,float)=5;AT(actor,0x51C,float)=4;AT(actor,0x520,float)=7;
    AT(data[0],0x10,float)=2;AT(data[0],0x2C,float)=2;AT(data[0],0x40,float)=5;AT(data[0],0x3C,float)=4;
    AT(data[0],0x94,float)=3;AT(data[0],0x98,float)=5;AT(data[0],0x9C,float)=2.5f;AT(data[0],0x6C,float)=9;
    AT(actor,0x3DC,u32)=3;AT(actor,0x3E0,u32)=4;AT(actor,0x3E4,u32)=5;
    D_003F2D40=effect_storage;D_0046A0F0.field00=registry_slots;D_0046A0F0.field04=registry_slots;D_0046A0F0.field08=registry_slots+4;
    vectors=words=voids=predicates=outputs=command_calls=requests=effects=allocations=insertions=registrations=hashes=notifications=0;
    releases=matrix_calls=effect_creates=cleanup_calls=array_calls=clear_calls=stop_calls=flag_calls=0;
    soft_from=soft_sub=soft_mul=soft_add=soft_cmp=0;
    predicate_result=moving_result=query_result=vehicle_end=selected_command=selected_mode=command110=0;
    command118=request_result=1;vector_mode=request_mode=command_mode=output_mode=registry_mode=release_mode=soft_mode=0;
    observed_clear=observed_self=observed_effect=0;observed_effect_word=0;array_count=3;
}

void *func_002AEE60(u32 size) {++allocations;CHECK(size==0x1B4||size==12);return size==12?(void *)&record_storage:(void *)effect_storage;}
void *func_002481F0(void *object) {CHECK(object==effect_storage);return object;}
s32 func_00100AA8(const void*a,const void*b) {return **(const u32 *const *)a<**(const u32 *const *)b;}
void **func_00100C30(void **first,void **last,void *const *key,s32(*compare)(const void*,const void*)) {
    CHECK(first==registry_slots);CHECK(last==registry_slots);CHECK(*key==&record_storage);CHECK(compare==func_00100AA8);
    CHECK(record_storage.field00==9);CHECK(record_storage.field04==D_00421160);CHECK(record_storage.field08==D_003F2D40);
    if(registry_mode==1)D_0046A0F0.field08=last;
    if(registry_mode==2)D_0046A0F0.field04=registry_slots+1;
    return last;
}
void func_001007E0(GeorgeActorPointerRange*r,void**position,void*const*key) {CHECK(r==&D_0046A0F0);CHECK(position==registry_slots);CHECK(*key==&record_storage);++insertions;}
void func_002BD340(void) {}
s32 func_00396260(void(*callback)(void)) {CHECK(callback==func_002BD340);++registrations;return 0;}
u32 *func_002BEBA0(u32*out,const u8*text) {CHECK(text==D_0042D640);++hashes;*out=77;if(registry_mode==3)D_003F2D40=replacement;return out;}
void func_00251DB0(void*object,u32 key,const GeorgeMathVec3*position) {++effects;observed_effect=object;observed_effect_word=key;observed_vector=*position;}
void func_001B67C0(void*context,GeorgeGoalEntity*entity,u32 w0,u32 w1,u32 w2,u32 w3) {CHECK(context==VECTOR(actor,0x40));CHECK(entity==AT(actor,0x3B8,GeorgeGoalEntity*));CHECK(w0==AT(actor,0x3B4,u32));CHECK(w1==8||w1==9||w1==10);CHECK(w2==0&&w3==0);++notifications;}
s32 func_00270510(void*object,u32 word,u32 m0,u32 m1,u32 owner,GeorgeActorRequestCallback cb,GeorgeGoalEntity*context,u32 invoke,u32 last,float time) {
    CHECK(requests<10);CHECK(m0==0&&m1==0&&invoke==0&&last==0);CHECK(owner==AT(actor,0x368,u32));
    request_objects[requests]=object;request_words[requests]=word;request_times[requests]=time;request_callbacks[requests]=cb;
    if(cb){CHECK(cb==func_00194DE0);CHECK(context==actor);}else CHECK(context==0);
    ++requests;
    if(request_mode==1&&requests==1)AT(actor,0x1B0,void*)=replacement,AT(actor,0x368,u32)=101;
    if(request_mode==2&&requests==1)AT(actor,0x744,float)=7.0f;
    return request_result;
}
void func_00194DE0(u32 unused,GeorgeGoalEntity*entity,void*source) {(void)unused;(void)entity;(void)source;}
float func_002A35C0(GeorgeMathVec3*out,const GeorgeMathVec3*in,float scale) {
    float x=in->x,y=in->y,z=in->z,length=sqrtf((x*x+y*y)+z*z);
    if(length==0)out->x=scale,out->y=out->z=0;else{float factor=scale/length;out->x=x*factor;out->y=in->y*factor;out->z=in->z*factor;}return length;
}
void func_002393F8(u32 word) {CHECK(releases<10);released_words[releases++]=word;if(release_mode==1)AT(actor,0x294,void*)=replacement;}
void func_00235CD8(void*object,u32 key,u32 word) {CHECK(object==primary);CHECK(key==0xB95616B6U&&word==1);++notifications;}
u32 *func_002AAF50(void*array,s32 index) {CHECK(array==primary||array==replacement);CHECK(index>=0&&index<3);++array_calls;return array_values+index;}
s32 func_002AAF88(void*array) {CHECK(array==primary);return array_count;}
void func_002AAFD8(void*array) {++clear_calls;observed_clear=array;}
void func_00272A58(void*object) {CHECK(object==AT(actor,0x1B0,void*));++stop_calls;}
void func_00196980(GeorgeGoalEntity*entity) {CHECK(entity==actor);AT(entity,0x2D8,u32)=5;}
void func_002A2200(GeorgeRotationMatrix*out,const GeorgeRotationMatrix*left,const GeorgeRotationMatrix*right) {
    unsigned i;CHECK(right==(const GeorgeRotationMatrix *)ADDRESS(actor,0xB0));++matrix_calls;
    for(i=0;i<16;i++)CLOSE(left->element[i],i==2||i==4||i==9||i==15?1.0f:0.0f);
    for(i=0;i<16;i++)out->element[i]=(float)i;
}
u32 func_00236A10(const GeorgeRotationMatrix*m,u32 word,u32 a,u32 b,u32 c,u32 d) {CHECK(m->element[15]==15);CHECK(a==0&&b==0&&c==0&&d==0);++effect_creates;observed_effect_word=word;return 0;}
void func_0018FD30(GeorgeGoalEntity*entity,GeorgeActorBits64 mask) {CHECK(entity==actor);CHECK(mask==1ULL<<42);++flag_calls;AT(entity,0x4C,float)=11;AT(entity,0x50,float)=12;AT(entity,0x54,float)=13;}
void *func_0022C760(u32 word) {CHECK(word==12);AT(map_object,0xC,void*)=target_object;return map_object;}
void *func_00238BA0(void*object,u32 key) {CHECK(object==target_object);CHECK(key==0xD44DAD70U);return target_object;}
void func_0017EBF0(GeorgeGoalEntity*entity) {CHECK(entity==actor);++cleanup_calls;if(release_mode==2)entity->field18=(GeorgeGoalEntityData*)data[1];}
s32 func_0018FCF0(GeorgeGoalEntity*entity,GeorgeActorBits64 mask) {CHECK(entity==actor);CHECK(mask==0x10000000ULL);return query_result;}
s32 func_00272C10(void*object) {CHECK(object==AT(actor,0x1B0,void*));return selected_mode;}
s32 func_00272C30(void*object) {CHECK(object==AT(actor,0x1B0,void*));return selected_mode;}
GeorgeActorBits64 func_00374848(float x) {++soft_from;return bits(x);}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_sub;return bits(value(a)-value(b));}
GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_mul;return bits(value(a)*value(b));}
GeorgeActorBits64 func_00372C68(GeorgeActorBits64 a,GeorgeActorBits64 b) {++soft_add;return bits(value(a)+value(b));}
s32 func_00373250(GeorgeActorBits64 a,GeorgeActorBits64 b) {double x=value(a),y=value(b);++soft_cmp;if(soft_mode==1&&soft_cmp==2)*retained_angle=1;return x<y?-1:x>y?1:0;}
float func_003734F8(GeorgeActorBits64 b) {return (float)value(b);}

static void jump_tests(void) {
    float g=9.8100004196166992f,initial,time;
    reset();AT(actor,0x52C,signed char)=1;func_0017DD10(actor);
    initial=sqrtf((g+g)*7);time=(initial+sqrtf(initial*initial-(g+g)*5))/g;
    CLOSE(AT(actor,0x528,float),time);CHECK(requests==2&&request_words[0]==111);CLOSE(request_times[1],time);
    CLOSE(captured_vectors[0].x,10/time);CLOSE(captured_vectors[0].y,initial);CLOSE(captured_vectors[0].z,4/time);
    CHECK(AT(actor,0x52F,signed char)==1);CLOSE(AT(actor,0x388,float),9);CHECK(effects==1&&hashes==1);
    reset();AT(actor,0x68,float)=0;AT(actor,0x530,float)=.2f;AT(actor,0x190,GeorgeActorBits64)=0x80ULL;
    func_0017DD10(actor);CHECK(request_words[0]==110);CHECK(AT(actor,0x190,GeorgeActorBits64)==1ULL<<35);CLOSE(AT(actor,0x528,float),10/9.8f);
    reset();D_003F2D40=0;func_0017DD10(actor);CHECK(allocations==2&&registrations==1&&insertions==0);CHECK(registry_slots[0]==&record_storage);CHECK(D_0046A0F0.field04==registry_slots+1);
    reset();D_003F2D40=0;registry_mode=1;func_0017DD10(actor);CHECK(insertions==1&&registrations==1);
    reset();registry_mode=3;func_0017DD10(actor);CHECK(observed_effect==effect_storage&&D_003F2D40==replacement);
    reset();AT(actor,0x52C,signed char)=1;AT(actor,0x518,float)=30;func_0017DD10(actor);
    initial=sqrtf((g+g)*7);time=(initial+sqrtf(-((initial*initial)-(g+g)*30)))/g;CLOSE(AT(actor,0x528,float),time);
    reset();AT(actor,0x52C,signed char)=1;AT(actor,0x518,float)=nan32();func_0017DD10(actor);CHECK(isnan(AT(actor,0x528,float)));
    reset();AT(actor,0x528,float)=.05f;AT(actor,0x438,float)=.05f;AT(actor,0x52C,signed char)=1;predicate_result=1;
    func_0017E3A0(actor);CHECK(AT(actor,0x14,u32)==3&&predicates==1);CLOSE(AT(actor,0x438,float),-.05f);CLOSE(AT(actor,0x3C4,float),-2);
    reset();AT(actor,0x52C,signed char)=1;AT(actor,0x528,float)=.05f;AT(actor,0x190,GeorgeActorBits64)=0x80000ULL;
    func_0017E3A0(actor);CHECK(AT(actor,0x14,u32)==5&&predicates==0);
    reset();AT(actor,0x52C,signed char)=1;AT(actor,0x528,float)=nan32();func_0017E3A0(actor);CHECK(AT(actor,0x14,u32)==99);
    reset();AT(actor,0x52E,signed char)=1;AT(actor,0x524,float)=.4f;AT(actor,0x528,float)=1;AT(actor,0x52F,signed char)=1;
    AT(data[0],0x80,u32)=9;func_0017E3A0(actor);CHECK(AT(actor,0x52D,signed char)==1);CHECK(request_words[0]==15);CHECK(AT(actor,0x524,u32)==0);CHECK(AT(actor,0x52F,signed char)==0);CHECK(notifications==1);
    reset();AT(actor,0x528,float)=10;AT(actor,0x52F,signed char)=0;moving_result=1;vector_mode=4;func_0017E3A0(actor);CHECK(AT(actor,0x14,u32)==99);CHECK(words==1&&vectors==2);
    reset();AT(actor,0x528,float)=.05f;AT(actor,0x410,u32)=12;func_0017E3A0(actor);
    CHECK(flag_calls==1&&AT(actor,0x14,u32)==0);CLOSE(AT(actor,0x414,float),11);CLOSE(AT(actor,0x418,float),12);CLOSE(AT(actor,0x41C,float),13);CHECK(AT(actor,0x420,void*)==target_object);
}
static void initializer_tests(void) {
    s32 mode;
    for(mode=-1;mode<4;mode++) {reset();func_0017ED18(actor,mode);CHECK(effect_creates==1&&releases==1&&released_words[0]==0);CHECK(observed_effect_word==(mode==0?3U:mode==2?5U:4U));}
    reset();AT(actor,0x3DC,u32)=0;func_0017ED18(actor,0);CHECK(effect_creates==0&&matrix_calls==1);
    for(mode=-1;mode<3;mode++) {reset();AT(actor,0x3D0+4*mode,u32)=80+mode;AT(data[0],0x90,u32)=1;func_0017EE30(actor,mode);CHECK(effects==1&&observed_effect_word==(u32)(80+mode));CHECK(hashes==0);}
    reset();AT(actor,0x3D0,u32)=1;func_0017EE30(actor,0);CHECK(effects==0);
    reset();AT(actor,0x3C4,float)=-5;AT(actor,0x294,void*)=primary;AT(actor,0x2D8,u32)=1;AT(actor,0x300,u32)=1;release_mode=1;
    func_0017EF88(actor);CHECK(request_words[0]==24);CLOSE(request_times[0],2.5f);CHECK(cleanup_calls==1&&array_calls==3&&clear_calls==1);CHECK(observed_clear==replacement);CHECK(AT(actor,0x2D8,u32)==0);CHECK(stop_calls==1);CHECK(matrix_calls==1);
    reset();AT(actor,0x3C4,float)=-3;AT(actor,0x68,float)=0;func_0017EF88(actor);CHECK(request_words[0]==23);CHECK(cleanup_calls==0);CLOSE(observed_vector.x,1);CLOSE(observed_vector.y,0);CLOSE(observed_vector.z,3);
    reset();AT(actor,0x3C4,float)=-2;AT(actor,0x68,float)=0;func_0017EF88(actor);CHECK(request_words[0]==20);
    reset();AT(actor,0x68,float)=5;func_0017EF88(actor);CHECK(request_words[0]==22);
    reset();AT(actor,0x3C4,float)=nan32();func_0017EF88(actor);CHECK(request_words[0]==21);
    reset();AT(actor,0x3C4,float)=-5;release_mode=2;AT(data[1],0xA4,u32)=10;func_0017EF88(actor);CHECK(notifications==1);
    reset();AT(actor,0x3C4,float)=-5;AT(actor,0x294,void*)=primary;array_count=-1;func_0017EF88(actor);CHECK(array_calls==0&&clear_calls==1);
}
static void vehicle_tests(void) {
    static const s32 waiting[]={1,101,201,301,401,2001,2101,2201};
    static const s32 entry[]={0,-1,3,99,102,199,203,299,303,399,403,1999,2003,2099,2103,2199,2203,2299,2301,65536};
    static const s32 requests_phase[]={200,300,2000,2100,2200};
    static const s32 success_phase[]={201,301,2001,2101,2201};
    static const s32 failure_phase[]={300,400,2100,2200,2300};
    unsigned i;
    for(i=0;i<sizeof(waiting)/sizeof(waiting[0]);i++){reset();AT(actor,0x740,s32)=waiting[i];func_00183C58(actor);CHECK(AT(actor,0x740,s32)==waiting[i]);CHECK(outputs==3&&requests==0);CLOSE(AT(actor,0x744,float),.1f);CHECK(AT(actor,0x78,u32)==0);}
    for(i=0;i<sizeof(entry)/sizeof(entry[0]);i++){reset();AT(actor,0x740,s32)=entry[i];func_00183C58(actor);CHECK(AT(actor,0x740,u32)==1);CHECK(requests==2&&request_words[0]==116);CHECK(request_callbacks[1]==func_00194DE0);CLOSE(AT(actor,0x170,float),10);}
    for(i=0;i<5;i++) {
        reset();AT(actor,0x740,s32)=requests_phase[i];AT(actor,0x68,float)=0;if(requests_phase[i]==300)command110=1;
        func_00183C58(actor);CHECK(AT(actor,0x740,s32)==success_phase[i]);CHECK(requests==2);CLOSE(AT(actor,0x744,float),0);
        reset();AT(actor,0x740,s32)=requests_phase[i];AT(actor,0x68,float)=0;request_result=0;if(requests_phase[i]==300)command110=1;
        func_00183C58(actor);CHECK(AT(actor,0x740,s32)==failure_phase[i]);
    }
    reset();AT(actor,0x740,u32)=100;func_00183C58(actor);CHECK(AT(actor,0x740,u32)==200);
    reset();request_mode=1;output_mode=1;command_mode=1;func_00183C58(actor);CHECK(AT(actor,0x730,void*)==vehicles[1]);CHECK(request_objects[1]==replacement);CHECK(request_words[0]==120);CHECK(request_commands[1]==0);
    reset();AT(actor,0x740,u32)=202;AT(actor,0x170,float)=12;AT(actor,0x174,float)=24;AT(actor,0x178,float)=36;AT(actor,0x744,float)=.4f;
    func_00183C58(actor);CLOSE(AT(actor,0x40,float),11);CLOSE(AT(actor,0x44,float),22);CLOSE(AT(actor,0x48,float),33);CLOSE(AT(actor,0x74C,float),.4f);
    reset();AT(actor,0x740,u32)=202;AT(actor,0x748,float)=nan32();func_00183C58(actor);CHECK(AT(actor,0x740,u32)==202);CHECK(isnan(AT(actor,0x40,float)));
    reset();AT(actor,0x740,u32)=402;AT(actor,0x738,s32)=-65536;AT(actor,0x744,float)=.9f;AT(actor,0x754,u32)=1;
    func_00183C58(actor);CHECK(AT(actor,0x734,s32)==-65536&&AT(actor,0x740,u32)==2000);CLOSE(AT(actor,0x40,float),14);
    reset();AT(actor,0x740,u32)=400;AT(actor,0x738,s32)=-65536;AT(configuration,0x48,u32)=0;AT(configuration,0x4C,u32)=0;
    func_00183C58(actor);CHECK(AT(actor,0x734,s32)==-65536&&AT(actor,0x740,u32)==1000);
    reset();AT(actor,0x740,u32)=400;AT(actor,0x738,s32)=1;func_00183C58(actor);CHECK(request_words[0]==176&&AT(actor,0x740,u32)==401);
    reset();AT(actor,0x740,u32)=1000;func_00183C58(actor);CHECK(AT(actor,0x750,u32)==1);CHECK(request_words[0]==108);CLOSE(AT(primary,0x428,float),.5f);
    reset();AT(actor,0x740,u32)=1000;AT(configuration,0x60,float)=2;func_00183C58(actor);CHECK(AT(primary,0x428,u32)==0x3F7FBE77U);
    reset();AT(actor,0x740,u32)=1000;AT(configuration,0x60,float)=nan32();func_00183C58(actor);CHECK(AT(primary,0x428,u32)==0);
    reset();AT(actor,0x740,u32)=1000;command110=1;AT(actor,0x754,u32)=1;func_00183C58(actor);CHECK(AT(actor,0x750,u32)==0&&AT(actor,0x740,u32)==400);
    reset();AT(actor,0x740,u32)=2102;AT(actor,0x170,float)=20;AT(actor,0x174,float)=40;AT(actor,0x178,float)=60;AT(actor,0x744,float)=.9f;predicate_result=1;
    func_00183C58(actor);CHECK(AT(actor,0x14,u32)==3);CLOSE(AT(actor,0x40,float),20);CLOSE(AT(actor,0x74C,float),.2f);
    reset();AT(actor,0x740,u32)=2300;AT(actor,0x190,GeorgeActorBits64)=0x80ULL;func_00183C58(actor);CHECK(AT(actor,0x14,u32)==0&&predicates==0);
    reset();AT(actor,0x740,u32)=2300;predicate_result=1;vehicle_end=1;func_00183C58(actor);CHECK(AT(actor,0x14,s32)==-1);CHECK(AT(actor,0x3BC,u32)==0x3BAC798CU);
    reset();AT(actor,0x740,u32)=2;AT(actor,0x170,float)=12;AT(actor,0x174,float)=24;AT(actor,0x178,float)=36;
    func_00183C58(actor);CLOSE(AT(actor,0x40,float),12);CLOSE(AT(actor,0x74C,float),.1f);CHECK(soft_from==4&&soft_add==1&&soft_mul==2);
    reset();AT(actor,0x740,u32)=2;output_angles[0]=4;AT(actor,0x744,float)=.9f;func_00183C58(actor);
    CLOSE(AT(actor,0x74C,float),(float)((4.0-value(0x401921FB60000000ULL))*.1*5));CHECK(soft_sub==1&&AT(actor,0x740,u32)==100);
    reset();AT(actor,0x740,u32)=2;output_angles[0]=-4;func_00183C58(actor);CLOSE(AT(actor,0x74C,float),(float)((value(0x401921FB60000000ULL)-4.0)*.1*5));CHECK(soft_sub==3);
    reset();output_mode=2;func_00183C58(actor);CLOSE(AT(actor,0x170,float),50);
    reset();AT(actor,0x740,u32)=2;output_angles[0]=4;soft_mode=1;func_00183C58(actor);
    CLOSE(AT(actor,0x74C,float),(float)((1.0-value(0x401921FB60000000ULL))*.1*5));CHECK(soft_sub==1);
    reset();AT(actor,0x740,u32)=2;AT(actor,0x35C,float)=nan32();func_00183C58(actor);CHECK(AT(actor,0x740,u32)==2&&isnan(AT(actor,0x74C,float)));
}
int main(void) {
    jump_tests();initializer_tests();vehicle_tests();
    printf("actor_states2 semantic checks: %u passed\n",checks);return 0;
}
