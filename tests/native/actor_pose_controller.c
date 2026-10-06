/* Authored controlled-callback tests. No original instructions or game assets. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "george/actor_pose_controller.h"

static unsigned checks;
#define CHECK(test) do { ++checks; if (!(test)) { fprintf(stderr,"pose_controller check %u failed at line %d: %s\n",checks,__LINE__,#test); exit(1); } } while (0)
typedef union Storage { unsigned long long aligned; unsigned char bytes[0x800]; } Storage;
typedef struct Event { u32 kind; u32 argument[4]; float value[16]; } Event;
static Storage objects[4],inputs[4],components[3];
static void *object,*source;
static GeorgeCameraTransform cameras[3];
static float configuration[2][8];
static Event events[96];
static u32 event_count,resource_count,identity_count;
static void (*hook)(Event *);
static GeorgeCameraTransform *allocation;
static GeorgeGoalVirtualWord method_tables[3][2];

const u8 D_0042B078[88]={0},D_0042BD08[88]={0};
const char D_0042B048[]="fixture pose configuration",D_0042B960[]="fixture point configuration";

#include "../../src/game/actor_pose_controller.c"

static u32 bits(float value) { union {float value;u32 bits;} v;v.value=value;return v.bits; }
static float value(u32 encoding) { union {float value;u32 bits;} v;v.bits=encoding;return v.value; }
static void same(float actual,float expected) { CHECK((isnan(actual)&&isnan(expected))||bits(actual)==bits(expected)); }
static Event *record(u32 kind) { Event *e; CHECK(event_count<96);e=&events[event_count++];memset(e,0,sizeof(*e));e->kind=kind;return e; }
static void finish(Event *e) { if(hook)hook(e); }
static Event *find(u32 kind,u32 occurrence) { u32 i;for(i=0;i<event_count;++i)if(events[i].kind==kind){if(occurrence--==0)return &events[i];}CHECK(0);return NULL; }
static u32 count(u32 kind) { u32 n=0,i;for(i=0;i<event_count;++i)n+=events[i].kind==kind;return n; }
static void identity(GeorgeRotationMatrix *matrix) { u32 i;for(i=0;i<16;++i)matrix->element[i]=(i%5==0)?1.0f:0.0f; }

GeorgeScriptObject *func_002D06E8(GeorgeScriptObject *p)
{ Event *e=record(0x2D06E8);e->argument[0]=(u32)p;p->table=NULL;p->vtable=NULL;finish(e);return p; }
void func_002D0700(GeorgeScriptObject *p,u32 flags)
{ Event *e=record(0x2D0700);e->argument[0]=(u32)p;e->argument[1]=flags;e->argument[2]=FIELD(p,4,u32);finish(e); }
void *func_0023C298(const char *name,s32 mode)
{ Event *e=record(0x23C298);void *result;CHECK(resource_count<2);result=configuration[resource_count++];e->argument[0]=(u32)name;e->argument[1]=(u32)mode;finish(e);return result; }
GeorgeCameraTransform *func_0029A100(float x,float y,float z)
{ Event *e=record(0x29A100);e->value[0]=x;e->value[1]=y;e->value[2]=z;finish(e);return allocation; }
void func_0029A168(void *p)
{ Event *e=record(0x29A168);e->argument[0]=(u32)p;finish(e); }
void func_0029A260(GeorgeCameraTransform *output,const GeorgeCameraTransform *input)
{
 Event *e=record(0x29A260);u32 i;u8 scratch[32];e->argument[0]=(u32)output;e->argument[1]=(u32)input;
 /* Original copies three captured 32-byte groups, then the last eight bytes. */
 for(i=0;i<96;i+=32){memcpy(scratch,(const u8 *)input+i,32);memcpy((u8 *)output+i,scratch,32);}
 memcpy(scratch,(const u8 *)input+96,8);memcpy((u8 *)output+96,scratch,8);finish(e);
}
void func_002A1C08(void *output,const void *input)
{ Event *e=record(0x2A1C08);e->argument[0]=(u32)output;e->argument[1]=(u32)input;memmove(output,input,64);finish(e); }
void func_002A1C30(GeorgeRotationMatrix *matrix)
{ Event *e=record(0x2A1C30);e->argument[0]=(u32)matrix;identity(matrix);++identity_count;finish(e); }
void func_0029A308(GeorgeCameraMotionTransform *transform,GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward)
{
 Event *e=record(0x29A308);GeorgeCameraPose *pose=(GeorgeCameraPose *)transform;e->argument[0]=(u32)transform;e->argument[1]=(u32)inverse;e->argument[2]=(u32)forward;
 e->value[0]=pose->field04.x;e->value[1]=pose->field04.y;e->value[2]=pose->field04.z;e->value[3]=pose->field10.x;e->value[4]=pose->field10.y;e->value[5]=pose->field10.z;
 memcpy(&e->value[6],&inverse->element[12],16);finish(e);
}
static void virtual_release(void *adjusted,u32 flags)
{ Event *e=record(0xFE000001);e->argument[0]=(u32)adjusted;e->argument[1]=flags;finish(e); }

