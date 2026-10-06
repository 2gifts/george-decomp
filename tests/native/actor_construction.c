/* Authored, asset-free callback and alias checks of five whole actor methods.
 * Engine, collision and curve callbacks below are controlled observations. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_construction.c"

typedef union Storage { GeorgeActorBits64 alignment; u8 bytes[0x1800]; } Storage;
static Storage actor_store, data_store, other_data, physics_store, context_store;
static Storage control_store, config_store, vehicle_store, allocations[4], components[12], records[4];
static GeorgeGoalEntity *actor;
static GeorgeActorControlObject *control;
static u8 vehicle_table[0x200], object_table[0x40], data_table[0xC0], context_table[0x40];
static GeorgeRotationMatrix identity_frame;
static GeorgeMathVec3 input, position;
const u8 D_0042BF90[120]={0}, D_0042C018[16]={0}, D_0042E7A8[16]={0};
void *D_003F8A68;

typedef struct Event {u32 address;GeorgeActorBits64 argument[16], result;u32 snapshot[48];} Event;
typedef struct Reply {u32 address;GeorgeActorBits64 result;} Reply;
static Event events[4096];
static Reply replies[80];
static int event_count, reply_count, checks, component_count;
static int primary_hit[4], fallback_result, plane_result, query_count;
static float hit_fraction[4], curve_single, curve_first, curve_second;
static int curve_no_write;
static void (*hook)(Event *);
static u32 float_bits(float f){union {float f;u32 u;} v;v.f=f;return v.u;}
static float float_value(u32 u){union {float f;u32 u;} v;v.u=u;return v.f;}
static GeorgeActorBits64 double_bits(double d){union {double d;GeorgeActorBits64 u;}v;v.d=d;return v.u;}
static double double_value(GeorgeActorBits64 u){union {double d;GeorgeActorBits64 u;}v;v.u=u;return v.d;}
static void check(int ok,const char *name,int line)
{++checks;if(!ok){fprintf(stderr,"FAIL %s:%d (%d events)\n",name,line,event_count);exit(1);}}
#define CHECK(c) check((c),#c,__LINE__)
static void reply(u32 address,GeorgeActorBits64 result)
{int i;for(i=0;i<reply_count;++i)if(replies[i].address==address){replies[i].result=result;return;}CHECK(reply_count<80);replies[reply_count].address=address;replies[reply_count++].result=result;}
static Event *event(u32 address)
{Event *e;int i;CHECK(event_count<4096);e=&events[event_count++];memset(e,0,sizeof(*e));e->address=address;for(i=0;i<reply_count;++i)if(replies[i].address==address)e->result=replies[i].result;return e;}
static void finish(Event *e){if(hook!=0)hook(e);}
static int count(u32 address){int i,n=0;for(i=0;i<event_count;++i)if(events[i].address==address)++n;return n;}
static Event *nth(u32 address,int n){int i;for(i=0;i<event_count;++i)if(events[i].address==address&&n--==0)return &events[i];return 0;}
static void snapshot(Event *e,u32 offset,const void *p,u32 size)
{CHECK(offset*4U+size<=sizeof(e->snapshot));memcpy(e->snapshot+offset,p,size);}
static void set_vector(void *p,u32 offset,float x,float y,float z)
{FIELD(p,offset,float)=x;FIELD(p,offset+4,float)=y;FIELD(p,offset+8,float)=z;}
static void check_vector(const void *p,u32 offset,float x,float y,float z)
{CHECK(fabsf(FIELD(p,offset,float)-x)<0.0001f);CHECK(fabsf(FIELD(p,offset+4,float)-y)<0.0001f);CHECK(fabsf(FIELD(p,offset+8,float)-z)<0.0001f);}
static void identity(GeorgeRotationMatrix *m)
{int i;memset(m,0,sizeof(*m));for(i=0;i<4;++i)m->element[i*5]=1;}
static void virtual_word(void *p,u32 word)
{Event *e=event(0xF0000001);e->argument[0]=(u32)p;e->argument[1]=word;finish(e);}
static GeorgeGoalVirtualObject *virtual_pointer(void *p)
{Event *e=event(0xF0000002);e->argument[0]=(u32)p;finish(e);return(GeorgeGoalVirtualObject *)(u32)e->result;}
static void virtual_vector(void *p,const GeorgeMathVec4 *v,float dt)
{Event *e=event(0xF0000003);e->argument[0]=(u32)p;e->argument[1]=(u32)v;e->argument[2]=float_bits(dt);snapshot(e,0,v,16);finish(e);}
static void virtual_index(void *p,GeorgeGoalEntity *a,signed char index)
{Event *e=event(0xF0000004);e->argument[0]=(u32)p;e->argument[1]=(u32)a;e->argument[2]=(s32)index;finish(e);}
static void reset(void)
{
 int i;memset(&actor_store,0,sizeof(actor_store));memset(&data_store,0,sizeof(data_store));memset(&other_data,0,sizeof(other_data));
 memset(&physics_store,0,sizeof(physics_store));memset(&context_store,0,sizeof(context_store));memset(&control_store,0,sizeof(control_store));
 memset(&config_store,0,sizeof(config_store));memset(&vehicle_store,0,sizeof(vehicle_store));memset(allocations,0,sizeof(allocations));
 memset(components,0,sizeof(components));memset(records,0,sizeof(records));memset(vehicle_table,0,sizeof(vehicle_table));
 memset(object_table,0,sizeof(object_table));memset(data_table,0,sizeof(data_table));memset(context_table,0,sizeof(context_table));
 actor=(GeorgeGoalEntity *)(actor_store.bytes+0x100);control=(GeorgeActorControlObject *)(control_store.bytes+0x100);
 FIELD(actor,0x18,void *)=data_store.bytes;FIELD(control,0x18,void *)=data_store.bytes;FIELD(control,0x1C,void *)=context_store.bytes;
 FIELD(context_store.bytes,0x27C,void *)=config_store.bytes;FIELD(context_store.bytes,8,void *)=actor;
 FIELD(context_store.bytes,4,void *)=context_table;FIELD(actor,0x21C,void *)=records[3].bytes;
 FIELD(vehicle_store.bytes,4,void *)=vehicle_table;FIELD(data_store.bytes,0xA0,void *)=data_table;
 ((ActorConstructionVirtualIndex *)(vehicle_table+0x1A8))->invoke=virtual_index;
 ((GeorgeGoalVirtualWord *)(object_table+8))->invoke=virtual_word;
 ((GeorgeGoalVirtualWord *)(object_table+0x10))->invoke=virtual_word;
 ((GeorgeGoalVirtualPointer *)(context_table+0x10))->invoke=virtual_pointer;
 ((ActorConstructionVirtualVectorScalar *)(data_table+0xA8))->invoke=virtual_vector;
 ((ActorConstructionVirtualVectorScalar *)(data_table+0xB8))->invoke=virtual_vector;
 for(i=0;i<12;++i){FIELD(components[i].bytes,0x20,void *)=object_table;FIELD(components[i].bytes,0,void *)=object_table;}
 for(i=0;i<4;++i){FIELD(records[i].bytes,0,void *)=object_table;}
 identity(&identity_frame);identity(FRAME(actor,0xB0));identity(FRAME(actor,0xF0));
 set_vector(data_store.bytes,0x120,0,0,0);FIELD(data_store.bytes,0x12C,float)=1;
 FIELD(context_store.bytes,0x234,float)=1;FIELD(context_store.bytes,0x238,float)=1;FIELD(context_store.bytes,0x25C,u32)=1;
 FIELD(config_store.bytes,0x20,float)=1;FIELD(config_store.bytes,0x38,float)=1;
 FIELD(config_store.bytes,0x24,float)=10;FIELD(config_store.bytes,0x3C,float)=10;
 FIELD(context_store.bytes,0x260,s32)=1;
 for(i=0;i<4;++i){set_vector(context_store.bytes,0x90U+(u32)i*0x68U,(i&1)?1:-1,0,i<2?-1:1);primary_hit[i]=1;hit_fraction[i]=0.5f;}
 FIELD(data_store.bytes,0x370,float)=3;FIELD(data_store.bytes,0xCC,float)=4;
 input.x=input.y=0;input.z=0.1f;position.x=10;position.y=20;position.z=30;
 event_count=reply_count=component_count=query_count=0;fallback_result=plane_result=0;curve_single=curve_first=curve_second=0;curve_no_write=0;hook=0;
 D_003F8A68=allocations[3].bytes;
 reply(0x00192748,(u32)&identity_frame);reply(0x001A1988,(u32)control_store.bytes);reply(0x001A2278,(u32)control_store.bytes);
 reply(0x001A6158,(u32)control_store.bytes);reply(0x001F6350,(u32)-1);reply(0x0022D4B0,(u32)physics_store.bytes);
 reply(0x002AAF98,(u32)allocations[0].bytes);reply(0x0021B848,(u32)allocations[1].bytes);reply(0x002AEE60,(u32)allocations[1].bytes);
 reply(0x002AEF08,(u32)allocations[2].bytes);reply(0x0022C1E0,(u32)allocations[0].bytes);reply(0xF0000002,(u32)actor);
}

/* Typed observation stubs follow. */
void func_00175AF0(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00175AF0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_001767B8(GeorgeGoalEntity * a0)
{ Event *e=event(0x001767B8U); e->argument[0]=(u32)a0; finish(e); }

void func_00176A20(GeorgeGoalEntity * a0, u32 a1, u32 a2)
{ Event *e=event(0x00176A20U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); }

void func_0017ADA8(void * a0, GeorgeGoalEntity * a1)
{ Event *e=event(0x0017ADA8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00183C58(GeorgeGoalEntity *a0)
{ Event *e=event(0x00183C58U); e->argument[0]=(u32)a0; finish(e); }

void func_0018FD30(GeorgeGoalEntity * a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x0018FD30);e->argument[0]=(u32)a0;e->argument[1]=a1;FIELD(a0,0x190,GeorgeActorBits64)|=a1;finish(e);
}

void func_0018FD40(GeorgeGoalEntity * a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x0018FD40);e->argument[0]=(u32)a0;e->argument[1]=a1;FIELD(a0,0x190,GeorgeActorBits64)&=~a1;finish(e);
}

void func_00190E00(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00190E00U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_001910C0(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x001910C0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00191DC8(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00191DC8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

GeorgeRotationMatrix * func_00192748(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x00192748U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (GeorgeRotationMatrix *)(u32)e->result; }

void func_00194028(void * a0, GeorgeGoalEntity * a1)
{ Event *e=event(0x00194028U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00194790(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194790U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00194A40(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194A40U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00194D58(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00194D58U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00195358(GeorgeGoalEntity *a0, void *a1)
{ Event *e=event(0x00195358U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_00196418(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x00196418U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void func_001968D0(GeorgeGoalEntity * a0, void * a1)
{ Event *e=event(0x001968D0U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

void * func_001A1988(GeorgeGoalEntity * a0)
{ Event *e=event(0x001A1988U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void * func_001A2278(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x001A2278U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (void *)(u32)e->result; }

void * func_001A6158(GeorgeGoalEntity * a0, u32 a1)
{ Event *e=event(0x001A6158U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); return (void *)(u32)e->result; }

void func_001F6138(void * a0)
{ Event *e=event(0x001F6138U); e->argument[0]=(u32)a0; finish(e); }

s32 func_001F6350(void * a0)
{ Event *e=event(0x001F6350U); e->argument[0]=(u32)a0; finish(e); return (s32)e->result; }

void * func_00210078(u32 a0, u32 a1, u32 a2)
{ Event *e=event(0x00210078U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); return (void *)(u32)e->result; }

void * func_0021B848(void * a0)
{ Event *e=event(0x0021B848U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

s32 func_0022BB70(const GeorgeMathVec3 * a0, const GeorgeMathVec3 * a1, u32 a2, float * a3, GeorgeMathVec3 * a4, u32 * a5)
{
 Event *e=event(0x0022BB70);int hit;e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=a2;e->argument[3]=(u32)a3;e->argument[4]=(u32)a4;e->argument[5]=(u32)a5;snapshot(e,0,a0,12);snapshot(e,3,a1,12);if(a3!=0){CHECK(query_count<4);hit=primary_hit[query_count];if(hit){*a3=hit_fraction[query_count];if(a4!=0){a4->x=0;a4->y=1;a4->z=0;}if(a5!=0)*a5=(u32)query_count;}++query_count;}else hit=fallback_result;e->result=(u32)hit;finish(e);return(s32)e->result;
}

void * func_0022C1E0(void)
{ Event *e=event(0x0022C1E0U); finish(e); return (void *)(u32)e->result; }

void * func_0022C800(u32 a0)
{ Event *e=event(0x0022C800U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void func_0022D318(void * a0)
{ Event *e=event(0x0022D318U); e->argument[0]=(u32)a0; finish(e); }

void * func_0022D4B0(const GeorgeMathVec3 * a0, void (*a1)(void *, GeorgeGoalEntity *), void (*a2)(void *, GeorgeGoalEntity *), GeorgeGoalEntity * a3, u32 a4, u32 a5, u32 a6, float a7)
{
 Event *e=event(0x0022D4B0);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;e->argument[3]=(u32)a3;e->argument[4]=a4;e->argument[5]=a5;e->argument[6]=a6;e->argument[7]=float_bits(a7);snapshot(e,0,a0,12);finish(e);return(void *)(u32)e->result;
}

void func_0022D788(void * a0)
{ Event *e=event(0x0022D788U); e->argument[0]=(u32)a0; finish(e); }

void func_0022D838(void * a0)
{ Event *e=event(0x0022D838U); e->argument[0]=(u32)a0; finish(e); }

void func_00235CD8(void * a0, u32 a1, u32 a2)
{ Event *e=event(0x00235CD8U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; e->argument[2]=(u32)a2; finish(e); }

u32 func_00236A10(const GeorgeRotationMatrix * a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
{
 Event *e=event(0x00236A10);e->argument[0]=(u32)a0;e->argument[1]=a1;e->argument[2]=a2;e->argument[3]=a3;e->argument[4]=a4;e->argument[5]=a5;snapshot(e,0,a0,64);CHECK(component_count<12);e->result=(u32)components[component_count++].bytes;finish(e);return(u32)e->result;
}

void * func_00236CB8(const void * a0, void * a1, u32 a2, u32 a3, u32 a4)
{
 Event *e=event(0x00236CB8);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=a2;e->argument[3]=a3;e->argument[4]=a4;snapshot(e,0,a0,64);CHECK(component_count<12);e->result=(u32)components[component_count++].bytes;finish(e);return(void *)(u32)e->result;
}

void * func_00238BA0(void * a0, u32 a1)
{
 Event *e=event(0x00238BA0);e->argument[0]=(u32)a0;e->argument[1]=a1;e->result=(u32)(a1==0xBA44E6F8U?records[2].bytes:(a0==components[0].bytes?records[0].bytes:records[1].bytes));finish(e);return(void *)(u32)e->result;
}

void * func_00239E40(u32 a0, void * a1, u32 a2, GeorgeActorAttachmentCallback a3, void * a4, u32 a5, u32 a6)
{
 Event *e=event(0x00239E40);e->argument[0]=a0;e->argument[1]=(u32)a1;e->argument[2]=a2;e->argument[3]=(u32)a3;e->argument[4]=(u32)a4;e->argument[5]=a5;e->argument[6]=a6;snapshot(e,0,a1,64);if(a4!=0)snapshot(e,16,a4,16);finish(e);return(void *)(u32)e->result;
}

u32 func_00297640(u32 a0)
{ Event *e=event(0x00297640U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

float func_0029B940(float a0, float a1)
{
 Event *e=event(0x0029B940);float result=atan2f(a0,a1);e->argument[0]=float_bits(a0);e->argument[1]=float_bits(a1);e->result=float_bits(result);finish(e);return float_value((u32)e->result);
}

float func_0029C090(float a0)
{
 Event *e=event(0x0029C090);e->argument[0]=float_bits(a0);e->result=float_bits(sinf(a0));finish(e);return float_value((u32)e->result);
}

float func_0029C168(float a0)
{
 Event *e=event(0x0029C168);e->argument[0]=float_bits(a0);e->result=float_bits(cosf(a0));finish(e);return float_value((u32)e->result);
}

s32 func_0029F080(const float * a0, const void * a1, float * a2)
{
 Event *e=event(0x0029F080);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a0,28);if(plane_result!=0)*a2=0.25f;e->result=(u32)plane_result;finish(e);return(s32)e->result;
}

void func_002A1C08(void * a0, const void * a1)
{
 Event *e=event(0x002A1C08);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;snapshot(e,0,a1,64);memmove(a0,a1,64);finish(e);
}

void func_002A1C60(const void * a0, const GeorgeMathVec3 * a1, GeorgeMathVec3 * a2)
{
 Event *e=event(0x002A1C60);const float *m=(const float *)a0;GeorgeMathVec3 v=*a1;e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a1,12);a2->x=((m[0]*v.x+m[4]*v.y)+m[8]*v.z)+m[12];a2->y=((m[1]*v.x+m[5]*v.y)+m[9]*v.z)+m[13];a2->z=((m[2]*v.x+m[6]*v.y)+m[10]*v.z)+m[14];finish(e);
}

void func_002A1F18(GeorgeRotationMatrix * a0, const GeorgeMathVec4 * a1, const GeorgeMathVec3 * a2)
{
 Event *e=event(0x002A1F18);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a1,16);snapshot(e,4,a2,12);identity(a0);a0->element[12]=a2->x;a0->element[13]=a2->y;a0->element[14]=a2->z;finish(e);
}

void func_002A2200(GeorgeRotationMatrix * a0, const GeorgeRotationMatrix * a1, const GeorgeRotationMatrix * a2)
{
 Event *e=event(0x002A2200);e->argument[0]=(u32)a0;e->argument[1]=(u32)a1;e->argument[2]=(u32)a2;snapshot(e,0,a1,64);snapshot(e,16,a2,64);memmove(a0,a1,64);finish(e);
}

float func_002A3538(GeorgeMathVec3 *a0)
{
 Event *e=event(0x002A3538);float length=sqrtf((a0->x*a0->x+a0->y*a0->y)+a0->z*a0->z);e->argument[0]=(u32)a0;snapshot(e,0,a0,12);if(length!=0){a0->x=a0->x/length;a0->y=a0->y/length;a0->z=a0->z/length;}finish(e);return length;
}

u32 func_002A7418(u32 a0)
{ Event *e=event(0x002A7418U); e->argument[0]=(u32)a0; finish(e); return (u32)e->result; }

void * func_002AAF98(u32 a0)
{ Event *e=event(0x002AAF98U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void func_002ADC80(u32 a0, const void * a1, u32 a2, float * a3, float a4)
{
 Event *e=event(0x002ADC80);e->argument[0]=a0;e->argument[1]=(u32)a1;e->argument[2]=a2;e->argument[3]=(u32)a3;e->argument[4]=float_bits(a4);if(a2==1)*a3=curve_single;else{CHECK(a2==2);snapshot(e,0,a3,12);if(!curve_no_write){a3[0]=curve_first;a3[1]=curve_second;}}finish(e);
}

void * func_002AEE60(u32 a0)
{ Event *e=event(0x002AEE60U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void * func_002AEF08(u32 a0)
{ Event *e=event(0x002AEF08U); e->argument[0]=(u32)a0; finish(e); return (void *)(u32)e->result; }

void func_002AF100(void * a0)
{ Event *e=event(0x002AF100U); e->argument[0]=(u32)a0; finish(e); }

GeorgeScriptObject * func_002D06E8(GeorgeScriptObject *a0)
{ Event *e=event(0x002D06E8U); e->argument[0]=(u32)a0; finish(e); return (GeorgeScriptObject *)(u32)e->result; }

void func_002D0858(GeorgeScriptObject * a0)
{ Event *e=event(0x002D0858U); e->argument[0]=(u32)a0; finish(e); }

void func_00307850(void * a0)
{ Event *e=event(0x00307850U); e->argument[0]=(u32)a0; finish(e); }

void func_00316040(void * a0, void * a1)
{ Event *e=event(0x00316040U); e->argument[0]=(u32)a0; e->argument[1]=(u32)a1; finish(e); }

GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x00372CC0);e->argument[0]=a0;e->argument[1]=a1;e->result=double_bits(double_value(a0)-double_value(a1));finish(e);return e->result;
}

GeorgeActorBits64 func_00372D28(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x00372D28);e->argument[0]=a0;e->argument[1]=a1;e->result=double_bits(double_value(a0)*double_value(a1));finish(e);return e->result;
}

s32 func_00373250(GeorgeActorBits64 a0, GeorgeActorBits64 a1)
{
 Event *e=event(0x00373250);double first=double_value(a0),second=double_value(a1);e->argument[0]=a0;e->argument[1]=a1;e->result=(u32)((isnan(first)||isnan(second))?1:first<second?-1:first>second?1:0);finish(e);return(s32)e->result;
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

s32 func_00397178(void)
{ Event *e=event(0x00397178U); finish(e); return (s32)e->result; }
static int hook_mode;
static void mutate(Event *e)
{
 if(hook_mode==1&&e->address==0x001767B8){FIELD(actor,0x18,void *)=other_data.bytes;FIELD(other_data.bytes,0xCC,float)=91;}
 if(hook_mode==2&&e->address==0x0021B848)FIELD(actor,0x190,GeorgeActorBits64)=(1ULL<<35)|(1ULL<<30)|(1ULL<<39)|(1ULL<<40);
 if(hook_mode==3&&e->address==0x0022D838&&e->argument[0]==(u32)allocations[0].bytes){FIELD(records[0].bytes,0x28,u32)=0xDEADBEEF;FIELD(records[0].bytes,0x3C,void *)=allocations[2].bytes;}
 if(hook_mode==4&&e->address==0x002A2200){FIELD(actor,0x18,void *)=other_data.bytes;FIELD(other_data.bytes,0xE4,u32)=0xABCD;}
 if(hook_mode==5&&e->address==0x00239E40&&e->argument[3]==(u32)func_00194A40)FIELD(data_store.bytes,0x288,s32)=1;
 if(hook_mode==6&&e->address==0x00397178){FIELD(actor,0x18,void *)=other_data.bytes;FIELD(other_data.bytes,0x280,s32)=3;e->result=5;}
 if(hook_mode==7&&e->address==0x001F6138)FIELD(actor,0x730,void *)=allocations[3].bytes;
 if(hook_mode==8&&e->address==0x00307850){input.z=2.0f;}
 if(hook_mode==8&&e->address==0xF0000003){input.z=3.0f;FIELD(data_store.bytes,0x180,float)=999;}
 if(hook_mode==9&&e->address==0x0022BB70&&e->argument[3]!=0&&query_count==1){memcpy(other_data.bytes,context_store.bytes,0x300);FIELD(control,0x1C,void *)=other_data.bytes;}
 if(hook_mode==10&&e->address==0x00316040)FIELD(actor,0x278,void *)=records[1].bytes;
 if(hook_mode==11&&e->address==0x0022D4B0){FIELD(physics_store.bytes,0x10,void *)=allocations[0].bytes;FIELD(physics_store.bytes,0x34,u32)=101;FIELD(physics_store.bytes,0x3C,u32)=202;}
 if(hook_mode==11&&e->address==0x0022D838&&e->argument[0]==(u32)allocations[0].bytes){FIELD(physics_store.bytes,0x2C,u32)=303;FIELD(physics_store.bytes,0x10,void *)=allocations[2].bytes;}
 if(hook_mode==12&&e->address==0x002AF100)CHECK(FIELD((void *)(u32)e->argument[0],4,const u8 *)==D_0042C018);
 if(hook_mode==13&&e->address==0x0022BB70&&e->argument[3]!=0){snapshot(e,16,ADDRESS(CONTEXT(control),((u32)query_count-1U)*0x68U+0xA8U),12);}
 if(hook_mode==14&&e->address==0x002ADC80&&e->argument[2]==2)FIELD(context_store.bytes,0x270,float)=2;
}
static void initialize(void *vehicle,s32 index,u32 word9,void *pointer10,u32 word11)
{func_0016EF18(actor,&position,(GeorgeGoalEntityData *)data_store.bytes,0xA3,0xA4,0xA5,0xA6,vehicle,index,word9,pointer10,word11,0xAC,0xAD,0xAE,0.75f);}
static void check_output(int n,float x,float y,float z)
{Event *e=nth(0xF0000003,n);CHECK(e!=0);check_vector(e->snapshot,0,x,y,z);CHECK(e->snapshot[3]==0);}
static void check_physics_shape(void)
{
 int i;CHECK(query_count==4);CHECK(count(0xF0000003)==2);CHECK(count(0x00307850)==2);
 CHECK(count(0x002A1C60)==4);CHECK(count(0x002A3538)==16);
 for(i=0;i<4;++i){Event *e=nth(0x0022BB70,i);CHECK(e!=0);CHECK(e->argument[2]==0x2D);}
 CHECK(nth(0xF0000003,0)->argument[0]==(u32)ADDRESS(data_store.bytes,0xA0));
 CHECK(nth(0xF0000003,1)->argument[0]==(u32)ADDRESS(data_store.bytes,0xA0));
}
static void physics_tests(void)
{
 int i,j;reset();func_0016CF30(control,&input);check_physics_shape();check_output(0,0,40,0);check_output(1,0,0,0);
 CHECK(count(0x0029B940)==4);CHECK(count(0x0029C168)==1);CHECK(count(0x0029C090)==1);
 for(i=0;i<4;++i){u32 o=(u32)i*0x68;CHECK(FIELD(context_store.bytes,o+0xB4,u32)==1);CHECK(FIELD(context_store.bytes,o+0xBC,float)==1);CHECK(FIELD(context_store.bytes,o+0xB8,float)==0);CHECK(FIELD(context_store.bytes,o+0xD8,float)==10);}
 reset();for(i=0;i<4;++i)primary_hit[i]=0;func_0016CF30(control,&input);check_output(0,0,0,0);check_output(1,0,0,0);CHECK(count(0x0022BB70)==5);CHECK(count(0x0029F080)==4);
 reset();for(i=0;i<4;++i)primary_hit[i]=0;plane_result=1;func_0016CF30(control,&input);check_output(0,0,60,0);CHECK(FIELD(context_store.bytes,0xBC,float)==0.5f);
 reset();for(i=0;i<4;++i)primary_hit[i]=0;fallback_result=1;func_0016CF30(control,&input);check_output(0,0,0,0);CHECK(count(0x0029F080)==0);CHECK(count(0x0022BB70)==5);
 for(j=0;j<4;++j){reset();for(i=0;i<4;++i)hit_fraction[i]=0.25f+0.25f*(float)j;func_0016CF30(control,&input);check_output(0,0,(float)(60-20*j),0);CHECK(FIELD(context_store.bytes,0xD8,float)==(float)(15-5*j));}
 reset();for(i=0;i<4;++i)primary_hit[i]=0;set_vector(data_store.bytes,0x180,2,0,3);FIELD(config_store.bytes,0x2D8,float)=2;FIELD(config_store.bytes,0x2EC,float)=3;FIELD(config_store.bytes,0x2E8,float)=4;func_0016CF30(control,&input);check_output(0,-6,-12,-18);
 reset();hook_mode=13;hook=mutate;set_vector(data_store.bytes,0x180,3,4,5);set_vector(data_store.bytes,0x190,0,2,0);func_0016CF30(control,&input);check_vector(nth(0x0022BB70,0)->snapshot,64,1,4,7);
 reset();hook_mode=8;hook=mutate;((ActorConstructionVirtualVectorScalar *)(data_table+0xA8))->adjustment=-12;((ActorConstructionVirtualVectorScalar *)(data_table+0xB8))->adjustment=20;func_0016CF30(control,&input);CHECK(nth(0xF0000003,0)->argument[2]==float_bits(0.1f));CHECK(nth(0xF0000003,1)->argument[2]==float_bits(3));CHECK(nth(0xF0000003,0)->argument[0]==(u32)ADDRESS(data_store.bytes,0x94));CHECK(nth(0xF0000003,1)->argument[0]==(u32)ADDRESS(data_store.bytes,0xB4));check_output(0,0,40,0);
 reset();hook_mode=9;hook=mutate;func_0016CF30(control,&input);CHECK(CONTEXT(control)==other_data.bytes);CHECK(FIELD(other_data.bytes,0xB4,u32)==1);CHECK(FIELD(context_store.bytes,0xB4,u32)==0);check_output(0,0,40,0);
 for(j=0;j<2;++j){reset();set_vector(context_store.bytes,0xC,0,0,j?1.3f:0.9f);FIELD(context_store.bytes,0x24C,float)=-1;FIELD(context_store.bytes,0x260,s32)=5;func_0016CF30(control,&input);CHECK(FIELD(context_store.bytes,0x260,s32)==(j?1:-1));CHECK(FIELD(context_store.bytes,0x264,float)==0.1f);}
 reset();FIELD(context_store.bytes,0x264,float)=0.15f;func_0016CF30(control,&input);CHECK(FIELD(context_store.bytes,0x264,float)==0.25f);
 reset();curve_single=0.25f;func_0016CF30(control,&input);check_output(0,-10,40,0);CHECK(nth(0x002ADC80,0)->argument[2]==1);
 reset();curve_single=0.25f;FIELD(context_store.bytes,0x25C,u32)=0;func_0016CF30(control,&input);check_output(0,-5,40,0);
 reset();curve_single=1;curve_first=0.25f;curve_second=0.5f;FIELD(context_store.bytes,0x254,u32)=1;func_0016CF30(control,&input);check_output(0,-15,40,0);CHECK(count(0x002ADC80)==8);
 reset();curve_first=2;FIELD(context_store.bytes,0x244,u32)=1;FIELD(context_store.bytes,0x24C,float)=3;FIELD(config_store.bytes,0,float)=4;func_0016CF30(control,&input);check_output(0,0,40,192);
 reset();curve_no_write=1;set_vector(data_store.bytes,0x180,2,0,3);FIELD(context_store.bytes,0x244,u32)=1;FIELD(context_store.bytes,0x24C,float)=1;FIELD(config_store.bytes,0,float)=1;func_0016CF30(control,&input);check_output(0,0,40,16);
 for(i=0;i<4;++i){Event *e=nth(0x002ADC80,2*i+1);CHECK(e!=0&&e->argument[2]==2);check_vector(e->snapshot,0,2,0,3);CHECK(FIELD(context_store.bytes,(u32)i*0x68+0xF4,float)==2*0.0001f);}
 reset();for(i=0;i<4;++i)primary_hit[i]=(i==0);curve_single=1;func_0016CF30(control,&input);check_output(0,-7.5f,10,0);CHECK(FIELD(context_store.bytes,0xF0,float)==-7.5f*0.0001f);
 reset();FIELD(config_store.bytes,0x30,float)=3;hit_fraction[0]=0.25f;hit_fraction[1]=0.75f;func_0016CF30(control,&input);CHECK(FIELD(context_store.bytes,0xDC,float)==3);CHECK(FIELD(context_store.bytes,0x144,float)==-3);CHECK(FIELD(context_store.bytes,0xD8,float)==18);CHECK(FIELD(context_store.bytes,0x140,float)==2);
 reset();FIELD(data_store.bytes,0xE0,float)=7;FIELD(config_store.bytes,4,float)=2;FIELD(config_store.bytes,8,float)=3;FIELD(config_store.bytes,12,float)=4;func_0016CF30(control,&input);check_vector(control,0xB0,9,3,4);
 for(j=0;j<4;++j){reset();set_vector(context_store.bytes,0xC,0,0,3);set_vector(data_store.bytes,0x180,0,0,3);FIELD(context_store.bytes,0x264,float)=0.3f;FIELD(context_store.bytes,0x24C,float)=1;FIELD(context_store.bytes,0x270,float)=10;FIELD(context_store.bytes,0x240,s32)=j;curve_first=2;func_0016CF30(control,&input);check_output(0,0,40,j==2?80:j==3?0:40);CHECK(count(0x002ADC80)==(j==2?8:j==3?4:6));}
 reset();set_vector(context_store.bytes,0xC,0,0,3);set_vector(data_store.bytes,0x180,0,0,3);FIELD(context_store.bytes,0x264,float)=0.3f;FIELD(context_store.bytes,0x24C,float)=-1;FIELD(context_store.bytes,0x270,float)=10;FIELD(config_store.bytes,0x2DC,float)=2;func_0016CF30(control,&input);check_output(0,0,40,-80);CHECK(nth(0x00372D28,0)->argument[1]==0xBFF0000000000000ULL);
 reset();set_vector(context_store.bytes,0xC,0,0,3);set_vector(data_store.bytes,0x180,0,0,3);FIELD(context_store.bytes,0x264,float)=0.3f;FIELD(context_store.bytes,0x270,float)=10;FIELD(config_store.bytes,0x2E4,float)=2;func_0016CF30(control,&input);check_output(0,0,40,-80);
 reset();set_vector(context_store.bytes,0xC,0,0,3);set_vector(data_store.bytes,0x180,0,0,3);FIELD(context_store.bytes,0x264,float)=0.3f;FIELD(context_store.bytes,0x24C,float)=1;FIELD(context_store.bytes,0x270,float)=10;FIELD(context_store.bytes,0x240,s32)=2;curve_first=2;hook_mode=14;hook=mutate;func_0016CF30(control,&input);check_output(0,0,40,0);
 reset();set_vector(context_store.bytes,0xC,0,0,float_value(0x7FC01234));func_0016CF30(control,&input);CHECK(FIELD(context_store.bytes,0x260,s32)==-1);CHECK((s32)nth(0x00373250,0)->result==1);
}
static void initializer_tests(void)
{
 Event *e;int i;reset();initialize(0,0,0,0,0);
 CHECK(FIELD(actor,8,u32)==1);CHECK(FIELD(actor,0xC,s32)==-1);CHECK(FIELD(actor,0x18,void *)==data_store.bytes);
 check_vector(actor,0x40,10,20,30);CHECK(FIELD(actor,0x80,float)==20);CHECK(FIELD(actor,0x84,float)==20);CHECK(FIELD(actor,0x8C,float)==1);
 CHECK(FIELD(actor,0x7C,float)==0.75f);CHECK(FIELD(actor,0x428,float)==4);CHECK(FIELD(actor,0x334,float)==3);
 CHECK(FIELD(actor,0x3B4,u32)==0xAC);CHECK(FIELD(actor,0x3B8,u32)==0xAD);CHECK(FIELD(actor,0xA34,u32)==0xA6);
 CHECK(FIELD(actor,0x1A8,u32)==0xA4);CHECK(FIELD(actor,0x1AC,u32)==0xA3);CHECK(FIELD(actor,0x36C,float)==1);CHECK(FIELD(actor,0x9F4,float)==2);
 CHECK(count(0x00236A10)==2);CHECK(FIELD(records[0].bytes,0x34,void *)==actor);CHECK(FIELD(records[0].bytes,0x28,u32)==1);CHECK(FIELD(records[0].bytes,0x2C,u32)==1);
 CHECK(FIELD(records[1].bytes,0x28,u32)==0x40010080);CHECK(FIELD(physics_store.bytes,0x2C,u32)==0xF800000A);CHECK(FIELD(physics_store.bytes,0x30,u32)==0xF800000A);
 e=nth(0x0022D4B0,0);CHECK(e->argument[1]==(u32)func_0017ADA8);CHECK(e->argument[2]==(u32)func_00194028);CHECK(e->argument[3]==(u32)actor);CHECK(e->argument[4]==0&&e->argument[5]==0&&e->argument[6]==0&&e->argument[7]==0);
 CHECK(nth(0x00176A20,0)->argument[1]==0xA3&&nth(0x00176A20,0)->argument[2]==0xA5);CHECK(nth(0x00190E00,0)->argument[1]==0);CHECK(count(0x002D0858)==1);
 reset();hook_mode=1;hook=mutate;initialize(0,0,1,(void *)1,1);CHECK(FIELD(actor,0x428,float)==91);CHECK(FIELD(actor,0x18,void *)==other_data.bytes);CHECK(FIELD(actor,0x190,GeorgeActorBits64)&0x10000000ULL);CHECK(FIELD(actor,0x19C,u32)==0x98197A65);CHECK(nth(0x00175AF0,0)->argument[1]==1);
 reset();hook_mode=2;hook=mutate;initialize(0,0,0,0,0);CHECK(FIELD(actor,0x190,GeorgeActorBits64)==(1ULL<<40));
 for(i=0;i<3;++i){reset();FIELD(data_store.bytes,0x1D8,u32)=i==0?0x218568E4:i==1?0x35F655E9:0x45A78000;FIELD(data_store.bytes,0x1DC,u32)=0xBADD;initialize(0,0,0,0,0);CHECK(FIELD(actor,0x20,void *)==control_store.bytes);if(i==1)CHECK(nth(0x001A2278,0)->argument[1]==0xBADD);if(i==2)CHECK(nth(0x001A6158,0)->argument[1]==0xAE);}
 reset();FIELD(actor,0x20,u32)=0xCAFEBABE;initialize(0,0,0,0,0);CHECK(FIELD(actor,0x20,u32)==0xCAFEBABE);
 reset();FIELD(data_store.bytes,0xF0,u32)=0xFFFF;FIELD(data_store.bytes,0x374,void *)=records[3].bytes;initialize(0,0,0,0,0);CHECK(nth(0x00191DC8,0)->argument[1]==0xFFFF);CHECK(nth(0x00195358,0)->argument[1]==(u32)records[3].bytes);
 reset();FIELD(data_store.bytes,0xA8,u32)=0x80000000;FIELD(data_store.bytes,0xAC,u32)=1;FIELD(records[0].bytes,0x28,u32)=4;FIELD(records[0].bytes,0x3C,void *)=allocations[0].bytes;hook_mode=3;hook=mutate;initialize(0,0,0,0,0);CHECK(FIELD(records[0].bytes,0x2C,u32)==0xDEADBEEF);CHECK(nth(0x0022D788,0)->argument[0]==(u32)allocations[2].bytes);
 reset();for(i=0;i<4;++i)FIELD(data_store.bytes,0xD4U+(u32)i*4U,u32)=0x100U+(u32)i;initialize(0,0,0,0,0);CHECK(count(0x00236A10)==6);CHECK(count(0x00235CD8)==1);for(i=0;i<4;++i)CHECK(FIELD(actor,0x214U+(u32)i*4U,void *)==components[2+i].bytes);
 reset();FIELD(data_store.bytes,0xE4,u32)=123;reply(0x00210078,(u32)components[10].bytes);((GeorgeGoalVirtualWord *)(object_table+0x10))->adjustment=-8;hook_mode=4;hook=mutate;initialize(0,0,0,0,0);CHECK(nth(0x00210078,0)->argument[0]==0xABCD);CHECK(FIELD(actor,0x464,u32)==0xABCD);CHECK(FIELD(components[2].bytes,0xA0,u32)==0x100);CHECK(nth(0xF0000001,0)->argument[0]==(u32)ADDRESS(components[10].bytes,-8));
 reset();FIELD(data_store.bytes,0xE4,u32)=123;initialize(0,0,0,0,0);CHECK(FIELD(actor,0x224,u32)==0);CHECK(FIELD(actor,0x464,u32)==123);
 reset();FIELD(data_store.bytes,0xE8,u32)=124;reply(0x00210078,(u32)components[10].bytes);initialize(0,0,0,0,0);CHECK(FIELD(actor,0x228,void *)==components[2].bytes);CHECK(FIELD(actor,0x460,u32)==124);
 reset();FIELD(data_store.bytes,0xF8,u32)=11;FIELD(data_store.bytes,0xFC,u32)=12;FIELD(data_store.bytes,0x114,u32)=13;initialize(0,0,0,0,0);CHECK(count(0x00239E40)==3);CHECK(nth(0x00239E40,0)->argument[3]==(u32)func_001968D0);CHECK(nth(0x00239E40,1)->argument[3]==(u32)func_001910C0);CHECK(nth(0x00239E40,2)->argument[3]==(u32)func_00196418);
 reset();FIELD(data_store.bytes,0x288,s32)=2;for(i=0;i<6;++i)FIELD(data_store.bytes,0x28CU+(u32)i*4U,u32)=20U+(u32)i;reply(0x00297640,0x80);reply(0x002A7418,0x81);initialize(0,0,0,0,0);CHECK(count(0x00239E40)==2);CHECK(nth(0x00239E40,0)->argument[0]==20);CHECK(nth(0x00239E40,1)->argument[0]==23);CHECK(FIELD(actor,0x5D4,u32)==0);CHECK(FIELD(actor,0x5E4,u32)==1);CHECK(FIELD(actor,0x5D8,u32)==0x80&&FIELD(actor,0x5DC,u32)==0x81);
 reset();FIELD(data_store.bytes,0x288,s32)=2;FIELD(data_store.bytes,0x28C,u32)=22;hook_mode=5;hook=mutate;initialize(0,0,0,0,0);CHECK(count(0x00239E40)==1);
 reset();FIELD(data_store.bytes,0x2EC,s32)=2;FIELD(data_store.bytes,0x2F0,u32)=40;FIELD(data_store.bytes,0x2FC,u32)=41;initialize(0,0,0,0,0);CHECK(count(0x00239E40)==2);CHECK(nth(0x00239E40,0)->argument[3]==(u32)func_00194790);CHECK(FIELD(actor,0x660,u32)==0);CHECK(FIELD(actor,0x670,u32)==1);
 reset();FIELD(data_store.bytes,0x280,s32)=2;FIELD(data_store.bytes,0x28C,u32)=0x7777;hook_mode=6;hook=mutate;initialize(0,0,0,0,0);CHECK(nth(0x00239E40,0)->argument[0]==0x7777);CHECK(nth(0x00239E40,0)->argument[3]==(u32)func_00194D58);
 reset();FIELD(data_store.bytes,0x140,u32)=88;FIELD(data_store.bytes,0x144,u32)=99;reply(0x002A7418,0x89);reply(0x00210078,(u32)components[10].bytes);initialize(0,0,0,0,0);CHECK(nth(0x00192748,0)->argument[1]==0x89);CHECK(FIELD(actor,0x23C,void *)==components[2].bytes);CHECK(nth(0x00236CB8,0)->argument[2]==0);
 reset();hook_mode=11;hook=mutate;FIELD(data_store.bytes,0x408,u32)=1;initialize(0,0,0,0,0);CHECK(FIELD(physics_store.bytes,0x30,u32)==303);CHECK(FIELD(physics_store.bytes,0x38,u32)==101);CHECK(FIELD(physics_store.bytes,0x40,u32)==202);CHECK(nth(0x0022D788,0)->argument[0]==(u32)allocations[2].bytes);
 reset();initialize(vehicle_store.bytes,0x180,1,0,0);CHECK(count(0x001F6350)==0);CHECK(nth(0xF0000004,0)->argument[2]==(GeorgeActorBits64)(s32)-128);CHECK(FIELD(actor,0x738,u32)==0x180);CHECK(FIELD(actor,0x734,u32)==0x180);CHECK(FIELD(actor,0x73C,u32)==1);CHECK(nth(0x00175AF0,0)->argument[1]==0);CHECK(nth(0x00190E00,0)->argument[1]==14);CHECK(count(0x00183C58)==1);
 reset();FIELD(allocations[3].bytes,4,void *)=vehicle_table;((ActorConstructionVirtualIndex *)(vehicle_table+0x1A8))->adjustment=-16;hook_mode=7;hook=mutate;reply(0x001F6350,0x1234);initialize(vehicle_store.bytes,-1,0,0,0);CHECK(nth(0xF0000004,0)->argument[0]==(u32)ADDRESS(allocations[3].bytes,-16));CHECK(nth(0xF0000004,0)->argument[2]==0x34);CHECK(FIELD(actor,0x738,u32)==0x1234);
 reset();FIELD(data_store.bytes,0x278,u32)=1;initialize(0,0,0,0,0);CHECK(nth(0x00190E00,0)->argument[1]==1);
 reset();set_vector(actor,0x44,3,4,5);position=construction_read_vector(actor,0x44);func_0016EF18(actor,VECTOR(actor,0x44),(GeorgeGoalEntityData *)data_store.bytes,0,0,0,0,0,0,0,0,0,0,0,0,0);check_vector(actor,0x40,3,4,5);
 reset();set_vector(actor,0x3C,3,4,5);func_0016EF18(actor,VECTOR(actor,0x3C),(GeorgeGoalEntityData *)data_store.bytes,0,0,0,0,0,0,0,0,0,0,0,0,0);check_vector(actor,0x40,3,3,3);
}
static void lifecycle_tests(void)
{
 int mode;reset();reply(0x002D06E8,(u32)records[3].bytes);CHECK(func_0016EEB0(actor)==actor);CHECK(FIELD(actor,4,const u8 *)==D_0042E7A8);CHECK(FIELD(actor,0x4F0,u32)==5);CHECK(FIELD(actor,0x4F4,u32)==0);CHECK(FIELD(actor,0x4F8,void *)==allocations[2].bytes);CHECK(FIELD(actor,8,u32)==0);CHECK(nth(0x002AEF08,0)->argument[0]==0x28);
 for(mode=0;mode<4;++mode){reset();hook_mode=12;hook=mutate;func_0016EE80(actor,(u32)mode);CHECK(FIELD(actor,4,const u8 *)==D_0042C018);CHECK(count(0x002AF100)==(mode&1));}
 reset();FIELD(actor,0x278,void *)=records[0].bytes;FIELD(records[0].bytes,4,u16)=1;FIELD(records[0].bytes,6,u16)=1;((GeorgeGoalVirtualWord *)(object_table+8))->adjustment=-8;func_0016EA40(actor,0);CHECK(FIELD(records[0].bytes,6,u16)==0);CHECK(nth(0xF0000001,0)->argument[0]==(u32)ADDRESS(records[0].bytes,-8));CHECK(nth(0xF0000001,0)->argument[1]==3);CHECK(FIELD(actor,0x278,u32)==0);CHECK(nth(0x00316040,0)->argument[1]==(u32)records[0].bytes);
 reset();FIELD(actor,0x278,void *)=records[0].bytes;FIELD(records[0].bytes,4,u16)=0;FIELD(records[0].bytes,6,u16)=7;func_0016EA40(actor,0);CHECK(FIELD(records[0].bytes,6,u16)==7);CHECK(count(0xF0000001)==0);CHECK(FIELD(actor,0x278,u32)==0);
 reset();FIELD(actor,0x278,void *)=records[0].bytes;FIELD(records[0].bytes,4,u16)=1;FIELD(records[0].bytes,6,u16)=0;func_0016EA40(actor,0);CHECK(FIELD(records[0].bytes,6,u16)==65535);CHECK(count(0xF0000001)==0);
 reset();FIELD(actor,0x278,void *)=records[0].bytes;FIELD(records[1].bytes,4,u16)=1;FIELD(records[1].bytes,6,u16)=1;hook_mode=10;hook=mutate;func_0016EA40(actor,1);CHECK(FIELD(records[1].bytes,6,u16)==0);CHECK(count(0x002AF100)==1);CHECK(nth(0xF0000001,0)->argument[0]==(u32)records[1].bytes);
}
int main(void){physics_tests();initializer_tests();lifecycle_tests();printf("actor_construction: %d checks passed\n",checks);return 0;}
