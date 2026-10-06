/* Authored asset-free branch, callback, alias and numeric observations.
 * Jitter reads indeterminate original stack input and is deliberately excluded. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../src/game/actor_camera_state.c"

typedef union Storage {GeorgeActorBits64 alignment;u8 bytes[0x1000];} Storage;
typedef struct Event {u32 kind;u32 argument[8];float values[16];GeorgeActorBits64 wide[2];} Event;
static Storage objects[3],paths[2],inputs;
static void *object,*lookup_result,*object_results[2],*path_results[2],*configuration_result;
static GeorgeCameraTransform cameras[3];
static GeorgeDeimosPoolNode nodes[3];
static GeorgeDeimosValue arguments[5],alternate_arguments[5];
static GeorgeMathVec3 reference,points[2];
static float configuration[2][4],fraction_result;
static Event events[128];
static unsigned checks,event_count,object_lookup_count,path_lookup_count,reference_count;
static void (*hook)(Event *);
GeorgeDeimosPoolNode *D_003F83D4;
GeorgeDeimosValue *D_00474F48;
GeorgeDeimosValue D_00474748[16];
float D_FLT_003F8A70,D_FLT_003F8A74;
static void *mode_result;
const u8 D_0042B680[88]={0};
const char D_0042B0E0[]="fixture D_0042B0E0";
const char D_0042B0F0[]="fixture D_0042B0F0";
const char D_0042B100[]="fixture D_0042B100";
const char D_0042B120[]="fixture D_0042B120";
const char D_0042B130[]="fixture D_0042B130";
const char D_0042B148[]="fixture D_0042B148";
const char D_0042B160[]="fixture D_0042B160";
const char D_0042B180[]="fixture D_0042B180";
const char D_0042B190[]="fixture D_0042B190";
const char D_0042B1A8[]="fixture D_0042B1A8";
const char D_0042B1B8[]="fixture D_0042B1B8";
const char D_0042B1D0[]="fixture D_0042B1D0";
const char D_0042B1E8[]="fixture D_0042B1E8";
const char D_0042B208[]="fixture D_0042B208";
const char D_0042B220[]="fixture D_0042B220";
const char D_0042B240[]="fixture D_0042B240";
const char D_0042B258[]="fixture D_0042B258";
const char D_0042B278[]="fixture D_0042B278";
const char D_0042B290[]="fixture D_0042B290";
const char D_0042B2B0[]="fixture D_0042B2B0";
const char D_0042B2C0[]="fixture D_0042B2C0";
const char D_0042B2D8[]="fixture D_0042B2D8";
const char D_0042B2E8[]="fixture D_0042B2E8";
const char D_0042B300[]="fixture D_0042B300";
const char D_0042B310[]="fixture D_0042B310";
const char D_0042B328[]="fixture D_0042B328";
const char D_0042B330[]="fixture D_0042B330";
const char D_0042B340[]="fixture D_0042B340";
const char D_0042B358[]="fixture D_0042B358";
const char D_0042B378[]="fixture D_0042B378";
const char D_0042B388[]="fixture D_0042B388";
const char D_0042B3A0[]="fixture D_0042B3A0";
const char D_0042B3E0[]="fixture D_0042B3E0";
const char D_0042B428[]="fixture D_0042B428";
const char D_0042B488[]="fixture D_0042B488";
const char D_0042B4F8[]="fixture D_0042B4F8";
const char D_0042B548[]="fixture D_0042B548";
const char D_0042B590[]="fixture D_0042B590";
const char D_0042B5D0[]="fixture D_0042B5D0";

#define CHECK(c) do {++checks;if(!(c)){fprintf(stderr,"camera_state check %u line %d: %s\n",checks,__LINE__,#c);exit(1);}}while(0)
static u32 bits(float f){union{float f;u32 u;}v;v.f=f;return v.u;}
static float scalar(u32 u){union{float f;u32 u;}v;v.u=u;return v.f;}
static GeorgeActorBits64 wide_bits(double d){union{double d;GeorgeActorBits64 u;}v;v.d=d;return v.u;}
static double wide_value(GeorgeActorBits64 u){union{double d;GeorgeActorBits64 u;}v;v.u=u;return v.d;}
static void same(float a,float b){CHECK((isnan(a)&&isnan(b))||bits(a)==bits(b));}
static void close_value(float a,float b){CHECK((isnan(a)&&isnan(b))||fabsf(a-b)<0.00001f);}
static void vector_value(void *p,u32 o,float x,float y,float z){FIELD(p,o,float)=x;FIELD(p,o+4,float)=y;FIELD(p,o+8,float)=z;}
static void vector_same(const void *p,u32 o,float x,float y,float z){close_value(FIELD(p,o,float),x);close_value(FIELD(p,o+4,float),y);close_value(FIELD(p,o+8,float),z);}
static Event *event(u32 kind){Event *e;CHECK(event_count<128);e=&events[event_count++];memset(e,0,sizeof(*e));e->kind=kind;return e;}
static void finish(Event *e){if(hook)hook(e);}
static u32 count(u32 kind){u32 i,n=0;for(i=0;i<event_count;++i)n+=events[i].kind==kind;return n;}
static Event *nth(u32 kind,u32 occurrence){u32 i;for(i=0;i<event_count;++i)if(events[i].kind==kind&&occurrence--==0)return &events[i];CHECK(0);return NULL;}
static void identity(GeorgeRotationMatrix *m){u32 i;memset(m,0,sizeof(*m));for(i=0;i<4;++i)m->element[5*i]=1;}
static void reset(void)
{
 u32 i;memset(objects,0,sizeof(objects));memset(paths,0,sizeof(paths));memset(cameras,0,sizeof(cameras));memset(arguments,0,sizeof(arguments));memset(alternate_arguments,0,sizeof(alternate_arguments));memset(D_00474748,0xA5,sizeof(D_00474748));memset(events,0,sizeof(events));
 object=objects[0].bytes+0x100;lookup_result=object;object_results[0]=objects[1].bytes+0x100;object_results[1]=objects[2].bytes+0x100;path_results[0]=paths[0].bytes+0x100;path_results[1]=paths[1].bytes+0x100;
 event_count=object_lookup_count=path_lookup_count=reference_count=0;hook=NULL;D_003F83D4=NULL;D_00474F48=arguments;mode_result=NULL;configuration_result=configuration[0];fraction_result=0.25f;
 for(i=0;i<2;++i){configuration[i][0]=(float)(i+2);configuration[i][1]=0;configuration[i][2]=2;configuration[i][3]=3;FIELD(path_results[i],0x30,float)=8;}
 D_FLT_003F8A74=1;D_FLT_003F8A70=0.25f;FIELD(object,0x160,void *)=configuration[0];FIELD(object,8,void *)=&cameras[0];FIELD(object,0x154,float)=10;FIELD(object,0x138,float)=-1;FIELD(object,0x120,u32)=1;
 identity(FRAME(object,0x10));identity(FRAME(object,0x50));identity(FRAME(object,0x90));
 vector_value(object,0x40,9,11,13);vector_value(object,0xF0,1,2,3);vector_value(object,0x108,5,10,15);vector_value(object,0xFC,4,6,8);vector_value(object,0x114,8,12,16);
 reference.x=3;reference.y=4;reference.z=0;points[0].x=points[0].y=points[0].z=0;points[1].x=10;points[1].y=points[1].z=0;
 arguments[1].tag=6;arguments[1].payload.bits=0x11111111U;arguments[2].tag=2;arguments[2].payload.scalar=2;arguments[3].tag=2;arguments[3].payload.scalar=2;
}

void *func_00166CA8(void *p,const GeorgeRotationMatrix *m)
{Event *e=event(0x166CA8);e->argument[0]=(u32)p;e->argument[1]=(u32)m;memmove(ADDRESS(p,0x10),m,64);FIELD(p,8,void *)=&cameras[0];finish(e);return p;}
void *func_0023C298(const char *name,s32 mode)
{Event *e=event(0x23C298);e->argument[0]=(u32)name;e->argument[1]=(u32)mode;finish(e);return configuration_result;}
void *func_002D0B48(u32 key)
{Event *e=event(0x2D0B48);e->argument[0]=key;finish(e);return lookup_result;}
void *func_002CDF90(void)
{Event *e=event(0x2CDF90);finish(e);return mode_result;}
void func_002CC938(const char *text,...)
{Event *e=event(0x2CC938);e->argument[0]=(u32)text;finish(e);}
GeorgeDeimosPoolNode *func_002CD348(GeorgeDeimosHashTable *p)
{Event *e=event(0x2CD348);e->argument[0]=(u32)p;finish(e);return &nodes[0];}
void func_002CD0B8(GeorgeDeimosPoolNode *p)
{Event *e=event(0x2CD0B8);e->argument[0]=(u32)p;e->argument[1]=(u32)D_003F83D4;finish(e);}
void func_002CE6B8(GeorgeDeimosPoolNode *p,const char *label,const char *types,u32 n,void(*callback)(s32,s32),u32 flags)
{Event *e=event(0x2CE6B8);e->argument[0]=(u32)p;e->argument[1]=(u32)label;e->argument[2]=(u32)types;e->argument[3]=n;e->argument[4]=(u32)callback;e->argument[5]=flags;finish(e);}
void *func_00239FD8(u32 key)
{Event *e=event(0x239FD8);void *result;CHECK(object_lookup_count<2);result=object_results[object_lookup_count++];e->argument[0]=key;finish(e);return result;}
void *func_00238BA0(void *p,u32 key)
{Event *e=event(0x238BA0);void *result;CHECK(path_lookup_count<2);result=path_results[path_lookup_count++];e->argument[0]=(u32)p;e->argument[1]=key;finish(e);return result;}
GeorgeMathVec3 *func_00161C30(s32 index)
{Event *e=event(0x161C30);e->argument[0]=(u32)index;++reference_count;finish(e);return &reference;}
void func_00135D10(void *p,GeorgeMathVec3 *out,const GeorgeMathVec3 *ref)
{Event *e=event(0x135D10);u32 index=(p==path_results[1]);e->argument[0]=(u32)p;e->argument[1]=(u32)out;e->argument[2]=(u32)ref;e->argument[3]=FIELD(object,0x120,u32);*out=points[index];finish(e);}
float func_00135D88(void *p,const GeorgeMathVec3 *ref)
{Event *e=event(0x135D88);e->argument[0]=(u32)p;e->argument[1]=(u32)ref;finish(e);return fraction_result;}
void func_00135E88(void *p,GeorgeMathVec3 *out,float fraction)
{Event *e=event(0x135E88);e->argument[0]=(u32)p;e->argument[1]=(u32)out;e->argument[2]=FIELD(object,0x120,u32);e->values[0]=fraction;out->x=fraction;out->y=2*fraction;out->z=3*fraction;finish(e);}
GeorgeActorBits64 func_00374848(float f)
{Event *e=event(0x374848);e->values[0]=f;finish(e);return wide_bits((double)f);}
s32 func_00373250(GeorgeActorBits64 a,GeorgeActorBits64 b)
{Event *e=event(0x373250);double left=wide_value(a),right=wide_value(b);e->wide[0]=a;e->wide[1]=b;finish(e);return(isnan(left)||isnan(right))?1:left<right?-1:left>right?1:0;}
GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 a,GeorgeActorBits64 b)
{Event *e=event(0x372CC0);e->wide[0]=a;e->wide[1]=b;finish(e);return wide_bits(wide_value(a)-wide_value(b));}
s32 func_00397178(void){CHECK(0);return 0;} /* Jitter is intentionally unexecuted. */
float func_0029B940(float y,float x){(void)y;(void)x;CHECK(0);return 0;}
float func_0029C090(float x){(void)x;CHECK(0);return 0;}
float func_0029C168(float x){(void)x;CHECK(0);return 0;}
float func_0029C230(float x){(void)x;CHECK(0);return 0;}
void func_0029A308(GeorgeCameraMotionTransform *p,GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward)
{Event *e=event(0x29A308);GeorgeCameraPose *pose=(GeorgeCameraPose *)p;e->argument[0]=(u32)p;e->argument[1]=(u32)inverse;e->argument[2]=(u32)forward;memcpy(e->values,&pose->field04,24);memcpy(e->values+6,inverse->element+12,16);finish(e);}