static void reset(void)
{
 u32 i,j;memset(objects,0xA5,sizeof(objects));memset(inputs,0,sizeof(inputs));memset(components,0,sizeof(components));memset(cameras,0,sizeof(cameras));memset(events,0,sizeof(events));
 object=objects[0].bytes+0x100;source=inputs[0].bytes+0x100;event_count=resource_count=identity_count=0;hook=NULL;allocation=&cameras[0];
 for(i=0;i<3;++i){cameras[i].field48=2.0f;method_tables[i][1].adjustment=(s16)(i==1?-12:i==2?32767:0);method_tables[i][1].unknown02=0;method_tables[i][1].invoke=virtual_release;FIELD(components[i].bytes+0x100,4,void *)=method_tables[i];}
 for(i=0;i<2;++i)for(j=0;j<8;++j)configuration[i][j]=(float)(i*10+j+1);
 configuration[0][0]=60;configuration[0][1]=0.25f;
 identity(FRAME(source,0));FRAME(source,0)->element[12]=7;FRAME(source,0)->element[13]=8;FRAME(source,0)->element[14]=9;
 FIELD(object,8,GeorgeCameraTransform *)=&cameras[0];FIELD(object,0x10C,void *)=configuration[1];FIELD(object,0xD8,u32)=0;
}
static void expect_common(void *p)
{
 CHECK(FIELD(p,4,const u8 *)==D_0042B078);CHECK(TRANSFORM(p)==&cameras[0]);CHECK(FIELD(p,0xC,u32)==0);CHECK(FIELD(p,0xE0,void *)==configuration[0]);
 CHECK(FIELD(p,0xD0,u32)==0);CHECK(FIELD(p,0xD4,u32)==0);CHECK(FIELD(p,0xD8,u32)==0);CHECK(FIELD(p,0xDC,u32)==0);
 same(cameras[0].field3C,0.25f);same(cameras[0].field44,30.0f);CHECK(find(0x23C298,0)->argument[0]==(u32)D_0042B048);CHECK(find(0x23C298,0)->argument[1]==0xFFFFFFFFU);
 CHECK(find(0x29A308,0)->argument[0]==(u32)&cameras[0]);CHECK(find(0x29A308,0)->argument[1]==(u32)ADDRESS(p,0x50));CHECK(find(0x29A308,0)->argument[2]==(u32)ADDRESS(p,0x90));
}
static void mutate_initializer(Event *e)
{
 if(e->kind==0x23C298){FRAME(source,0)->element[12]=17;FRAME(source,0)->element[13]=18;FRAME(source,0)->element[14]=19;}
 if(e->kind==0x2A1C30&&identity_count==2){FIELD(object,8,GeorgeCameraTransform *)=&cameras[1];FIELD(object,0xE0,void *)=configuration[1];configuration[1][0]=99;configuration[1][1]=0.75f;cameras[1].field48=3;}
}
static void initialize_tests(void)
{
 u32 i;reset();CHECK(func_00166CA8(object,FRAME(source,0))==object);expect_common(object);
 same(find(0x29A100,0)->value[0],7);same(find(0x29A100,0)->value[1],8);same(find(0x29A100,0)->value[2],9);
 for(i=0;i<16;++i)same(FRAME(object,0x10)->element[i],FRAME(source,0)->element[i]);CHECK(count(0x2A1C08)==1);CHECK(count(0x2A1C30)==2);
 reset();CHECK(func_00166DD0(object)==object);expect_common(object);for(i=0;i<3;++i)same(find(0x29A100,0)->value[i],0);CHECK(count(0x2A1C30)==3);CHECK(count(0x2A1C08)==0);
 for(i=0;i<16;++i)same(FRAME(object,0x10)->element[i],(i%5==0)?1.0f:0.0f);
 reset();hook=mutate_initializer;func_00166CA8(object,FRAME(source,0));same(find(0x29A100,0)->value[0],17);same(find(0x29A100,0)->value[1],18);same(find(0x29A100,0)->value[2],19);CHECK(TRANSFORM(object)==&cameras[1]);same(cameras[1].field3C,0.75f);same(cameras[1].field44,33);CHECK(find(0x29A308,0)->argument[0]==(u32)&cameras[1]);
}
static void mutate_copy(Event *e)
{
 if(e->kind==0x29A100){FIELD(source,8,GeorgeCameraTransform *)=&cameras[2];cameras[2].pose.fields.field04.x=91;}
 if(e->kind==0x29A260)FIELD(source,0xC,u32)=0x87654321U;
 if(e->kind==0x2A1C08&&e->argument[0]==(u32)ADDRESS(object,0x10)){FRAME(source,0x50)->element[2]=123;FIELD(source,0xD0,float)=8;FIELD(source,0xD4,float)=9;}
}
static void copy_tests(void)
{
 u32 i,j;reset();FIELD(source,8,GeorgeCameraTransform *)=&cameras[1];cameras[1].field44=121;FIELD(source,0xC,u32)=0x12345678;FIELD(source,0xD0,float)=1.5f;FIELD(source,0xD4,float)=-2.5f;
 for(i=0;i<3;++i)for(j=0;j<16;++j)FRAME(source,0x10+i*0x40)->element[j]=(float)(i*20+j);
 CHECK(func_00166EB8(object,source)==object);CHECK(FIELD(object,4,const u8 *)==D_0042B078);CHECK(TRANSFORM(object)==&cameras[0]);CHECK(FIELD(object,0xC,u32)==0x12345678);same(cameras[0].field44,121);
 for(i=0;i<3;++i)for(j=0;j<16;++j)same(FRAME(object,0x10+i*0x40)->element[j],(float)(i*20+j));same(FIELD(object,0xD0,float),1.5f);same(FIELD(object,0xD4,float),-2.5f);
 CHECK(FIELD(object,0xD8,u32)==0);CHECK(FIELD(object,0xDC,u32)==0xA5A5A5A5U);CHECK(FIELD(object,0xE0,u32)==0xA5A5A5A5U);
 reset();FIELD(source,8,GeorgeCameraTransform *)=&cameras[1];hook=mutate_copy;func_00166EB8(object,source);CHECK(find(0x29A260,0)->argument[1]==(u32)&cameras[2]);same(cameras[0].pose.fields.field04.x,91);CHECK(FIELD(object,0xC,u32)==0x87654321U);same(FRAME(object,0x50)->element[2],123);same(FIELD(object,0xD0,float),8);same(FIELD(object,0xD4,float),9);
 /* Sequential final reads preserve a shifted source/destination overlap. */
 reset();source=ADDRESS(object,0x10);FIELD(source,8,GeorgeCameraTransform *)=&cameras[1];FIELD(source,0xC,u32)=0;FIELD(source,0xD0,float)=31;FIELD(source,0xD4,float)=32;func_00166EB8(object,source);same(FIELD(object,0xD0,float),31);same(FIELD(object,0xD4,float),32);
}
static void mutate_free(Event *e)
{
 if(e->kind==0x29A168)FIELD(object,0xC,void *)=components[1].bytes+0x100;
 if(e->kind==0xFE000001){FIELD(object,0xC,void *)=NULL;FIELD(object,4,u32)=0x55667788;}
}
static void release_tests(void)
{
 u32 flags[4]={0,1,3,0x80000002U},i;for(i=0;i<4;++i){reset();FIELD(object,0xC,void *)=components[0].bytes+0x100;func_00166F58(object,flags[i]);CHECK(find(0x29A168,0)->argument[0]==(u32)&cameras[0]);CHECK(find(0xFE000001,0)->argument[0]==(u32)(components[0].bytes+0x100));CHECK(find(0xFE000001,0)->argument[1]==3);CHECK(find(0x2D0700,0)->argument[1]==flags[i]);CHECK(find(0x2D0700,0)->argument[2]==(u32)D_0042B078);}
 reset();FIELD(object,8,void *)=NULL;FIELD(object,0xC,void *)=NULL;func_00166F58(object,2);CHECK(count(0x29A168)==0);CHECK(count(0xFE000001)==0);CHECK(count(0x2D0700)==1);
 reset();FIELD(object,0xC,void *)=components[0].bytes+0x100;hook=mutate_free;func_00166F58(object,7);CHECK(find(0xFE000001,0)->argument[0]==(u32)(components[1].bytes+0x100-12));CHECK(find(0x2D0700,0)->argument[2]==0x55667788);
 reset();FIELD(object,8,void *)=NULL;FIELD(object,0xC,void *)=components[2].bytes+0x100;func_00166F58(object,0);CHECK(find(0xFE000001,0)->argument[0]==(u32)(components[2].bytes+0x100)+32767U);
}
static GeorgeMathVec3 target(const void *p,u32 base)
{
 GeorgeMathVec3 v;float a=FIELD(p,base+8,float),b=FIELD(p,base+4,float),c=FIELD(p,base,float);
 v.x=((FIELD(p,0x40,float)+a*FIELD(p,0x30,float))+b*FIELD(p,0x20,float))+c*FIELD(p,0x10,float);
 v.y=((FIELD(p,0x44,float)+a*FIELD(p,0x34,float))+b*FIELD(p,0x24,float))+c*FIELD(p,0x14,float);
 v.z=((FIELD(p,0x48,float)+a*FIELD(p,0x38,float))+b*FIELD(p,0x28,float))+c*FIELD(p,0x18,float);return v;
}
static void check_vector(const GeorgeMathVec3 *a,const GeorgeMathVec3 *b) {same(a->x,b->x);same(a->y,b->y);same(a->z,b->z);}
static void derived_tests(void)
{
 u32 i;GeorgeMathVec3 first,second;reset();for(i=0;i<16;++i)FRAME(source,0)->element[i]=(float)i/2.0f;
 CHECK(func_0016B948(object,FRAME(source,0))==object);CHECK(FIELD(object,4,const u8 *)==D_0042BD08);CHECK(FIELD(object,0x10C,void *)==configuration[1]);CHECK(cameras[0].pose.fields.field00==2);same(FIELD(object,0x108,float),configuration[1][6]);
 for(i=0;i<6;++i)same(FIELD(object,0xF0+4*i,float),configuration[1][i]);first=target(object,0xFC);second=target(object,0xF0);check_vector(&cameras[0].pose.fields.field04,&first);check_vector(&cameras[0].pose.fields.field10,&second);CHECK(find(0x23C298,1)->argument[0]==(u32)D_0042B960);CHECK(count(0x29A308)==1);
}
static void setup_update(u32 seed)
{
 u32 i;reset();for(i=0;i<16;++i)FIELD(object,0x10+4*i,float)=(float)((s32)((seed*17+i*23)%83)-41)*0.25f;
 for(i=0;i<7;++i)configuration[1][i]=(float)((s32)((seed*7+i*11)%17)-8)*0.125f;
 cameras[0].pose.fields.field04.x=-11;cameras[0].pose.fields.field04.y=12;cameras[0].pose.fields.field04.z=-13;
 cameras[0].pose.fields.field10.x=3;cameras[0].pose.fields.field10.y=-4;cameras[0].pose.fields.field10.z=5;
}
static void update_tests(void)
{
 u32 i,j,k;float durations[7]={0,-1,2,0.5f,0,0,1};float steps[7]={0.25f,1,0.25f,2,-0.5f,1,-2};u8 snapshot_bytes[0x110];
 durations[4]=value(0x7FC12345);durations[5]=value(0x7F800000);
 for(i=0;i<48;++i)for(j=0;j<7;++j){GeorgeMathVec3 first,second,old_first,old_second;float d=durations[j],ratio;setup_update(i);configuration[1][6]=d;
  /* Independent observed-field formula uses values copied from the resource. */
  for(k=0;k<6;++k)FIELD(object,0xF0+4*k,float)=configuration[1][k];
  first=target(object,0xFC);second=target(object,0xF0);old_first=cameras[0].pose.fields.field04;old_second=cameras[0].pose.fields.field10;
  if(d>0){ratio=steps[j]/d;first.x=old_first.x+ratio*(first.x-old_first.x);first.z=old_first.z+ratio*(first.z-old_first.z);first.y=old_first.y+ratio*(first.y-old_first.y);second.x=old_second.x+ratio*(second.x-old_second.x);second.z=old_second.z+ratio*(second.z-old_second.z);second.y=old_second.y+ratio*(second.y-old_second.y);}
  func_0016BDA0(object,steps[j]);check_vector(&cameras[0].pose.fields.field04,&first);check_vector(&cameras[0].pose.fields.field10,&second);same(FIELD(object,0x108,float),d);same(FIELD(object,0x80,float),second.x);same(FIELD(object,0x84,float),second.y);same(FIELD(object,0x88,float),second.z);same(FIELD(object,0x8C,float),1);CHECK(count(0x29A308)==1);CHECK(find(0x29A308,0)->argument[0]==(u32)&cameras[0]);
 }
 reset();FIELD(object,0xD8,u32)=0x80000000U;memcpy(snapshot_bytes,object,sizeof(snapshot_bytes));func_0016BDA0(object,0.5f);CHECK(event_count==0);CHECK(memcmp(snapshot_bytes,object,sizeof(snapshot_bytes))==0);
 /* No clamp: elapsed greater than duration intentionally extrapolates. */
 setup_update(5);configuration[1][6]=1;func_0016BDA0(object,3);CHECK(count(0x29A308)==1);
 /* First point X aliases the stored transform pointer. Its bit-preserving
  * publication must redirect the independently reloaded second point target. */
 reset();FIELD(object,8,void *)=ADDRESS(object,4);configuration[1][6]=0;
 for(i=0;i<6;++i)configuration[1][i]=0;
 for(i=0;i<12;++i)FIELD(object,0x10+4*i,float)=0;
 FIELD(object,0x40,float)=value((u32)&cameras[1]);FIELD(object,0x44,float)=7;FIELD(object,0x48,float)=8;
 func_0016BDA0(object,1);CHECK(TRANSFORM(object)==&cameras[1]);CHECK(bits(cameras[1].pose.fields.field10.x)==(u32)&cameras[1]);same(cameras[1].pose.fields.field10.y,7);same(cameras[1].pose.fields.field10.z,8);CHECK(find(0x29A308,0)->argument[0]==(u32)&cameras[1]);
}
int main(void)
{
 CHECK(sizeof(void *)==4);CHECK(sizeof(GeorgeCameraTransform)==0x68);
 initialize_tests();copy_tests();release_tests();derived_tests();update_tests();printf("actor_pose_controller: %u checks passed\n",checks);return 0;
}
