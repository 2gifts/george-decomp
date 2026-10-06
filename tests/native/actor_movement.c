/* Independent finite host model and mutating callbacks; no original assets. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/actor_movement.c"

#define AT(p,n,t) (*(t *)((u8 *)(p) + (n)))
#define VEC(p,n) ((GeorgeMathVec3 *)((u8 *)(p)+(n)))
#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
#define CLOSE(a,b) CHECK(fabsf((a)-(b)) < 0.00003f)

GeorgeActorStateRecord D_003F83F0[42];
static unsigned checks, init_calls, teardown_calls, normalize_calls, trig_calls;
static unsigned ref_calls, predicate_calls, configure_calls, collision_calls, height_calls;
static unsigned notify_calls, release_calls, clear_calls, basis_calls, vector_calls, identity_calls;
static unsigned collection_calls, update_calls, matrix_calls, action_calls;
static union { GeorgeActorBits64 align; u8 bytes[0xA00]; } actor_store;
static union { GeorgeActorBits64 align; u8 bytes[0x500]; } data_store[2];
static GeorgeGoalEntity *actor;
static GeorgeGoalEntityData *data[2];
static u8 vehicle_store[2][0x200], vehicle_table[2][0x1C0];
static GeorgeGoalVirtualObject *vehicle[2];
static u8 object_store[0x120], object_table[0x100], reference_store[0x80];
static u8 array_store[2][0x30], collection_store[4][0x40], collection_table[4][0x60];
static GeorgeGoalVirtualObject *collection[4];
static u8 item_store[0x40], item_table[0x60], transform_store[0xB0], model_store[0x50];
static GeorgeGoalVirtualObject *item;
static GeorgeMathVec3 basis, first_vector, second_vector, motion_result, direction_result;
static float matrix[16], observed_scale, observed_float, observed_angle[2];
static GeorgeMathVec3 normalize_input[5];
static float normalize_scale[5];
static s32 observed_command[6], predicate_result[2];
static u32 init_state[8], teardown_state[8], observed_word;
static void *expected_this;
static GeorgeActorCollisionRecord *pending_records;
static s32 init_mode, teardown_mode, normalize_mode, mask_mode, ref_mode, vector_mode;
static s32 collision_mode, height_mode, trig_mode, collection_mode, reference_mode, notify_mode;
static u32 identities[2], collision_identity[2][2];
static s32 collision_count[2];
static u32 array_words[3];
static GeorgeMathVec3 *teardown_target;

static void initialized(void *pointer)
{
    if (expected_this != 0) CHECK(pointer == expected_this);
    init_state[init_calls++] = actor->field0C;
    if (init_mode == 1) actor->field0C = 2;
    if (init_mode == 2) actor->field0C = 14;
}
static void torn_down(void *pointer)
{
    if (expected_this != 0) CHECK(pointer == expected_this);
    teardown_state[teardown_calls++] = actor->field0C;
    if (teardown_mode == 1) actor->field0C = 2;
    if (teardown_mode == 2 && teardown_target != 0)
        teardown_target->x = 31, teardown_target->y = 32, teardown_target->z = 33;
    if (teardown_mode == 3) AT(actor,0x568,u32) = 0x7788;
}
static void install_initializer(u32 state)
{
    D_003F83F0[state].field00.selector = -1;
    D_003F83F0[state].field00.target.direct = initialized;
}
static void install_teardown(u32 state)
{
    D_003F83F0[state].field08.selector = -1;
    D_003F83F0[state].field08.target.direct = torn_down;
}
static const GeorgeMathVec3 *basis_callback(void *pointer)
{
    CHECK(pointer == vehicle[0] || pointer == vehicle[1]);
    ++basis_calls; return (const GeorgeMathVec3 *)matrix;
}
static const GeorgeMathVec3 *vector_callback(void *pointer)
{
    CHECK(pointer == vehicle[0] || pointer == vehicle[1]);
    if (++vector_calls == 1) return &first_vector;
    if (vector_mode == 1) first_vector.x = 0, first_vector.y = 0, first_vector.z = 0;
    return &second_vector;
}
static const GeorgeMathVec3 *project_callback(void *pointer)
{
    CHECK(pointer == object_store);
    ++basis_calls; return &basis;
}
static s32 predicate_callback(void *pointer, s32 command)
{
    CHECK(pointer == vehicle[0]);
    observed_command[predicate_calls] = command;
    return predicate_result[predicate_calls++];
}
static void configure_callback(void *pointer, GeorgeGoalEntity *entity, s32 command)
{
    CHECK(pointer == vehicle[ref_mode == 1]); CHECK(entity == actor);
    observed_command[4 + configure_calls++] = command;
}
static void command_callback(void *pointer, s32 command, GeorgeMathVec3 *output, GeorgeMathVec3 *scratch)
{
    CHECK(pointer == vehicle[0] || pointer == vehicle[1]); CHECK(scratch == 0);
    observed_command[action_calls++] = command;
    output->x = 10 + command; output->y = 20; output->z = 30;
}
static s32 identity_callback(void *pointer)
{
    unsigned index = pointer == vehicle[1];
    CHECK(pointer == vehicle[index]);
    ++identity_calls;
    if (collision_mode == 2 && pending_records != 0) pending_records[0].field00 = identities[index];
    return (s32)identities[index];
}
static void collection_begin(void *pointer, float value)
{
    CHECK(pointer == collection[0] || pointer == collection[1] || pointer == collection[2]);
    CLOSE(value, 0.375f); ++collection_calls;
    if (collection_mode == 1 && pointer == collection[0]) AT(actor,0x29C,void *) = collection[3];
}
static s32 collection_count(void *pointer)
{
    CHECK(pointer == collection[0] || pointer == collection[1] || pointer == collection[2] || pointer == collection[3]);
    return pointer == collection[3] || (pointer == collection[0] && collection_mode == 2) ? 2 : 1;
}
static GeorgeGoalVirtualObject *collection_entry(void *pointer, u32 index)
{
    CHECK(pointer == collection[0] || pointer == collection[1] || pointer == collection[2] || pointer == collection[3]);
    CHECK(index < (pointer == collection[3] || collection_mode == 2 ? 2U : 1U));
    return item;
}
static void item_update(void *pointer, float value)
{
    CHECK(pointer == item); CLOSE(value,0.375f); ++update_calls;
    if (collection_mode == 2 && update_calls == 1) AT(actor,0x29C,void *) = collection[3];
}
static void reset(void)
{
    unsigned i;
    memset(&actor_store,0,sizeof(actor_store)); memset(data_store,0,sizeof(data_store));
    memset(D_003F83F0,0,sizeof(D_003F83F0)); memset(vehicle_store,0,sizeof(vehicle_store));
    memset(vehicle_table,0,sizeof(vehicle_table)); memset(object_store,0,sizeof(object_store));
    memset(object_table,0,sizeof(object_table)); memset(reference_store,0,sizeof(reference_store));
    memset(array_store,0,sizeof(array_store)); memset(collection_store,0,sizeof(collection_store));
    memset(collection_table,0,sizeof(collection_table)); memset(item_store,0,sizeof(item_store));
    memset(item_table,0,sizeof(item_table)); memset(transform_store,0,sizeof(transform_store));
    memset(model_store,0,sizeof(model_store)); memset(matrix,0,sizeof(matrix));
    memset(init_state,0,sizeof(init_state)); memset(teardown_state,0,sizeof(teardown_state));
    memset(predicate_result,0,sizeof(predicate_result)); memset(observed_command,0,sizeof(observed_command));
    memset(collision_identity,0,sizeof(collision_identity)); memset(collision_count,0,sizeof(collision_count));
    actor = (GeorgeGoalEntity *)actor_store.bytes;
    for(i=0;i<2;++i) {
        data[i] = (GeorgeGoalEntityData *)data_store[i].bytes;
        AT(data[i],0x10,float)=2; AT(data[i],0x14,float)=0.5f;
        AT(data[i],0x20,float)=0.25f; AT(data[i],0x24,float)=0.5f;
        AT(data[i],0x28,float)=0.8f; AT(data[i],0x2C,float)=0.5f;
        AT(data[i],0x30,float)=0.3f; AT(data[i],0x34,float)=0.4f;
        vehicle[i]=(GeorgeGoalVirtualObject *)(vehicle_store[i]+16); vehicle[i]->field04=vehicle_table[i];
        AT(vehicle_table[i],0x38,GeorgeGoalVirtualVector).invoke=vector_callback;
        AT(vehicle_table[i],0x98,GeorgeGoalVirtualVector).invoke=basis_callback;
        AT(vehicle_table[i],0xA0,GeorgeGoalVirtualInt).invoke=identity_callback;
        AT(vehicle_table[i],0xD8,GeorgeActorVirtualCommandVector).invoke=command_callback;
        AT(vehicle_table[i],0x1B8,GeorgeGoalVirtualCommand).invoke=predicate_callback;
        AT(vehicle_table[i],0x1A8,GeorgeActorVirtualConfigure).invoke=configure_callback;
    }
    actor->field18=data[0];
    AT(actor,0x730,void *)=vehicle[0]; AT(actor,0x20,void *)=object_store;
    AT(actor,0xB0,float)=1; AT(actor,0xC4,float)=1; actor->fieldD0.z=1;
    AT(object_store,0,const u8 *)=object_table;
    AT(object_table,0x58,GeorgeGoalVirtualVector).invoke=project_callback;
    basis.x=1; basis.y=basis.z=0;
    first_vector.x=1; first_vector.y=first_vector.z=0;
    second_vector.y=1; second_vector.x=second_vector.z=0;
    matrix[5]=1;
    identities[0]=100; identities[1]=200;
    for(i=0;i<4;++i) {
        collection[i]=(GeorgeGoalVirtualObject *)collection_store[i]; collection[i]->field04=collection_table[i];
        AT(collection_table[i],0x28,GeorgeGoalVirtualFloat).invoke=collection_begin;
        AT(collection_table[i],0x38,GeorgeGoalVirtualInt).invoke=collection_count;
        AT(collection_table[i],0x40,GeorgeActorVirtualIndex).invoke=collection_entry;
    }
    item=(GeorgeGoalVirtualObject *)item_store;item->field04=item_table;
    AT(item_table,0x48,GeorgeGoalVirtualFloat).invoke=item_update;
    init_calls=teardown_calls=normalize_calls=trig_calls=ref_calls=predicate_calls=configure_calls=0;
    collision_calls=height_calls=notify_calls=release_calls=clear_calls=basis_calls=vector_calls=identity_calls=0;
    collection_calls=update_calls=matrix_calls=action_calls=0;
    init_mode=teardown_mode=normalize_mode=mask_mode=ref_mode=vector_mode=0;
    collision_mode=height_mode=trig_mode=collection_mode=reference_mode=notify_mode=0;
    expected_this=0; pending_records=0; teardown_target=0;
    array_words[0]=0x11;array_words[1]=0;array_words[2]=0x33;
}

void *func_00239FD8(u32 word) { observed_word=word;return reference_store; }
void *func_00238BA0(void *object,u32 key) { CHECK(object==reference_store);CHECK(key==0xDE01EBFEU);return reference_store+16; }
void func_001A5E90(void *object,void *value) { CHECK(object==object_store);CHECK(value==reference_store+16);CHECK(AT(object,0x34,u32)==0);++action_calls; }
void func_001F6138(GeorgeGoalVirtualObject *object) { CHECK(object==vehicle[0]);++ref_calls;if(ref_mode==1)AT(actor,0x730,void *)=vehicle[1]; }
s32 func_00120EA0(void *reference,s32 word,GeorgeGoalEntity *entity) { CHECK(entity==actor);CHECK(reference!=0);observed_word=(u32)word;return reference_mode!=1; }
void func_00121A80(void *reference,GeorgeMathVec3 *position,GeorgeMathVec3 *direction) { CHECK(reference!=0);position->x=7;position->y=8;position->z=9;direction->x=4;direction->y=5;direction->z=6; }
float func_0029B940(float first,float second) { CLOSE(first,6);CLOSE(second,4);return 1.25f; }
float func_0029C168(float value) { observed_angle[trig_calls++]=value;if(trig_mode==1)AT(actor,0x58,float)=0.5f;return 1; }
float func_0029C090(float value) { observed_angle[trig_calls++]=value;return 0; }
GeorgeActorBits64 func_00374848(float value) { union {double value;GeorgeActorBits64 bits;} u;u.value=value;return u.bits; }
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first,GeorgeActorBits64 second) { union {double value;GeorgeActorBits64 bits;} a,b;a.bits=first;b.bits=second;a.value-=b.value;return a.bits; }
s32 func_00373250(GeorgeActorBits64 first,GeorgeActorBits64 second) { union {double value;GeorgeActorBits64 bits;} a,b;a.bits=first;b.bits=second;return a.value<b.value?-1:a.value>b.value?1:0; }
float func_00192DA8(GeorgeGoalEntity *entity) { CHECK(entity==actor);++height_calls;if(height_mode==1&&height_calls==1)AT(actor,0x734,s32)=3;return 2; }
s32 func_001F6308(s32 command) { return command==1?0:command==2?3:command==3?2:1; }
s32 func_0022BCB0(const GeorgeMathVec3 *start,const GeorgeMathVec3 *end,s32 mode,GeorgeActorCollisionRecord *records,s32 capacity)
{
    unsigned index=collision_calls++;
    CHECK(index<2); CHECK(mode==18||mode==19);CHECK(capacity==2);
    CLOSE(start->y,AT(actor,0x44,float)+2);CLOSE(end->y,22);
    records[0].field00=collision_identity[index][0];records[1].field00=collision_identity[index][1];
    pending_records=records;return collision_count[index];
}
void func_0018FD30(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) { AT(entity,0x190,GeorgeActorBits64)|=mask;if(mask_mode==1)entity->field0C=23;if(mask_mode==2)AT(entity,0xA0,u32)=3; }
void func_0018FD40(GeorgeGoalEntity *entity,GeorgeActorBits64 mask) { AT(entity,0x190,GeorgeActorBits64)&=~mask;if(mask_mode==1)entity->field0C=23;if(mask_mode==2)AT(entity,0xA0,u32)=3; }
void func_0018D8A0(GeorgeGoalEntity *entity,const GeorgeMathVec3 *vector) { CHECK(entity==actor);motion_result=*vector;++action_calls; }
void func_00235CD8(void *object,u32 key,u32 value) { CHECK(object!=0);CHECK(key==0xB95616B6U||key==0x9F79558FU);CHECK(value==1);++notify_calls;if(notify_mode==1)AT(actor,0x294,void *)=array_store[1]; }
u32 func_002AAF88(const void *array) { CHECK(array==array_store[0]||array==array_store[1]);return 3; }
u32 *func_002AAF50(void *array,s32 index) { CHECK(array==array_store[release_calls?1:0]);CHECK(index>=0&&index<3);return array_words+index; }
void func_002AAFD8(void *array) { CHECK(array==array_store[1]);++clear_calls; }
void func_002393F8(u32 word) { CHECK(word==0x11||word==0x33);++release_calls;AT(actor,0x294,void *)=array_store[1]; }
void func_00272A58(void *object) { CHECK(object==object_store);++action_calls; }
void func_00196980(GeorgeGoalEntity *entity) { CHECK(entity==actor);++action_calls; }
void func_00192078(GeorgeGoalEntity *entity,u32 key,float adjustment) { CHECK(entity==actor);CHECK(key==0xEC789588U);observed_float=adjustment;++action_calls; }
void func_0018B710(GeorgeGoalEntity *entity,const GeorgeMathVec3 *direction,const GeorgeMathVec3 *position) { CHECK(entity==actor);direction_result=*direction;motion_result=*position;++action_calls; }
void func_00191DC8(GeorgeGoalEntity *entity,u32 word) { CHECK(entity==actor);observed_word=word;AT(actor,0x298,void *)=transform_store; }
void *func_00192748(GeorgeGoalEntity *entity,u32 index) { CHECK(entity==actor);CHECK(index==41);return model_store; }
void func_002A1C60(const void *matrix_input,const GeorgeMathVec3 *input,GeorgeMathVec3 *output) { CHECK(matrix_input==actor_store.bytes+0xF0);CHECK(input==VEC(model_store,0x30));output->x=10;output->y=20;output->z=30;actor->field18=data[1];++matrix_calls; }
void func_002A1C30(float *output) { unsigned i;for(i=0;i<16;++i)output[i]=(i%5==0)?1:0; }
void func_002A1C08(void *output,const void *input) { CHECK(output==transform_store+0x10);memcpy(output,input,64); }
float func_002A35C0(GeorgeMathVec3 *output,const GeorgeMathVec3 *input,float scale)
{
    float x=input->x,length=sqrtf((x*x+input->y*input->y)+input->z*input->z);
    CHECK(normalize_calls<5);normalize_input[normalize_calls]=*input;normalize_scale[normalize_calls++]=scale;
    if(length==0) {output->z=0;output->x=scale;output->y=0;}
    else {float factor=scale/length;output->x=x*factor;output->y=input->y*factor;output->z=input->z*factor;}
    observed_scale=scale;
    if(normalize_mode==1&&normalize_calls==1)actor->field18=data[1];
    if(normalize_mode==2&&normalize_calls==2)actor->field18=data[1];
    if(normalize_mode==3&&normalize_calls==1)AT(actor,0x58,float)=0.75f;
    return length;
}

static void test_states(void)
{
    GeorgeGoalVirtualVoid table[1];
    reset();actor->field0C=4;install_teardown(4);teardown_mode=1;
    func_00170538(actor);CHECK(teardown_calls==1);CHECK(actor->field0C==0xFFFFFFFFU);CHECK(AT(actor,0x10,u32)==2);
    reset();actor->field0C=7;func_00170538(actor);CHECK(teardown_calls==0);CHECK(AT(actor,0x10,u32)==7);
    reset();actor->field0C=4;D_003F83F0[4].field08.selector=1;D_003F83F0[4].field08.target.vtable_offset=0x80;
    D_003F83F0[4].field08.adjustment=32760;AT(actor,0x80,void *)=table;table[0].adjustment=32760;table[0].invoke=torn_down;
    expected_this=(u8 *)((u32)actor+65520U);func_00170538(actor);CHECK(teardown_calls==1);CHECK(AT(actor,0x10,u32)==4);
    reset();actor->field0C=4;D_003F83F0[4].field08.selector=1;D_003F83F0[4].field08.target.vtable_offset=0x80;
    D_003F83F0[4].field08.adjustment=-32760;AT(actor,0x80,void *)=table;table[0].adjustment=-32760;table[0].invoke=torn_down;
    expected_this=(u8 *)((u32)actor-65520U);func_00170538(actor);CHECK(teardown_calls==1);CHECK(AT(actor,0x10,u32)==4);
    reset();install_initializer(7);CHECK(func_00173648(actor)==1);CHECK(init_state[0]==7);CHECK(actor->field0C==7);
    reset();actor->field0C=3;CHECK(func_00173648(actor)==0);CHECK(actor->field0C==3);
    reset();actor->field0C=23;install_initializer(8);AT(actor,0x45C,u32)=9;CHECK(func_00173720(actor)==1);CHECK(AT(actor,0x45C,u32)==0);
    reset();actor->field0C=12;install_teardown(12);install_teardown(0);install_initializer(0);install_initializer(35);
    CHECK(func_00173818(actor)==1);CHECK(teardown_calls==2);CHECK(init_calls==2);CHECK(init_state[0]==0&&init_state[1]==35);
    reset();actor->field0C=1;CHECK(func_00173818(actor)==0);
    reset();AT(actor,0x190,GeorgeActorBits64)=0xA500000000000001ULL;actor->field0C=9;
    CHECK(func_00173B20(actor,1)==1);CHECK(actor->field0C==9);CHECK(func_00173B20(actor,0)==0);CHECK(AT(actor,0x190,GeorgeActorBits64)==0xA500000000000000ULL);
    reset();install_initializer(34);CHECK(func_00173B20(actor,-5)==1);CHECK(actor->field0C==34);CHECK(AT(actor,0x190,GeorgeActorBits64)==1);
    reset();actor->field0C=0;func_00177B80(actor);CHECK(init_calls==0);
    reset();actor->field0C=2;install_initializer(0);func_00177B80(actor);CHECK(actor->field0C==0);CHECK(init_calls==1);
}

static void test_jump_setup(void)
{
    unsigned state;
    GeorgeMathVec3 target={1,2,3};
    for(state=0;state<28;++state) {
        s32 expected=(state==0||state==2||state==4||state==12||state==26||state==25);
        reset();actor->field0C=state;AT(actor,0x880,u32)=200;AT(data[0],0x3C,float)=7;AT(data[0],0x248,float)=9;
        CHECK(func_00176F58(actor)==expected);
        if(expected&&state!=4) {CHECK(actor->field0C==4);CLOSE(AT(actor,0x534,float),state==25?9:7);}
        if(state==4)CHECK(AT(actor,0x52E,u8)==1);
    }
    reset();actor->field0C=25;AT(actor,0x880,u32)=199;CHECK(func_00176F58(actor)==0);
    reset();actor->field2D8=7;CHECK(func_00176F58(actor)==0);
    reset();install_teardown(0);teardown_mode=2;teardown_target=&target;AT(data[0],0x3C,float)=0.75f;
    install_initializer(4);CHECK(func_00177270(actor,&target,0,8)==1);CLOSE(AT(actor,0x514,float),31);CLOSE(AT(actor,0x518,float),32);CLOSE(AT(actor,0x51C,float),33);
    CLOSE(AT(actor,0x520,float),8);CLOSE(AT(actor,0x534,float),0.75f);CHECK(actor->field0C==4&&AT(actor,0x52C,u8)==1);
    reset();AT(data[0],0x1D8,u32)=0x45A78000U;AT(object_store,0x34,u32)=9;target.x=1;target.y=2;target.z=3;
    CHECK(func_00177270(actor,&target,0xABC,6)==1);CHECK(observed_word==0xABC);CHECK(action_calls==1);
    reset();*VEC(actor,0x510)=target;CHECK(func_00177270(actor,VEC(actor,0x510),0,5)==1);
    CLOSE(AT(actor,0x514,float),1);CLOSE(AT(actor,0x518,float),1);CLOSE(AT(actor,0x51C,float),1);
    reset();actor->field0C=4;AT(actor,0x4C,float)=3;AT(actor,0x54,float)=4;AT(data[0],0x4C,float)=6;CHECK(func_001739A0(actor,&target)==0);
    AT(data[0],0x4C,float)=5;CHECK(func_001739A0(actor,&target)==1);CHECK(actor->field0C==33);CLOSE(AT(actor,0x434,float),3);
    reset();actor->field0C=4;AT(actor,0x4C,float)=3;AT(actor,0x54,float)=4;*VEC(actor,0x42C)=target;CHECK(func_001739A0(actor,&target)==0);
    target.y=NAN;CHECK(func_001739A0(actor,&target)==1);CHECK(isnan(AT(actor,0x430,float)));
}

static void test_vehicle_setup(void)
{
    unsigned first,second;
    for(first=0;first<4;++first)for(second=0;second<4;++second) {
        s32 expected=!((second<2&&first>=2)||(second>=2&&first<2));
        reset();CHECK(func_001773F0(actor,(u32)vehicle[0],first,second)==expected);
        if(expected){CHECK(actor->field0C==14);CHECK(AT(actor,0x734,u32)==first);CHECK(AT(actor,0x738,u32)==second);CHECK(predicate_calls==2);}
        else CHECK(predicate_calls==0);
    }
    reset();ref_mode=1;CHECK(func_001773F0(actor,(u32)vehicle[0],0x80,0x81)==1);
    CHECK(observed_command[0]==-128);CHECK(observed_command[1]==-127);CHECK(observed_command[4]==-127);CHECK(AT(actor,0x734,u32)==0x80&&AT(actor,0x738,u32)==0x81);
    reset();predicate_result[0]=1;CHECK(func_001773F0(actor,(u32)vehicle[0],0,1)==0);CHECK(predicate_calls==1&&ref_calls==0);
    reset();ref_mode=1;CHECK(func_001778C8(actor,(u32)vehicle[0],0x1FF)==1);CHECK(observed_command[4]==-1);CHECK(AT(actor,0x738,u32)==0x1FF);CHECK(AT(actor,0x734,u32)==0x1FF&&AT(actor,0x73C,u32)==1);
    reset();actor->field0C=14;CHECK(func_001778C8(actor,(u32)vehicle[0],1)==0);CHECK(ref_calls==0);
    reset();actor->field0C=14;install_initializer(13);CHECK(func_00176E10(actor)==1);CHECK(AT(actor,0x724,void *)==vehicle[0]);CHECK(ref_calls==1);
    CHECK(AT(actor,0x448,u32)==0&&AT(actor,0x450,u32)==0&&AT(actor,0x444,u32)==0);
    reset();actor->field0C=14;AT(actor,0x190,GeorgeActorBits64)=0x20;CHECK(func_00176E10(actor)==0);CHECK(ref_calls==0&&actor->field0C==14);
    reset();actor->field0C=2;AT(actor,0x3BC,u32)=0x7F9000CFU;CHECK(func_00176E10(actor)==0);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;AT(actor,0x734,u32)=1;
    CHECK(func_001775A8(actor,-1)==1);CHECK(observed_command[0]==1);CHECK(AT(actor,0x738,u32)==1);CHECK(height_calls==2&&collision_calls==1);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;matrix[5]=-1;height_mode=1;
    collision_count[0]=1;collision_identity[0][0]=999;collision_count[1]=1;collision_identity[1][0]=100;
    CHECK(func_001775A8(actor,-1)==1);CHECK(observed_command[0]==3&&observed_command[1]==2);CHECK(AT(actor,0x738,u32)==2);CHECK(collision_calls==2&&identity_calls==2);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;collision_count[0]=1;collision_count[1]=1;collision_identity[0][0]=999;collision_identity[1][0]=999;
    CHECK(func_001775A8(actor,0)==0);CHECK(AT(actor,0x754,u32)==0&&collision_calls==2);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;collision_count[0]=1;collision_identity[0][0]=999;collision_mode=2;
    CHECK(func_001775A8(actor,0)==1);CHECK(collision_calls==1&&identity_calls==1);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;collision_count[0]=2;collision_identity[0][0]=999;collision_identity[0][1]=100;
    CHECK(func_001775A8(actor,0)==1);CHECK(collision_calls==2&&identity_calls==2);CHECK(AT(actor,0x738,u32)==1);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;vector_mode=1;first_vector.x=0;first_vector.y=1;
    CHECK(func_001775A8(actor,0)==1);CHECK(vector_calls==2&&collision_calls==1);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;matrix[5]=0.949999988079071f;CHECK(func_001775A8(actor,0)==0);CHECK(height_calls==0);
    reset();actor->field0C=14;AT(actor,0x750,u32)=1;first_vector.x=NAN;CHECK(func_001775A8(actor,0)==0);CHECK(basis_calls==0);
}

static void test_reference_and_idle(void)
{
    unsigned state;
    reset();AT(reference_store,0x2C,u32)=11;AT(reference_store,0x30,u32)=12;AT(reference_store,0x34,u32)=13;AT(reference_store,0x38,u32)=14;
    CHECK(func_001779F8(actor,reference_store,-9)==1);CHECK(observed_word==(u32)-9);CHECK(AT(actor,0x56C,u32)==11&&AT(actor,0x578,u32)==14);CLOSE(AT(actor,0x57C,float),1.25f);CHECK(AT(actor,0x580,void *)==reference_store&&actor->field0C==6);
    reset();install_teardown(0);teardown_mode=3;AT(actor,0x568,u32)=1;AT(actor,0x56C,u32)=2;AT(actor,0x570,u32)=3;AT(actor,0x574,u32)=4;
    CHECK(func_001779F8(actor,actor_store.bytes+0x53C,0)==1);CHECK(AT(actor,0x56C,u32)==0x7788&&AT(actor,0x570,u32)==0x7788&&AT(actor,0x578,u32)==0x7788);
    reset();CHECK(func_001779F8(actor,0,0)==0);actor->field2D8=4;CHECK(func_001779F8(actor,reference_store,0)==0);
    for(state=0;state<16;++state) {
        s32 expected=!(state==1||state==3||state==6||state==11||state==12||state==13||state==14);
        reset();actor->field0C=state;CHECK(func_00177C40(actor,11,22,33,44,55,0,0.75f)==expected);
        if(expected){CHECK(actor->field0C==12);CHECK(AT(actor,0x700,u32)==11&&AT(actor,0x704,u32)==22&&AT(actor,0x708,u32)==33);CHECK(AT(actor,0x70C,u32)==44&&AT(actor,0x71C,u32)==55);CLOSE(AT(actor,0x720,float),0.75f);}
    }
    reset();actor->field0C=12;AT(actor,0x23C,void *)=object_store;AT(actor,0x294,void *)=array_store[0];actor->field2D8=1;AT(actor,0x1B0,void *)=object_store;AT(actor,0x300,u32)=1;
    CHECK(func_00177C40(actor,1,2,3,4,5,0x80000000U,0.25f)==1);CHECK(release_calls==2&&clear_calls==1&&notify_calls==1);CHECK(actor->field2D8==0&&action_calls==2);
    reset();actor->field0C=1;CHECK(func_00177C40(actor,1,2,3,4,5,1,0)==0);
}

static void test_movement(void)
{
    unsigned state;
    unsigned i;
    static const float projections[6]={0.3f,-0.3f,0.30000004f,-0.30000004f,0.25f,-0.25f};
    GeorgeMathVec3 input={1,2,3};
    for(state=0;state<43;++state) {
        reset();actor->field0C=state;AT(data[0],0x1D4,u32)=1;AT(actor,0x190,GeorgeActorBits64)=0xFEDC000000001000ULL;
        if(state==3||state==4||state==27||state==29||state==30) {
            CHECK(func_00177E48(actor,&input)==1);CLOSE(AT(actor,0x68,float),2);CLOSE(AT(actor,0x6C,float),4);CLOSE(AT(actor,0x70,float),6);CHECK(AT(actor,0x190,GeorgeActorBits64)==0xFEDC000000000000ULL);
        } else if(state==10||state==14||state==26||state==41) {
            CHECK(func_00177E48(actor,&input)==1);CLOSE(AT(actor,0x68,float),1);CLOSE(AT(actor,0x70,float),3);
        } else if(state==31) {
            CHECK(func_00177E48(actor,&input)==1);CLOSE(motion_result.x,2);CLOSE(motion_result.y,4);CLOSE(motion_result.z,6);
        } else if(state==0||state==2||state==5||state==12||state==23||state==32||state==42) {
            CHECK(func_00177E48(actor,&input)==1);CHECK(normalize_calls>=1);
        } else {CHECK(func_00177E48(actor,&input)==0);CHECK(AT(actor,0x68,u32)==0&&AT(actor,0x6C,u32)==0&&AT(actor,0x70,u32)==0);}
    }
    reset();actor->field0C=3;*VEC(actor,0x64)=input;CHECK(func_00177E48(actor,VEC(actor,0x64))==1);CLOSE(AT(actor,0x68,float),2);CLOSE(AT(actor,0x6C,float),4);CLOSE(AT(actor,0x70,float),8);
    reset();actor->field0C=14;*VEC(actor,0x64)=input;CHECK(func_00177E48(actor,VEC(actor,0x64))==1);CLOSE(AT(actor,0x68,float),1);CLOSE(AT(actor,0x6C,float),1);CLOSE(AT(actor,0x70,float),1);
    reset();AT(actor,0x438,float)=0.001f;CHECK(func_00177E48(actor,&input)==0);CHECK(normalize_calls==0);
    reset();AT(actor,0x438,float)=NAN;actor->field0C=14;CHECK(func_00177E48(actor,&input)==1);
    reset();actor->field0C=31;input.x=0;input.y=3;input.z=4;CHECK(func_00177E48(actor,&input)==1);CLOSE(motion_result.y,6);CLOSE(motion_result.z,8);CLOSE(normalize_scale[0],10);
    reset();actor->field0C=31;AT(data[0],0x1D8,u32)=0x45A78000U;input.x=0.5f;input.y=0;input.z=0;CHECK(func_00177E48(actor,&input)==1);CLOSE(motion_result.x,1);CHECK(basis_calls==1);
    reset();actor->field0C=31;AT(data[0],0x1D8,u32)=0x45A78000U;input.x=0.25f;CHECK(func_00177E48(actor,&input)==1);CLOSE(motion_result.x,0.125f);
    for(i=0;i<6;++i) {
        float factor=projections[i]>0.3f?1:projections[i]<-0.3f?-1:projections[i];
        reset();actor->field0C=31;AT(data[0],0x1D8,u32)=0x45A78000U;AT(data[0],0x1E0,float)=1;
        input.x=projections[i];input.y=0;input.z=0;
        CHECK(func_00177E48(actor,&input)==1);CLOSE(motion_result.x,factor*fabsf(input.x)*2);CHECK(basis_calls==1);
    }
    reset();AT(data[0],0x1D8,u32)=0x45A78000U;AT(data[0],0x1D4,u32)=1;input.x=0.25f;input.y=0;input.z=0;
    CHECK(func_00177E48(actor,&input)==1);CLOSE(normalize_input[0].x,0.0625f);CHECK(basis_calls==1);
    reset();AT(data[0],0x1D8,u32)=0x45A78000U;AT(data[0],0x1E0,float)=1;AT(data[0],0x1D4,u32)=1;
    CHECK(func_00177E48(actor,&input)==1);CLOSE(normalize_input[0].x,0.25f);CHECK(basis_calls==0);
    reset();AT(data[0],0x1D8,u32)=0x45A78000U;AT(data[0],0x1E0,float)=NAN;AT(data[0],0x1D4,u32)=1;
    CHECK(func_00177E48(actor,&input)==1);CHECK(basis_calls==0);
    reset();normalize_mode=1;AT(data[1],0x1D4,u32)=1;input.x=0;input.y=0;input.z=0.3f;
    CHECK(func_00177E48(actor,&input)==1);CHECK(normalize_calls==2);CHECK(actor->field18==data[1]);
    reset();AT(data[0],0x1D4,u32)=1;input.x=0.4f;input.y=0.5f;input.z=-0.6f;
    CHECK(func_00177E48(actor,&input)==1);CLOSE(normalize_scale[0],2*sqrtf((0.25f*0.6f)*0.6f+(0.5f*0.4f)*0.4f+0.25f));
    reset();AT(data[0],0x1D4,u32)=1;input.x=0.4f;input.y=0;input.z=0.8f;mask_mode=1;
    CHECK(func_00177E48(actor,&input)==1);CHECK(actor->field0C==23&&AT(actor,0x74,u32)==0);CHECK(normalize_calls==1);
    reset();AT(data[0],0x1D4,u32)=1;mask_mode=2;CHECK(func_00177E48(actor,&input)==1);CHECK(AT(actor,0xA0,u32)==3&&AT(actor,0x74,u32)==0);
    reset();input.x=0.05f;input.y=0;input.z=0;CHECK(func_00177E48(actor,&input)==1);CHECK(AT(actor,0x68,u32)==0&&AT(actor,0x70,u32)==0);
    reset();input.x=0;input.y=0;input.z=-0.6f;normalize_mode=2;AT(data[1],0x2C,float)=0.99f;CHECK(func_00177E48(actor,&input)==1);CHECK(normalize_calls==3);CHECK(AT(actor,0x190,GeorgeActorBits64)&0x1000ULL);
    reset();AT(actor,0x68,float)=1;trig_mode=1;AT(actor,0x58,float)=0.25f;CHECK(func_00179070(actor,0.8f)==0);CLOSE(observed_angle[0],0.25f);CLOSE(observed_angle[1],0.5f);
    reset();AT(actor,0x68,float)=-1;CHECK(func_00179070(actor,0.8f)==1);
    reset();AT(actor,0x70,float)=-1;CHECK(func_00179070(actor,0.8f)==2);
    reset();AT(actor,0x70,float)=1;CHECK(func_00179070(actor,0.8f)==3);
    reset();AT(actor,0x68,float)=NAN;CHECK(func_00179070(actor,0.8f)==3);
}

static void test_collections_and_geometry(void)
{
    reset();AT(actor,0x29C,void *)=collection[0];AT(actor,0x2A0,void *)=collection[1];AT(actor,0x2A4,void *)=collection[2];collection_mode=1;
    func_001765D0(actor,0.375f);CHECK(collection_calls==3&&update_calls==4);
    reset();func_001765D0(actor,0.375f);CHECK(collection_calls==0&&update_calls==0);
    reset();AT(actor,0x29C,void *)=collection[0];collection_mode=2;
    func_001765D0(actor,0.375f);CHECK(collection_calls==1&&update_calls==2);CHECK(AT(actor,0x29C,void *)==collection[3]);
    reset();actor->fieldD0.x=1;actor->fieldD0.y=2;actor->fieldD0.z=3;AT(actor,0x4C,float)=1;AT(actor,0x40,float)=5;AT(actor,0x44,float)=6;AT(actor,0x48,float)=7;
    func_00174770(actor,0,0.625f);CLOSE(observed_float,0.625f);CLOSE(direction_result.x,-1);CLOSE(direction_result.y,-3);CLOSE(direction_result.z,-3);CLOSE(motion_result.x,9);CLOSE(motion_result.y,6);CLOSE(motion_result.z,7);
    reset();actor->field0C=13;func_00174770(actor,1,8);CHECK(action_calls==0);
    reset();AT(data[0],0x11C,u32)=0;func_00177160(actor);CHECK(matrix_calls==0);
    reset();AT(data[0],0x11C,u32)=0x1234;AT(data[1],0x124,float)=2;AT(data[1],0x120,float)=3;AT(actor,0x110,float)=1;AT(actor,0x114,float)=2;AT(actor,0x118,float)=3;
    func_00177160(actor);CHECK(observed_word==0x1234);CHECK(matrix_calls==1&&notify_calls==1);CLOSE(AT(transform_store,0x40,float),12);CLOSE(AT(transform_store,0x44,float),27);CLOSE(AT(transform_store,0x48,float),36);CHECK(AT(transform_store,0xA0,u32)==0x100);
}

int main(void)
{
    test_states();test_jump_setup();test_vehicle_setup();test_reference_and_idle();test_movement();test_collections_and_geometry();
    printf("actor_movement: %u checks passed\n",checks);return 0;
}