static void constructor_hook(Event *e)
{if(e->kind==0x23C298){CHECK(FIELD(object,4,const u8 *)==D_0042B680);same(FIELD(object,0x154,float),10);FIELD(object,8,void *)=&cameras[1];vector_value(object,0x40,17,18,19);configuration[0][0]=4;}}
static void constructor_tests(void)
{
 u32 i;GeorgeRotationMatrix *input=FRAME(inputs.bytes,0);identity(input);input->element[12]=1;input->element[13]=2;input->element[14]=3;
 reset();CHECK(func_001670A8(object,input)==object);CHECK(FIELD(object,4,const u8 *)==D_0042B680);CHECK(FIELD(object,0x160,void *)==configuration[0]);same(FIELD(object,0x148,float),2);same(FIELD(object,0x154,float),10);CHECK(cameras[0].pose.fields.field00==2);
 vector_same(object,0xF0,1,4.8f,3);vector_same(object,0xFC,1,2,3);vector_same(object,0x108,1,4.8f,3);vector_same(object,0x114,1,2,3);
 for(i=0x124;i<=0x144;i+=4)CHECK(FIELD(object,i,u32)==0);CHECK(FIELD(object,0x158,u32)==0);CHECK(FIELD(object,0x15C,u32)==0);CHECK(FIELD(object,0x150,u32)==0);CHECK(FIELD(object,0x14C,u32)==0);CHECK(FIELD(object,0x120,u32)==0);
 CHECK(nth(0x23C298,0)->argument[0]==(u32)D_0042B0E0);CHECK(nth(0x23C298,0)->argument[1]==0xFFFFFFFFU);
 reset();hook=constructor_hook;func_001670A8(object,input);same(FIELD(object,0x148,float),4);CHECK(cameras[1].pose.fields.field00==2);vector_same(object,0xF0,17,20.8f,19);vector_same(object,0x114,17,18,19);
}
static void registry_hook(Event *e)
{if(e->kind==0x2CE6B8)D_003F83D4=&nodes[count(0x2CE6B8)%3];}
static void registry_tests(void)
{
 const char *labels[15]={D_0042B0F0,D_0042B120,D_0042B148,D_0042B180,D_0042B1A8,D_0042B1D0,D_0042B208,D_0042B240,D_0042B278,D_0042B2B0,D_0042B2D8,D_0042B300,D_0042B328,D_0042B340,D_0042B378};
 const char *types[15]={D_0042B100,D_0042B130,D_0042B160,D_0042B190,D_0042B1B8,D_0042B1E8,D_0042B220,D_0042B258,D_0042B290,D_0042B2C0,D_0042B2E8,D_0042B310,D_0042B330,D_0042B358,D_0042B388};
 void(*callbacks[15])(s32,s32)={func_00168398,func_001683F0,func_00167B98,func_00167CC0,func_00167F98,func_001683F0,func_00167B98,func_00167CC0,func_00167F98,func_001684D0,func_001684F8,func_00168550,func_001685A8,func_00168630,func_00168660};
 u32 counts[15]={2,2,3,0xFFFFFFFFU,3,2,3,0xFFFFFFFFU,3,1,2,2,2,1,2},i,n;
 reset();hook=registry_hook;CHECK(func_001678E0()==&nodes[0]);CHECK(count(0x2CE6B8)==15);CHECK(nth(0x2CD0B8,0)->argument[0]==(u32)&nodes[0]);CHECK(nth(0x2CD0B8,0)->argument[1]==(u32)&nodes[0]);
 for(i=0;i<15;++i){Event *e=nth(0x2CE6B8,i);CHECK(e->argument[0]==(u32)&nodes[i%3]);CHECK(e->argument[1]==(u32)labels[i]);CHECK(e->argument[2]==(u32)types[i]);CHECK(e->argument[3]==counts[i]);CHECK(e->argument[4]==(u32)callbacks[i]);CHECK(e->argument[5]==(i>=5&&i<=8));}
 n=event_count;CHECK(func_001678E0()==&nodes[0]);CHECK(event_count==n);reset();D_003F83D4=&nodes[2];CHECK(func_001678E0()==&nodes[2]);CHECK(event_count==0);
}
static void updater_tests(void)
{
 u32 i,j;float phases[9]={0,0.3f,0.30001f,0.5f,0.69999f,0.7f,1,-1,0};phases[8]=scalar(0x7FC12345);
 for(i=0;i<9;++i)for(j=0;j<3;++j){reset();FIELD(object,0x138,float)=0;FIELD(object,0x140,float)=phases[i];FIELD(object,0x144,float)=1;FIELD(object,0x13C,float)=7;FIELD(object,0x130,float)=3;FIELD(object,0x158,u32)=j;
  func_00168290(object);if(j==0&&0.3f<phases[i]&&phases[i]<0.7f){CHECK(FIELD(object,0x158,u32)==1);same(FIELD(object,0x13C,float),10);}else{CHECK(FIELD(object,0x158,u32)==j);same(FIELD(object,0x13C,float),7);}CHECK(event_count==0);
 }
 reset();FIELD(object,0x138,float)=-1;FIELD(object,0x12C,float)=5;FIELD(object,0x134,float)=1;func_00168290(object);same(FIELD(object,0x13C,float),5);same(FIELD(object,0x140,float),0);same(FIELD(object,0x138,float),0);same(FIELD(object,0x144,float),10/(9.8f-1));CHECK(FIELD(object,0x158,u32)==0);
 reset();FIELD(object,0x138,float)=scalar(0x7FC12345);FIELD(object,0x158,u32)=1;func_00168290(object);CHECK(isnan(FIELD(object,0x138,float)));CHECK(FIELD(object,0x158,u32)==1);
}
static void update_refresh_hook(Event *e)
{if(e->kind==0x29A308&&count(0x29A308)==1){FIELD(object,8,void *)=&cameras[1];cameras[1].pose.fields.field10.y=100;FIELD(object,0x138,float)=2;FIELD(object,0x13C,float)=4;FIELD(object,0x140,float)=10;}}
static void update_soft_hook(Event *e)
{if(e->kind==0x373250&&count(0x373250)==2){FIELD(object,0x160,void *)=configuration[1];configuration[1][2]=0.5f;FIELD(object,0x14C,float)=20;FIELD(object,0x154,float)=4;}}
static void update_alias_hook(Event *e)
{if(e->kind==0x29A308&&count(0x29A308)==1){FIELD(object,8,void *)=ADDRESS(object,0x12C);FIELD(object,0x138,float)=2;FIELD(object,0x13C,float)=3;FIELD(object,0x140,float)=10;}}
static void update_tests(void)
{
 u32 i,j;float steps[7]={0,0.25f,0.5f,1,2,-0.5f,0};u32 suppression[4]={0,1,0x80000000U,3};u8 before[0x170];steps[6]=scalar(0x7FC12345);
 for(i=0;i<7;++i)for(j=0;j<4;++j){float factor,remaining;reset();FIELD(object,0x124,float)=1;FIELD(object,0x15C,u32)=suppression[j];remaining=1;
  if(suppression[j]==0){remaining-=steps[i];factor=steps[i]/remaining;if(!(factor>=0))factor=0;else factor=george_ee_minimum(factor,1);}
  else factor=1;
  func_001671D8(object,steps[i]);vector_same(&cameras[0].pose.fields.field10,0,1+4*factor,2+8*factor,3+12*factor);vector_same(&cameras[0].pose.fields.field04,0,4+4*factor,6+6*factor,8+8*factor);
  same(FIELD(object,0x124,float),remaining<0?0:remaining);CHECK(FIELD(object,0x15C,u32)==(suppression[j]>0&&suppression[j]<0x80000000U?suppression[j]-1:suppression[j]));CHECK(count(0x29A308)==2);CHECK(count(0x374848)==0);CHECK(count(0x373250)==0);
 }
 reset();FIELD(object,0xD8,u32)=0x80000000U;memcpy(before,object,sizeof(before));func_001671D8(object,1);CHECK(event_count==0);CHECK(memcmp(before,object,sizeof(before))==0);
 reset();FIELD(object,0xC,u32)=1;func_001671D8(object,1);CHECK(count(0x29A308)==1);same(FIELD(object,0xF0,float),1);same(FIELD(object,0x138,float),-1);
 reset();configuration[0][1]=0.5f;func_001671D8(object,0.25f);vector_same(&cameras[0].pose.fields.field10,0,1.5f,3,4.5f);vector_same(&cameras[0].pose.fields.field04,0,4.5f,6.75f,9);
 reset();FIELD(object,0x160,void *)=ADDRESS(object,0xEC);func_001671D8(object,0.5f);vector_same(&cameras[0].pose.fields.field10,0,3,6,9);vector_same(&cameras[0].pose.fields.field04,0,10,15,20);
 reset();FIELD(object,0x120,u32)=0;FIELD(object,0x128,float)=2;func_001671D8(object,0.25f);vector_same(object,0x114,9,13,13);vector_same(&cameras[0].pose.fields.field04,0,8,12,16);
 reset();FIELD(object,0x148,float)=2;FIELD(object,0x150,float)=1;func_001671D8(object,0.25f);same(FIELD(object,0x14C,float),5);vector_same(object,0x114,8,12,17);vector_same(object,0x80,0,0,2);CHECK(nth(0x373250,1)->wide[1]==0x3FB99999A0000000ULL);
 reset();FIELD(object,0x148,float)=2;FIELD(object,0x14C,float)=-0.1f;func_001671D8(object,0.25f);same(FIELD(object,0x14C,float),0);CHECK(count(0x372CC0)==0);
 reset();FIELD(object,0x148,float)=2;FIELD(object,0x14C,float)=3;func_001671D8(object,0.25f);same(FIELD(object,0x14C,float),-4.5f);CHECK(count(0x372CC0)==1);
 reset();FIELD(object,0x148,float)=2;FIELD(object,0x150,float)=1;hook=update_soft_hook;func_001671D8(object,0.25f);same(FIELD(object,0x14C,float),20.5f);
 reset();FIELD(object,0x138,float)=0;FIELD(object,0x13C,float)=3;FIELD(object,0x140,float)=1;func_001671D8(object,0.25f);same(FIELD(object,0x138,float),0.75f);same(FIELD(object,0x13C,float),3-0.25f*19.6f);same(FIELD(object,0x140,float),1.25f);same(cameras[0].pose.fields.field10.y,10.75f);
 reset();hook=update_refresh_hook;func_001671D8(object,0.5f);same(FIELD(object,0x138,float),4);same(cameras[1].pose.fields.field10.y,104);CHECK(nth(0x29A308,1)->argument[0]==(u32)&cameras[1]);
 reset();hook=update_alias_hook;func_001671D8(object,0.5f);same(FIELD(object,0x140,float),14);CHECK(nth(0x29A308,1)->argument[0]==(u32)ADDRESS(object,0x12C));
 reset();FIELD(object,0x124,float)=1;D_FLT_003F8A74=-0.0f;D_FLT_003F8A70=0.25f;func_001671D8(object,0.5f);same(FIELD(object,0x124,float),0.75f);
 reset();FIELD(object,0x124,float)=1;D_FLT_003F8A74=scalar(0x7FC12345);func_001671D8(object,0.5f);same(FIELD(object,0x124,float),0.5f);
 /* First transform-output X aliases the object's stored transform pointer.
  * The separately reloaded second output and both refresh calls use it. */
 reset();FIELD(object,8,void *)=ADDRESS(object,4);vector_value(object,0x114,scalar((u32)&cameras[1]),7,8);func_001671D8(object,0.25f);CHECK(TRANSFORM(object)==&cameras[1]);vector_same(&cameras[1].pose.fields.field10,0,5,10,15);CHECK(nth(0x29A308,0)->argument[0]==(u32)&cameras[1]);CHECK(nth(0x29A308,1)->argument[0]==(u32)&cameras[1]);
 /* First point stores overlap the later source vector; second point must
  * consume those new values, rather than a snapshot made before first output. */
 reset();FIELD(object,8,void *)=ADDRESS(object,0xEC);func_001671D8(object,0.25f);vector_same(object,0xF0,8,12,16);vector_same(object,0xFC,8,12,16);CHECK(nth(0x29A308,0)->argument[0]==(u32)ADDRESS(object,0xEC));
}

