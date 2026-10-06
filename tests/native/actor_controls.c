/* Asset-free independent callback outcomes; no original instructions or data. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_controls.c"

enum { CONTROL=1, ATAN, CONVERT, COMPARE, ADD, SUBTRACT, FLOAT_RESULT,
       ALLOC, EFFECT_CONSTRUCTOR, UPPER, INSERT, EXIT_REGISTRY, HASH, EMIT,
       CONFIGURE, START, STOP, FREE, MOTION, DECAY, REQUEST, RELEASE,
       LOOKUP, CREATE, REFERENCE, COUNT, RECORDS, MATRIX, COPY, TRANSFORM,
       IDENTITY, NOTIFY, ENTER };
typedef struct Event { int kind; void *object, *argument; u32 word, other; float scalar; } Event;
static Event events[128];
static int checks, event_count;
static u8 storage[0xC00] __attribute__((aligned(16))), data[0x600];
static u8 main0[0x800], main1[0x800], companion0[0x800], companion1[0x800];
static u8 control_table[0x80], ref[0x100], ref_table[0x40];
static u8 objects[4][0x400], object_source[0x200], effect[0x300], alternate_effect[0x300];
static u8 model_records[8*32], alternate_records[8*32];
static GeorgeRotationMatrix matrices[8], alternate_matrices[8], reference_matrices[8];
static struct { GeorgeActorControlObject object; u8 rest[32]; } control;
static GeorgeGoalEntity *actor=(GeorgeGoalEntity *)(storage+32);
static void (*hook)(Event *);
static float atan_values[16], forced_float;
static int atan_index, float_override, allocated, created;
static u32 record_count;
static void *lookup_result;
static GeorgeMathVec3 motion_value;
static GeorgeRotationMatrix copied;
static void **upper_result;
static GeorgeActorEffectRecord effect_record;
static void *registry[8];
u8 D_003F83F0[42*28] __attribute__((aligned(8)));
void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[]={0}, D_0042DA78[]={1};

static void check(int okay,const char *message,int line)
{++checks;if(!okay){fprintf(stderr,"actor_controls:%d %s\n",line,message);exit(1);}}
#define CHECK(x) check((x),#x,__LINE__)
static u32 bits(float x){union{float f;u32 u;}v;v.f=x;return v.u;}
static float scalar(u32 x){union{float f;u32 u;}v;v.u=x;return v.f;}
static void near_value(float x,float y){CHECK(fabsf(x-y)<0.0001f);}
static Event *emit(int kind,void *object,void *argument,u32 word,u32 other,float value)
{Event *e;CHECK(event_count<128);e=&events[event_count++];e->kind=kind;e->object=object;e->argument=argument;e->word=word;e->other=other;e->scalar=value;if(hook)hook(e);return e;}
static int count(int kind){int i,n=0;for(i=0;i<event_count;++i)if(events[i].kind==kind)++n;return n;}
static Event *nth(int kind,int n){int i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&n--==0)return &events[i];CHECK(0);return 0;}
static void mock_control(void *self){emit(CONTROL,self,0,0,0,0);}
static void mock_reference(void *self,u32 word){emit(REFERENCE,self,0,word,0,0);}
static void mock_enter(void *self){emit(ENTER,self,0,0,0,0);}
static void callback(u32 ignored,GeorgeGoalEntity *context,void *source){(void)ignored;(void)context;(void)source;}
static void reset(void)
{
    memset(storage,0,sizeof(storage));memset(data,0,sizeof(data));memset(main0,0,sizeof(main0));memset(main1,0,sizeof(main1));
    memset(companion0,0,sizeof(companion0));memset(companion1,0,sizeof(companion1));memset(objects,0,sizeof(objects));
    memset(object_source,0,sizeof(object_source));memset(D_003F83F0,0,sizeof(D_003F83F0));
    memset(model_records,0,sizeof(model_records));memset(alternate_records,0,sizeof(alternate_records));
    memset(matrices,0,sizeof(matrices));memset(alternate_matrices,0,sizeof(alternate_matrices));memset(reference_matrices,0,sizeof(reference_matrices));
    memset(ref,0,sizeof(ref));memset(ref_table,0,sizeof(ref_table));memset(control_table,0,sizeof(control_table));
    memset(events,0,sizeof(events));memset(atan_values,0,sizeof(atan_values));
    hook=0;event_count=atan_index=allocated=created=float_override=0;forced_float=0;record_count=3;lookup_result=ref;
    actor->field18=(GeorgeGoalEntityData *)data;
    FIELD(actor,0x20,GeorgeActorControlObject *)=&control.object;control.object.field00=control_table;
    ((GeorgeGoalVirtualVoid *)(control_table+0x38))->adjustment=-4;
    ((GeorgeGoalVirtualVoid *)(control_table+0x38))->invoke=mock_control;
    FIELD(ref,0x20,void *)=ref_table;
    ((GeorgeGoalVirtualWord *)(ref_table+0x10))->adjustment=12;
    ((GeorgeGoalVirtualWord *)(ref_table+0x10))->invoke=mock_reference;
    FIELD(actor,0x1B0,void *)=main0;FIELD(actor,0x1B4,void *)=companion0;FIELD(actor,0x368,u32)=0xABCD;
    FIELD(actor,0x424,void *)=object_source;
    FIELD(main0,0x378,void *)=model_records;FIELD(main1,0x378,void *)=alternate_records;
    FIELD(companion0,0x378,void *)=model_records;FIELD(companion1,0x378,void *)=alternate_records;
    FIELD(main0,0x3E8,void *)=matrices;FIELD(main1,0x3E8,void *)=alternate_matrices;
    FIELD(companion0,0x3E8,void *)=matrices;FIELD(companion1,0x3E8,void *)=alternate_matrices;
    FIELD(data,0x10,float)=2;FIELD(data,0x14,float)=2;FIELD(data,0x260,float)=3;
    D_003F2D40=effect;D_0046A0F0.field00=registry;D_0046A0F0.field04=registry;D_0046A0F0.field08=registry+8;upper_result=registry;
}

float func_0029B940(float first,float second)
{float result;CHECK(atan_index<16);result=atan_values[atan_index++];emit(ATAN,0,0,bits(second),0,first);return result;}
static GeorgeActorBits64 pack(double value){union{double d;GeorgeActorBits64 u;}v;v.d=value;return v.u;}
static double unpack(GeorgeActorBits64 value){union{double d;GeorgeActorBits64 u;}v;v.u=value;return v.d;}
GeorgeActorBits64 func_00374848(float x){emit(CONVERT,0,0,0,0,x);return pack(x);}
s32 func_00373250(GeorgeActorBits64 x,GeorgeActorBits64 y){double a=unpack(x),b=unpack(y);emit(COMPARE,0,0,0,0,(float)a);return a<b?-1:(a>b?1:0);}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 x,GeorgeActorBits64 y){emit(SUBTRACT,0,0,0,0,0);return pack(unpack(x)-unpack(y));}
GeorgeActorBits64 func_00372C68(GeorgeActorBits64 x,GeorgeActorBits64 y){emit(ADD,0,0,0,0,0);return pack(unpack(x)+unpack(y));}
float func_003734F8(GeorgeActorBits64 x){float result=float_override?forced_float:(float)unpack(x);emit(FLOAT_RESULT,0,0,0,0,result);return result;}
void *func_002AEE60(u32 size){void *result;CHECK(size==12||size==0x1B4);result=size==12?(void *)&effect_record:effect;++allocated;emit(ALLOC,result,0,size,0,0);return result;}
void *func_002481F0(void *object){emit(EFFECT_CONSTRUCTOR,object,0,0,0,0);return object;}
s32 func_00100AA8(const void *first,const void *second){(void)first;(void)second;return 0;}
void **func_00100C30(void **begin,void **end,void *const *value,s32 (*compare)(const void *,const void *))
{CHECK(begin==registry&&end==registry);CHECK(*value==&effect_record&&compare==func_00100AA8);emit(UPPER,begin,0,0,0,0);return upper_result;}
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value){CHECK(range==&D_0046A0F0&&position==upper_result&&*value==&effect_record);emit(INSERT,range,0,0,0,0);}
void func_002BD340(void){}
s32 func_00396260(void (*function)(void)){CHECK(function==func_002BD340);emit(EXIT_REGISTRY,0,0,0,0,0);return 0;}
u32 *func_002BEBA0(u32 *output,const u8 *string){CHECK(string==D_0042DA78);*output=0x12345678;emit(HASH,output,(void *)string,0,0,0);return output;}
void *func_00251A88(void *object,u32 word){emit(EMIT,object,0,word,0,0);return objects[0];}
void func_002469C0(void *object){emit(CONFIGURE,object,0,0,0,0);}
void func_002455C0(void *object){emit(START,object,0,0,0,0);}
void func_002457D8(void *object){emit(STOP,object,0,0,0,0);}
void func_00245B08(void *object){emit(FREE,object,0,0,0,0);}
void func_001413B8(void *object,const GeorgeMathVec3 *input,float value){motion_value=*input;emit(MOTION,object,(void *)input,0,0,value);}
void func_00141378(void *object,float value){emit(DECAY,object,0,0,0,value);}
s32 func_00270510(void *object,u32 word,u32 mode0,u32 mode1,u32 owner,
                 GeorgeActorRequestCallback function,GeorgeGoalEntity *context,u32 call_word,u32 callback_word,float time)
{CHECK(mode0==0&&mode1==0&&call_word==0&&callback_word==0);emit(REQUEST,object,(void *)context,word,owner,time);CHECK(function==0||function==callback);return object==main1?-9:7;}
void func_002393F8(u32 object){emit(RELEASE,(void *)object,0,0,0,0);}
void *func_00210078(u32 word,u32 zero0,u32 zero1){void *result=lookup_result;CHECK(zero0==0&&zero1==0);emit(LOOKUP,result,0,word,0,0);return result;}
void *func_00236CB8(const void *source,void *reference,u32 mode,u32 zero0,u32 zero1)
{void *result;CHECK(created<4);CHECK(zero0==0&&zero1==0);result=objects[created++];emit(CREATE,result,(void *)source,mode,(u32)reference,0);return result;}
u32 func_002A6460(const void *object){u32 result=record_count;emit(COUNT,(void *)object,0,0,0,0);return result;}
void *func_002A6468(const void *object,s32 index){CHECK(index==0);emit(RECORDS,(void *)object,0,0,0,0);return (void *)object;}
void func_002A2200(GeorgeRotationMatrix *output,const GeorgeRotationMatrix *source,const GeorgeRotationMatrix *second)
{int i;for(i=0;i<16;++i)output->element[i]=(float)(i+1);emit(MATRIX,(void *)source,(void *)second,0,0,0);}
void func_002A1C08(void *output,const void *input){memcpy(&copied,input,64);emit(COPY,output,(void *)input,0,0,0);}
void func_002A1C60(const void *source,const GeorgeMathVec3 *input,GeorgeMathVec3 *output)
{CHECK(source==ADDRESS(actor,0xF0));CHECK(input==VECTOR(matrices,0x30));output->x=11;output->y=12;output->z=13;emit(TRANSFORM,(void *)source,(void *)input,0,0,0);}
void func_002A1C30(float *output){int i;for(i=0;i<16;++i)output[i]=(i%5)==0?1:0;emit(IDENTITY,output,0,0,0,0);}
void func_00235CD8(void *object,u32 key,u32 word){CHECK(key==0x9F79558F&&word==1);emit(NOTIFY,object,0,key,word,0);}

static void test_flags(void)
{unsigned i;GeorgeActorBits64 seed=0xFEDCBA9876543210ULL;for(i=0;i<80;++i){GeorgeActorBits64 mask;reset();seed=seed*0x100000001B3ULL+0x34567;mask=seed^0xA55AA55AA55AA55AULL;FIELD(actor,0x190,GeorgeActorBits64)=seed;func_0018FD30(actor,mask);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(seed|mask));func_0018FD40(actor,mask);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(seed&~mask));}}
static void test_movement(void)
{u32 state;GeorgeMathVec3 direction={2,3,4},position={5,6,7};for(state=0;state<44;++state){int allowed=state==0||state==2||state==4||state==29||state==30||state==40;reset();actor->field0C=state;FIELD(actor,0x58,float)=9;FIELD(actor,0x468,float)=88;CHECK(func_0018B710(actor,&direction,&position)==allowed);CHECK(FIELD(actor,0x468,float)==(allowed?9:88));CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(allowed?(1ULL<<49):0));if(allowed){CHECK(FIELD(actor,0x480,float)==4&&FIELD(actor,0x474,float)==7);}}
 reset();actor->field0C=0;FIELD(actor,0xD0,float)=2;FIELD(actor,0xD4,float)=-3;FIELD(actor,0xD8,float)=0;CHECK(func_0018B710(actor,0,0)==1);CHECK(FIELD(actor,0x478,float)==-2&&FIELD(actor,0x47C,float)==3);CHECK(bits(FIELD(actor,0x480,float))==0x80000000U);CHECK(FIELD(actor,0x46C,u32)==0&&FIELD(actor,0x470,u32)==0&&FIELD(actor,0x474,u32)==0);
 reset();FIELD(actor,0x474,float)=10;FIELD(actor,0x478,float)=20;FIELD(actor,0x47C,float)=30;FIELD(actor,0x468,float)=40;FIELD(actor,0x46C,float)=50;FIELD(actor,0x470,float)=60;CHECK(func_0018B710(actor,VECTOR(actor,0x474),VECTOR(actor,0x468))==1);CHECK(FIELD(actor,0x478,float)==10&&FIELD(actor,0x47C,float)==10&&FIELD(actor,0x480,float)==10);CHECK(FIELD(actor,0x46C,float)==0&&FIELD(actor,0x470,float)==0&&FIELD(actor,0x474,float)==0);
 reset();actor->field0C=0xFFFFFFFFU;CHECK(func_0018B710(actor,0,0)==0);}

static void request_hook(Event *e){if(e->kind==REQUEST&&e->object==companion0){FIELD(actor,0x1B0,void *)=main1;FIELD(actor,0x368,u32)=0x89AB;}}
static void test_requests(void)
{reset();hook=request_hook;CHECK(func_00192C58(actor,0x76543210,callback,actor,2.5f)==-9);CHECK(FIELD(actor,0x4E4,u32)==0x76543210);CHECK(count(REQUEST)==2);CHECK(nth(REQUEST,0)->object==companion0&&nth(REQUEST,0)->argument==0);CHECK(nth(REQUEST,0)->other==0xABCD);CHECK(nth(REQUEST,1)->object==main1&&nth(REQUEST,1)->argument==actor&&nth(REQUEST,1)->other==0x89AB);CHECK(nth(REQUEST,0)->scalar==2.5f&&nth(REQUEST,1)->scalar==2.5f);
 reset();FIELD(actor,0x1B0,void *)=0;CHECK(func_00192C58(actor,9,callback,actor,7)==0);CHECK(count(REQUEST)==0&&FIELD(actor,0x4E4,u32)==9);
 reset();FIELD(actor,0x1B4,void *)=0;CHECK(func_00192C58(actor,8,0,(GeorgeGoalEntity *)main1,scalar(0x80000000))==7);CHECK(count(REQUEST)==1&&nth(REQUEST,0)->argument==main1);CHECK(bits(nth(REQUEST,0)->scalar)==0x80000000);}

static void matrix_hook(Event *e){if(e->kind==COUNT){FIELD(actor,0x1B0,void *)=main1;FIELD(actor,0x1B4,void *)=companion1;}if(e->kind==RECORDS){FIELD(main1,0x3E8,void *)=reference_matrices;FIELD(companion1,0x3E8,void *)=reference_matrices;}}
static void test_find(void)
{int companion;for(companion=0;companion<2;++companion){reset();model_records[0x1D]=7;model_records[32+0x1D]=9;CHECK((companion?func_00192818(actor,9):func_00192748(actor,9))==matrices+1);CHECK(count(COUNT)==1&&count(RECORDS)==1);CHECK((companion?func_00192818(actor,0x109):func_00192748(actor,0x109))==0);
 reset();record_count=0xFFFFFFFFU;CHECK((companion?func_00192818(actor,3):func_00192748(actor,3))==0);CHECK(count(RECORDS)==0);
 reset();record_count=0;CHECK((companion?func_00192818(actor,3):func_00192748(actor,3))==0);
 reset();hook=matrix_hook;alternate_records[0x1D]=5;CHECK((companion?func_00192818(actor,5):func_00192748(actor,5))==reference_matrices);CHECK(nth(COUNT,0)->object==model_records&&nth(RECORDS,0)->object==alternate_records);
 reset();FIELD(main0,0x0C,u32)=3;FIELD(main0,0x380,void *)=model_records;FIELD(companion0,0x380,void *)=model_records;FIELD(main0,0x3F0,void *)=matrices;FIELD(companion0,0x3F0,void *)=matrices;model_records[0x1D]=3;CHECK((companion?func_00192818(actor,3):func_00192748(actor,3))==matrices);}
 reset();FIELD(actor,0x1B0,void *)=0;CHECK(func_00192748(actor,9)==0&&count(COUNT)==0);reset();FIELD(actor,0x1B4,void *)=0;CHECK(func_00192818(actor,9)==0&&count(COUNT)==0);}

static void release_hook(Event *e){if(e->kind==RELEASE){FIELD(actor,0x290,u32)=(u32)objects[2];FIELD(actor,0x228,void *)=objects[3];FIELD(actor,0x224,void *)=objects[3];}}
static void test_release(void)
{int side;reset();FIELD(actor,0x28C,u32)=(u32)objects[0];FIELD(actor,0x290,u32)=(u32)objects[1];hook=release_hook;func_00190F70(actor);CHECK(count(RELEASE)==2&&nth(RELEASE,1)->object==objects[2]);CHECK(FIELD(actor,0x28C,u32)==0&&FIELD(actor,0x290,u32)==0);
 reset();func_00190F70(actor);CHECK(count(RELEASE)==0);reset();FIELD(actor,0x298,u32)=(u32)objects[0];func_00191E50(actor);CHECK(count(RELEASE)==1&&FIELD(actor,0x298,u32)==0);
 for(side=0;side<2;++side){u32 slot=side?0x224:0x228,key=side?0x464:0x460;reset();FIELD(actor,slot,void *)=objects[0];FIELD(objects[0],8,u32)=0x76543210;hook=release_hook;if(side)func_001913E0(actor);else func_001913A0(actor);CHECK(FIELD(actor,key,u32)==0x76543210&&FIELD(actor,slot,void *)==0);CHECK(count(RELEASE)==1&&nth(RELEASE,0)->object==objects[0]);reset();FIELD(actor,key,u32)=88;if(side)func_001913E0(actor);else func_001913A0(actor);CHECK(count(RELEASE)==0&&FIELD(actor,key,u32)==88);}}

static void copy_hook(Event *e){if(e->kind==COPY){FIELD(actor,0x228,void *)=objects[3];FIELD(actor,0x224,void *)=objects[3];FIELD(actor,0x298,void *)=objects[3];FIELD(objects[0],0xA0,u32)=0x400;}}
static void test_create(void)
{int side;reset();func_00190EB0(actor,77);CHECK(count(CREATE)==2&&count(REFERENCE)==1);CHECK(nth(CREATE,0)->argument==ADDRESS(actor,0xB0)&&nth(CREATE,0)->word==0);CHECK(FIELD(actor,0x28C,void *)==objects[0]&&FIELD(actor,0x290,void *)==objects[1]);CHECK(nth(REFERENCE,0)->object==ref+12);
 reset();lookup_result=0;FIELD(actor,0x28C,u32)=(u32)objects[0];func_00190EB0(actor,88);CHECK(count(RELEASE)==1&&count(CREATE)==0&&FIELD(actor,0x28C,u32)==0);
 reset();FIELD(actor,0x298,u32)=(u32)objects[3];func_00191DC8(actor,99);CHECK(count(RELEASE)==1&&count(CREATE)==1&&FIELD(actor,0x298,void *)==objects[0]);CHECK(nth(LOOKUP,0)->word==99&&nth(CREATE,0)->word==0);
 for(side=0;side<2;++side){u32 slot=side?0x224:0x228;reset();model_records[0x1D]=side?0x61:0x62;hook=copy_hook;if(side)func_001912C8(actor,99);else func_001911F0(actor,99);CHECK(count(MATRIX)==1&&nth(MATRIX,0)->object==matrices);CHECK(nth(CREATE,0)->word==1&&nth(LOOKUP,0)->word==99);CHECK(count(COPY)==1&&count(REFERENCE)==1);CHECK(FIELD(actor,slot,void *)==objects[3]);CHECK(FIELD(objects[0],0xA0,u32)==0x500&&FIELD(objects[3],0xA0,u32)==0);
 reset();FIELD(actor,slot,void *)=objects[3];FIELD(objects[3],8,u32)=123;if(side)func_001912C8(actor,0);else func_001911F0(actor,0);CHECK(nth(LOOKUP,0)->word==123);CHECK(count(RELEASE)==1);
 reset();if(side)func_001912C8(actor,0);else func_001911F0(actor,0);CHECK(event_count==0);}}

static void test_dispatch(void)
{GeorgeGoalMember *member;GeorgeGoalVirtualVoid *pair;reset();func_00190E00(actor,3);CHECK(actor->field0C==3&&count(ENTER)==0);
 reset();member=(GeorgeGoalMember *)(D_003F83F0+5*28);member->selector=-1;member->adjustment=-20;member->target.direct=mock_enter;func_00190E00(actor,5);CHECK(actor->field0C==5&&nth(ENTER,0)->object==ADDRESS(actor,-20));
 reset();member=(GeorgeGoalMember *)(D_003F83F0+7*28);member->selector=2;member->adjustment=32760;member->target.vtable_offset=0x24;FIELD(actor,0x24,void *)=ref_table;pair=(GeorgeGoalVirtualVoid *)(ref_table+8);pair->adjustment=32760;pair->invoke=mock_enter;func_00190E00(actor,7);CHECK(nth(ENTER,0)->object==ADDRESS(actor,65520));}

static void test_attachment(void)
{int mode;reset();func_00191D08(actor);CHECK(event_count==0);reset();FIELD(data,0x10C,u32)=77;model_records[0x1D]=0x29;hook=copy_hook;func_00191D08(actor);CHECK(nth(LOOKUP,0)->word==77&&count(TRANSFORM)==1&&count(IDENTITY)==1);CHECK(copied.element[12]==11&&copied.element[13]==12&&copied.element[14]==13&&copied.element[15]==1);CHECK(nth(NOTIFY,0)->object==objects[3]&&FIELD(objects[0],0xA0,u32)==0x500);
 for(mode=0;mode<2;++mode){reset();FIELD(actor,0x938,s16)=399;func_00191670(actor,mode);CHECK(FIELD(actor,0x938,s16)==(mode?400:450)&&event_count==0);reset();FIELD(actor,0x938,s16)=400;FIELD(data,0x10C,u32)=77;func_00191670(actor,mode);CHECK(event_count==0&&FIELD(actor,0x938,s16)==400);reset();FIELD(actor,0x938,s16)=-1;FIELD(data,0x10C,u32)=77;model_records[0x1D]=0x29;func_00191670(actor,mode);CHECK(count(NOTIFY)==1&&FIELD(actor,0x938,s16)==(mode?400:450));}}

static void angle_hook(Event *e)
{if(e->kind==ATAN){FIELD(actor,0x484,void *)=&reference_matrices[atan_index];reference_matrices[atan_index].element[8]=(float)atan_index;reference_matrices[atan_index].element[10]=(float)(atan_index+10);}if(e->kind==CONTROL&&count(CONTROL)==2){CHECK(FIELD(actor,0x490,float)==54);CHECK(FIELD(actor,0x488,float)==12);CHECK(FIELD(actor,0x48C,float)==55);}}
static void test_reference(void)
{GeorgeMathVec3 input={2,3,4};int path;for(path=0;path<3;++path){reset();FIELD(actor,0x190,GeorgeActorBits64)=~0ULL;atan_values[0]=path==0?4:0;atan_values[1]=path==1?-4:1;atan_values[2]=2;atan_values[path==0?2:3]=path==0?4:0;atan_values[path==0?3:4]=path==1?-4:1;atan_values[path==0?4:5]=2;func_00185EA0(actor,&reference_matrices[0],1,0,0);CHECK(count(CONTROL)==2&&count(ATAN)==(path==0?4:6));CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x4000ULL)==0);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&(1ULL<<41))!=0&&(FIELD(actor,0x190,GeorgeActorBits64)&(1ULL<<40))==0);CHECK(FIELD(actor,0x494,u32)==0&&FIELD(actor,0x498,u32)==0&&FIELD(actor,0x49C,u32)==0);CHECK(FIELD(actor,0x504,float)==FIELD(actor,0x60,float));}
 reset();reference_matrices[0].element[12]=4;reference_matrices[0].element[13]=5;reference_matrices[0].element[14]=6;reference_matrices[0].element[8]=10;reference_matrices[0].element[9]=12.5f;reference_matrices[0].element[10]=15;hook=angle_hook;atan_values[0]=4;atan_values[1]=1;/* The second matrix becomes the basis source after two calls. */reference_matrices[2].element[12]=4;reference_matrices[2].element[13]=5;reference_matrices[2].element[14]=6;reference_matrices[2].element[8]=10;reference_matrices[2].element[9]=12.5f;reference_matrices[2].element[10]=15;func_00185EA0(actor,reference_matrices,0,1,&input);CHECK(nth(ATAN,0)->word==bits(10));CHECK(nth(ATAN,1)->word==bits(1));CHECK(FIELD(actor,0x494,float)==2&&FIELD(actor,0x498,float)==3&&FIELD(actor,0x49C,float)==4);
 reset();FIELD(actor,0x490,float)=7;FIELD(actor,0x494,float)=8;FIELD(actor,0x498,float)=9;func_00185EA0(actor,reference_matrices,0,0,VECTOR(actor,0x490));CHECK(FIELD(actor,0x494,float)==7&&FIELD(actor,0x498,float)==7&&FIELD(actor,0x49C,float)==7);
 reset();atan_values[0]=scalar(0x7FC00000);atan_values[1]=scalar(0x7FC00000);atan_values[2]=scalar(0x7FC00000);func_00185EA0(actor,reference_matrices,0,0,0);CHECK(count(ATAN)==6);}

