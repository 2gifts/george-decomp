/* Asset-free caller observations of the complete actor+0x2D8 sub-state family.
 * Retail assets and original machine code are not linked into this harness. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_substates.c"

typedef union Storage { unsigned long long alignment; u8 bytes[0x1000]; } Storage;
static Storage actor_store, data_store, other_data, control_store, main_store, companion_store;
static Storage records[8], allocate_store, reference_store, external_record, manager_store;
static GeorgeGoalEntity *actor;
static GeorgeGoalEntityData *data;
static u8 control_table[0x120], record_table[0x80];
static GeorgeRotationMatrix input_frames[3];
#define input_frame input_frames[0]
static u8 animation_records[0x100];
static void *effect_values[8];
const u8 D_0042E7A8[0x38] = {0}, D_00421160[12] = {0}, D_0042C360[20] = {0};
u8 D_003F83F0[0x1C * 42];
void *D_003F2D40, *D_004961F4;
GeorgeActorPointerRange D_0046A0F0;

enum { RELEASE_WORD=1, MANAGER, UNLINK, RELEASE_RECORD, CREATE_KEY, CONFIGURE,
       HEIGHT, ALLOCATE, CREATE_SCALAR, CREATE_BOUNDS, ASSOCIATE, CREATE_OWNER,
       INSERT_OWNER, LOOKUP, UPDATE_BEGIN, UPDATE_END, CANCEL, NOTIFY, STATUS,
       STATUS_ONCE, REFERENCE, REQUEST, COPY, MULTIPLY, CREATE_EFFECT, CLEAR,
       UPDATE_BOUNDS, READY, MASK, IDENTIFIER, COLLECTION_COUNT, COLLECTION_ITEM,
       COLLECTION_CLEAR, KEY_CHANGE, ANIMATION_CANCEL, MAIN_STATE_CLEAR,
       MEMBER, OUTPUT, UPDATE_A, ARRAY_COUNT, ARRAY_ITEM, PAIR_REQUEST,
       UPDATE_B, EFFECT_CREATE, EFFECT_POSITION, EFFECT_INSERT, ATEXIT, HASH,
       EFFECT_NOTICE, SCRIPT_DESTROY };
typedef struct Event {
    u32 kind, object, words[8], input, output;
    float floats[5];
} Event;
static Event events[512];
static int event_count, checks;
static void (*hook)(Event *);
static u32 statuses[32], once_status, mask_value, identifier_value;
static int status_index, ready_index, ready_values[16], collection_size, array_size;
static u32 lookup_key, request_word;
static float height_value;
static void *lookup_result, *matrix_result;
static const GeorgeMathVec4 *last_bounds;
static GeorgeMathVec4 bounds_copy[2];

static Event *event(u32 kind, const void *object)
{
    Event *e;
    if (event_count == 512) { fprintf(stderr,"event overflow\n"); exit(1); }
    e = &events[event_count++]; memset(e, 0, sizeof(*e));
    e->kind = kind; e->object = (u32)object; return e;
}
static void finish(Event *e) { if (hook != 0) hook(e); }
static void check(int ok, const char *name, int line)
{
    ++checks;
    if (!ok) { fprintf(stderr,"FAIL %s:%d (%d events)\n",name,line,event_count); exit(1); }
}
#define CHECK(condition) check((condition), #condition, __LINE__)
static u32 bits(float value) { union { float f; u32 u; } x; x.f=value; return x.u; }
static int count(u32 kind) { int i,n=0; for(i=0;i<event_count;++i) if(events[i].kind==kind) ++n; return n; }
static Event *nth(u32 kind,int n) { int i; for(i=0;i<event_count;++i) if(events[i].kind==kind && n--==0) return &events[i]; return 0; }
static void virtual_word(void *object, u32 word)
{ Event *e=event(CONFIGURE,object); e->words[0]=word; finish(e); }
static void virtual_release(void *object, u32 word)
{ Event *e=event(RELEASE_RECORD,object); e->words[0]=word; finish(e); }
static s32 virtual_predicate(void *object,float first,float second)
{ Event *e=event(OUTPUT,object); e->floats[0]=first; e->floats[1]=second; finish(e); return (s32)mask_value; }
static void virtual_output(void *object,GeorgeGoalOutput *value)
{ Event *e=event(OUTPUT,object); e->output=(u32)value; finish(e); }
static void virtual_member(void *object)
{ Event *e=event(MEMBER,object); finish(e); }

static void reset(void)
{
    int i;
    memset(&actor_store,0,sizeof(actor_store)); memset(&data_store,0,sizeof(data_store));
    memset(&other_data,0,sizeof(other_data)); memset(&control_store,0,sizeof(control_store));
    memset(&main_store,0,sizeof(main_store)); memset(&companion_store,0,sizeof(companion_store));
    memset(records,0,sizeof(records)); memset(&allocate_store,0,sizeof(allocate_store));
    memset(&reference_store,0,sizeof(reference_store)); memset(&external_record,0,sizeof(external_record));
    memset(control_table,0,sizeof(control_table)); memset(record_table,0,sizeof(record_table));
    memset(D_003F83F0,0,sizeof(D_003F83F0)); memset(input_frames,0,sizeof(input_frames));
    memset(animation_records,0,sizeof(animation_records)); memset(statuses,0,sizeof(statuses));
    memset(ready_values,0,sizeof(ready_values));
    actor=(GeorgeGoalEntity *)(actor_store.bytes+0x100); data=(GeorgeGoalEntityData *)data_store.bytes;
    actor->field18=data;
    FIELD(actor,0x20,GeorgeActorControlObject *)=(GeorgeActorControlObject *)control_store.bytes;
    FIELD(control_store.bytes,0,const u8 *)=control_table;
    ((GeorgeGoalVirtualWord *)(control_table+0x20))->invoke=virtual_word;
    ((GeorgeGoalVirtualOutput *)(control_table+0xB0))->invoke=virtual_output;
    ((GeorgeActorVirtualPredicate *)(control_table+0xD0))->invoke=virtual_predicate;
    ((GeorgeGoalVirtualWord *)(record_table+8))->invoke=virtual_release;
    ((GeorgeGoalVirtualWord *)(record_table+0x28))->invoke=virtual_release;
    for(i=0;i<8;++i) FIELD(records[i].bytes,0,const u8 *)=record_table;
    FIELD(actor,0x228,void *)=records[0].bytes;
    FIELD(actor,0x264,void *)=records[1].bytes;
    FIELD(actor,0x238,void *)=records[2].bytes;
    FIELD(actor,0x23C,void *)=records[3].bytes;
    FIELD(actor,0x230,void *)=records[4].bytes;
    FIELD(actor,0x1B0,void *)=main_store.bytes;
    FIELD(actor,0x1B4,void *)=companion_store.bytes;
    FIELD(actor,0x40,float)=10.0f; FIELD(actor,0x44,float)=20.0f; FIELD(actor,0x48,float)=30.0f;
    FIELD(actor,0x120,float)=40.0f; FIELD(actor,0x124,float)=50.0f; FIELD(actor,0x128,float)=60.0f;
    FIELD(actor,0x2E8,float)=10.0f; FIELD(actor,0x35C,float)=0.25f;
    FIELD(records[0].bytes,0x20,float)=2.0f; FIELD(records[0].bytes,0x24,float)=3.0f;
    FIELD(records[0].bytes,0x28,float)=4.0f; FIELD(records[0].bytes,0x40,float)=100.0f;
    FIELD(records[0].bytes,0x44,float)=200.0f; FIELD(records[0].bytes,0x48,float)=300.0f;
    for(i=0;i<16;++i) input_frame.element[i]=(float)(i+1);
    memcpy(&input_frames[1],&input_frame,64); memcpy(&input_frames[2],&input_frame,64);
    memcpy(FRAME(actor,0xB0),&input_frame,64); memcpy(FRAME(actor,0xF0),&input_frame,64);
    FIELD(actor,0x120,float)=40.0f; FIELD(actor,0x124,float)=50.0f; FIELD(actor,0x128,float)=60.0f;
    FIELD(data,0x130,float)=2.0f; FIELD(data,0x134,float)=3.0f;
    FIELD(data,0x14C,float)=4.0f; FIELD(data,0x15C,float)=5.0f;
    FIELD(data,0x35C,float)=1000.0f; FIELD(data,0x360,float)=1000.0f;
    FIELD(data,0x364,float)=1000.0f; FIELD(data,0x368,float)=2.0f;
    FIELD(data,0x36C,float)=3.0f; FIELD(data,0x370,float)=4.0f; FIELD(data,0x378,float)=5.0f;
    lookup_result=reference_store.bytes; matrix_result=&input_frame;
    D_003F2D40=external_record.bytes; D_004961F4=manager_store.bytes;
    D_0046A0F0.field00=effect_values; D_0046A0F0.field04=effect_values;
    D_0046A0F0.field08=effect_values+8;
    status_index=ready_index=event_count=0; collection_size=array_size=0;
    once_status=mask_value=0; identifier_value=0x28; height_value=2.0f; hook=0;
    lookup_key=request_word=0; last_bounds=0;
}

void *func_002AEE60(u32 size)
{ Event *e=event(ALLOCATE,0); e->words[0]=size; finish(e); return allocate_store.bytes; }
void func_002AF120(void *value) { finish(event(RELEASE_WORD,value)); }
void func_002393F8(u32 value) { finish(event(RELEASE_WORD,(void *)value)); }
void *func_0022C1E0(void) { finish(event(MANAGER,0)); return manager_store.bytes; }
void func_00311680(void *manager,void *value)
{ Event *e=event(UNLINK,manager);e->words[0]=(u32)value;finish(e); }
void func_00317910(void *value) { finish(event(RELEASE_RECORD,value)); }
u32 func_00236A10(const GeorgeRotationMatrix *frame,u32 key,u32 a,u32 b,u32 c,u32 d)
{ Event *e=event(CREATE_KEY,frame);e->words[0]=key;e->words[1]=a;e->words[2]=b;e->words[3]=c;e->words[4]=d;finish(e);return 0x1234; }
float func_00192DA8(GeorgeGoalEntity *entity) { finish(event(HEIGHT,entity));return height_value; }
void *func_002E2BB0(void *manager,u32 kind,u32 group)
{ Event *e=event(ALLOCATE,manager);e->words[0]=kind;e->words[1]=group;finish(e);return external_record.bytes; }
void *func_002F0520(void *memory,float value)
{ Event *e=event(CREATE_SCALAR,memory);e->floats[0]=value;FIELD(memory,0,const u8 *)=record_table;FIELD(memory,6,u16)=1;finish(e);return memory; }
void *func_0022B950(void *source,const GeorgeMathVec3 *point,const GeorgeMathVec4 *direction,
                   u32 selector,u32 count_word,u32 a,u32 b,float c,float d,float f,float g,float h)
{ Event *e=event(CREATE_BOUNDS,source);e->input=(u32)point;e->words[0]=selector;e->words[1]=count_word;e->words[2]=a;e->words[3]=b;e->words[4]=(u32)direction;e->floats[0]=c;e->floats[1]=d;e->floats[2]=f;e->floats[3]=g;e->floats[4]=h;memcpy(&bounds_copy[0],point,12);finish(e);return records[5].bytes; }
void func_0022C360(void *value,u32 word)
{ Event *e=event(ASSOCIATE,value);e->words[0]=word;finish(e); }
void *func_0014F1D0(void *memory,void *record,GeorgeGoalEntity *entity)
{ Event *e=event(CREATE_OWNER,memory);e->words[0]=(u32)record;e->words[1]=(u32)entity;finish(e);return records[6].bytes; }
void func_00310CC0(void *manager,void *record,u32 word)
{ Event *e=event(INSERT_OWNER,manager);e->words[0]=(u32)record;e->words[1]=word;finish(e); }
void *func_00238BA0(void *value,u32 key)
{ Event *e=event(LOOKUP,value);e->words[0]=key;finish(e);return records[7].bytes; }
void func_0022D838(void *value) { finish(event(UPDATE_BEGIN,value)); }
void func_0022D788(void *value) { finish(event(UPDATE_END,value)); }
void func_002727D8(void *value) { finish(event(CANCEL,value)); }
void func_00235CD8(void *value,u32 key,u32 word)
{ Event *e=event(NOTIFY,value);e->words[0]=key;e->words[1]=word;finish(e); }
u32 func_00272C30(void *value)
{ Event *e=event(STATUS,value);u32 v=statuses[status_index++ &31];finish(e);return v; }
u32 func_00272C10(void *value) { finish(event(STATUS_ONCE,value));return once_status; }
void *func_0023C230(void *key,u32 kind)
{ Event *e=event(REFERENCE,key);e->words[0]=kind;lookup_key=(u32)key;finish(e);return lookup_result; }
s32 func_00192D18(GeorgeGoalEntity *entity,u32 word,void *reference,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context)
{ Event *e=event(REQUEST,entity);e->words[0]=word;e->words[1]=(u32)reference;e->words[2]=(u32)callback;e->words[3]=(u32)context;request_word=word;finish(e);return 1; }
void func_002A1C08(void *output,const void *input)
{ Event *e=event(COPY,input);e->output=(u32)output;memmove(output,input,64);finish(e); }
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *first,const GeorgeRotationMatrix *second)
{ Event *e=event(MULTIPLY,first);e->input=(u32)second;e->output=(u32)output;memmove(output,first!=0?first:second,64);finish(e); }
void *func_001EDF40(void *memory,const GeorgeRotationMatrix *frame,void *opaque,void *reference,GeorgeGoalEntity *entity)
{ Event *e=event(CREATE_EFFECT,memory);e->input=(u32)frame;e->words[0]=(u32)opaque;e->words[1]=(u32)reference;e->words[2]=(u32)entity;finish(e);return records[6].bytes; }
void func_00191150(GeorgeGoalEntity *entity) { finish(event(CLEAR,entity)); }
void func_0030C440(void *record,const GeorgeMathVec4 *bounds)
{ Event *e=event(UPDATE_BOUNDS,record);last_bounds=bounds;memcpy(bounds_copy,bounds,32);finish(e); }
s32 func_00238D50(void *record)
{ Event *e=event(READY,record);s32 value=ready_values[ready_index++ &15];finish(e);return value; }
u32 func_00297640(u32 word)
{ Event *e=event(MASK,(void *)word);finish(e);return mask_value; }
u32 func_002A7418(u32 word)
{ Event *e=event(IDENTIFIER,(void *)word);finish(e);return identifier_value; }
u32 func_002AAF88(const void *record) { finish(event(COLLECTION_COUNT,record));return (u32)collection_size; }
void *func_002AAF50(void *record,s32 index)
{ Event *e=event(COLLECTION_ITEM,record);e->words[0]=(u32)index;finish(e);return records[(u32)index &7].bytes; }
void func_002AAFD8(void *record) { finish(event(COLLECTION_CLEAR,record)); }
void func_00191B40(GeorgeGoalEntity *entity,u32 key)
{ Event *e=event(KEY_CHANGE,entity);e->words[0]=key;finish(e); }
void func_00272A58(void *main) { finish(event(ANIMATION_CANCEL,main)); }
void func_00170538(GeorgeGoalEntity *entity) { finish(event(MAIN_STATE_CLEAR,entity)); }
void func_00181B70(GeorgeGoalEntity *entity) { finish(event(UPDATE_A,entity)); }
u32 func_002A6460(const void *array) { finish(event(ARRAY_COUNT,array));return (u32)array_size; }
void *func_002A6468(const void *array,s32 index)
{ Event *e=event(ARRAY_ITEM,array);e->words[0]=(u32)index;finish(e);return animation_records; }
void func_00272970(void *main,u32 word,u32 a,u32 b,u32 owner,void *reference,GeorgeActorRequestCallback callback,GeorgeGoalEntity *context)
{ Event *e=event(PAIR_REQUEST,main);e->words[0]=word;e->words[1]=a;e->words[2]=b;e->words[3]=owner;e->words[4]=(u32)reference;e->words[5]=(u32)callback;e->words[6]=(u32)context;finish(e); }
void func_00196A00(GeorgeGoalEntity *entity) { finish(event(UPDATE_B,entity)); }
void *func_002481F0(void *memory) { finish(event(EFFECT_CREATE,memory));return external_record.bytes; }
s32 func_00100AA8(const void *first,const void *second) { (void)first;(void)second;return 0; }
void **func_00100C30(void **begin,void **end,void *const *value,s32 (*compare)(const void *,const void *))
{ Event *e=event(EFFECT_POSITION,begin);e->words[0]=(u32)end;e->words[1]=(u32)*value;(void)compare;finish(e);return end; }
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value)
{ Event *e=event(EFFECT_INSERT,range);e->words[0]=(u32)position;e->words[1]=(u32)*value;finish(e); }
void func_002BD340(void) { }
s32 func_00396260(void (*callback)(void)) { Event *e=event(ATEXIT,(void *)callback);finish(e);return 0; }
u32 *func_002BEBA0(u32 *output,const u8 *text)
{ Event *e=event(HASH,text);*output=0xABCD;finish(e);return output; }
void func_00251CC8(void *effect,u32 key)
{ Event *e=event(EFFECT_NOTICE,effect);e->words[0]=key;finish(e); }
void func_002D0700(GeorgeScriptObject *entity,u32 flags)
{ Event *e=event(SCRIPT_DESTROY,entity);e->words[0]=flags;finish(e); }
void func_00190FC0(u32 unused,GeorgeGoalEntity *entity,void *source)
{ (void)unused;(void)entity;(void)source; }
void func_00191DC8(GeorgeGoalEntity *entity,u32 key)
{ Event *e=event(KEY_CHANGE,entity);e->words[0]=key;finish(e); }
GeorgeRotationMatrix *func_00192748(GeorgeGoalEntity *entity,u32 identifier)
{ Event *e=event(LOOKUP,entity);e->words[0]=identifier;finish(e);return matrix_result; }
void func_00196980(GeorgeGoalEntity *entity) { finish(event(CLEAR,entity)); }
void func_0018E5A8(GeorgeGoalEntity *entity,const GeorgeMathVec3 *lower,const GeorgeMathVec3 *upper,u32 word,u32 count_word)
{ Event *e=event(CREATE_BOUNDS,entity);e->words[0]=word;e->words[1]=count_word;memcpy(&bounds_copy[0],lower,12);memcpy(&bounds_copy[1],upper,12);finish(e); }

static void test_lifecycle(void)
{
    int i;
    for(i=0;i<8;++i) {
        reset();FIELD(actor,0x4F8,void *)=i&1?records[0].bytes:0;
        func_00190C18(actor,(u32)i);
        CHECK(FIELD(actor,4,const u8 *)==D_0042E7A8);
        CHECK(count(RELEASE_WORD)==(i&1));CHECK(count(SCRIPT_DESTROY)==1);
        CHECK(nth(SCRIPT_DESTROY,0)->words[0]==(u32)i);
        CHECK(FIELD(actor,0x4F8,void *)==(i&1?records[0].bytes:0));
    }
    for(i=0;i<8;++i) {
        reset();FIELD(actor,0x2D8,u32)=(u32)i;
        FIELD(actor,0x350,u32)=17;FIELD(actor,0x354,u32)=18;FIELD(actor,0x2DC,u32)=19;
        func_001910C8(actor);
        CHECK(count(CANCEL)==(i==4));CHECK(count(NOTIFY)==(i==4));
        CHECK(FIELD(actor,0x350,u32)==(i==4?0:17));
        CHECK(FIELD(actor,0x354,u32)==(i==4?0:18));
        CHECK(FIELD(actor,0x2D8,u32)==(i==4?0:(u32)i));
        CHECK(FIELD(actor,0x2DC,u32)==(i==4?0:19));
        if(i==4) { CHECK(FIELD(actor,0x334,float)==4.0f);CHECK(FIELD(records[4].bytes,0x30,float)==5.0f); }
    }
    reset();FIELD(actor,0x1B0,void *)=0;FIELD(actor,0x300,void *)=records[0].bytes;
    func_00191180(actor);CHECK(nth(CANCEL,0)->object==0);CHECK(count(CLEAR)==1);
    CHECK(FIELD(actor,0x2D8,u32)==0);CHECK(FIELD(records[4].bytes,0x30,float)==5.0f);
}

static void mutate_cleanup(Event *e)
{
    if(e->kind==CANCEL) { FIELD(actor,0x264,void *)=records[6].bytes;FIELD(actor,0x238,void *)=records[7].bytes; }
    if(e->kind==NOTIFY) { actor->field18=(GeorgeGoalEntityData *)other_data.bytes;FIELD(actor,0x230,void *)=records[5].bytes; }
    if(e->kind==CLEAR) { actor->field18=data;FIELD(actor,0x230,void *)=records[6].bytes; }
}
static void test_cleanup_reloads(void)
{
    reset();FIELD(actor,0x2D8,u32)=4;FIELD(other_data.bytes,0x370,float)=12;FIELD(other_data.bytes,0x378,float)=13;
    hook=mutate_cleanup;func_001910C8(actor);
    CHECK(nth(NOTIFY,0)->object==(u32)records[6].bytes);CHECK(FIELD(actor,0x334,float)==12);
    CHECK(FIELD(records[5].bytes,0x30,float)==13);
    reset();FIELD(actor,0x300,void *)=records[0].bytes;hook=mutate_cleanup;func_00191180(actor);
    CHECK(nth(NOTIFY,0)->object==(u32)records[7].bytes);CHECK(FIELD(records[6].bytes,0x30,float)==5);
}

static void test_configure(void)
{
    int enabled, flag;
    for(enabled=0;enabled<2;++enabled) for(flag=0;flag<2;++flag) {
        reset();FIELD(actor,0x1A4,u32)=0x4455;FIELD(actor,0x1A0,void *)=records[0].bytes;
        FIELD(actor,0x454,void *)=records[1].bytes;
        FIELD(actor,0x190,GeorgeActorBits64)=(enabled?0x400ULL:0)|(flag?0x10000000ULL:0);
        func_00175AF0(actor,0xFFFFFFFFU);
        CHECK(nth(RELEASE_WORD,0)->object==0x4455);CHECK(nth(UNLINK,0)->words[0]==(u32)records[0].bytes);
        CHECK(nth(CREATE_KEY,0)->words[0]==0x09DB5CDDU);CHECK(nth(CONFIGURE,0)->words[0]==0xFFFFFFFFU);
        CHECK(FIELD(actor,0x1A4,u32)==0x1234);CHECK(FIELD(actor,0x454,void *)==0);
        CHECK(count(CREATE_BOUNDS)==enabled);
        if(enabled) {
            Event *e=nth(CREATE_BOUNDS,0);
            CHECK(e->words[0]==(u32)(flag?25:26));CHECK(e->words[1]==6);
            CHECK(bits(e->floats[0])==0x3F800000U);CHECK(bits(e->floats[1])==0x3F000000U);
            CHECK(bits(e->floats[2])==0x3ECCCCCDU);CHECK(bits(e->floats[3])==0);
            CHECK(bits(e->floats[4])==0x3D4CCCCDU);CHECK(bounds_copy[0].x==10);
            CHECK(bounds_copy[0].y==22);CHECK(bounds_copy[0].z==30);
            CHECK(FIELD(actor,0x1A0,void *)==records[5].bytes);CHECK(FIELD(actor,0x374,void *)==records[6].bytes);
            CHECK(FIELD(external_record.bytes,6,u16)==0);CHECK(count(RELEASE_RECORD)==3);
        } else CHECK(FIELD(actor,0x1A0,void *)==0);
    }
    reset();FIELD(actor,0x1C8,void *)=reference_store.bytes;FIELD(data,0xA8,u32)=0x80;
    func_00190C70(actor,data);CHECK(nth(LOOKUP,0)->words[0]==0x1A6B0F5DU);
    CHECK(FIELD(records[7].bytes,0x28,u32)==0x81);CHECK(FIELD(records[7].bytes,0x2C,u32)==0x81);
}

static void test_1726(void)
{
    int phase;
    for(phase=0;phase<9;++phase) {
        reset();FIELD(actor,0x330,u32)=(u32)phase;FIELD(actor,0x2D8,u32)=4;
        FIELD(actor,0x32C,u32)=0x55;FIELD(actor,0x324,float)=0.5f;
        func_001726C8(actor,0.25f);
        CHECK(FIELD(actor,0x2E8,float)==9.75f);CHECK(FIELD(actor,0x334,float)==0.25f);
        CHECK(count(COPY)==(phase>=1 && phase<=3?2:1));
        if(phase==0) { CHECK(FIELD(actor,0x330,u32)==4);CHECK(FIELD(actor,0x324,u32)==0);CHECK(request_word==0x58); }
        if(phase>=1 && phase<=3) {
            CHECK(FIELD(records[1].bytes,0xA0,u32)==0x100);
            CHECK(FIELD(records[1].bytes,0x40,float)==100.0f+2.0f*0.34999999403953552f);
            CHECK(FIELD(records[1].bytes,0x44,float)==200.0f+3.0f*0.34999999403953552f);
            CHECK(FIELD(records[1].bytes,0x48,float)==300.0f+4.0f*0.34999999403953552f);
        }
        CHECK(count(REQUEST)==(phase==0?2:0));
    }
    reset();FIELD(actor,0x330,u32)=4;FIELD(actor,0x2D8,u32)=4;FIELD(data,0x35C,float)=0;
    FIELD(actor,0x32C,u32)=0;func_001726C8(actor,1.0f);
    CHECK(FIELD(actor,0x330,u32)==1);CHECK(request_word==0x5F);CHECK(nth(NOTIFY,0)->words[0]==0x9F79558FU);
    for(phase=5;phase<=6;++phase) {
        reset();FIELD(actor,0x330,u32)=(u32)phase;FIELD(actor,0x324,float)=4;
        FIELD(actor,0x2D8,u32)=4;FIELD(data,0x354,u32)=0x123;FIELD(data,0x350,u32)=0x456;
        statuses[0]=0x2000;func_001726C8(actor,0.25f);
        CHECK(lookup_key==(u32)(phase==5?0x123:0x456));CHECK(FIELD(actor,0x324,float)==2);
        CHECK(FIELD(records[6].bytes,0x18,float)==6);CHECK(count(CREATE_EFFECT)==1);
    }
    reset();FIELD(actor,0x330,u32)=1;FIELD(actor,0x2D8,u32)=4;FIELD(actor,0x354,u32)=1;
    func_001726C8(actor,0);CHECK(count(CANCEL)==1);CHECK(FIELD(actor,0x2D8,u32)==0);
    reset();FIELD(actor,0x330,u32)=3;FIELD(actor,0x354,u32)=1;FIELD(actor,0x2D8,u32)=4;
    FIELD(data,0x358,u32)=0xCAFE;func_001726C8(actor,0);
    CHECK(nth(KEY_CHANGE,0)->words[0]==0xCAFE);CHECK(actor->field0C==18);CHECK(count(MAIN_STATE_CLEAR)==1);
}

static void test_172c(void)
{
    int branch,toggle;
    for(branch=0;branch<2;++branch) for(toggle=0;toggle<2;++toggle) {
        reset();FIELD(actor,0x33C,s16)=100;FIELD(actor,0x33E,signed char)=(signed char)toggle;
        FIELD(actor,0x5C8,float)=2;FIELD(actor,0x300,void *)=records[6].bytes;
        if(branch) FIELD(actor,0x228,void *)=0;
        once_status=0x2000;statuses[0]=0x4000;
        func_00172C08(actor,0.25f);CHECK(FIELD(actor,0x2E8,float)==9.75f);
        CHECK(FIELD(actor,0x33E,signed char)==(signed char)!toggle);CHECK(count(CLEAR)==1);
        CHECK(count(CREATE_BOUNDS)==!toggle);CHECK(count(UPDATE_BOUNDS)==1);
        if(!toggle) CHECK(nth(CREATE_BOUNDS,0)->words[0]==1 && nth(CREATE_BOUNDS,0)->words[1]==5);
        CHECK(bounds_copy[0].w==0);CHECK(bounds_copy[1].w==0);
        if(!branch) {CHECK(bounds_copy[0].x==99);CHECK(bounds_copy[1].z==313);}
        else {CHECK(bounds_copy[0].x==8);CHECK(bounds_copy[1].z==20);}
    }
    reset();func_00172C08(actor,1);CHECK(FIELD(actor,0x33C,s16)==100);CHECK(request_word==0x5E);
    reset();FIELD(actor,0x33C,s16)=-1;func_00172C08(actor,1);CHECK(event_count==0);CHECK(FIELD(actor,0x2E8,float)==9);
    reset();FIELD(actor,0x33C,s16)=100;FIELD(actor,0x2E8,float)=NAN;
    func_00172C08(actor,0);CHECK(count(CLEAR)==1);CHECK(count(UPDATE_BOUNDS)==0);
}

static void test_1732(void)
{
    int phase;
    for(phase=-1;phase<6;++phase) {
        reset();FIELD(actor,0x340,s32)=phase;func_001732F0(actor);
        CHECK(FIELD(records[2].bytes,0x40,float)==40);CHECK(FIELD(records[2].bytes,0x44,float)==50);
        CHECK(FIELD(records[2].bytes,0x48,float)==60);CHECK(FIELD(records[2].bytes,0x4C,float)==1);
        CHECK(FIELD(records[2].bytes,0xA0,u32)==0x100);
        CHECK(count(REQUEST)==(phase==0));CHECK(count(UPDATE_BOUNDS)==(phase==3));
        if(phase==3) {CHECK(bounds_copy[0].x==6);CHECK(bounds_copy[0].y==20);CHECK(bounds_copy[1].y==24);}
    }
    reset();FIELD(actor,0x340,u32)=2;statuses[0]=0x2000;statuses[1]=0x80000;FIELD(actor,0x34C,u32)=1;
    func_001732F0(actor);CHECK(FIELD(actor,0x340,u32)==3);CHECK(FIELD(actor,0x344,u32)==1);
    CHECK(nth(CREATE_BOUNDS,0)->words[0]==4);CHECK(nth(CREATE_BOUNDS,0)->words[1]==12);
    CHECK(nth(NOTIFY,0)->words[0]==0x9F79558FU);CHECK(FIELD(actor,0x34C,u32)==0);
    reset();FIELD(actor,0x340,u32)=3;statuses[0]=0x80000;func_001732F0(actor);
    CHECK(FIELD(actor,0x340,u32)==4);CHECK(FIELD(actor,0x34C,u32)==1);CHECK(nth(NOTIFY,0)->words[0]==0xB95616B6U);
    reset();FIELD(actor,0x340,u32)=4;FIELD(actor,0x2E8,float)=0;FIELD(actor,0x348,u32)=1;
    func_001732F0(actor);CHECK(FIELD(actor,0x340,u32)==0);CHECK(FIELD(actor,0x348,u32)==0);
    reset();FIELD(actor,0x340,u32)=4;FIELD(actor,0x2E8,float)=0;
    FIELD(records[2].bytes,0xAC,void *)=records[5].bytes;FIELD(records[5].bytes,0xAC,void *)=records[6].bytes;
    ready_values[0]=1;ready_values[1]=1;ready_values[2]=0;
    func_001732F0(actor);CHECK(count(READY)==3);CHECK(count(CANCEL)==0);
    reset();FIELD(actor,0x340,u32)=4;FIELD(actor,0x2E8,float)=0;ready_values[0]=1;
    func_001732F0(actor);CHECK(count(CANCEL)==1);CHECK(FIELD(actor,0x2D8,u32)==0);
}

static void test_1748(void)
{
    int toggle;
    for(toggle=0;toggle<2;++toggle) {
        reset();FIELD(actor,0x320,signed char)=(signed char)toggle;
        FIELD(actor,0x300,void *)=records[6].bytes;statuses[0]=0x10;mask_value=0x10;
        func_00174868(actor,0.25f);CHECK(FIELD(actor,0x320,signed char)==(signed char)!toggle);
        CHECK(count(CLEAR)==1);CHECK(count(CREATE_BOUNDS)==!toggle);
        CHECK(count(MULTIPLY)==1);CHECK(FIELD(records[3].bytes,0xA0,u32)==0x100);
        if(!toggle) {CHECK(bounds_copy[0].x==8);CHECK(bounds_copy[0].y==20);CHECK(bounds_copy[1].y==23);}
    }
    reset();FIELD(actor,0x2D8,u32)=1;FIELD(actor,0x2E8,float)=0;
    func_00174868(actor,0);CHECK(count(ANIMATION_CANCEL)==1);CHECK(FIELD(actor,0x2D8,u32)==0);
    reset();FIELD(actor,0x2E8,float)=0;FIELD(data,0x148,s32)=3;FIELD(actor,0x31C,u32)=1;
    FIELD(data,0x214,u32)=0x123;FIELD(data,0x210,u32)=4;
    func_00174868(actor,0);CHECK(FIELD(actor,0x318,u32)==1);CHECK(FIELD(actor,0x31C,u32)==0);
    CHECK(nth(KEY_CHANGE,0)->words[0]==0x123);CHECK(request_word==0x67);
    reset();FIELD(actor,0x2E8,float)=0;FIELD(data,0x148,s32)=1;FIELD(data,0x12C,u32)=1;FIELD(actor,0x318,u32)=1;
    ((GeorgeGoalMember *)(D_003F83F0+22*28))->selector=-1;
    ((GeorgeGoalMember *)(D_003F83F0+22*28))->target.direct=virtual_member;
    ((GeorgeGoalMember *)(D_003F83F0+22*28))->adjustment=-8;
    func_00174868(actor,0);CHECK(actor->field0C==22);CHECK(nth(MEMBER,0)->object==(u32)actor-8U);
}

static void test_1811(void)
{
    int phase;
    for(phase=99;phase<=202;++phase) if(phase<103 || phase>198) {
        reset();FIELD(actor,0x5C4,s32)=phase;FIELD(actor,0x5B8,u32)=3;
        func_00181120(actor);CHECK(count(OUTPUT)==1);
        CHECK(FIELD(actor,0x2E8,float)==9.75f);
        if(phase==101 || phase==201) CHECK(count(UPDATE_A)==1);
        else {CHECK(count(PAIR_REQUEST)==2);CHECK(FIELD(actor,0x5C4,u32)==(u32)(phase==200?201:101));CHECK(nth(PAIR_REQUEST,0)->words[0]==(u32)(phase==200?0x5F:0x5E));}
    }
    reset();FIELD(actor,0x5C4,u32)=101;FIELD(data,0x288,s32)=1;FIELD(actor,0x244,void *)=records[0].bytes;
    FIELD(actor,0x5D8,u32)=0x20;FIELD(actor,0x5DC,u32)=7;statuses[0]=0x20;
    FIELD(main_store.bytes,0xC,u32)=3;FIELD(main_store.bytes,0x380,void *)=reference_store.bytes;
    FIELD(main_store.bytes,0x3F0,const GeorgeRotationMatrix *)=&input_frame;
    array_size=2;animation_records[0x1D]=3;animation_records[0x3D]=7;
    func_00181120(actor);CHECK(nth(MULTIPLY,0)->object==(u32)&input_frame+64U);
    CHECK(FIELD(actor,0x5CC,u16)==1);CHECK(nth(NOTIFY,0)->words[0]==0x9F79558FU);
    CHECK(FIELD(records[0].bytes,0xA0,u32)==0x100);
    reset();FIELD(actor,0x5C4,u32)=101;FIELD(actor,0x2E8,float)=0;FIELD(actor,0x5BC,u32)=1;FIELD(actor,0x5B8,u32)=0xFFFFFFFFU;
    func_00181120(actor);CHECK(FIELD(actor,0x5C4,u32)==100);CHECK(FIELD(actor,0x5B8,u32)==0);CHECK(FIELD(actor,0x5BC,u32)==0);
    reset();FIELD(actor,0x5C4,u32)=201;FIELD(actor,0x2E8,float)=0;FIELD(data,0x288,s32)=2;
    FIELD(actor,0x244,void *)=records[0].bytes;FIELD(actor,0x248,void *)=0;
    func_00181120(actor);CHECK(FIELD(actor,0x2D8,u32)==0);CHECK(count(CANCEL)==1);CHECK(count(NOTIFY)==1);
    CHECK(count(UPDATE_B)==1);CHECK(FIELD(records[4].bytes,0x30,float)==5);
}

static void test_dispatch(void)
{
    int state;
    for(state=0;state<11;++state) {
        reset();FIELD(actor,0x2D8,u32)=(u32)state;
        if(state==4) FIELD(actor,0x330,u32)=8;
        if(state==5) FIELD(actor,0x33C,s16)=-1;
        if(state==6) FIELD(actor,0x340,s32)=-1;
        if(state==7) FIELD(actor,0x5C4,u32)=101;
        func_00191000(actor,0.5f);
        CHECK(FIELD(actor,0x2E8,float)==(state==1||state==2||state==4||state==5?9.5f:state==6||state==7?9.75f:10.0f));
        CHECK(count(OUTPUT)==(state==7));
    }
    reset();FIELD(actor,0x2D8,u32)=2;FIELD(actor,0x2E8,float)=0.5f;
    func_00191000(actor,0.5f);CHECK(FIELD(actor,0x2D8,u32)==0);CHECK(FIELD(actor,0x2E8,float)==0);
    reset();FIELD(actor,0x2D8,u32)=2;FIELD(actor,0x2E8,float)=NAN;
    func_00191000(actor,1);CHECK(FIELD(actor,0x2D8,u32)==2);CHECK(isnan(FIELD(actor,0x2E8,float)));
    reset();FIELD(actor,0x2D8,u32)=4;FIELD(actor,0x2DC,u32)=1;
    func_00191000(actor,1);CHECK(event_count==0);CHECK(FIELD(actor,0x2E8,float)==10);
}

static int hook_mode;
static void mutation(Event *e)
{
    if(hook_mode==1) {
        if(e->kind==UNLINK) FIELD(actor,0x1A0,void *)=records[1].bytes;
        if(e->kind==CONFIGURE) FIELD(actor,0x190,GeorgeActorBits64)=0x10000400ULL;
        if(e->kind==HEIGHT) FIELD(actor,0x44,float)=99;
        if(e->kind==CREATE_BOUNDS) FIELD(actor,0x19C,u32)=0xCAFE;
        if(e->kind==CREATE_OWNER) FIELD(actor,0x1A0,void *)=records[7].bytes;
    } else if(hook_mode==2) {
        if(e->kind==UPDATE_BEGIN) { FIELD(records[7].bytes,0x28,u32)=0xA1;FIELD(records[7].bytes,0x3C,void *)=records[6].bytes; }
    } else if(hook_mode==3) {
        if(e->kind==COPY && count(COPY)==1) {
            FIELD(records[0].bytes,0x40,float)=900;
            FIELD(actor,0x264,void *)=records[6].bytes;
        }
        if(e->kind==COPY && count(COPY)==2) FIELD(actor,0x264,void *)=records[7].bytes;
        if(e->kind==REQUEST) FIELD(actor,0x32C,u32)=0xBAD;
    } else if(hook_mode==4) {
        if(e->kind==STATUS_ONCE) FIELD(actor,0x228,void *)=records[7].bytes;
        if(e->kind==CREATE_BOUNDS) FIELD(actor,0x2E8,float)=0;
    } else if(hook_mode==5) {
        if(e->kind==HASH) D_003F2D40=records[7].bytes;
        if(e->kind==MULTIPLY) { actor->field18=(GeorgeGoalEntityData *)other_data.bytes;FIELD(actor,0x300,void *)=records[7].bytes; }
    } else if(hook_mode==6) {
        if(e->kind==READY && count(READY)==1) { FIELD(actor,0x238,void *)=records[7].bytes;FIELD(records[7].bytes,0xAC,void *)=records[6].bytes; }
    } else if(hook_mode==7) {
        if(e->kind==KEY_CHANGE) { FIELD(actor,0x318,u32)=2;actor->field18=(GeorgeGoalEntityData *)other_data.bytes; }
        if(e->kind==MULTIPLY) FIELD(actor,0x23C,void *)=records[7].bytes;
    } else if(hook_mode==8) {
        if(e->kind==PAIR_REQUEST && count(PAIR_REQUEST)==1) {
            FIELD(actor,0x1B4,void *)=records[7].bytes;
            FIELD(actor,0x368,u32)=0xBEEF;FIELD(actor,0x5B8,u32)=99;
        }
    } else if(hook_mode==9) {
        if(e->kind==ARRAY_COUNT) {
            FIELD(actor,0x1B0,void *)=companion_store.bytes;
            FIELD(companion_store.bytes,0x380,void *)=records[7].bytes;
            FIELD(companion_store.bytes,0x3F0,const GeorgeRotationMatrix *)=&input_frame;
            FIELD(actor,0x5DC,u32)=0xFFFF;
        }
        if(e->kind==MULTIPLY) FIELD(actor,0x244,void *)=records[6].bytes;
        if(e->kind==COPY) FIELD(actor,0x244,void *)=records[7].bytes;
    } else if(hook_mode==10) {
        if(e->kind==NOTIFY) { actor->field18=(GeorgeGoalEntityData *)other_data.bytes;FIELD(other_data.bytes,0x288,s32)=0; }
        if(e->kind==UPDATE_B) { FIELD(actor,0x230,void *)=records[7].bytes;FIELD(other_data.bytes,0x378,float)=123; }
    } else if(hook_mode==11) {
        if(e->kind==CREATE_SCALAR) FIELD(external_record.bytes,6,u16)=0;
    }
}

static void test_callback_aliases(void)
{
    int index;
    reset();FIELD(actor,0x1A0,void *)=records[0].bytes;hook_mode=1;hook=mutation;
    func_00175AF0(actor,7);
    CHECK(nth(RELEASE_RECORD,0)->object==(u32)records[1].bytes);
    CHECK(nth(CREATE_BOUNDS,0)->words[0]==25);CHECK(bounds_copy[0].y==22);
    CHECK(nth(ASSOCIATE,0)->words[0]==0xCAFE);
    CHECK(nth(INSERT_OWNER,0)->words[0]==(u32)records[7].bytes);
    reset();FIELD(data,0xA8,u32)=2;FIELD(records[7].bytes,0x3C,void *)=records[0].bytes;
    hook_mode=2;hook=mutation;func_00190C70(actor,data);
    CHECK(FIELD(records[7].bytes,0x2C,u32)==0xA1);CHECK(nth(UPDATE_END,0)->object==(u32)records[6].bytes);
    reset();FIELD(actor,0x330,u32)=4;FIELD(data,0x35C,float)=0;hook_mode=3;hook=mutation;
    func_001726C8(actor,1);
    CHECK(FIELD(records[6].bytes,0x40,float)==100.0f+2.0f*0.34999999403953552f);
    CHECK(FIELD(records[6].bytes,0xA0,u32)==0x100);CHECK(FIELD(records[7].bytes,0xA0,u32)==0);
    CHECK(nth(NOTIFY,0)->object==(u32)records[7].bytes);CHECK(FIELD(actor,0x32C,u32)==0x5F);
    reset();FIELD(actor,0x33C,s16)=100;once_status=0x2000;hook_mode=4;hook=mutation;
    FIELD(records[7].bytes,0x40,float)=6;FIELD(records[7].bytes,0x44,float)=7;FIELD(records[7].bytes,0x48,float)=8;
    func_00172C08(actor,0);
    CHECK(bits(bounds_copy[0].x)==bits(6.0f+-0.00999999977648258f));CHECK(count(CLEAR)==1);CHECK(count(UPDATE_BOUNDS)==0);
    reset();FIELD(actor,0x228,void *)=0;FIELD(actor,0x33C,s16)=100;statuses[0]=0x4000;
    FIELD(actor,0x190,GeorgeActorBits64)=~0ULL;FIELD(actor,0x300,void *)=records[6].bytes;
    FIELD(other_data.bytes,0x15C,float)=2;hook_mode=5;hook=mutation;
    func_00172C08(actor,0);
    CHECK(nth(EFFECT_NOTICE,0)->object==(u32)external_record.bytes);
    CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x400000000000ULL)==0);
    CHECK(nth(UPDATE_BOUNDS,0)->object==(u32)records[7].bytes);
    CHECK(bounds_copy[0].x==11);CHECK(bounds_copy[1].z==17);
    reset();FIELD(actor,0x340,u32)=4;FIELD(actor,0x2E8,float)=0;ready_values[0]=ready_values[1]=1;
    hook_mode=6;hook=mutation;func_001732F0(actor);
    CHECK(count(READY)==2);CHECK(nth(READY,1)->object==(u32)records[6].bytes);CHECK(count(CANCEL)==1);
    reset();FIELD(actor,0x238,void *)=ADDRESS(actor,0xE4);FIELD(actor,0x340,s32)=-1;
    func_001732F0(actor);
    CHECK(FIELD(actor,0x124,float)==40);CHECK(FIELD(actor,0x128,float)==40);CHECK(FIELD(actor,0x12C,float)==40);
    CHECK(FIELD(actor,0x130,float)==1);
    reset();FIELD(actor,0x238,void *)=ADDRESS(actor,0xDC);FIELD(actor,0x340,s32)=-1;
    func_001732F0(actor);
    CHECK(FIELD(actor,0x11C,float)==40);CHECK(FIELD(actor,0x120,float)==50);
    CHECK(FIELD(actor,0x124,float)==60);CHECK(FIELD(actor,0x128,float)==1);
    reset();FIELD(actor,0x2E8,float)=0;FIELD(actor,0x31C,u32)=1;FIELD(data,0x148,s32)=3;
    FIELD(data,0x214,u32)=0x123;FIELD(other_data.bytes,0x220,u32)=7;hook_mode=7;hook=mutation;
    func_00174868(actor,0);
    CHECK(request_word==0x6A);CHECK(FIELD(records[7].bytes,0xA0,u32)==0x100);
    reset();FIELD(actor,0x294,void *)=reference_store.bytes;collection_size=1;
    FIELD(records[0].bytes,0,void *)=ADDRESS(actor,-0x44);FIELD(actor,0x23C,void *)=0;
    func_00174868(actor,0);
    CHECK(FIELD(actor,0x3C,float)==10);CHECK(FIELD(actor,0x40,float)==20);
    CHECK(FIELD(actor,0x44,float)==30);CHECK(FIELD(actor,0x48,float)==1);
    reset();FIELD(actor,0x5C4,u32)=200;FIELD(actor,0x5B8,u32)=3;hook_mode=8;hook=mutation;
    func_00181120(actor);
    CHECK(nth(PAIR_REQUEST,0)->words[0]==0x5F);CHECK(nth(PAIR_REQUEST,1)->words[0]==0x5F);
    CHECK(nth(PAIR_REQUEST,1)->object==(u32)records[7].bytes);CHECK(nth(PAIR_REQUEST,1)->words[3]==0xBEEF);
    reset();FIELD(actor,0x5C4,u32)=101;FIELD(data,0x288,s32)=1;FIELD(actor,0x244,void *)=records[0].bytes;
    FIELD(actor,0x5D8,u32)=1;FIELD(actor,0x5DC,u32)=7;FIELD(main_store.bytes,0xC,u32)=3;
    statuses[0]=1;array_size=1;animation_records[0x1D]=7;hook_mode=9;hook=mutation;
    func_00181120(actor);
    CHECK(nth(ARRAY_ITEM,0)->object==(u32)records[7].bytes);
    CHECK(nth(MULTIPLY,0)->object==(u32)&input_frame);CHECK(FIELD(records[6].bytes,0xA0,u32)==0x100);
    CHECK(nth(NOTIFY,0)->object==(u32)records[7].bytes);
    reset();FIELD(actor,0x5C4,u32)=201;FIELD(actor,0x2E8,float)=0;FIELD(data,0x288,s32)=3;
    FIELD(actor,0x244,void *)=records[0].bytes;FIELD(actor,0x248,void *)=records[1].bytes;
    hook_mode=10;hook=mutation;func_00181120(actor);
    CHECK(count(NOTIFY)==1);CHECK(FIELD(records[7].bytes,0x30,float)==123);
    /* The shift uses a full 32-bit mask, then signed low-halfword selection. */
    for(index=0;index<32;++index) {
        int i;
        u16 initial=(u16)(index&1?0x8000:0), expected;
        u32 wide=(u32)initial ^ (1U<<((u32)index&31));
        s32 selected=((s32)(s16)wide>>((u32)index&31))&1;
        reset();FIELD(actor,0x5C4,u32)=101;FIELD(data,0x288,s32)=index+1;
        for(i=0;i<=index;++i) FIELD(actor,0x244U+((u32)i<<2),void *)=0;
        FIELD(actor,0x244U+((u32)index<<2),void *)=records[0].bytes;
        FIELD(actor,0x5D8U+((u32)index<<4),u32)=1;FIELD(actor,0x5CC,u16)=initial;
        statuses[0]=1;func_00181120(actor);expected=(u16)wide;
        CHECK(FIELD(actor,0x5CC,u16)==expected);
        CHECK(nth(NOTIFY,0)->words[0]==(selected?0x9F79558FU:0xB95616B6U));
        CHECK(nth(MULTIPLY,0)->object==0);
    }
    reset();FIELD(actor,0x330,u32)=3;FIELD(actor,0x354,u32)=1;FIELD(actor,0x2D8,u32)=4;
    FIELD(actor,0x30,const GeorgeGoalVirtualVoid *)=(const GeorgeGoalVirtualVoid *)record_table;
    ((GeorgeGoalVirtualVoid *)record_table)->adjustment=32760;
    ((GeorgeGoalVirtualVoid *)record_table)->invoke=virtual_member;
    ((GeorgeGoalMember *)(D_003F83F0+18*28))->selector=1;
    ((GeorgeGoalMember *)(D_003F83F0+18*28))->target.vtable_offset=0x30;
    ((GeorgeGoalMember *)(D_003F83F0+18*28))->adjustment=32760;
    func_001726C8(actor,0);CHECK(nth(MEMBER,0)->object==(u32)actor+65520U);
    reset();FIELD(actor,0x190,GeorgeActorBits64)=0x400ULL;hook_mode=11;hook=mutation;
    func_00175AF0(actor,0);CHECK(FIELD(external_record.bytes,6,u16)==65535);CHECK(count(RELEASE_RECORD)==0);
    reset();FIELD(actor,0x228,void *)=0;FIELD(actor,0x33C,s16)=100;statuses[0]=0x4000;D_003F2D40=0;
    func_00172C08(actor,0);CHECK(count(EFFECT_CREATE)==1);CHECK(count(EFFECT_POSITION)==1);
    CHECK(count(EFFECT_INSERT)==0);CHECK(count(ATEXIT)==1);CHECK(D_0046A0F0.field04==effect_values+1);
    CHECK(((GeorgeActorEffectRecord *)effect_values[0])->field00==9);
    CHECK(((GeorgeActorEffectRecord *)effect_values[0])->field04==D_00421160);
    CHECK(((GeorgeActorEffectRecord *)effect_values[0])->field08==external_record.bytes);
    reset();FIELD(actor,0x228,void *)=0;FIELD(actor,0x33C,s16)=100;statuses[0]=0x4000;D_003F2D40=0;
    D_0046A0F0.field08=effect_values;func_00172C08(actor,0);
    CHECK(count(EFFECT_INSERT)==1);CHECK(count(ATEXIT)==1);
}