static void gate_tests(void)
{
 void(*setters[3])(s32,s32)={func_00168398,func_001684F8,func_00168550};u32 offsets[3]={0x148,0x128,0x124};
 s16 tags[7]={2,0,1,6,-1,-32768,32767};u32 i,j;
 for(i=0;i<3;++i)for(j=0;j<7;++j){reset();arguments[1].tag=(u16)tags[j];arguments[1].payload.scalar=scalar(0x80000000U);FIELD(object,offsets[i],float)=123;setters[i](-7,-9);
  CHECK(count(0x2D0B48)==1);CHECK(nth(0x2D0B48,0)->argument[0]==0x9A825260U);CHECK(count(0x2CC938)==(tags[j]!=2));same(FIELD(object,offsets[i],float),tags[j]==2?-0.0f:123);CHECK(bits(D_00474748[0].payload.scalar)==0xA5A5A5A5U);
 }
 for(i=1;i<3;++i){reset();lookup_result=NULL;D_00474F48=NULL;setters[i](2,3);CHECK(count(0x2CC938)==1);CHECK(count(0x239FD8)==0);}
 reset();FIELD(object,0x120,u32)=0xABCDEF01;func_001684D0(1,2);CHECK(FIELD(object,0x120,u32)==0);
 reset();lookup_result=NULL;func_001684D0(1,2);CHECK(count(0x2CC938)==0);
 reset();func_00168630(1,2);CHECK(FIELD(object,0x15C,u32)==1);reset();lookup_result=NULL;func_00168630(1,2);CHECK(count(0x2CC938)==0);
}
static void fraction_hook(Event *e)
{
 if(e->kind==0x135D88){D_00474F48=alternate_arguments;alternate_arguments[2].payload.scalar=-4;FIELD(path_results[0],0x30,float)=4;}
 if(e->kind==0x135E88)FIELD(object,0x120,u32)=77;
}
static void point_hook(Event *e)
{if(e->kind==0x161C30){D_00474F48=alternate_arguments;alternate_arguments[1].tag=0;}if(e->kind==0x135D10)FIELD(object,0x120,u32)=77;}
static void scalar_callback_tests(void)
{
 u32 i,j;float offsets[8]={-4,-2,0,2,6,8,16,0};float fractions[8]={0,0,0.25f,0.25f,0.25f,0.25f,0.25f,0.25f};offsets[7]=scalar(0x7FC12345);
 for(i=0;i<8;++i)for(j=0;j<2;++j){float expected;reset();fraction_result=fractions[i];arguments[2].payload.scalar=offsets[i];mode_result=j?(void *)1:NULL;FIELD(object,0x120,u32)=0;
  expected=fractions[i]+offsets[i]/8;if(!(expected>=0))expected=0;else expected=george_ee_minimum(expected,1);
  func_00167B98(3,4);same(nth(0x135E88,0)->values[0],expected);CHECK(nth(0x135E88,0)->argument[1]==(u32)VECTOR(object,j?0x114:0x108));CHECK(nth(0x135E88,0)->argument[2]==0);CHECK(FIELD(object,0x120,u32)==j);CHECK(count(0x161C30)==1);CHECK(nth(0x161C30,0)->argument[0]==0xFFFFFFFFU);
 }
 reset();mode_result=(void *)1;hook=fraction_hook;func_00167B98(3,-1);same(nth(0x135E88,0)->values[0],0);CHECK(FIELD(object,0x120,u32)==1);
 for(i=0;i<2;++i){reset();mode_result=i?(void *)1:NULL;FIELD(object,0x120,u32)=0;arguments[2].payload.scalar=2.5f;func_00167F98(3,-1);same(nth(0x135E88,0)->values[0],2.5f);CHECK(nth(0x135E88,0)->argument[1]==(u32)VECTOR(object,i?0x114:0x108));CHECK(nth(0x135E88,0)->argument[2]==0);CHECK(FIELD(object,0x120,u32)==i);CHECK(count(0x161C30)==0);}
 for(i=0;i<2;++i){reset();mode_result=i?(void *)1:NULL;FIELD(object,0x120,u32)=0;hook=point_hook;points[0].x=6;points[0].y=7;points[0].z=8;func_001683F0(2,-1);vector_same(object,i?0x114:0x108,6,7,8);CHECK(nth(0x135D10,0)->argument[1]==(u32)VECTOR(object,i?0x114:0x108));CHECK(FIELD(object,0x120,u32)==(i?1:77));}
}
static GeorgeMathVec3 *first_point_output;
static void cross_hook(Event *e)
{
 if(e->kind==0x239FD8&&object_lookup_count==1)arguments[2].payload.bits=0xCCCCCCCCU;
 if(e->kind==0x135D10){if(count(0x135D10)==1){first_point_output=(GeorgeMathVec3 *)e->argument[1];CHECK(e->argument[3]==1);}else{first_point_output->x=5;FIELD(object,0x120,u32)=77;}}
}
static void cross_tests(void)
{
 u32 i,j,k;float positions[7]={-5,0,3,5,10,15,0},offsets[5]={-10,-2,0,2,10};positions[6]=scalar(0x7FC12345);
 for(i=0;i<7;++i)for(j=0;j<5;++j)for(k=0;k<2;++k){float factor,expected;reset();arguments[2].tag=6;arguments[2].payload.bits=0x22222222U;reference.x=positions[i];arguments[3].payload.scalar=offsets[j];mode_result=k?(void *)1:NULL;FIELD(object,0x120,u32)=0;
  factor=positions[i]/10+offsets[j]/10;expected=factor<0?0:factor>1?10:factor*10;
  func_00167CC0(4,7);vector_same(object,k?0x114:0x108,expected,isnan(expected)?expected:0,isnan(expected)?expected:0);CHECK(count(0x135D10)==2);CHECK(count(0x161C30)==3);CHECK(nth(0x135D10,0)->argument[3]==k);CHECK(FIELD(object,0x120,u32)==k);CHECK(nth(0x239FD8,1)->argument[0]==0x22222222U);
 }
 reset();arguments[2].tag=6;arguments[2].payload.bits=0x22222222U;mode_result=(void *)1;FIELD(object,0x120,u32)=0;hook=cross_hook;func_00167CC0(3,-1);CHECK(nth(0x239FD8,1)->argument[0]==0x22222222U);vector_same(object,0x114,5,0,0);CHECK(FIELD(object,0x120,u32)==77);
 reset();arguments[2].tag=6;arguments[3].payload.scalar=10;func_00167CC0(5,-1);vector_same(object,0x108,3,0,0);
 reset();arguments[2].tag=6;arguments[3].tag=6;func_00167CC0(4,-1);vector_same(object,0x108,3,0,0);
 reset();arguments[2].tag=6;points[1].x=0;reference.x=0;func_00167CC0(3,-1);CHECK(isnan(FIELD(object,0x108,float)));CHECK(isnan(FIELD(object,0x10C,float)));CHECK(isnan(FIELD(object,0x110,float)));
}
static void lookup_failure_tests(void)
{
 void(*callbacks[6])(s32,s32)={func_00167B98,func_00167CC0,func_00167F98,func_001683F0,func_001685A8,func_00168660};u32 i,j;u8 before[0x170];
 for(i=0;i<6;++i)for(j=0;j<4;++j){reset();if(i==1)arguments[2].tag=6;memcpy(before,object,sizeof(before));
  if(j==0)arguments[1].tag=(u16)-32768;else if(j==1)object_results[0]=NULL;else if(j==2)path_results[0]=NULL;else if(i==1)arguments[2].tag=2;else arguments[1].tag=2;
  callbacks[i](3,3);CHECK(count(0x135D10)==0);CHECK(count(0x135D88)==0);CHECK(count(0x135E88)==0);CHECK(memcmp(before,object,sizeof(before))==0);CHECK(count(0x2CC938)==(j==0||j==3));
 }
 reset();func_00167CC0(2,3);CHECK(count(0x2CC938)==1);CHECK(count(0x239FD8)==0);
 reset();lookup_result=NULL;D_00474F48=NULL;func_001685A8(2,3);CHECK(count(0x2CC938)==1);CHECK(count(0x239FD8)==0);
 reset();func_001685A8(2,3);same(FIELD(path_results[0],0x44,float),-1);same(FIELD(path_results[0],0x48,float),-1);CHECK(count(0x2CC938)==0);
}
static void output_tests(void)
{
 s32 destinations[5]={-1,0,1,7,15};u32 i,j;u8 before[sizeof(D_00474748)];
 for(i=0;i<5;++i){s32 d=destinations[i];reset();fraction_result=-0.0f;memcpy(before,D_00474748,sizeof(before));func_00168660(2,d);CHECK(count(0x135D88)==1);CHECK(count(0x161C30)==1);
  if(d==-1)CHECK(memcmp(before,D_00474748,sizeof(before))==0);else{CHECK(D_00474748[d].tag==2);CHECK(D_00474748[d].subtype==0);same(D_00474748[d].payload.scalar,-0.0f);for(j=0;j<sizeof(before);++j)if(j<(u32)d*8||j>=(u32)d*8+8)CHECK(((u8 *)D_00474748)[j]==before[j]);}
 }
}
int main(void)
{
 CHECK(sizeof(void *)==4);CHECK(sizeof(GeorgeDeimosValue)==8);constructor_tests();registry_tests();updater_tests();update_tests();gate_tests();scalar_callback_tests();cross_tests();lookup_failure_tests();output_tests();printf("actor_camera_state: %u checks passed (jitter indeterminate-output path excluded)\n",checks);return 0;
}