static void effect_hook(Event *e)
{if(e->kind==CONVERT){FIELD(object_source,0x74,float)=100;FIELD(object_source,0x78,float)=200;FIELD(object_source,0x7C,float)=300;}
 if(e->kind==HASH)D_003F2D40=alternate_effect;
 if(e->kind==CONFIGURE){CHECK(FIELD(actor,0x2F4,void *)==objects[0]);FIELD(actor,0x2F4,void *)=objects[1];}
 if(e->kind==STOP)FIELD(actor,0x2F4,void *)=objects[2];}
static void test_motion(void)
{GeorgeMathVec3 input={3,4,0};reset();FIELD(object_source,0x74,float)=-2;FIELD(object_source,0x78,float)=-3;FIELD(object_source,0x7C,float)=-4;hook=effect_hook;func_0018D8A0(actor,&input);CHECK(nth(CONVERT,0)->scalar==-4&&nth(CONVERT,1)->scalar==-3&&nth(CONVERT,2)->scalar==-2);CHECK(count(SUBTRACT)==3&&nth(FLOAT_RESULT,0)->scalar==9);CHECK(nth(EMIT,0)->object==effect&&nth(START,0)->object==objects[1]);CHECK(nth(MOTION,0)->object==object_source);near_value(nth(MOTION,0)->scalar,7.5f);
 reset();FIELD(object_source,0x9C,u32)=1;input.x=1;input.y=0;func_0018D8A0(actor,&input);CHECK(count(DECAY)==1&&bits(nth(DECAY,0)->scalar)==0x3F733333);
 reset();FIELD(actor,0x2F4,void *)=objects[0];hook=effect_hook;func_0018D8A0(actor,&input);CHECK(count(STOP)==1&&nth(FREE,0)->object==objects[2]&&FIELD(actor,0x2F4,void *)==0);
 reset();float_override=1;forced_float=-1;FIELD(actor,0x2F4,void *)=objects[0];FIELD(data,0x14,float)=0;input.x=2;func_0018D8A0(actor,&input);CHECK(count(STOP)==1&&count(MOTION)==1&&count(FREE)==1);
 reset();D_003F2D40=0;FIELD(object_source,0x74,float)=1;FIELD(object_source,0x9C,u32)=1;func_0018D8A0(actor,&input);CHECK(allocated==2&&count(UPPER)==1&&count(EXIT_REGISTRY)==1&&count(EMIT)==1);CHECK(registry[0]==&effect_record&&D_0046A0F0.field04==registry+1);CHECK(effect_record.field00==9&&effect_record.field04==D_00421160&&effect_record.field08==effect);
 reset();FIELD(data,0x14,float)=scalar(0x7FC00000);FIELD(object_source,0x9C,u32)=1;func_0018D8A0(actor,&input);CHECK(count(DECAY)==1&&count(MOTION)==0);}

