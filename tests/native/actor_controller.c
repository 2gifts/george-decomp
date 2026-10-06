/* Authored asset-free callback/alias observations of fourteen whole routines. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_controller.c"

typedef union Storage {GeorgeActorBits64 alignment;u8 bytes[0x1000];} Storage;
static Storage object_store, physical_store, alternate_store, actor_store, config_store, wheel_store, child_store, other_child_store;
static void *object;
static GeorgeGoalEntity *actor;
static GeorgeDeimosValue arguments[4], alternate_arguments[4];
static GeorgeDeimosPoolNode pool_nodes[3];
static u8 actor_table[0x100];
static GeorgeRotationMatrix initial_matrix;
const u8 D_0042BD08[8]={0},D_0042BF90[120]={0},D_0042BF48[72]={0},D_0040DC60[8]={0};
const GeorgeMathVec3 D_0042BD90={0,1,0};
GeorgeDeimosPoolNode *D_003F83D8;
GeorgeDeimosValue *D_00474F48;
void *D_004961F4;

typedef struct Event {u32 address;GeorgeActorBits64 argument[16],result;u32 snapshot[40];} Event;
typedef struct Reply {u32 address;GeorgeActorBits64 result;} Reply;
static Event events[512];static Reply replies[32];
static int event_count,reply_count,checks;
static float curve_x,curve_y,virtual_result,matrix_up_y;
static void (*hook)(Event *);
static u32 float_bits(float x){union{float f;u32 u;}v;v.f=x;return v.u;}
static float float_value(u32 x){union{float f;u32 u;}v;v.u=x;return v.f;}
static GeorgeActorBits64 double_bits(double x){union{double d;GeorgeActorBits64 u;}v;v.d=x;return v.u;}
static double double_value(GeorgeActorBits64 x){union{double d;GeorgeActorBits64 u;}v;v.u=x;return v.d;}
static void check(int ok,const char *name,int line){++checks;if(!ok){fprintf(stderr,"FAIL %s:%d (%d events)\n",name,line,event_count);exit(1);}}
#define CHECK(c) check((c),#c,__LINE__)
static void reply(u32 address,GeorgeActorBits64 result){int i;for(i=0;i<reply_count;++i)if(replies[i].address==address){replies[i].result=result;return;}CHECK(reply_count<32);replies[reply_count].address=address;replies[reply_count++].result=result;}
static Event *event(u32 address){Event *e;int i;CHECK(event_count<512);e=&events[event_count++];memset(e,0,sizeof(*e));e->address=address;for(i=0;i<reply_count;++i)if(replies[i].address==address)e->result=replies[i].result;return e;}
static void finish(Event *e){if(hook!=0)hook(e);}
static int count(u32 address){int i,n=0;for(i=0;i<event_count;++i)if(events[i].address==address)++n;return n;}
static Event *nth(u32 address,int n){int i;for(i=0;i<event_count;++i)if(events[i].address==address&&n--==0)return &events[i];return 0;}
static void snapshot(Event *e,u32 offset,const void *p,u32 size){CHECK(offset*4+size<=sizeof(e->snapshot));memcpy(e->snapshot+offset,p,size);}
static void identity(GeorgeRotationMatrix *m){int i;memset(m,0,sizeof(*m));for(i=0;i<4;++i)m->element[i*5]=1;}
static void set_vector(void *p,u32 offset,float x,float y,float z){FIELD(p,offset,float)=x;FIELD(p,offset+4,float)=y;FIELD(p,offset+8,float)=z;}
static void check_vector(const void *p,u32 offset,float x,float y,float z){CHECK(fabsf(FIELD(p,offset,float)-x)<0.00001f);CHECK(fabsf(FIELD(p,offset+4,float)-y)<0.00001f);CHECK(fabsf(FIELD(p,offset+8,float)-z)<0.00001f);}
static float virtual_float(void *p){Event *e=event(0xF0000001);e->argument[0]=(u32)p;e->result=float_bits(virtual_result);finish(e);return float_value((u32)e->result);}
static void reset(void)
{
 memset(&object_store,0,sizeof(object_store));memset(&physical_store,0,sizeof(physical_store));memset(&alternate_store,0,sizeof(alternate_store));
 memset(&actor_store,0,sizeof(actor_store));memset(&config_store,0,sizeof(config_store));memset(&wheel_store,0,sizeof(wheel_store));memset(&child_store,0,sizeof(child_store));memset(&other_child_store,0,sizeof(other_child_store));
 memset(arguments,0,sizeof(arguments));memset(alternate_arguments,0,sizeof(alternate_arguments));memset(pool_nodes,0,sizeof(pool_nodes));memset(actor_table,0,sizeof(actor_table));
 object=object_store.bytes+0x100;actor=(GeorgeGoalEntity *)(actor_store.bytes+0x100);D_003F83D8=0;D_00474F48=arguments;D_004961F4=config_store.bytes;
 FIELD(object,0, u32)=1;FIELD(object,8,void *)=actor;FIELD(object,0x274,void *)=physical_store.bytes;FIELD(object,0x27C,void *)=config_store.bytes;
 FIELD(actor,4,void *)=actor_table;FIELD(actor,0x21C,void *)=wheel_store.bytes;FIELD(actor,0x8EC,void *)=physical_store.bytes;
 ((GeorgeGoalVirtualFloatResult *)(actor_table+0xE0))->invoke=virtual_float;
 identity(&initial_matrix);memcpy(ADDRESS(object,0x20),&initial_matrix,64);FIELD(physical_store.bytes,0x12C,float)=1;
 FIELD(config_store.bytes,0x2F4,float)=2;FIELD(config_store.bytes,0x2F0,u32)=0x88;
 FIELD(config_store.bytes,0x1C,float)=10;FIELD(config_store.bytes,0x34,float)=20;
 FIELD(wheel_store.bytes,0x2C,float)=3;FIELD(wheel_store.bytes,0x50,float)=4;
 set_vector(wheel_store.bytes,0x30,5,6,7);set_vector(wheel_store.bytes,0x54,8,9,11);
 event_count=reply_count=0;curve_x=curve_y=virtual_result=0;matrix_up_y=1;hook=0;
 reply(0x002CD348,(u32)&pool_nodes[0]);reply(0x002D0B48,(u32)object);reply(0x002E2BB0,(u32)child_store.bytes);reply(0x0022C1E0,(u32)&pool_nodes[2]);
}

const char D_0042B960[]="fixture D_0042B960";
const char D_0042B970[]="fixture D_0042B970";
const char D_0042B980[]="fixture D_0042B980";
const char D_0042B9A0[]="fixture D_0042B9A0";
const char D_0042B9B0[]="fixture D_0042B9B0";
const char D_0042B9C8[]="fixture D_0042B9C8";
const char D_0042B9E0[]="fixture D_0042B9E0";
const char D_0042BA00[]="fixture D_0042BA00";
const char D_0042BA18[]="fixture D_0042BA18";
const char D_0042BA38[]="fixture D_0042BA38";
const char D_0042BA50[]="fixture D_0042BA50";
const char D_0042BA70[]="fixture D_0042BA70";
const char D_0042BA88[]="fixture D_0042BA88";
const char D_0042BAA8[]="fixture D_0042BAA8";
const char D_0042BAB8[]="fixture D_0042BAB8";
const char D_0042BAD0[]="fixture D_0042BAD0";
const char D_0042BB10[]="fixture D_0042BB10";
const char D_0042BB50[]="fixture D_0042BB50";
const char D_0042BB90[]="fixture D_0042BB90";
const char D_0042BBD0[]="fixture D_0042BBD0";
const char D_0042BC10[]="fixture D_0042BC10";
const char D_0042BC50[]="fixture D_0042BC50";
void func_00166F58(void * a0, u32 a1)
{ Event *e=event(0x00166F58U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void * func_0016EE68(void * a0)
{
 Event *e=event(0x0016EE68);e->argument[0]=(u32)a0;e->result=(u32)a0;finish(e);return(void *)(u32)e->result;
}

void * func_0022C1E0(void)
{ Event *e=event(0x0022C1E0U); finish(e); return (void *)(u32)e->result; }

void func_0023A5D0(u32 a0, const char * a1, u32 a2, u32 a3, u32 a4, const u8 * a5, u32 a6, u32 a7)
{ Event *e=event(0x0023A5D0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; e->argument[4]=(u32)a4; e->argument[5]=(u32)a5; e->argument[6]=(u32)a6; e->argument[7]=(u32)a7; finish(e); }

u32 func_0029C648(const char * a0)
{ Event *e=event(0x0029C648U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

void func_002A0B00(GeorgeMathVec4 * a0, const GeorgeRotationMatrix * a1)
{
 Event *e=event(0x002A0B00);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;snapshot(e,0,a1,64);a0->x=0;a0->y=0;a0->z=0;a0->w=1;finish(e);
}

void func_002A1C08(void * a0, const void * a1)
{
 Event *e=event(0x002A1C08);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;snapshot(e,0,a1,64);memmove(a0,a1,64);finish(e);
}

void func_002A1C30(GeorgeRotationMatrix * a0)
{
 Event *e=event(0x002A1C30);e->argument[0]=(u32)a0;identity(a0);finish(e);
}

void func_002A1F18(GeorgeRotationMatrix * a0, const GeorgeMathVec4 * a1, const GeorgeMathVec3 * a2)
{
 Event *e=event(0x002A1F18);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a1,16);snapshot(e,4,a2,12);identity(a0);a0->element[5]=matrix_up_y;a0->element[12]=a2->x;a0->element[13]=a2->y;a0->element[14]=a2->z;finish(e);
}

void func_002ADC80(u32 a0, const void * a1, u32 a2, float * a3, float a4)
{
 Event *e=event(0x002ADC80);e->argument[0]=a0;e->argument[1]=(u32)a1;e->argument[2]=a2;e->argument[3]=(u32)a3;e->argument[4]=float_bits(a4);CHECK(a2==2);a3[0]=curve_x;a3[1]=curve_y;finish(e);
}

void func_002CC938(const char * a0, ...)
{ Event *e=event(0x002CC938U); e->argument[0]=(u32)a0; finish(e); }

void func_002CD0B8(GeorgeDeimosPoolNode *a0)
{ Event *e=event(0x002CD0B8U); e->argument[0]=(u32)a0; finish(e); }

GeorgeDeimosPoolNode * func_002CD348(GeorgeDeimosHashTable *a0)
{ Event *e=event(0x002CD348U); e->argument[0]=(u32)a0; finish(e); return (GeorgeDeimosPoolNode *)(u32)e->result; }

void func_002CE6B8(GeorgeDeimosPoolNode * a0, const char * a1, const char * a2, u32 a3, void (*a4)(s32, s32), u32 a5)
{ Event *e=event(0x002CE6B8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; e->argument[4]=(u32)a4; e->argument[5]=(u32)a5; finish(e); }

s32 func_002D0978(void * a0, u32 a1)
{ Event *e=event(0x002D0978U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (s32)e->result; }

void * func_002D0B48(u32 a0)
{ Event *e=event(0x002D0B48U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void * func_002E2BB0(void * a0, u32 a1, u32 a2)
{ Event *e=event(0x002E2BB0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); return (void *)(u32)e->result; }

void func_002E2CD8(void * a0, void * a1, u32 a2, u32 a3)
{ Event *e=event(0x002E2CD8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; finish(e); }

void * func_00306460(void * a0, void * a1, u32 a2)
{
 Event *e=event(0x00306460);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=a2;FIELD(a0,0x18,void *)=a1;finish(e);return a0;
}

void func_003064F0(void * a0, u32 a1)
{ Event *e=event(0x003064F0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00307850(void * a0)
{ Event *e=event(0x00307850U); e->argument[0]=(u32)a0; finish(e); }

void func_003095D8(void * a0, const GeorgeMathVec4 * a1)
{
 Event *e=event(0x003095D8);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;snapshot(e,0,a1,16);finish(e);
}

void func_00311F80(void * a0, void * a1)
{ Event *e=event(0x00311F80U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x00372CC0);e->argument[0]=a0;e->argument[1]=a1;e->result=double_bits(double_value(a0)-double_value(a1));finish(e);return e->result;
}

s32 func_00373250(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x00373250);double x=double_value(a0),y=double_value(a1);e->argument[0]=a0;e->argument[1]=a1;e->result=(u32)((isnan(x)||isnan(y))?1:x<y?-1:x>y?1:0);finish(e);return(s32)e->result;
}

float func_003734F8(GeorgeActorBits64 a0)
{
 Event *e=event(0x003734F8);e->argument[0]=a0;e->result=float_bits((float)double_value(a0));finish(e);return float_value((u32)e->result);
}

GeorgeActorBits64 func_00374848(float a0)
{
 Event *e=event(0x00374848);e->argument[0]=float_bits(a0);e->result=double_bits((double)a0);finish(e);return e->result;
}

void * func_003936A0(void * a0, s32 a1, u32 a2)
{
 Event *e=event(0x003936A0);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=a2;memset(a0,a1,a2);finish(e);return a0;
}
static int hook_mode;
static void mutate(Event *e)
{
 if(hook_mode==1&&e->address==0x002CE6B8)D_003F83D8=&pool_nodes[1];
 if(hook_mode==2&&e->address==0x002D0B48){D_00474F48=alternate_arguments;alternate_arguments[1].tag=2;alternate_arguments[1].payload.scalar=91;}
 if(hook_mode==3&&e->address==0x003936A0){FIELD(object,0x27C,void *)=alternate_store.bytes;FIELD(alternate_store.bytes,0x1C,float)=33;FIELD(alternate_store.bytes,0x34,float)=44;}
 if(hook_mode==4&&e->address==0x00306460){FIELD(object,0x274,void *)=alternate_store.bytes;}
 if(hook_mode==5&&e->address==0x0022C1E0){FIELD(object,0x278,void *)=other_child_store.bytes;}
 if(hook_mode==6&&e->address==0x003064F0){FIELD(child_store.bytes,4,u16)=0xBEEF;D_004961F4=alternate_store.bytes;}
 if(hook_mode==7&&e->address==0x002ADC80){FIELD(object,0,u32)=0;}
 if(hook_mode==8&&e->address==0x002A1F18){FIELD(object,0x274,void *)=alternate_store.bytes;set_vector(alternate_store.bytes,0x180,7,8,9);}
 if(hook_mode==9&&e->address==0xF0000001){FIELD(object,0x248,float)=9.5f;FIELD(object,0x274,void *)=alternate_store.bytes;}
 if(hook_mode==10&&e->address==0x00373250&&count(0x00373250)==2){FIELD(object,0x23C,float)=5;FIELD(object,0x250,float)=2;}
 if(hook_mode==11&&e->address==0x002CD0B8){D_003F83D8=&pool_nodes[1];}
}
static void script_tests(void)
{
 static void (*callbacks[7])(s32,s32)={func_0016C2F8,func_0016C350,func_0016C3A8,func_0016C400,func_0016C458,func_0016C4B0,func_0016C508};
 static u32 offsets[7]={0xF8,0xF4,0xF0,0x104,0x100,0xFC,0x108};
 static const char *errors[7]={D_0042BAD0,D_0042BB10,D_0042BB50,D_0042BB90,D_0042BBD0,D_0042BC10,D_0042BC50};
 int i,j;Event *e;reset();CHECK(func_0016C098()==&pool_nodes[0]);CHECK(count(0x002CD348)==1&&count(0x002CD0B8)==1&&count(0x002CE6B8)==7);
 for(i=0;i<7;++i){e=nth(0x002CE6B8,i);CHECK(e->argument[0]==(u32)&pool_nodes[0]);CHECK(e->argument[3]==2&&e->argument[4]==(u32)callbacks[i]&&e->argument[5]==0);}
 event_count=0;CHECK(func_0016C098()==&pool_nodes[0]);CHECK(event_count==0);
 reset();hook_mode=1;hook=mutate;CHECK(func_0016C098()==&pool_nodes[1]);CHECK(nth(0x002CE6B8,0)->argument[0]==(u32)&pool_nodes[0]);CHECK(nth(0x002CE6B8,6)->argument[0]==(u32)&pool_nodes[1]);
 reset();hook_mode=11;hook=mutate;func_0016C098();CHECK(nth(0x002CE6B8,0)->argument[0]==(u32)&pool_nodes[1]);
 for(i=0;i<7;++i)for(j=0;j<3;++j){reset();arguments[1].tag=(u16)(j==0?2:j==1?0x8002:3);arguments[1].subtype=0xCAFE;arguments[1].payload.scalar=1.25f;FIELD(object,offsets[i],u32)=0xDEADBEEF;callbacks[i](-5,9);CHECK(arguments[1].subtype==0xCAFE&&arguments[1].payload.scalar==1.25f);if(j==0){CHECK(FIELD(object,offsets[i],float)==1.25f);CHECK(count(0x002CC938)==0);}else{CHECK(FIELD(object,offsets[i],u32)==0xDEADBEEF);CHECK(nth(0x002CC938,0)->argument[0]==(u32)errors[i]);}CHECK(nth(0x002D0B48,0)->argument[0]==0x4907C265);}
 reset();hook_mode=2;hook=mutate;func_0016C2F8(0,-1);CHECK(FIELD(object,0xF8,float)==91);
 reset();CHECK(func_0016C238(object,0x4907C265)==1);CHECK(count(0x002D0978)==0);CHECK(func_0016C238(object,5)==0);reply(0x002D0978,0xFFFFFFFF);CHECK(func_0016C238(object,5)==1);
 reset();func_0016C280(object,7);CHECK(FIELD(object,4,const u8 *)==D_0042BD08);CHECK(nth(0x00166F58,0)->argument[1]==7);
 reset();reply(0x0029C648,0xABCDEF);func_0016C560();e=nth(0x0023A5D0,0);CHECK(e->argument[0]==0xABCDEF&&e->argument[1]==(u32)D_0042B960);CHECK(e->argument[2]==0&&e->argument[3]==0&&e->argument[4]==0x1C&&e->argument[5]==(u32)D_0040DC60&&e->argument[6]==8&&e->argument[7]==0);
}
static void setup_tests(void)
{
 Event *e;int mode;reset();CHECK(func_0016C5B8(object,&initial_matrix,config_store.bytes,actor)==object);
 CHECK(FIELD(object,4,const u8 *)==D_0042BF90);CHECK(FIELD(object,8,void *)==actor);CHECK(FIELD(object,0x27C,void *)==config_store.bytes);
 CHECK(FIELD(object,0x270,float)==1000&&FIELD(object,0x264,float)==100);CHECK(FIELD(object,0x234,float)==3&&FIELD(object,0x238,float)==4);CHECK(FIELD(object,0x240,u32)==0x88);
 check_vector(object,0x90,5,10,7);check_vector(object,0xF8,-5,10,7);check_vector(object,0x160,8,20,11);check_vector(object,0x1C8,-8,20,11);
 CHECK(FIELD(object,0xBC,float)==7&&FIELD(object,0x124,float)==7&&FIELD(object,0x18C,float)==15&&FIELD(object,0x1F4,float)==15);
 CHECK(FIELD(child_store.bytes,0,const u8 *)==D_0042BF48);CHECK(FIELD(child_store.bytes,0x1C,void *)==object);CHECK(FIELD(child_store.bytes,4,u16)==0xC0);CHECK(FIELD(object,0x278,void *)==child_store.bytes);
 e=nth(0x002E2BB0,0);CHECK(e->argument[0]==(u32)config_store.bytes&&e->argument[1]==0xC0&&e->argument[2]==0x27);CHECK(nth(0x00311F80,0)->argument[1]==(u32)child_store.bytes);
 reset();hook_mode=3;hook=mutate;func_0016C5B8(object,&initial_matrix,config_store.bytes,actor);check_vector(object,0x90,5,33,7);check_vector(object,0x160,8,44,11);CHECK(FIELD(object,0xBC,float)==30&&FIELD(object,0x18C,float)==39);
 reset();hook_mode=4;hook=mutate;func_0016C5B8(object,&initial_matrix,config_store.bytes,actor);CHECK(FIELD(child_store.bytes,0x18,void *)==physical_store.bytes);CHECK(FIELD(object,0x274,void *)==alternate_store.bytes);
 reset();hook_mode=5;hook=mutate;func_0016C5B8(object,&initial_matrix,config_store.bytes,actor);CHECK(nth(0x00311F80,0)->argument[1]==(u32)other_child_store.bytes);
 for(mode=0;mode<4;++mode){reset();FIELD(child_store.bytes,4,u16)=0x1234;func_0016CED0(child_store.bytes,(u32)mode);CHECK(nth(0x003064F0,0)->argument[1]==0);CHECK(count(0x002E2CD8)==(mode&1));if(mode&1){e=nth(0x002E2CD8,0);CHECK(e->argument[1]==(u32)child_store.bytes&&e->argument[2]==0x1234&&e->argument[3]==0x27);}}
 reset();hook_mode=6;hook=mutate;func_0016CED0(child_store.bytes,1);e=nth(0x002E2CD8,0);CHECK(e->argument[0]==(u32)alternate_store.bytes&&e->argument[2]==0xBEEF);
}
static void update_tests(void)
{
 Event *e;int i;reset();curve_y=4;func_0016C7E0(object,0.5f);CHECK(FIELD(object,0x18,float)==0&&FIELD(object,0x230,float)==4);CHECK(count(0xF0000001)==1&&count(0x00307850)==1);CHECK(FIELD(object,0x248,u32)==0&&FIELD(object,0x258,u32)==0);CHECK(nth(0x00307850,0)->argument[0]==(u32)physical_store.bytes);
 reset();set_vector(object,0xC,3,0,4);func_0016C7E0(object,0);CHECK(FIELD(object,0x18,float)==5);e=nth(0x002ADC80,0);CHECK(fabsf(float_value((u32)e->argument[4])-5.7600002288818359375f)<0.00001f);CHECK(e->argument[2]==2);
 reset();FIELD(object,0x23C,float)=9;FIELD(object,0x24C,float)=8;hook_mode=7;hook=mutate;func_0016C7E0(object,0.5f);CHECK(FIELD(object,0x23C,float)==0&&FIELD(object,0x24C,float)==0);CHECK(FIELD(object,0x254,u32)==1&&FIELD(object,0x1B8,u32)==1&&FIELD(object,0x220,u32)==1);CHECK(FIELD(object,0x258,float)==0.5f);
 reset();hook_mode=8;hook=mutate;func_0016C7E0(object,0);check_vector(object,0xC,7,8,9);CHECK(nth(0x00307850,0)->argument[0]==(u32)alternate_store.bytes);
 for(i=0;i<4;++i){reset();FIELD(object,0x23C,float)=i<2?3:-3;FIELD(object,0x250,float)=0;func_0016C7E0(object,i&1?2:0.5f);CHECK(FIELD(object,0x250,float)==(i<2?(i&1?3:1):(i&1?-3:-1)));}
 reset();FIELD(object,0x23C,float)=2;func_0016C7E0(object,1);CHECK(FIELD(object,0x250,float)==2);
 reset();FIELD(object,0x23C,float)=3;hook_mode=10;hook=mutate;func_0016C7E0(object,0.5f);CHECK(FIELD(object,0x250,float)==3);
 for(i=0;i<3;++i){reset();FIELD(object,0xC,float)=i==0?0.5f:i==1?1.0f:float_value(0x7FC01234);func_0016C7E0(object,0);CHECK(count(0x003734F8)==0);}
 reset();FIELD(object,0x30,float)=0;matrix_up_y=0;FIELD(object,0x38,float)=1;virtual_result=1;FIELD(object,0x248,float)=9.75f;((GeorgeGoalVirtualFloatResult *)(actor_table+0xE0))->adjustment=-12;func_0016C7E0(object,0.25f);CHECK(FIELD(object,0x248,u32)==0);CHECK(nth(0xF0000001,0)->argument[0]==(u32)ADDRESS(actor,-12));CHECK(count(0x003095D8)==1);check_vector(nth(0x003095D8,0)->snapshot,0,0,0,0);CHECK(nth(0x003095D8,0)->snapshot[3]==float_bits(1));
 reset();matrix_up_y=0;virtual_result=1;hook_mode=9;hook=mutate;func_0016C7E0(object,0.5f);CHECK(count(0x003095D8)==1);CHECK(nth(0x003095D8,0)->argument[0]==(u32)alternate_store.bytes);
 for(i=0;i<4;++i){reset();matrix_up_y=0;virtual_result=i==0?0:i==1?-1:i==2?float_value(0x7FC00000):1;FIELD(object,0x248,float)=9;func_0016C7E0(object,0.5f);CHECK(FIELD(object,0x248,float)==(i==3?9.5f:0));CHECK(count(0x003095D8)==0);}
 reset();FIELD(object,8,void *)=0;FIELD(object,0x248,float)=3;func_0016C7E0(object,0);CHECK(count(0xF0000001)==0&&FIELD(object,0x248,u32)==0);
 for(i=0;i<3;++i){reset();matrix_up_y=i==0?0.3499999940395355f:i==1?0.35000002384185791016f:float_value(0x7FC01234);virtual_result=1;FIELD(object,0x248,float)=2;func_0016C7E0(object,0.5f);CHECK(FIELD(object,0x248,float)==(i==0?2.5f:0));}
 reset();matrix_up_y=0;virtual_result=1;FIELD(object,0x248,float)=9.4999f;func_0016C7E0(object,0.5f);CHECK(count(0x003095D8)==0);
 for(i=0;i<16;++i){int j;reset();FIELD(object,0x24C,float)=1;for(j=0;j<4;++j)FIELD(object,0xB4U+(u32)j*0x68U,u32)=(i>>j)&1;func_0016C7E0(object,0);CHECK(FIELD(object,0x244,u32)==(i==15?0:1));CHECK(FIELD(object,0x240,u32)==(i==15?0:2));}
 for(i=0;i<3;++i){reset();FIELD(object,0x24C,float)=1;FIELD(physical_store.bytes,0x180+(u32)i*4,float)=0.05000000074505806f;func_0016C7E0(object,0);CHECK(FIELD(object,0x244,u32)==0);}
 reset();FIELD(object,0x244,u32)=1;FIELD(physical_store.bytes,0x180,float)=3;func_0016C7E0(object,0);CHECK(FIELD(object,0x244,u32)==0&&FIELD(object,0x240,u32)==0x88);
 reset();FIELD(object,0x244,u32)=1;FIELD(physical_store.bytes,0x180,float)=2.99f;func_0016C7E0(object,0);CHECK(FIELD(object,0x244,u32)==1);
 reset();FIELD(object,0x244,u32)=1;FIELD(physical_store.bytes,0x180,float)=float_value(0x7FC01234);func_0016C7E0(object,0);CHECK(FIELD(object,0x244,u32)==1);
 reset();FIELD(object,0x254,u32)=0xABC;FIELD(object,0x258,float)=2;func_0016C7E0(object,-0.5f);CHECK(FIELD(object,0x1B8,u32)==0xABC&&FIELD(object,0x220,u32)==0xABC&&FIELD(object,0x258,float)==1.5f);
}
int main(void){script_tests();setup_tests();update_tests();printf("actor_controller: %d checks passed\n",checks);return 0;}
