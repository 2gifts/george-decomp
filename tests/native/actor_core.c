/* Authored, asset-free observations of five complete actor core routines.
 * Engine callbacks are controlled test doubles, not recovered engine code. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_core.c"

typedef union Storage { GeorgeActorBits64 alignment; u8 bytes[0x1800]; } Storage;
static Storage actor_store, data_store, other_data, control_store, second_control;
static Storage main_store, companion_store, physics_store, manager_store;
static Storage records[20], allocation[2];
static GeorgeGoalEntity *actor;
static GeorgeGoalEntityData *data;
static u8 control_table[0x120], second_table[0x120], object_table[0x80], contact_table[0x50];
static GeorgeRotationMatrix frames[4];
static void *registry[8];
u8 D_003F83F0[42*28];
void *D_003F2D40;
GeorgeActorPointerRange D_0046A0F0;
const u8 D_00421160[12]={0}, D_0042C330[12]={0};
const GeorgeMathVec4 D_004872E0={0,0,0,0};
GeorgeMathVec3 D_FLT_00469F00;

typedef struct Event { u32 address; GeorgeActorBits64 argument[12], result; u32 snapshot[48]; } Event;
typedef struct Reply { u32 address; GeorgeActorBits64 value; } Reply;
static Event events[2048];
static Reply replies[128];
static int event_count, reply_count, checks;
static void (*hook)(Event *);
static u32 float_bits(float f) { union {float f;u32 u;} x; x.f=f;return x.u; }
static float float_value(u32 u) { union {float f;u32 u;} x;x.u=u;return x.f; }
static GeorgeActorBits64 double_bits(double d) { union {double d;GeorgeActorBits64 u;} x;x.d=d;return x.u; }
static double double_value(GeorgeActorBits64 u) { union {double d;GeorgeActorBits64 u;} x;x.u=u;return x.d; }
static void check(int ok,const char *name,int line)
{ ++checks;if(!ok){fprintf(stderr,"FAIL %s:%d (%d events)\n",name,line,event_count);exit(1);} }
#define CHECK(condition) check((condition),#condition,__LINE__)
static void reply(u32 address,GeorgeActorBits64 value)
{ int i;for(i=0;i<reply_count;++i)if(replies[i].address==address){replies[i].value=value;return;}CHECK(reply_count<128);replies[reply_count].address=address;replies[reply_count++].value=value; }
static Event *event(u32 address)
{ Event *e;int i;CHECK(event_count<2048);e=&events[event_count++];memset(e,0,sizeof(*e));e->address=address;for(i=0;i<reply_count;++i)if(replies[i].address==address)e->result=replies[i].value;return e; }
static void finish(Event *e) { if(hook!=0)hook(e); }
static int count(u32 address) {int i,n=0;for(i=0;i<event_count;++i)if(events[i].address==address)++n;return n;}
static Event *nth(u32 address,int n) {int i;for(i=0;i<event_count;++i)if(events[i].address==address&&n--==0)return &events[i];return 0;}
static void snapshot(Event *e,int offset,const void *pointer,int size)
{ CHECK(offset>=0&&offset*4+size<=(int)sizeof(e->snapshot));memcpy(e->snapshot+offset,pointer,(size_t)size); }
static void virtual_void(void *p) {Event *e=event(0xF0000001);e->argument[0]=(u32)p;finish(e);}
static void virtual_scalar(void *p,float f) {Event *e=event(0xF0000002);e->argument[0]=(u32)p;e->argument[1]=float_bits(f);finish(e);}
static s32 virtual_integer(void *p) {Event *e=event(0xF0000003);e->argument[0]=(u32)p;finish(e);return(s32)e->result;}
static void virtual_word(void *p,u32 word) {Event *e=event(0xF0000004);e->argument[0]=(u32)p;e->argument[1]=word;finish(e);}
static void member(void *p) {Event *e=event(0xF0000005);e->argument[0]=(u32)p;finish(e);}
static void initialize_member(u32 state,u32 offset,s16 adjustment)
{ GeorgeGoalMember *m=(GeorgeGoalMember *)(D_003F83F0+state*28+offset);m->selector=-1;m->adjustment=adjustment;m->target.direct=member; }
static void identity(GeorgeRotationMatrix *m)
{ int i;memset(m,0,sizeof(*m));for(i=0;i<4;++i)m->element[i*5]=1.0f; }
static void reset(void)
{
 int i;memset(&actor_store,0,sizeof(actor_store));memset(&data_store,0,sizeof(data_store));memset(&other_data,0,sizeof(other_data));
 memset(&control_store,0,sizeof(control_store));memset(&second_control,0,sizeof(second_control));
 memset(&main_store,0,sizeof(main_store));memset(&companion_store,0,sizeof(companion_store));memset(&physics_store,0,sizeof(physics_store));
 memset(records,0,sizeof(records));memset(allocation,0,sizeof(allocation));memset(control_table,0,sizeof(control_table));memset(second_table,0,sizeof(second_table));
 memset(object_table,0,sizeof(object_table));memset(contact_table,0,sizeof(contact_table));memset(D_003F83F0,0,sizeof(D_003F83F0));
 actor=(GeorgeGoalEntity *)(actor_store.bytes+0x100);data=(GeorgeGoalEntityData *)data_store.bytes;actor->field18=data;
 FIELD(actor,0x20,void *)=control_store.bytes;FIELD(control_store.bytes,0,void *)=control_table;FIELD(second_control.bytes,0,void *)=second_table;
 ((GeorgeGoalVirtualFloat *)(control_table+0xC0))->invoke=virtual_scalar;
 ((GeorgeGoalVirtualFloat *)(control_table+0xC8))->invoke=virtual_scalar;
 ((GeorgeGoalVirtualFloat *)(control_table+0xE0))->invoke=virtual_scalar;
 ((GeorgeGoalVirtualVoid *)(control_table+0x88))->invoke=virtual_void;
 ((GeorgeGoalVirtualVoid *)(control_table+0x10))->invoke=virtual_void;
 ((GeorgeGoalVirtualInt *)(control_table+0x18))->invoke=virtual_integer;
 ((GeorgeGoalVirtualWord *)(control_table+8))->invoke=virtual_word;
 memcpy(second_table,control_table,sizeof(control_table));
 ((GeorgeGoalVirtualWord *)(object_table+0x18))->invoke=virtual_word;
 ((GeorgeGoalVirtualWord *)(object_table+0x28))->invoke=virtual_word;
 ((GeorgeGoalVirtualInt *)(contact_table+0x30))->invoke=virtual_integer;
 for(i=0;i<20;++i){FIELD(records[i].bytes,0,void *)=object_table;FIELD(records[i].bytes,4,void *)=contact_table;}
 for(i=0;i<4;++i)identity(frames+i);
 FIELD(actor,0x1D0,void *)=physics_store.bytes;
 FIELD(actor,0x40,float)=10;FIELD(actor,0x44,float)=20;FIELD(actor,0x48,float)=30;
 FIELD(actor,0x36C,float)=1;FIELD(actor,0x484,void *)=frames;
 memcpy(FRAME(actor,0xB0),frames,64);memcpy(FRAME(actor,0xF0),frames,64);
 FIELD(actor,0x190,GeorgeActorBits64)=0x1000000ULL; /* suppress optional ground query */
 D_FLT_00469F00.x=D_FLT_00469F00.y=D_FLT_00469F00.z=0;
 D_003F2D40=records[19].bytes;D_0046A0F0.field00=registry;D_0046A0F0.field04=registry;D_0046A0F0.field08=registry+8;
 event_count=reply_count=0;hook=0;
 reply(0x00192748,(u32)frames);reply(0x00192818,(u32)frames);reply(0x0022C1E0,(u32)manager_store.bytes);
 reply(0x002AAF50,(u32)records[18].bytes);reply(0x00161B80,(u32)records[17].bytes);
 reply(0x00251A88,(u32)records[16].bytes);reply(0x00236A10,0xABCD);
}

