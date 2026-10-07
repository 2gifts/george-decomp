/* Initialized authored contracts; genuine helper TUs execute separately.
 * Typed pointer cells alone are translated. No capacity or EE bulk-read claim. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/buffer_completion.h"
#include "george/actor_states2.h"
#include "george/heap.h"
#include "george/deimos_tables.h"
#include "buffer_completion_golden.h"
#define BASE 0x20000u
#define END (BASE+COMPLETION_WORDS*4u)
static unsigned char backing[COMPLETION_WORDS*4+65536];
static u32 *arena;
static u32 expected[COMPLETION_WORDS];
static const struct CompletionGolden *current;
static u32 checks,fixture_index,events[320],event_count,allocations,length_calls;
void *D_003F2C44;
GeorgeBufferUi *D_003F9408;
GeorgeActorPointerRange D_0046A0F0;
GeorgeHeap *D_003FD204;
struct _reent;
struct _reent *D_00405694;
GeorgeGenericMap *D_00481760;
extern const u8 D_004208A0[];
extern const char D_00447F48[],D_00447238[];
extern u32 completion_real_length(const void *);
u32 func_00295050(const void *p){++length_calls;return completion_real_length(p);}
extern void completion_real_append(GeorgeBufferManager *,const char *);
extern char *completion_generic_strcpy(char *,const char *);
extern s32 completion_generic_strncmp(const char *,const char *,u32);
extern int completion_real_sprintf(char *,const char *,...);
extern int completion_real_atexit(void (*)(void));
extern void completion_legacy_layout(void);
static void fail(const char *s,u32 a,u32 b){printf("completion fixture %u %s %08x != %08x\n",fixture_index,s,a,b);exit(1);}
static void equal(const char *s,u32 a,u32 b){++checks;if(a!=b)fail(s,a,b);}
static void *pointer(u32 a){
 if(!a)return NULL;
 if(a>=BASE&&a<END)return (unsigned char *)arena+a-BASE;
 if(a==0x3F9408)return &D_003F9408;
 if(a>=0x46A0F0&&a<=0x46A0FC)return (unsigned char *)&D_0046A0F0+a-0x46A0F0;
 if(a==0x4208A0)return (void *)D_004208A0;
 if(a==0x2BD340)return (void *)func_002162D0; /* callback identity token replaced below */
 fail("unowned guest pointer",a,BASE);return NULL;
}
void func_002BD340(void){fail("unexpected registered cleanup execution",1,0);}
static void *mapped(u32 a){if(a==0x2BD340)return (void *)func_002BD340;return pointer(a);}
u32 completion_address(const void *p){
 u32 v=(u32)p;
 if(!p)return 0;
 if(v>=(u32)arena&&v<(u32)arena+sizeof(u32)*COMPLETION_WORDS)return BASE+v-(u32)arena;
 if(p==&D_003F9408)return 0x3F9408;
 if(v>=(u32)&D_0046A0F0&&v<=(u32)&D_0046A0F0+12)return 0x46A0F0+v-(u32)&D_0046A0F0;
 if(p==D_004208A0)return 0x4208A0;
 if(p==(void *)func_002BD340)return 0x2BD340;
 fail("undesigned address-valued cell",v,0);return 0;
}
void completion_event(u32 kind,u32 a,u32 b,u32 c){
 if(event_count+4>320)fail("event bound",event_count,320);
 events[event_count++]=kind;events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;
}
static u32 *word_at(u32 a){return (u32 *)pointer(a);}
void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment){
 equal("core heap",completion_address(heap),0x20E00);equal("core align",alignment,4);
 completion_event(1,completion_address(heap),size,alignment);++allocations;
 if(size==0x41C)return pointer(0x21A00);
 if(size!=12)fail("core allocation domain",size,12);
 if(current->mutation&1)D_003F2C44=pointer(0x21B00);
 return pointer(0x21F00);
}
s32 func_00394F68(const char *format,...){(void)format;fail("unexpected diagnostic",1,0);return 0;}
/* Unknown nested initializer is controlled, while the outer object/field18
 * sequence reproduces the actual support contract without a helper award. */
