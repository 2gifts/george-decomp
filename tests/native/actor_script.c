/* Independent asset-free VM slot and callback mutation specifications. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_script.c"

enum { LOOKUP=1, ERROR, VECTOR, TABLE, RELEASE, RETAIN, FIND, CREATE,
       REFERENCE, CANCEL_HINT, HINT, RENDER, FREEZE, SHEATHE, UNSHEATHE,
       SET_FLAG, CLEAR_FLAG, ZONE };
typedef struct Event {int kind;void *object,*argument;u32 word;float value;} Event;
static Event events[64];
static int checks,event_count;
static u8 entity_storage[0xB00] __attribute__((aligned(16))), data[0x600], gender[0x200];
static u8 object[0x300], reference[0x100], reference_table[0x40];
static GeorgeGoalEntity *actor=(GeorgeGoalEntity *)entity_storage;
static GeorgeGoalEntity *lookup_result;
static GeorgeDeimosPoolNode nodes[4];
static GeorgeDeimosValue args[4], alternate_args[4];
static void (*hook)(Event *);
static s32 zone_result;
GeorgeDeimosValue D_00474748[512] __attribute__((aligned(16)));
GeorgeDeimosValue *D_00474F48;
const char D_0042CF38[]="A",D_0042CF98[]="B",D_0042CFF8[]="C",D_0042D0D8[]="D";
const char D_0042D300[]="E",D_0042D330[]="F",D_0042D370[]="G",D_0042D3B8[]="H",D_0042D3E8[]="I";
static void check(int okay,const char *label,int line){++checks;if(!okay){fprintf(stderr,"actor_script:%d %s\n",line,label);exit(1);}}
#define CHECK(x) check((x),#x,__LINE__)
static u32 bits(float x){union{float f;u32 u;}v;v.f=x;return v.u;}
static float scalar(u32 u){union{float f;u32 u;}v;v.u=u;return v.f;}
static Event *emit(int kind,void *object0,void *argument,u32 word,float value)
{Event *e;CHECK(event_count<64);e=&events[event_count++];e->kind=kind;e->object=object0;e->argument=argument;e->word=word;e->value=value;if(hook)hook(e);return e;}
static int count(int kind){int i,n=0;for(i=0;i<event_count;++i)if(events[i].kind==kind)++n;return n;}
static Event *nth(int kind,int n){int i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&n--==0)return &events[i];CHECK(0);return 0;}
static void mock_reference(void *self,u32 word){emit(REFERENCE,self,0,word,0);}
static void reset(void)
{
    memset(entity_storage,0,sizeof(entity_storage));memset(data,0,sizeof(data));memset(gender,0,sizeof(gender));
    memset(object,0,sizeof(object));memset(reference,0,sizeof(reference));memset(reference_table,0,sizeof(reference_table));
    memset(args,0,sizeof(args));memset(alternate_args,0,sizeof(alternate_args));memset(nodes,0,sizeof(nodes));memset(events,0,sizeof(events));
    memset(D_00474748,0xA5,sizeof(D_00474748));event_count=0;hook=0;lookup_result=actor;zone_result=-9;
    actor->field18=(GeorgeGoalEntityData *)data;FIELD(actor,0x1AC,void *)=gender;
    D_00474F48=args;
    FIELD(reference,0x20,void *)=reference_table;
    ((GeorgeGoalVirtualWord *)(reference_table+0x10))->adjustment=-4;
    ((GeorgeGoalVirtualWord *)(reference_table+0x10))->invoke=mock_reference;
}
void *func_002D0B48(u32 key){void *result=lookup_result;CHECK(key==0x69F0BC67);emit(LOOKUP,result,0,key,0);return result;}
void func_002CC938(const char *format,...){emit(ERROR,(void *)format,0,0,0);}
GeorgeDeimosPoolNode *func_002D0178(const GeorgeMathVec3 *vector,float angle){CHECK(vector==(const GeorgeMathVec3 *)ADDRESS(actor,0x40));emit(VECTOR,(void *)vector,0,bits(vector->x),angle);return nodes;}
GeorgeDeimosPoolNode *func_002D0790(GeorgeScriptObject *source){emit(TABLE,source,0,0,0);return nodes+1;}
void func_002CD130(GeorgeDeimosPoolNode *source){emit(RELEASE,source,0,0,0);}
void func_002CD0B8(GeorgeDeimosPoolNode *source){emit(RETAIN,source,0,0,0);}
void func_00191E50(GeorgeGoalEntity *entity){CHECK(entity==actor);emit(RELEASE,FIELD(entity,0x298,void *),0,0,0);FIELD(entity,0x298,void *)=0;}
void *func_00210078(u32 key,u32 zero0,u32 zero1){CHECK(zero0==0&&zero1==0);emit(FIND,0,0,key,0);return reference;}
void *func_00236CB8(const void *source,void *ref,u32 mode,u32 zero0,u32 zero1){CHECK(ref==reference&&mode==0&&zero0==0&&zero1==0);emit(CREATE,object,(void *)source,0,0);return object;}
void func_00195260(GeorgeGoalEntity *entity){CHECK(entity==actor);emit(CANCEL_HINT,entity,0,0,0);}
void func_001951C0(GeorgeGoalEntity *entity,u32 word,float height){CHECK(entity==actor);emit(HINT,entity,0,word,height);}
void func_00178D10(GeorgeGoalEntity *entity,u32 word){CHECK(entity==actor);emit(RENDER,entity,0,word,0);}
void func_00178E80(GeorgeGoalEntity *entity,u32 word,float height){CHECK(entity==actor);emit(FREEZE,entity,0,word,height);}
s32 func_00173648(GeorgeGoalEntity *entity){CHECK(entity==actor);emit(SHEATHE,entity,0,0,0);return 1;}
s32 func_00173720(GeorgeGoalEntity *entity){CHECK(entity==actor);emit(UNSHEATHE,entity,0,0,0);return 1;}
void func_0018FD30(GeorgeGoalEntity *entity,GeorgeActorBits64 mask){FIELD(entity,0x190,GeorgeActorBits64)|=mask;emit(SET_FLAG,entity,0,(u32)mask,0);}
void func_0018FD40(GeorgeGoalEntity *entity,GeorgeActorBits64 mask){FIELD(entity,0x190,GeorgeActorBits64)&=~mask;emit(CLEAR_FLAG,entity,0,(u32)mask,0);}
s32 func_0014F438(void *source,u32 key){s32 result=zone_result;emit(ZONE,source,0,key,0);return result;}

typedef void (*Callback)(s32,s32);
static Callback all_callbacks[]={func_00192E70,func_00192ED0,func_00192F40,func_00192FA0,func_00193010,
func_00193070,func_001930F8,func_001931B0,func_00193218,func_00193298,func_00193348,func_001933A0,
func_001933D0,func_00193470,func_001934D0,func_00193558,func_001935D8,func_00193608,func_00193638,
func_001936C0,func_00193778,func_00193830,func_001938D8};
static int clears_when_absent(int i){return i==5||i==6||i==12||i==14||i==15||i==18||i==19||i==20||i==21;}
static void test_absent(void)
{int i;for(i=0;i<23;++i){reset();lookup_result=0;all_callbacks[i](99,3);CHECK(count(LOOKUP)==1&&event_count==1);CHECK(D_00474748[3].payload.bits==0xA5A5A5A5);CHECK(D_00474748[3].tag==(clears_when_absent(i)?0:0xA5A5));CHECK(D_00474748[3].subtype==(clears_when_absent(i)?0:0xA5A5));CHECK(D_00474748[2].tag==0xA5A5&&D_00474748[4].tag==0xA5A5);
 reset();lookup_result=0;all_callbacks[i](0,-1);CHECK(D_00474748[0].tag==0xA5A5&&D_00474748[3].tag==0xA5A5);}}
static void result_slot(u16 tag,u32 payload){CHECK(D_00474748[3].tag==tag&&D_00474748[3].subtype==0&&D_00474748[3].payload.bits==payload);}
static void empty_slot(void){result_slot(0,0xA5A5A5A5);}
static void test_queries(void)
{reset();FIELD(actor,0x19C,u32)=0xFA123456;func_00192E70(0,3);result_slot(6,0xFA123456);
 reset();FIELD(gender,0x164,u32)=0xBC7890;func_00192F40(0,3);result_slot(6,0xBC7890);
 reset();FIELD(data,0xA8,u32)=0x103030;func_00193470(0,3);result_slot(6,0x103030);
 reset();FIELD(actor,0x58,float)=2;func_00192FA0(0,3);result_slot(2,bits(2*57.2957763671875f));
 reset();FIELD(actor,0x36C,float)=scalar(0x80000000);func_00193010(0,3);result_slot(2,0x80000000);
 reset();actor->field0C=13;func_001931B0(0,3);result_slot(1,1);
 reset();actor->field0C=0x1000000D;func_001931B0(0,3);result_slot(1,0);
 reset();FIELD(actor,0x40,float)=9;FIELD(actor,0x58,float)=2.5f;func_00192ED0(0,3);result_slot(4,(u32)nodes);CHECK(count(VECTOR)==1&&nth(VECTOR,0)->word==bits(9)&&nth(VECTOR,0)->value==2.5f);
 reset();func_00192ED0(0,-1);CHECK(count(VECTOR)==1&&D_00474748[3].tag==0xA5A5);
 reset();actor->field0C=14;FIELD(actor,0x730,void *)=object;func_001933D0(0,3);result_slot(4,(u32)(nodes+1));CHECK(nth(TABLE,0)->object==object);
 reset();actor->field0C=0x1000000E;func_001933D0(0,3);empty_slot();CHECK(count(TABLE)==0);}
static void error_hook(Event *e){if(e->kind==ERROR)D_00474748[3].payload.bits=0x89ABCDEF;}
static void test_setters(void)
{unsigned i;u16 tags[]={0,1,2,3,4,5,6,7,0xFFFF,0x8002};for(i=0;i<sizeof(tags)/sizeof(tags[0]);++i){reset();args[1].tag=tags[i];args[1].payload.scalar=0.75f;FIELD(actor,0x36C,float)=0.25f;func_00193070(0,3);CHECK(FIELD(actor,0x36C,float)==(tags[i]==2?0.75f:0.25f));CHECK(count(ERROR)==(tags[i]!=2));empty_slot();
 reset();args[1].tag=tags[i];args[1].payload.scalar=3.5f;FIELD(actor,0x428,float)=4;func_00193638(0,3);CHECK(FIELD(actor,0x428,float)==(tags[i]==2?3.5f:4));CHECK(count(ERROR)==(tags[i]!=2));empty_slot();}
 reset();args[1].tag=1;hook=error_hook;func_00193070(0,3);result_slot(0,0x89ABCDEF);CHECK(nth(ERROR,0)->object==(void *)D_0042CF38);
 reset();args[1].tag=1;func_00193070(0,-1);CHECK(count(ERROR)==1&&D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=2;args[1].payload.bits=0x7FC00000;func_00193070(0,3);CHECK(bits(FIELD(actor,0x36C,float))==0x7FC00000);}
static void test_add_health(void)
{float old[]={0.25f,0.25f,0.75f,0.0f,0.5f};float amount[]={0.25f,-1.0f,1.0f,scalar(0x80000000),scalar(0x7FC00000)};u32 expected[]={0x3F000000,0,0x3F800000,0,0};unsigned i;for(i=0;i<5;++i){reset();FIELD(actor,0x36C,float)=old[i];args[1].tag=2;args[1].payload.scalar=amount[i];func_001930F8(0,3);CHECK(bits(FIELD(actor,0x36C,float))==expected[i]);empty_slot();}
 reset();args[1].tag=6;FIELD(actor,0x36C,float)=0.25f;func_001930F8(0,3);CHECK(count(ERROR)==1&&FIELD(actor,0x36C,float)==0.25f);CHECK(nth(ERROR,0)->object==(void *)D_0042CF98);empty_slot();}
static void test_flags(void)
{unsigned i;u32 inputs[]={0,1,0xFFFFFFFF,0x80000000};for(i=0;i<4;++i){GeorgeActorBits64 initial=0xFEDCBA9876543210ULL;reset();FIELD(actor,0x190,GeorgeActorBits64)=initial;args[1].tag=1;args[1].payload.bits=inputs[i];func_00193218(0,3);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(inputs[i]?initial|0x200000ULL:initial&~0x200000ULL));CHECK(D_00474748[3].tag==0xA5A5);
 reset();FIELD(actor,0x190,GeorgeActorBits64)=initial;args[1].tag=1;args[1].payload.bits=inputs[i];func_00193830(0,3);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(inputs[i]?initial|0x20ULL:initial&~0x20ULL));CHECK(count(inputs[i]?SET_FLAG:CLEAR_FLAG)==1);empty_slot();}
 reset();args[1].tag=0x8001;func_00193218(0,3);CHECK(count(ERROR)==1&&nth(ERROR,0)->object==(void *)D_0042CFF8&&D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=2;func_00193830(0,3);CHECK(count(ERROR)==1&&nth(ERROR,0)->object==(void *)D_0042D3B8);empty_slot();}
static void release_hook(Event *e)
{if(e->kind==RELEASE){D_00474F48=alternate_args;FIELD(actor,0x3AC,void *)=nodes+3;FIELD(actor,0x3B0,void *)=nodes+3;}if(e->kind==RETAIN){CHECK(FIELD(actor,0x3AC,void *)==e->object||FIELD(actor,0x3B0,void *)==e->object);}}
static void test_callbacks(void)
{int which;for(which=0;which<2;++which){Callback function=which?func_00193778:func_001936C0;u32 member=which?0x3B0:0x3AC;reset();args[1].tag=5;args[1].payload.pointer=nodes;alternate_args[1].tag=6;alternate_args[1].payload.pointer=nodes+2;FIELD(actor,member,void *)=nodes+1;hook=release_hook;function(0,3);CHECK(count(RELEASE)==1&&nth(RELEASE,0)->object==nodes+1);CHECK(count(RETAIN)==1&&nth(RETAIN,0)->object==nodes+2);CHECK(FIELD(actor,member,void *)==nodes+2);empty_slot();
 reset();args[1].tag=5;args[1].payload.pointer=0;function(0,3);CHECK(count(RELEASE)==0&&count(RETAIN)==1&&nth(RETAIN,0)->object==0);empty_slot();
 reset();args[1].tag=6;FIELD(actor,member,void *)=nodes+1;function(0,3);CHECK(count(RELEASE)==0&&count(RETAIN)==0&&count(ERROR)==1&&FIELD(actor,member,void *)==nodes+1);CHECK(nth(ERROR,0)->object==(void *)(which?D_0042D370:D_0042D330));empty_slot();}}
static void hint_hook(Event *e){if(e->kind==CANCEL_HINT)D_00474F48=alternate_args;if(e->kind==RELEASE){args[1].payload.bits=999;D_00474F48=alternate_args;}}
static void test_actions(void)
{reset();args[1].payload.bits=10;args[2].payload.scalar=1;alternate_args[1].payload.bits=20;alternate_args[2].payload.scalar=2;hook=hint_hook;func_00193348(0,3);CHECK(count(CANCEL_HINT)==1&&nth(HINT,0)->word==20&&nth(HINT,0)->value==2);CHECK(D_00474748[3].tag==0xA5A5);
 reset();func_001933A0(0,3);CHECK(count(CANCEL_HINT)==1&&D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=1;args[1].payload.bits=0x80000000;func_001934D0(0,3);CHECK(count(RENDER)==1&&nth(RENDER,0)->word==0x80000000);empty_slot();
 reset();args[1].tag=2;func_001934D0(0,3);CHECK(count(ERROR)==1&&nth(ERROR,0)->object==(void *)D_0042D0D8);empty_slot();
 reset();args[1].payload.bits=10;args[2].payload.scalar=2.5f;func_00193558(0,3);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x40000000ULL)!=0&&nth(FREEZE,0)->word==10&&nth(FREEZE,0)->value==2.5f);empty_slot();
 reset();func_001935D8(0,3);CHECK(count(SHEATHE)==1&&D_00474748[3].tag==0xA5A5);reset();func_00193608(0,3);CHECK(count(UNSHEATHE)==1&&D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=6;args[1].payload.bits=77;alternate_args[1].payload.bits=88;FIELD(actor,0x298,void *)=nodes+1;hook=hint_hook;func_00193298(0,3);CHECK(nth(FIND,0)->word==77&&count(RELEASE)==1);CHECK(FIELD(actor,0x298,void *)==object&&nth(REFERENCE,0)->object==reference-4);CHECK(D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=5;func_00193298(0,3);CHECK(count(CREATE)==0&&count(ERROR)==0&&D_00474748[3].tag==0xA5A5);}
static void zone_hook(Event *e){if(e->kind==ZONE)D_00474748[3].payload.bits=123;}
static void test_zone(void)
{reset();args[1].tag=6;args[1].payload.bits=0xFEEDBEEF;FIELD(actor,0x374,void *)=object;hook=zone_hook;func_001938D8(0,3);result_slot(1,(u32)-9);CHECK(nth(ZONE,0)->object==object&&nth(ZONE,0)->word==0xFEEDBEEF);
 reset();args[1].tag=1;func_001938D8(0,3);CHECK(count(ERROR)==1&&nth(ERROR,0)->object==(void *)D_0042D3E8&&D_00474748[3].tag==0xA5A5);
 reset();args[1].tag=6;func_001938D8(0,-1);CHECK(count(ZONE)==1&&D_00474748[3].tag==0xA5A5);}
static void test_aliases(void)
{
    /* Output tag at actor58 writes before payload for word queries. Its input
     * is fully captured before publication, so it cannot be read after tag. */
    reset();lookup_result=(GeorgeGoalEntity *)D_00474748;
    FIELD(lookup_result,0x19C,u32)=0xCAFEBABE;func_00192E70(0,0x198/8);
    CHECK(D_00474748[0x198/8].payload.bits==0xCAFEBABE);
    /* An old callback member aliases the fresh argument payload. Clearing it
     * after release must cause a null replacement, still passed to retain. */
    reset();D_00474F48=(GeorgeDeimosValue *)ADDRESS(actor,0x3A0);D_00474F48[1].tag=5;
    FIELD(actor,0x3AC,void *)=nodes+1;func_001936C0(0,3);
    CHECK(count(RELEASE)==1&&count(RETAIN)==1&&nth(RETAIN,0)->object==0);
    CHECK(FIELD(actor,0x3AC,void *)==0);empty_slot();
    /* Final empty result preserves payload, including when the result aliases
     * the actor field just updated by the scalar setter. */
    reset();lookup_result=(GeorgeGoalEntity *)D_00474748;args[1].tag=2;args[1].payload.scalar=0.75f;
    func_00193070(0,0x368/8);CHECK(FIELD(lookup_result,0x36C,float)==0.75f);
    CHECK(FIELD(lookup_result,0x368,u32)==0);
}
int main(void)
{test_absent();test_queries();test_setters();test_add_health();test_flags();test_callbacks();test_actions();test_zone();test_aliases();printf("actor_script native: %d checks passed\n",checks);return 0;}