static void test_boundaries(void)
{
    static const float values[]={-10.0f,-1.0f,-0.0f,0.0f,0.25f,1.0f,10.0f,INFINITY,-INFINITY,NAN};
    int i,j,phase;
    for(i=0;i<10;++i) for(j=0;j<10;++j) {
        reset();FIELD(actor,0x2D8,u32)=2;FIELD(actor,0x2E8,float)=values[i];
        func_00191000(actor,values[j]);
        CHECK(FIELD(actor,0x2D8,u32)==(u32)(values[i]-values[j]<=0?0:2));
        CHECK(isnan(values[i]-values[j])?isnan(FIELD(actor,0x2E8,float)):bits(FIELD(actor,0x2E8,float))==bits(values[i]-values[j]));
    }
    for(phase=1;phase<=2;++phase) for(i=0;i<10;++i) {
        reset();FIELD(actor,0x330,u32)=(u32)phase;FIELD(actor,0x324,float)=values[i];
        FIELD(actor,0x32C,u32)=0x55;FIELD(data,phase==1?0x360:0x364,float)=0;
        func_001726C8(actor,0);
        CHECK(count(NOTIFY)==(0<values[i]));CHECK(FIELD(actor,0x330,u32)==(u32)(0<values[i]?phase+1:phase));
    }
    for(i=0;i<10;++i) {
        float clamped=0.0f;
        reset();FIELD(actor,0x330,u32)=5;FIELD(actor,0x324,float)=values[i];statuses[0]=0x2000;
        if(0<=values[i]) clamped=george_ee_minimum(values[i]+0.0f,2.0f);
        func_001726C8(actor,0);
        CHECK(bits(FIELD(actor,0x324,float))==bits(clamped));
        CHECK(bits(FIELD(records[6].bytes,0x18,float))==bits(clamped*3.0f));
    }
}

int main(void)
{
    test_lifecycle();test_cleanup_reloads();test_configure();test_1726();test_172c();
    test_1732();test_1748();test_1811();test_dispatch();
    test_callback_aliases();test_boundaries();
    printf("actor_substates: %d checks passed\n",checks);return 0;
}