void *func_002BF550(void *object){
 equal("constructor object",completion_address(object),0x21A00);
 completion_event(2,completion_address(object)+4,0,0);
 ((u32 *)object)[6]=0;return object;
}
signed char *func_002BF580(void *object,u32 key){
 u32 index=((u32 *)object)[6];
 signed char *slot=(signed char *)((u32)object+(index<<6)+0x1C);
 completion_real_sprintf((char *)slot,D_00447F48,key);
 ((u32 *)object)[6]=(((u32 *)object)[6]+1u)&15u;
 return slot;
}
void completion_formatter_event(void *out,u32 key){
 completion_event(4,completion_address(out),key,completion_address(D_003F2C44));
 if(current->mutation&2)D_003F9408=pointer(0x20044);
 if(current->mutation&4)((u32 *)D_003F2C44)[6]=15;
 if(current->mutation&8)*word_at(current->routine==2?0x22100:0x22300)=0;
}
s32 func_00100AA8(const void *a,const void *b){return *(const u32 *)a<*(const u32 *)b;}
void func_001007E0(GeorgeActorPointerRange *range,void **position,void *const *value){
 u32 end=completion_address(range->field04);
 equal("controlled range",range==&D_0046A0F0,1);
 completion_event(3,0x46A0F0,completion_address(position),completion_address(*value));
 if(end<0x20300||end>=0x20320)fail("insertion domain",end,0x20300);
 *range->field04=*value;range->field04++;
}
s32 func_00396260(void (*callback)(void)){return completion_real_atexit(callback);}
s32 func_00393E48(const signed char *a,const signed char *b,u32 n){return completion_generic_strncmp((const char *)a,(const char *)b,n);}
signed char *func_00393B74(signed char *a,const signed char *b){return (signed char *)completion_generic_strcpy((char *)a,(const char *)b);}
void func_002A4DB8(GeorgeBufferManager *m,const char *s){
 u32 source;
 extern const char D_0043A690[];
 source=s==D_0043A690?0x43A690:completion_address(s);
 completion_event(5,completion_address(m),source,completion_address(D_003F9408));
 completion_real_append(m,s);
}
static int typed(u32 a){unsigned j;for(j=0;j<current->pointer_count;++j)if(current->pointer_cells[j]==a)return 1;return 0;}
static int address_value(u32 v){return v==0||(v>=BASE&&v<END)||v==0x3F9408||(v>=0x46A0F0&&v<0x46A0FC)||v==0x4208A0||v==0x2BD340;}
static void setup(void){unsigned i;u32 g;
 for(i=0;i<COMPLETION_WORDS;++i)arena[i]=expected[i]=0xA5870301u^(i*0x10103u);
 for(i=0;i<current->initial_count;++i){u32 n=current->initial[i*2];arena[n]=expected[n]=current->initial[i*2+1];}
 for(i=0;i<current->change_count;++i)expected[current->changes[i*2]]=current->changes[i*2+1];
 for(i=0;i<current->pointer_count;++i){g=current->pointer_cells[i];if(address_value(*word_at(g)))*word_at(g)=(u32)mapped(*word_at(g));}
 D_003F2C44=mapped(current->globals_initial[0]);D_003F9408=mapped(current->globals_initial[1]);
 D_0046A0F0.field00=mapped(current->globals_initial[2]);D_0046A0F0.field04=mapped(current->globals_initial[3]);D_0046A0F0.field08=mapped(current->globals_initial[4]);
 D_003FD204=mapped(current->globals_initial[5]);D_00405694=mapped(current->globals_initial[6]);D_00481760=mapped(current->globals_initial[7]);
 event_count=allocations=length_calls=0;
}
int main(void){unsigned i,j;u32 v,globals[8];
 arena=(u32 *)(((u32)backing+65535u)&~65535u);completion_legacy_layout();
 for(fixture_index=0;fixture_index<sizeof(completion_golden)/sizeof(completion_golden[0]);++fixture_index){
  current=&completion_golden[fixture_index];setup();
  if(current->routine==0)func_002162D0(current->args[0],NULL,NULL);
  else if(current->routine==1)func_00217598(pointer(current->args[0]),pointer(current->args[1]));
  else if(current->routine==2)func_002CDA20(pointer(current->args[0]),func_002162D0,NULL);
  else func_002CDDB0(func_002162D0,NULL);
  for(i=0;i<COMPLETION_WORDS;++i){v=arena[i];if(typed(BASE+4*i)&&address_value(expected[i]))v=completion_address((void *)v);equal("arena",v,expected[i]);}
  globals[0]=completion_address(D_003F2C44);globals[1]=completion_address(D_003F9408);
  globals[2]=completion_address(D_0046A0F0.field00);globals[3]=completion_address(D_0046A0F0.field04);globals[4]=completion_address(D_0046A0F0.field08);
  globals[5]=completion_address(D_003FD204);globals[6]=completion_address(D_00405694);globals[7]=completion_address(D_00481760);
  for(j=0;j<8;++j)equal("globals",globals[j],current->globals_final[j]);
  equal("repeated actual strlen calls",length_calls,current->length_calls);
  equal("event count",event_count,current->event_count);for(j=0;j<event_count;++j)equal("events",events[j],current->events[j]);
 }
 printf("buffer completion: %u checks passed\n",checks);return 0;
}