static void angle_alias_hook(Event *e)
{
    if(e->kind==CONTROL&&count(CONTROL)==2) {
        /* Matrix translated30 initially aliases output488. Each full basis
         * iteration loads all components, then stores X/Z/Y before the next. */
        CHECK(FIELD(actor,0x488,float)==91);
        CHECK(FIELD(actor,0x48C,float)==122);
        CHECK(FIELD(actor,0x490,float)==153);
    }
}
static void test_aliases(void)
{
    GeorgeMathVec3 input={2,3,4};
    unsigned i;
    reset();FIELD(actor,0x488,float)=1;FIELD(actor,0x48C,float)=2;FIELD(actor,0x490,float)=3;
    FIELD(actor,0x458,float)=3;FIELD(actor,0x45C,float)=4;FIELD(actor,0x460,float)=5;
    FIELD(actor,0x468,float)=12;FIELD(actor,0x46C,float)=16;FIELD(actor,0x470,float)=20;
    FIELD(actor,0x478,float)=12;FIELD(actor,0x47C,float)=16;FIELD(actor,0x480,float)=20;
    hook=angle_alias_hook;func_00185EA0(actor,ADDRESS(actor,0x458),0,0,&input);
    CHECK(FIELD(actor,0x488,float)==91&&FIELD(actor,0x48C,float)==122&&FIELD(actor,0x490,float)==153);
    /* Multiple independent finite vectors check the length threshold itself,
     * both distinct outcomes and original coefficient operand ordering. */
    for(i=1;i<=24;++i) {
        float length=(float)(5*i);
        reset();input.x=(float)(3*i);input.y=(float)(4*i);input.z=0;
        FIELD(data,0x14,float)=length;
        FIELD(object_source,0x9C,u32)=1;
        func_0018D8A0(actor,&input);
        CHECK(count(MOTION)==0&&count(DECAY)==1);
        reset();FIELD(data,0x14,float)=length-0.01f;
        FIELD(data,0x10,float)=4;FIELD(data,0x260,float)=2;
        func_0018D8A0(actor,&input);
        CHECK(count(DECAY)==0&&count(MOTION)==1);
        near_value(nth(MOTION,0)->scalar,2.0f*(length/4.0f));
        CHECK(motion_value.x==input.x&&motion_value.y==input.y&&motion_value.z==0);
    }
}

int main(void)
{test_flags();test_movement();test_requests();test_find();test_release();test_create();test_dispatch();test_attachment();test_reference();test_motion();test_aliases();printf("actor_controls native: %d checks passed\n",checks);return 0;}