void func_001007E0(GeorgeActorPointerRange * a0, void ** a1, void *const * a2)
{ Event *e=event(0x001007E0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); }

s32 func_00100AA8(const void * a0, const void * a1)
{ Event *e=event(0x00100AA8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (s32)e->result; }

void ** func_00100C30(void **a0, void **a1, void *const *a2, GeorgeStartupCompare a3)
{ Event *e=event(0x00100C30);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;e->argument[3]=(u32)a3;e->result=(u32)a1;snapshot(e,0,*a2,12);finish(e);return(void **)(u32)e->result; }

s32 func_0012AB70(void * a0, const GeorgeMathVec3 * a1)
{ Event *e=event(0x0012AB70U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (s32)e->result; }

s32 func_0013AA70(void * a0, const GeorgeMathVec3 * a1)
{ Event *e=event(0x0013AA70U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (s32)e->result; }

float func_0013B308(const void *a0)
{ Event *e=event(0x0013B308U); e->argument[0]=(u32)a0; finish(e); return float_value((u32)e->result); }

s32 func_00146AC8(void * a0, const GeorgeMathVec3 * a1, GeorgeMathVec3 * a2, float a3)
{ Event *e=event(0x00146AC8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=float_bits(a3); finish(e); return (s32)e->result; }

void func_0015F280(void * a0, float a1, float a2)
{ Event *e=event(0x0015F280U); e->argument[0]=(u32)a0; e->argument[1]=float_bits(a1); e->argument[2]=float_bits(a2); finish(e); }

void * func_00161B80(u32 a0)
{ Event *e=event(0x00161B80U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void func_00170538(GeorgeGoalEntity *a0)
{ Event *e=event(0x00170538U); e->argument[0]=(u32)a0; finish(e); }

void func_00173E00(GeorgeGoalEntity * a0, const GeorgeMathVec3 * a1)
{ Event *e=event(0x00173E00U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; snapshot(e,0,a1,12); finish(e); }

void * func_00175590(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00175590U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (void *)(u32)e->result; }

void func_001767B8(GeorgeGoalEntity * a0)
{ Event *e=event(0x001767B8U); e->argument[0]=(u32)a0; finish(e); }

s32 func_00176E10(GeorgeGoalEntity *a0)
{ Event *e=event(0x00176E10U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

s32 func_00177E48(GeorgeGoalEntity *a0, const GeorgeMathVec3 *a1)
{ Event *e=event(0x00177E48U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; snapshot(e,0,a1,12); finish(e); return (s32)e->result; }

void func_00182898(GeorgeGoalEntity * a0, float a1)
{ Event *e=event(0x00182898U); e->argument[0]=(u32)a0; e->argument[1]=float_bits(a1); finish(e); }

s32 func_0018FCF0(GeorgeGoalEntity * a0, GeorgeActorBits64 a1)
{ Event *e=event(0x0018FCF0U); e->argument[0]=(u32)a0; e->argument[1]=a1; finish(e); return (s32)e->result; }

void func_00190D80(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00190D80U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00191000(GeorgeGoalEntity * a0, float a1)
{ Event *e=event(0x00191000U); e->argument[0]=(u32)a0; e->argument[1]=float_bits(a1); finish(e); }

void func_001910C0(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x001910C0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_001910C8(GeorgeGoalEntity * a0)
{ Event *e=event(0x001910C8U); e->argument[0]=(u32)a0; finish(e); }

s32 func_00191420(GeorgeGoalEntity * a0, u32 a1, u32 a2)
{ Event *e=event(0x00191420U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); return (s32)e->result; }

void func_00191788(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00191788U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

s32 func_00191A78(GeorgeGoalEntity * a0)
{ Event *e=event(0x00191A78U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

void func_00191AB8(GeorgeGoalEntity * a0, GeorgeRotationMatrix * a1)
{ Event *e=event(0x00191AB8);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;memmove(a1,&frames[3],64);finish(e); }

void func_00191B00(GeorgeGoalEntity * a0, GeorgeRotationMatrix * a1)
{ Event *e=event(0x00191B00);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;memmove(a1,&frames[2],64);finish(e); }

void func_00191E50(GeorgeGoalEntity * a0)
{ Event *e=event(0x00191E50U); e->argument[0]=(u32)a0; finish(e); }

void func_00191E88(GeorgeGoalEntity * a0, u32 a1, float a2)
{ Event *e=event(0x00191E88U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=float_bits(a2); finish(e); }

void func_00192078(GeorgeGoalEntity * a0, u32 a1, float a2)
{ Event *e=event(0x00192078U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=float_bits(a2); finish(e); }

void func_00192180(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00192180U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

GeorgeRotationMatrix * func_00192748(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00192748U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (GeorgeRotationMatrix *)(u32)e->result; }

GeorgeRotationMatrix * func_00192818(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00192818U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (GeorgeRotationMatrix *)(u32)e->result; }

s32 func_00192A18(void * a0)
{ Event *e=event(0x00192A18U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

float func_00192DA8(GeorgeGoalEntity * a0)
{ Event *e=event(0x00192DA8U); e->argument[0]=(u32)a0; finish(e); return float_value((u32)e->result); }

void func_00194790(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194790U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00194A40(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194A40U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00194D58(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194D58U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00195260(GeorgeGoalEntity *a0)
{ Event *e=event(0x00195260U); e->argument[0]=(u32)a0; finish(e); }

void func_00195480(GeorgeGoalEntity * a0)
{ Event *e=event(0x00195480U); e->argument[0]=(u32)a0; finish(e); }

void func_00195550(GeorgeGoalEntity * a0)
{ Event *e=event(0x00195550U); e->argument[0]=(u32)a0; finish(e); }

void func_00195620(GeorgeGoalEntity * a0)
{ Event *e=event(0x00195620U); e->argument[0]=(u32)a0; finish(e); }

void func_00195668(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00195668U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

s32 func_00195850(GeorgeGoalEntity * a0, u32 a1, u32 a2, u32 a3)
{ Event *e=event(0x00195850U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; finish(e); return (s32)e->result; }

void func_00196418(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00196418U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_001968D0(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x001968D0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00196980(GeorgeGoalEntity *a0)
{ Event *e=event(0x00196980U); e->argument[0]=(u32)a0; finish(e); }

u32 func_001A6FE0(GeorgeGoalEntity * a0)
{ Event *e=event(0x001A6FE0U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

void func_001A7118(u32 a0)
{ Event *e=event(0x001A7118U); e->argument[0]=(u32)a0; finish(e); }

s32 func_001A7570(void *a0)
{ Event *e=event(0x001A7570U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

void func_001F1410(void * a0, const GeorgeRotationMatrix * a1, float a2)
{ Event *e=event(0x001F1410U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=float_bits(a2); snapshot(e,0,a1,64); finish(e); }

void func_00215100(GeorgeMathVec3 * a0, const GeorgeMathVec3 * a1, const GeorgeMathVec3 * a2, u32 a3, u32 a4, float a5, float a6, float a7, float a8)
{ Event *e=event(0x00215100U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; e->argument[4]=(u32)a4; e->argument[5]=float_bits(a5); e->argument[6]=float_bits(a6); e->argument[7]=float_bits(a7); e->argument[8]=float_bits(a8); snapshot(e,0,a0,12); snapshot(e,3,a1,12); snapshot(e,6,a2,12); finish(e); }

void func_0021BF28(void * a0, const GeorgeMathVec3 * a1, float a2)
{ Event *e=event(0x0021BF28U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=float_bits(a2); snapshot(e,0,a1,12); finish(e); }

void func_0021C088(void * a0, u32 a1)
{ Event *e=event(0x0021C088U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void * func_0022C1E0(void)
{ Event *e=event(0x0022C1E0U); finish(e); return (void *)(u32)e->result; }

void func_0022D3F0(void * a0)
{ Event *e=event(0x0022D3F0U); e->argument[0]=(u32)a0; finish(e); }

void func_0022D5D0(void * a0)
{ Event *e=event(0x0022D5D0U); e->argument[0]=(u32)a0; finish(e); }

void func_00235CD8(void * a0, u32 a1, u32 a2)
{ Event *e=event(0x00235CD8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); }

u32 func_00236A10(const GeorgeRotationMatrix * a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
{ Event *e=event(0x00236A10U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=(u32)a3; e->argument[4]=(u32)a4; e->argument[5]=(u32)a5; snapshot(e,0,a0,64); finish(e); return (u32)e->result; }

void func_00237608(GeorgeGoalEntity * a0, GeorgeActorAttachmentCallback a1)
{ Event *e=event(0x00237608U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

s32 func_00238D50(void * a0)
{ Event *e=event(0x00238D50U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

void func_002393F8(u32 a0)
{ Event *e=event(0x002393F8U); e->argument[0]=(u32)a0; finish(e); }

void func_002455C0(void * a0)
{ Event *e=event(0x002455C0U); e->argument[0]=(u32)a0; finish(e); }

void func_00245D40(void * a0, float a1, float a2)
{ Event *e=event(0x00245D40U); e->argument[0]=(u32)a0; e->argument[1]=float_bits(a1); e->argument[2]=float_bits(a2); finish(e); }

void func_00246A18(void * a0, float a1)
{ Event *e=event(0x00246A18U); e->argument[0]=(u32)a0; e->argument[1]=float_bits(a1); finish(e); }

void * func_002481F0(void * a0)
{ Event *e=event(0x002481F0);e->argument[0]=(u32)a0;e->result=(u32)a0;finish(e);return(void *)(u32)e->result; }

void * func_00251A88(void * a0, u32 a1)
{ Event *e=event(0x00251A88U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (void *)(u32)e->result; }

void func_00251DB0(void * a0, u32 a1, const GeorgeMathVec3 * a2)
{ Event *e=event(0x00251DB0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; snapshot(e,0,a2,12); finish(e); }

void func_0026F390(void * a0, u32 a1)
{ Event *e=event(0x0026F390U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_0026FE30(void * a0, const GeorgeRotationMatrix * a1, u32 a2, float a3, float a4)
{ Event *e=event(0x0026FE30U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=float_bits(a3); e->argument[4]=float_bits(a4); snapshot(e,0,a1,64); finish(e); }

s32 func_00271F70(void * a0)
{ Event *e=event(0x00271F70U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

void func_00271F90(void * a0, u32 a1)
{ Event *e=event(0x00271F90U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00272BD8(void * a0)
{ Event *e=event(0x00272BD8U); e->argument[0]=(u32)a0; finish(e); }

u32 func_00272C10(void * a0)
{ Event *e=event(0x00272C10U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

void func_00272C40(void * a0, u32 a1, u32 a2, float a3, float a4, float a5)
{ Event *e=event(0x00272C40U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=float_bits(a3); e->argument[4]=float_bits(a4); e->argument[5]=float_bits(a5); finish(e); }

void func_00276FF0(void * a0, GeorgeMathVec3 * a1)
{ Event *e=event(0x00276FF0);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;memmove(a1,a0,12);snapshot(e,0,a1,12);finish(e); }

float func_0029B940(float a0, float a1)
{ Event *e=event(0x0029B940U); e->argument[0]=float_bits(a0); e->argument[1]=float_bits(a1); finish(e); return float_value((u32)e->result); }

void func_002A1C08(void * a0, const void * a1)
{ Event *e=event(0x002A1C08);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;snapshot(e,0,a1,64);memmove(a0,a1,64);finish(e); }

void func_002A1C60(const void * a0, const GeorgeMathVec3 * a1, GeorgeMathVec3 * a2)
{ Event *e=event(0x002A1C60);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a1,12);memmove(a2,a1,12);finish(e); }

void func_002A2200(GeorgeRotationMatrix * a0, const GeorgeRotationMatrix * a1, const GeorgeRotationMatrix * a2)
{ Event *e=event(0x002A2200);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;CHECK(a1!=0&&a2!=0);snapshot(e,0,a1,64);snapshot(e,16,a2,64);memmove(a0,a1,64);finish(e); }

void * func_002AAF50(void * a0, s32 a1)
{ Event *e=event(0x002AAF50U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (void *)(u32)e->result; }

u32 func_002AAF88(const void *a0)
{ Event *e=event(0x002AAF88U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

void func_002AB020(void * a0)
{ Event *e=event(0x002AB020U); e->argument[0]=(u32)a0; finish(e); }

void * func_002AEE60(u32 a0)
{ Event *e=event(0x002AEE60);e->argument[0]=a0;e->result=(u32)allocation[count(0x002AEE60)==1?0:1].bytes;finish(e);return(void *)(u32)e->result; }

void func_002AF100(void * a0)
{ Event *e=event(0x002AF100U); e->argument[0]=(u32)a0; finish(e); }

void func_002B8878(void * a0)
{ Event *e=event(0x002B8878U); e->argument[0]=(u32)a0; finish(e); }

void func_002BD340(void)
{ Event *e=event(0x002BD340U); finish(e); }

u32 * func_002BEBA0(u32 * a0, const u8 * a1)
{ Event *e=event(0x002BEBA0);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;*a0=0x11223344;finish(e);return a0; }

void func_002CD130(GeorgeDeimosPoolNode *a0)
{ Event *e=event(0x002CD130U); e->argument[0]=(u32)a0; finish(e); }

void func_002D08E8(GeorgeGoalEntity * a0)
{ Event *e=event(0x002D08E8U); e->argument[0]=(u32)a0; finish(e); }

void func_00311680(void * a0, void * a1)
{ Event *e=event(0x00311680U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00312C00(void * a0, void * a1)
{ Event *e=event(0x00312C00U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00317910(void * a0)
{ Event *e=event(0x00317910U); e->argument[0]=(u32)a0; finish(e); }

void func_003565B8(const GeorgeMathVec4 * a0, const GeorgeMathVec4 * a1, void * a2, float a3)
{ Event *e=event(0x003565B8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; e->argument[3]=float_bits(a3); snapshot(e,0,a0,16); snapshot(e,4,a1,16); finish(e); }

GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{ Event *e=event(0x00372CC0);e->argument[0]=a0;e->argument[1]=a1;e->result=double_bits(double_value(a0)-double_value(a1));finish(e);return e->result; }

GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{ Event *e=event(0x00372D28);e->argument[0]=a0;e->argument[1]=a1;e->result=double_bits(double_value(a0)*double_value(a1));finish(e);return e->result; }

s32 func_00373250(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{ Event *e=event(0x00373250);double x=double_value(a0),y=double_value(a1);e->argument[0]=a0;e->argument[1]=a1;e->result=(s32)((isnan(x)||isnan(y))?1:x<y?-1:x>y?1:0);finish(e);return(s32)e->result; }

float func_003734F8(GeorgeActorBits64 a0)
{ Event *e=event(0x003734F8);e->argument[0]=a0;e->result=float_bits((float)double_value(a0));finish(e);return float_value((u32)e->result); }

GeorgeActorBits64 func_00374848(float a0)
{ Event *e=event(0x00374848);e->argument[0]=float_bits(a0);e->result=double_bits((double)a0);finish(e);return e->result; }

s32 func_00396260(void (*a0)(void))
{ Event *e=event(0x00396260U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }


static void baseline(void)
{
 reset();func_00170600(actor,0.25f);
 CHECK(FIELD(actor,0x35C,float)==0.25f);CHECK(FIELD(actor,0x14,s32)==-1);
 CHECK(FIELD(physics_store.bytes,0,float)==10);CHECK(FIELD(physics_store.bytes,4,float)==20);CHECK(FIELD(physics_store.bytes,8,float)==30);
 CHECK(count(0xF0000002)==3);CHECK(count(0x0021BF28)==1);
 CHECK(nth(0x0021BF28,0)->snapshot[0]==float_bits(10));CHECK(nth(0x0021BF28,0)->snapshot[1]==float_bits(20));CHECK(nth(0x0021BF28,0)->snapshot[2]==float_bits(30));
}
static int mode, mutation_count;
static GeorgeMathVec3 *captured_vector;
static void mutations(Event *e)
{
 if(mode==1&&e->address==0xF0000002&&count(e->address)==1)FIELD(actor,0x20,void *)=second_control.bytes;
 if(mode==2&&e->address==0xF0000005&&count(e->address)==1)FIELD(actor,0x14,u32)=4;
 if(mode==2&&e->address==0x00170538)FIELD(actor,0x14,u32)=33;
 if(mode==3&&e->address==0x00146AC8)captured_vector=(GeorgeMathVec3 *)(u32)e->argument[2];
 if(mode==3&&e->address==0x0018FCF0){CHECK(captured_vector!=0);captured_vector->x=6;captured_vector->y=8;captured_vector->z=0;}
 if(mode==4&&e->address==0x00146AC8){CHECK(e->argument[3]==float_bits(1.0f-9.800000190734863f));*(GeorgeMathVec3 *)(u32)e->argument[2]=(GeorgeMathVec3){7,8,9};e->result=1;}
 if(mode==5&&e->address==0x002A1C08)FIELD(actor,0x208,void *)=records[2].bytes;
 if(mode==6&&e->address==0x002BEBA0){D_003F2D40=records[8].bytes;FIELD(actor,0x40,float)=55;}
 if(mode==7&&e->address==0x00373250&&count(e->address)==1){FIELD(actor,0x58,float)=-1;FIELD(actor,0x7C,float)=1;}
 if(mode==7&&e->address==0x00372CC0&&count(e->address)==1)FIELD(actor,0x7C,float)=99;
 if(mode==8&&e->address==0x00312C00)FIELD(actor,0x304,void *)=records[3].bytes;
 if(mode==8&&e->address==0x00317910&&count(e->address)==1)FIELD(actor,0x2FC,void *)=records[4].bytes;
 if(mode==9&&e->address==0xF0000001)FIELD(actor,0x20,void *)=second_control.bytes;
 if(mode==10&&e->address==0xF0000003){e->result=mutation_count++==0?0:0x7FB6A6E3;}
 if(mode==10&&e->address==0x0012AB70){FIELD(actor,0x4F8,void *)=records[6].bytes;e->result=1;}
 if(mode==10&&e->address==0x00191E88)FIELD(actor,0x4F8,void *)=records[7].bytes;
 if(mode==11&&e->address==0x0029B940){if(count(e->address)==1){e->result=float_bits(4);FIELD(actor,0x484,void *)=frames+1;}else e->result=float_bits(5);}
 if(mode==12&&e->address==0x00215100){FIELD(actor,0x3EC,u32)=0xAABB;*(GeorgeMathVec3 *)(u32)e->argument[0]=(GeorgeMathVec3){91,92,93};}
 if(mode==13&&e->address==0x002455C0){CHECK(FIELD(actor,0x404,void *)==records[16].bytes);FIELD(actor,0x404,void *)=records[15].bytes;}
 if(mode==13&&e->address==0x00246A18)FIELD(actor,0x404,void *)=records[14].bytes;
 if(mode==14&&e->address==0x00100C30){FIELD(actor,0x400,u32)=0xBEEF;D_003F2D40=records[13].bytes;}
 if(mode==15&&e->address==0x001910C8){FIELD(actor,0x2D8,u32)=99;FIELD(actor,0x264,void *)=records[1].bytes;}
 if(mode==16&&e->address==0x002A2200)FIELD(actor,0x214,void *)=0;
 if(mode==17&&e->address==0x002A1C08){GeorgeMathVec3 *point=(GeorgeMathVec3 *)(u32)nth(0x002A1C60,0)->argument[2];*point=(GeorgeMathVec3){42,43,44};}
 if(mode==18&&e->address==0x002393F8&&count(e->address)==1){FIELD(data,0x288,s32)=1;FIELD(actor,0x244,u32)=0x123;}
 if(mode==19&&e->address==0xF0000005){actor->field0C=1;}
 if(mode==20&&e->address==0x00272C10){e->result=count(e->address)==1?0:0x40000000U;}
 if(mode==21&&e->address==0x00271F90&&count(e->address)==1)FIELD(actor,0x1B4,void *)=records[9].bytes;
 if(mode==22&&e->address==0x00170538){FIELD(actor,0x264,void *)=records[10].bytes;}
 if(mode==23&&e->address==0xF0000003)e->result=0xCA152581;
 if(mode==23&&e->address==0x0013AA70){FIELD(actor,0x4F8,void *)=records[6].bytes;e->result=1;}
}
static void timers_and_gates(void)
{
 static const float timers[]={-1,0,0.25f,1};int i;
 for(i=0;i<4;++i){reset();FIELD(actor,0x190,GeorgeActorBits64)=0x100000000ULL;FIELD(actor,0x4FC,float)=timers[i];FIELD(actor,0x1FC,u32)=0xABC;func_00170600(actor,0.5f);CHECK(event_count==0);CHECK(FIELD(actor,0x4FC,float)==(timers[i]>0?timers[i]-0.5f:timers[i]));CHECK(FIELD(actor,0x1FC,u32)==0xABC);}
 reset();FIELD(actor,0x190,GeorgeActorBits64)=0x104000000ULL;func_00170600(actor,0.5f);CHECK(count(0x00271F70)==1);CHECK(count(0x0021BF28)==0);
 reset();FIELD(actor,0x190,GeorgeActorBits64)=0x104000000ULL;FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0x1B4,void *)=companion_store.bytes;FIELD(actor,0x35C,float)=0.75f;reply(0x00271F70,1);func_00170600(actor,0.25f);
 CHECK(count(0x00272BD8)==2);CHECK(count(0x0026FE30)==2);CHECK(nth(0x0026FE30,0)->argument[3]==float_bits(0.25f));CHECK(nth(0x0026FE30,1)->argument[3]==float_bits(0.75f));CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x4000000)==0);CHECK(FIELD(actor,0x35C,float)==0.75f);
 for(i=0;i<3;++i){reset();FIELD(actor,0x1D4,float)=2;FIELD(actor,0x1DC,float)=i*0.25f;FIELD(actor,0x1D8,float)=1;FIELD(data,0x198,float)=3;FIELD(actor,0x378,void *)=records[0].bytes;FIELD(data,0x1CC,float)=7;FIELD(data,0x1D0,float)=8;func_00170600(actor,0.25f);CHECK(count(0x00192078)==(i==0));CHECK(FIELD(actor,0x1DC,float)==(i==0?0.75f:i*0.25f-0.25f));if(i==0){CHECK(nth(0x00192078,0)->argument[2]==float_bits(6));CHECK(nth(0x0015F280,0)->argument[1]==float_bits(7));CHECK(nth(0x0015F280,0)->argument[2]==float_bits(8));}}
 for(i=0;i<3;++i){reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x80;FIELD(actor,0x380,float)=2.75f+i*0.25f;func_00170600(actor,0.25f);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x80)==(i==0?0x80:0));CHECK(FIELD(actor,0x380,float)==(i==0?3:0));}
 reset();FIELD(actor,0x4FC,u32)=0x7FC00001;FIELD(actor,0x190,GeorgeActorBits64)=0x100000000ULL;func_00170600(actor,1);CHECK(FIELD(actor,0x4FC,u32)==0x7FC00001);
}
static void member_and_control_reload(void)
{
 reset();mode=1;hook=mutations;((GeorgeGoalVirtualFloat *)(second_table+0xE0))->adjustment=7;((GeorgeGoalVirtualFloat *)(second_table+0xC8))->adjustment=-5;func_00170600(actor,0.125f);
 CHECK(nth(0xF0000002,0)->argument[0]==(u32)control_store.bytes);CHECK(nth(0xF0000002,1)->argument[0]==(u32)second_control.bytes+7);CHECK(nth(0xF0000002,2)->argument[0]==(u32)second_control.bytes-5);
 reset();initialize_member(0,0x10,3);initialize_member(33,0,-7);mode=2;hook=mutations;func_00170600(actor,0.25f);CHECK(actor->field0C==33);CHECK(count(0xF0000005)==2);CHECK(nth(0xF0000005,0)->argument[0]==(u32)actor+3);CHECK(nth(0xF0000005,1)->argument[0]==(u32)actor-7);
 reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x2800000000000ULL;initialize_member(32,0,1);initialize_member(28,0,2);func_00170600(actor,0.25f);CHECK(actor->field0C==28);CHECK(count(0x00170538)==2);CHECK(count(0xF0000005)==2);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x2800000000000ULL)==0);
 reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x8000000;initialize_member(9,0,-9);FIELD(data,0x150,u32)=1;FIELD(actor,0x2D8,u32)=7;func_00170600(actor,0.25f);CHECK(actor->field0C==9);CHECK(FIELD(actor,0x5C4,u32)==201);CHECK(nth(0xF0000005,0)->argument[0]==(u32)actor-9);
 reset();actor->field0C=31;FIELD(actor,0x190,GeorgeActorBits64)|=0x8000000;func_00170600(actor,0.25f);CHECK(count(0x00195850)==1);CHECK(nth(0x00195850,0)->argument[1]==0x54);CHECK(nth(0x00195850,0)->argument[2]==0xB71B6C44);
 /* Positive selector loads a full virtual pair and adds full signed adjustments. */
 reset();{GeorgeGoalMember *m=(GeorgeGoalMember *)(D_003F83F0+0x10);GeorgeGoalVirtualVoid *p=(GeorgeGoalVirtualVoid *)object_table;m->selector=1;m->adjustment=32767;m->target.vtable_offset=0x28;FIELD(actor,0x28,void *)=object_table;p->adjustment=32767;p->invoke=member;func_00170600(actor,0.25f);CHECK(nth(0xF0000005,0)->argument[0]==(u32)actor+65534);}
}
static void projections_and_contacts(void)
{
 reset();actor->field18=(GeorgeGoalEntityData *)ADDRESS(actor,0x13C);FIELD(actor,0x194,u32)=0x3F800400;mode=4;hook=mutations;func_00170600(actor,0.25f);CHECK(count(0x00146AC8)==1);CHECK(count(0x00173E00)==1);CHECK(nth(0x00173E00,0)->snapshot[2]==float_bits(9));
 reset();FIELD(actor,0x208,void *)=records[1].bytes;mode=5;hook=mutations;func_00170600(actor,0.25f);CHECK((FIELD(records[1].bytes,0xA0,u32)&0x100)!=0);CHECK(FIELD(records[2].bytes,0xA0,u32)==0);
 reset();FIELD(actor,0x208,void *)=records[1].bytes;reply(0x00238D50,1);func_00170600(actor,0.25f);CHECK(FIELD(actor,0x208,u32)==0);CHECK(FIELD(actor,0x210,u32)==0);CHECK(count(0x002393F8)==1);
 reset();FIELD(actor,0x208,void *)=records[1].bytes;FIELD(actor,0x20C,u32)=1;FIELD(actor,0x210,float)=1.75f;func_00170600(actor,0.25f);CHECK(count(0x00235CD8)==1);CHECK(FIELD(actor,0x210,u32)==0);
 reset();FIELD(actor,0x4F4,u32)=1;FIELD(actor,0x4F8,void *)=records[5].bytes;FIELD(records[5].bytes,0,void *)=records[0].bytes;FIELD(records[0].bytes,0x94,float)=12;mode=10;mutation_count=0;hook=mutations;func_00170600(actor,0.25f);CHECK(count(0xF0000003)==2);CHECK(count(0x0012AB70)==1);CHECK(count(0x00191E88)==1);CHECK(nth(0x00191E88,0)->argument[1]==0); /* authored record+4 is nonzero */
 CHECK(nth(0x00191E88,0)->argument[2]==float_bits(3));CHECK(FIELD(records[7].bytes,4,u32)==1);CHECK(FIELD(records[6].bytes,4,u32)!=(u32)1);
 reset();FIELD(actor,0x4F4,u32)=1;FIELD(actor,0x4F8,void *)=records[5].bytes;FIELD(records[5].bytes,0,void *)=records[0].bytes;reply(0x0013B308,float_bits(8));mode=23;hook=mutations;func_00170600(actor,0.5f);CHECK(count(0x0013AA70)==1);CHECK(count(0x0013B308)==1);CHECK(nth(0x00191E88,0)->argument[2]==float_bits(4));
}
static void matrix_and_motion(void)
{
 int i;reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x30000080000ULL;frames[0].element[12]=100;frames[0].element[13]=200;frames[0].element[14]=300;FIELD(actor,0x494,float)=2;FIELD(actor,0x498,float)=3;FIELD(actor,0x49C,float)=4;FIELD(actor,0x488,float)=1;FIELD(actor,0x48C,float)=2;FIELD(actor,0x490,float)=3;frames[1].element[8]=7;frames[1].element[10]=8;mode=11;hook=mutations;func_00170600(actor,0.5f);
 CHECK(FIELD(actor,0x40,float)==102);CHECK(FIELD(actor,0x44,float)==203);CHECK(FIELD(actor,0x48,float)==304);CHECK(count(0x0029B940)==2);CHECK(nth(0x0029B940,1)->argument[0]==float_bits(8));CHECK(nth(0x0029B940,1)->argument[1]==float_bits(7));CHECK(FIELD(actor,0x60,float)==5-6.2831854820251465f);CHECK(FIELD(actor,0x58,float)==FIELD(actor,0x60,float));CHECK(FIELD(actor,0x64,float)==FIELD(actor,0x60,float));CHECK(nth(0x00177E48,0)->snapshot[0]==float_bits(202));CHECK(nth(0x00177E48,0)->snapshot[1]==float_bits(402));CHECK(nth(0x00177E48,0)->snapshot[2]==float_bits(602));
 for(i=0;i<2;++i){reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x10000080000ULL;frames[0].element[12]=4;frames[0].element[13]=5;frames[0].element[14]=6;func_00170600(actor,i?0.5f:0);CHECK(FIELD(actor,0x4C,float)==(i?8:4));CHECK(FIELD(actor,0x50,float)==(i?10:5));CHECK(FIELD(actor,0x54,float)==(i?12:6));CHECK(FIELD(actor,0x488,float)==(i?4:0));}
 reset();FIELD(actor,0x190,GeorgeActorBits64)|=0x40000000000ULL;FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0x1B4,void *)=companion_store.bytes;FIELD(actor,0x4C,float)=3;FIELD(actor,0x50,float)=4;captured_vector=0;mode=3;hook=mutations;func_00170600(actor,0.25f);CHECK(count(0x0026FE30)==2);CHECK(nth(0x0026FE30,0)->argument[4]==float_bits(5));CHECK(nth(0x0026FE30,1)->argument[4]==float_bits(10));CHECK(nth(0x00272C40,0)->argument[1]==0);CHECK(nth(0x00272C40,0)->argument[2]==0);for(i=3;i<6;++i)CHECK(nth(0x00272C40,0)->argument[i]==0);
 for(i=0;i<3;++i){float dt=i==0?0:i==1?0.01f:0.25f;reset();FIELD(actor,0x1A0,void *)=records[0].bytes;reply(0x00192DA8,float_bits(5));func_00170600(actor,dt);CHECK(count(0x003565B8)==1);CHECK(nth(0x003565B8,0)->snapshot[0]==float_bits(10));CHECK(nth(0x003565B8,0)->snapshot[1]==float_bits(25));CHECK(nth(0x003565B8,0)->snapshot[2]==float_bits(30));CHECK(nth(0x003565B8,0)->snapshot[3]==0);CHECK(nth(0x003565B8,0)->argument[3]==float_bits(1.0f/george_ee_maximum(dt,0.01666666753590107f)));}
 reset();FIELD(actor,0x1D0,void *)=ADDRESS(actor,0x30);FIELD(actor,0x40,u32)=1;FIELD(actor,0x44,u32)=2;FIELD(actor,0x48,u32)=3;func_00170600(actor,0.25f);CHECK(count(0x0022D3F0)==1);CHECK(FIELD(actor,0x30,u32)==1);CHECK(FIELD(actor,0x34,u32)==2);CHECK(FIELD(actor,0x38,u32)==3);
}
static void commands_and_effects(void)
{
 static const float distance[]={0,17,29.5f,30,44,45};int i;
 for(i=0;i<6;++i){reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0x1B4,void *)=companion_store.bytes;FIELD(actor,0xE0,float)=distance[i];func_00170600(actor,0.25f);CHECK(nth(0x00271F90,0)->argument[1]==(i==0?0:i<3?1:i<5?2:3));CHECK(nth(0x00271F90,1)->argument[1]==nth(0x00271F90,0)->argument[1]);}
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0x1B4,void *)=companion_store.bytes;mode=21;hook=mutations;func_00170600(actor,0.25f);CHECK(nth(0x00271F90,1)->argument[0]==(u32)records[9].bytes);
 reset();actor->field0C=14;FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0xE0,float)=0;func_00170600(actor,0.25f);CHECK(nth(0x00271F90,0)->argument[1]==1);
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0xE0,u32)=0x7FC00001;reply(0x00192A18,1);func_00170600(actor,0.25f);CHECK(nth(0x00271F90,0)->argument[1]==2);
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(data,0x90,u32)=1;reply(0x00272C10,0x80000000U);mode=6;hook=mutations;func_00170600(actor,0.25f);CHECK(count(0x00251DB0)==1);CHECK(nth(0x00251DB0,0)->argument[0]==(u32)records[19].bytes);CHECK(nth(0x00251DB0,0)->argument[1]==0x11223344);CHECK(nth(0x00251DB0,0)->snapshot[0]==float_bits(55));
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(actor,0x3CC,u32)=0x3456;FIELD(data,0x90,u32)=1;reply(0x00272C10,0x40000000);func_00170600(actor,0.25f);CHECK(count(0x002BEBA0)==0);CHECK(nth(0x00251DB0,0)->argument[1]==0x3456);
 reset();FIELD(actor,0x190,GeorgeActorBits64)=0x44;reply(0xF0000003,0);reply(0x00175590,(u32)records[0].bytes);func_00170600(actor,0.25f);CHECK(FIELD(actor,0x410,void *)==records[0].bytes);CHECK(nth(0x00175590,0)->argument[1]==1);
 reset();FIELD(actor,0x190,GeorgeActorBits64)=0x44;reply(0xF0000003,3);func_00170600(actor,0.25f);CHECK(nth(0x00175590,0)->argument[1]==0);
}
static void route_and_final_position(void)
{
 int i;for(i=0;i<2;++i){reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(main_store.bytes,0x3D8,void *)=frames+1;FIELD(actor,0x3E8,u32)=0x1234;FIELD(actor,0x3EC,u32)=0;FIELD(actor,0x3F0,s32)=0;FIELD(actor,0x3F4,s32)=0;FIELD(actor,0x88,float)=0;FIELD(actor,0x8C,float)=1;FIELD(actor,0x90,float)=0;FIELD(actor,0xE4,float)=10;FIELD(actor,0x80,float)=10;FIELD(actor,0xD0,float)=2;FIELD(actor,0xD4,float)=4;FIELD(actor,0xD8,float)=6;frames[1].element[12]=1;frames[1].element[13]=2;frames[1].element[14]=3;mode=i?20:12;hook=mutations;reply(0x00272C10,0x80000000);func_00170600(actor,0.25f);
 CHECK(count(0x00215100)==1);CHECK(count(0x00272C10)==(i?2:1));CHECK(nth(0x00215100,0)->snapshot[0]==float_bits(1.1f));CHECK(nth(0x00215100,0)->snapshot[1]==float_bits(10)); /* bit24 preserves the authored actor+80 projection height */ CHECK(nth(0x00215100,0)->snapshot[2]==float_bits(3.3f));CHECK(nth(0x00215100,0)->argument[3]==0x1234);CHECK(nth(0x00215100,0)->argument[4]==0);CHECK(nth(0x00215100,0)->argument[5]==float_bits(0.3499999940395355f));if(!i){CHECK(nth(0x00236A10,0)->argument[1]==0xAABB);CHECK(nth(0x00236A10,0)->snapshot[12]==float_bits(91));CHECK(nth(0x00236A10,0)->snapshot[13]==float_bits(92));CHECK(nth(0x00236A10,0)->snapshot[14]==float_bits(93));CHECK(nth(0x002393F8,0)->argument[0]==0xABCD);}}
 /* Upper 32 bits at +3EC participate in the original 64-bit nonzero gate. */
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(main_store.bytes,0x3D8,void *)=frames;FIELD(actor,0x3EC,u32)=0x5566;reply(0x00272C10,0x80000000);func_00170600(actor,0.25f);CHECK(count(0x00215100)==0);CHECK(count(0x00236A10)==1);
 reset();FIELD(actor,0x1B0,void *)=main_store.bytes;FIELD(main_store.bytes,0x43C,void *)=records[0].bytes;FIELD(records[0].bytes,0,float)=7;FIELD(records[0].bytes,4,float)=8;FIELD(records[0].bytes,8,float)=9;func_00170600(actor,0.25f);CHECK(nth(0x0021BF28,0)->snapshot[0]==float_bits(7));CHECK(nth(0x0021BF28,0)->snapshot[2]==float_bits(9));
 reset();reply(0x00191A78,1);frames[3].element[12]=71;frames[3].element[13]=72;frames[3].element[14]=73;func_00170600(actor,0.25f);CHECK(count(0x00191B00)==1);CHECK(count(0x00191AB8)==1);CHECK(nth(0x0021BF28,0)->snapshot[0]==float_bits(71));CHECK(nth(0x0021BF28,0)->snapshot[1]==float_bits(72));CHECK(nth(0x0021BF28,0)->snapshot[2]==float_bits(73));
}
static void angle_and_opacity(void)
{
 int i;static const float difference[]={-4,-1,-0.05f,0,0.05f,1,4};
 for(i=0;i<7;++i){float expected,d=difference[i],rate=0.1f;reset();FIELD(data,0x1D8,u32)=0x45A78000;FIELD(data,0x38,float)=0.4f;FIELD(actor,0x190,GeorgeActorBits64)|=0x2000000;FIELD(actor,0x58,float)=d;func_00170600(actor,0.25f);if(d>3.1415927410125732f)d-=6.2831854820251465f;else if(d< -3.1415927410125732f)d+=6.2831854820251465f;if(fabsf(d)<rate)rate=fabsf(d);expected=d<=0?-rate:rate;CHECK(FIELD(actor,0x7C,float)==expected);CHECK(count(0x00374848)>=3);}
 reset();FIELD(data,0x1D8,u32)=0x45A78000;FIELD(data,0x38,float)=1;FIELD(actor,0x190,GeorgeActorBits64)|=0x2000000;FIELD(actor,0x58,float)=-2;mode=7;hook=mutations;func_00170600(actor,0.25f);CHECK(FIELD(actor,0x7C,float)==0.75f);CHECK(nth(0x00372CC0,0)->argument[0]==0);CHECK(nth(0x00372CC0,0)->argument[1]==double_bits(-2));
 reset();FIELD(data,0x1D8,u32)=0x45A78000;FIELD(actor,0x190,GeorgeActorBits64)|=0x80000000000ULL;FIELD(actor,0x64,float)=1.25f;FIELD(actor,0x58,float)=-2;func_00170600(actor,0.25f);CHECK(FIELD(actor,0x7C,float)==1.25f);CHECK(count(0x00374848)==0);
 reset();FIELD(data,0x1D8,u32)=0x45A78000;FIELD(actor,0x58,float)=-2;func_00170600(actor,0.25f);CHECK(FIELD(actor,0x7C,float)==-2);
 for(i=0;i<2;++i){reset();FIELD(actor,0x78,float)=2;FIELD(actor,0x74,float)=4;if(i)FIELD(actor,0x190,GeorgeActorBits64)|=0x2000000;func_00170600(actor,0.1f);CHECK(FIELD(actor,0x78,float)==(i?3:1));}
 reset();FIELD(data,0x1D8,u32)=0x45A78000;FIELD(data,0x38,float)=1;FIELD(actor,0x190,GeorgeActorBits64)|=0x2000000;FIELD(actor,0x58,u32)=0x7FC00001;func_00170600(actor,0.25f);CHECK(FIELD(actor,0x7C,float)==0.25f); /* pinned GNU fpcmp_parts returns +1 for unordered operands */
}
static void effect_installation(void)
{
 reset();func_001703F0(actor);CHECK(event_count==0);FIELD(actor,0x400,u32)=1;FIELD(actor,0x404,void *)=records[0].bytes;func_001703F0(actor);CHECK(event_count==0);
 reset();FIELD(actor,0x400,u32)=0x123;mode=13;hook=mutations;func_001703F0(actor);CHECK(count(0x00251A88)==1);CHECK(nth(0x00251A88,0)->argument[1]==0x123);CHECK(nth(0x00246A18,0)->argument[0]==(u32)records[15].bytes);CHECK(nth(0x00245D40,0)->argument[0]==(u32)records[14].bytes);CHECK(nth(0x00245D40,0)->argument[1]==float_bits(1));CHECK(nth(0x00245D40,0)->argument[2]==float_bits(1));
 reset();FIELD(actor,0x400,u32)=0x123;D_003F2D40=0;mode=14;hook=mutations;func_001703F0(actor);CHECK(count(0x002AEE60)==2);CHECK(nth(0x002AEE60,0)->argument[0]==0x1B4);CHECK(nth(0x002AEE60,1)->argument[0]==12);CHECK(nth(0x00100C30,0)->snapshot[0]==9);CHECK(nth(0x00100C30,0)->snapshot[1]==(u32)D_00421160);CHECK(nth(0x00100C30,0)->snapshot[2]==(u32)allocation[0].bytes);CHECK(D_0046A0F0.field04==registry+1);CHECK(registry[0]==allocation[1].bytes);CHECK(count(0x00396260)==1);CHECK(nth(0x00251A88,0)->argument[0]==(u32)records[13].bytes);CHECK(nth(0x00251A88,0)->argument[1]==0xBEEF);
 reset();FIELD(actor,0x400,u32)=0x123;D_003F2D40=0;D_0046A0F0.field08=registry;func_001703F0(actor);CHECK(count(0x001007E0)==1);CHECK(D_0046A0F0.field04==registry);
}
static void attachment_aliases(void)
{
 int i;reset();frames[0].element[12]=10;frames[0].element[13]=20;frames[0].element[14]=30;FIELD(actor,0x1A4,void *)=ADDRESS(frames,0x38-0x4C);func_00171FB8(actor,frames);CHECK(frames[0].element[14]==1);CHECK(FIELD(ADDRESS(frames,0x38-0x4C),0x48,float)==30);
 reset();FIELD(actor,0x4E8,void *)=records[0].bytes;FIELD(actor,0x4EC,float)=2;FIELD(actor,0x1C8,void *)=records[1].bytes;FIELD(data,8,float)=6;FIELD(data,0xC,float)=4;frames[0].element[12]=1;frames[0].element[13]=2;frames[0].element[14]=3;func_00171FB8(actor,frames);CHECK(FIELD(records[0].bytes,0x44,float)==4);CHECK(FIELD(records[1].bytes,0x44,float)==9);CHECK(FIELD(records[1].bytes,0xA0,u32)==0x100);
 reset();FIELD(actor,0x214,void *)=records[0].bytes;mode=16;hook=mutations;func_00171FB8(actor,frames);CHECK(count(0x002A2200)==1);CHECK(count(0x002A1C08)==0);
 reset();reply(0x001A6FE0,1);frames[0].element[12]=10;frames[0].element[13]=20;frames[0].element[14]=30;mode=17;hook=mutations;func_00171FB8(actor,frames);CHECK(FIELD(actor,0x4D0,float)==42);CHECK(FIELD(actor,0x4D4,float)==43);CHECK(FIELD(actor,0x4D8,float)==44);CHECK(FIELD(actor,0x4DC,float)==1);
 reset();for(i=0;i<4;++i)FIELD(actor,0x218+i*4,void *)=records[i].bytes;FIELD(actor,0x228,void *)=records[4].bytes;FIELD(actor,0x28C,void *)=records[5].bytes;FIELD(actor,0x290,void *)=records[6].bytes;func_00171FB8(actor,frames);CHECK(count(0x002A2200)==7);CHECK(count(0x002A1C08)==7);for(i=0;i<7;++i)CHECK(FIELD(records[i].bytes,0xA0,u32)==0x100);
}
static void substate_signed_and_captured(void)
{
 int i;static const s32 states[]={-1,0,1,2,5,6,12,13};
 for(i=0;i<8;++i){s32 allowed=states[i]==0||states[i]==2||states[i]==5||states[i]==12;reset();actor->field0C=(u32)states[i];FIELD(actor,0x264,void *)=records[0].bytes;FIELD(actor,0x230,void *)=records[1].bytes;CHECK(func_00172550(actor)==allowed);CHECK(count(0x00170538)==(states[i]==12));if(allowed){CHECK(FIELD(actor,0x350,u32)==1);CHECK(FIELD(actor,0x2D8,u32)==4);CHECK(FIELD(actor,0x32C,u32)==0x58);CHECK(FIELD(records[1].bytes,0x30,float)==1);}}
 reset();FIELD(actor,0x2D8,u32)=4;FIELD(actor,0x334,float)=1;FIELD(data,0x370,float)=2;FIELD(actor,0x354,u32)=1;CHECK(func_00172550(actor)==0);CHECK(count(0x001910C8)==0);
 reset();FIELD(actor,0x2D8,u32)=4;FIELD(actor,0x334,u32)=0x7FC00001;FIELD(data,0x370,float)=2;FIELD(actor,0x354,u32)=1;mode=15;hook=mutations;CHECK(func_00172550(actor)==1);CHECK(FIELD(actor,0x2D8,u32)==4);CHECK(count(0x001910C8)==1);
 reset();actor->field0C=12;initialize_member(0,0,4);FIELD(actor,0x230,void *)=ADDRESS(actor,0x300);mode=22;hook=mutations;CHECK(func_00172550(actor)==1);CHECK(FIELD(actor,0x330,float)==1); /* captured record+30 aliases cleared actor+330 */
}
static void lifecycle_order(void)
{
 int i;reset();FIELD(actor,0x304,void *)=records[0].bytes;FIELD(actor,0x2FC,void *)=records[1].bytes;((GeorgeGoalVirtualWord *)(object_table+0x18))->adjustment=-3;mode=8;hook=mutations;func_0016FC98(actor);CHECK(nth(0x00312C00,0)->argument[1]==(u32)records[0].bytes);CHECK(nth(0x00317910,0)->argument[0]==(u32)records[3].bytes);CHECK(nth(0xF0000004,0)->argument[0]==(u32)records[4].bytes-3);CHECK(nth(0xF0000004,0)->argument[1]==3);CHECK(FIELD(actor,0x304,u32)==0);CHECK(FIELD(actor,0x2FC,u32)==0);
 reset();mode=9;hook=mutations;((GeorgeGoalVirtualWord *)(second_table+8))->adjustment=9;func_0016FC98(actor);CHECK(nth(0xF0000004,0)->argument[0]==(u32)second_control.bytes+9);CHECK((FIELD(actor,0x190,GeorgeActorBits64)&0x1000000000ULL)==0);
 reset();for(i=0;i<10;++i)FIELD(actor,0x38C+i*4,void *)=records[i].bytes;FIELD(actor,0x208,void *)=records[10].bytes;FIELD(actor,0x1C8,u32)=0x123;FIELD(actor,0x1CC,u32)=0x456;func_0016FC98(actor);CHECK(count(0x002CD130)==10);for(i=0;i<10;++i){CHECK(nth(0x002CD130,i)->argument[0]==(u32)records[i].bytes);CHECK(FIELD(actor,0x38C+i*4,u32)==0);}CHECK(count(0x002393F8)==3);CHECK(count(0x0022D5D0)==1);CHECK(FIELD(actor,0x1D0,u32)==0);CHECK(FIELD(actor,0x208,u32)==0);
 reset();FIELD(data,0x288,s32)=3;FIELD(actor,0x244,u32)=0x111;FIELD(actor,0x248,u32)=0x222;mode=18;hook=mutations;func_0016FC98(actor);CHECK(count(0x002393F8)==1);CHECK(FIELD(actor,0x248,u32)==0x222);CHECK(FIELD(actor,0x244,u32)==0); /* callback update is overwritten by captured-slot zero */
 reset();FIELD(data,0x288,s32)=2;FIELD(data,0x2EC,s32)=1;FIELD(actor,0x2C2,s16)=1;FIELD(actor,0x2C0,s16)=1;FIELD(actor,0x2B4,u32)=0;func_0016FC98(actor);CHECK(count(0x00237608)==4);CHECK(nth(0x00237608,0)->argument[0]==(u32)actor+0x5D0);CHECK(nth(0x00237608,1)->argument[0]==(u32)actor+0x5E0);CHECK(nth(0x00237608,2)->argument[0]==(u32)actor+0x65C);CHECK(nth(0x00237608,3)->argument[0]==(u32)actor+0x2A8);CHECK(nth(0x002393F8,0)->argument[0]==0);
 reset();actor->field0C=0;initialize_member(0,8,8);initialize_member(1,8,100);mode=19;hook=mutations;func_0016FC98(actor);CHECK(nth(0xF0000005,0)->argument[0]==(u32)actor+8);CHECK(actor->field0C==1);
 reset();FIELD(actor,0x294,void *)=records[0].bytes;reply(0x002AAF88,2);FIELD(records[18].bytes,0,void *)=records[1].bytes;func_0016FC98(actor);CHECK(count(0x002AAF50)==2);CHECK(count(0x002AB020)==1);CHECK(count(0x002393F8)==2);CHECK(nth(0x002AAF50,1)->argument[1]==1);
}
int main(void)
{
 baseline();timers_and_gates();member_and_control_reload();projections_and_contacts();matrix_and_motion();commands_and_effects();route_and_final_position();angle_and_opacity();effect_installation();attachment_aliases();substate_signed_and_captured();lifecycle_order();
 printf("actor_core: %d checks passed\n",checks);return 0;
}
