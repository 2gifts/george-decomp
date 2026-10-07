/* New clone-PC fixture harness. Genuine full SGI methods, static storage
 * and real placement-constructed objects are inherited cached native support.
 * Node12/pair/value8 are this counterpart's types, not original clone identity.
 * Original consumes next0/raw-u32 keybits4 only; pointer-like keys are opaque.
 * Header-prefix GNU alias and historical pointer arithmetic domains apply.
 */
/* Finite initialized GNU/native alias-view contract over real C++ objects.
 * No universal C/C++ lifetime, corrupted-object alias, capacity or EE claim. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "george/completion_resolver.h"
#include "george/heap.h"
#include "sgi_container_duplicates_golden.h"
#define BASE 0x20000u
#define END (BASE+RESOLVER_WORDS*4u)
#define NEW 0x26000u
static unsigned char backing[RESOLVER_WORDS*4u+65536u];
static u32 *arena, expected[RESOLVER_WORDS], initial[RESOLVER_WORDS];
static const struct ResolverGolden *current;
static u32 fixture_index, checks, allocations, events[320], event_count;
static int booting;
void *D_003F2C44;
u32 D_003F2C9C;
GeorgeActorPointerRange D_0046A0F0;
GeorgeHeap *D_003FD204;
void *D_00405694;
extern const u8 D_004208A0[], D_004200F8[];
extern const char D_00447F48[], D_00447238[];
extern void * _ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE[16];
extern char * _ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE;
extern char * _ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE;
extern u32 _ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE;
extern void (*_ZN23__malloc_alloc_templateILi0EE26__malloc_alloc_oom_handlerE)(void);
extern void resolver_construct(void *,void *,void *);
extern void resolver_destroy_empty(void *,void *,void *);
extern void resolver_node(void *,u32,void *,void *);
extern void resolver_layout(u32 *);
extern void *resolver_generic_memmove(void *,const void *,u32);
static void fail(const char *s,u32 a,u32 b){printf("resolver fixture %u %s %08x != %08x\n",fixture_index,s,a,b);exit(1);}
static void equal(const char *s,u32 a,u32 b){++checks;if(a!=b)fail(s,a,b);}
void resolver_unexecuted(void){fail("unexecuted supporting interface",1,0);}
static void oom(void);
static void *pointer(u32 a){
 if(!a)return NULL;
 if(a>=BASE&&a<=END)return (unsigned char *)arena+a-BASE;
 if(a==0x4208A0)return (void *)D_004208A0;
 if(a==0x4200F8)return (void *)D_004200F8;
 if(a==0xF0000010)return (void *)oom;
 fail("undesigned guest pointer",a,BASE);return NULL;
}
u32 resolver_address(const void *p){
 u32 a=(u32)p;if(!p)return 0;
 if(a>=(u32)arena&&a<=(u32)arena+RESOLVER_WORDS*4u)return BASE+a-(u32)arena;
 if(p==D_004208A0)return 0x4208A0;
 if(p==D_004200F8)return 0x4200F8;
 if(p==(void *)oom)return 0xF0000010;
 fail("undesigned native address cell",a,0);return 0;
}
static u32 *at(u32 a){return (u32 *)pointer(a);}
static u32 *global(u32 a){
 if(a==0x3F2C44)return (u32 *)&D_003F2C44;
 if(a==0x3F2C9C)return &D_003F2C9C;
 if(a>=0x46A0F0&&a<0x46A0FC)return (u32 *)((u8 *)&D_0046A0F0+a-0x46A0F0);
 if(a>=0x3F21B8&&a<0x3F21F8)return (u32 *)((u8 *)_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE+a-0x3F21B8);
 if(a==0x3F21F8)return (u32 *)&_ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE;
 if(a==0x3F21FC)return (u32 *)&_ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE;
 if(a==0x3F2200)return &_ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE;
 if(a==0x3F21B0)return (u32 *)&_ZN23__malloc_alloc_templateILi0EE26__malloc_alloc_oom_handlerE;
 if(a==0x3FD204)return (u32 *)&D_003FD204;
 if(a==0x405694)return (u32 *)&D_00405694;
 fail("unowned global",a,0);return NULL;
}
static u32 *cell(u32 a){return a>=BASE&&a<END?at(a):global(a);}
static int typed(u32 a){u32 i;for(i=0;i<current->pointer_count;++i)if(resolver_pointer[current->pointer_offset+i]==a)return 1;return 0;}
static int initial_typed(u32 a){u32 i;for(i=0;i<sizeof(resolver_initial_pointer)/sizeof(*resolver_initial_pointer);++i)if(resolver_initial_pointer[i]==a)return 1;return 0;}
static void store_pointer(u32 a,u32 v){*cell(a)=(u32)pointer(v);}
static void event(u32 k,u32 a,u32 b,u32 c){if(event_count+4>320)fail("event bound",event_count,320);events[event_count++]=k;events[event_count++]=a;events[event_count++]=b;events[event_count++]=c;}
static void oom(void){event(4,0,0,0);}
void *func_002ADF60(GeorgeHeap *heap,u32 size,u32 alignment){
 u32 value;equal("core heap",resolver_address(heap),0x22C00);equal("core alignment",alignment,3);
 if(!size||size>0x1400)fail("finite allocation domain",size,0x1400);
 ++allocations;if(allocations>3)fail("allocation attempts",allocations,3);
 value=current->parameters[12]&&allocations==1?0:NEW+(allocations-1)*0x2000;
 event(1,0x22C00,size,value);
 if((current->parameters[10]&1)&&(current->parameters[0]==0||current->parameters[0]==2)){
  u32 table=current->parameters[0]==0?0x20104:0x20540;
  store_pointer(table+4,0x20B00);store_pointer(table+8,0x20B08);store_pointer(table+12,0x20B10);
 }
 return pointer(value);
}
void func_002AE158(void *p){
 u32 a=resolver_address(p);if(a<BASE||a>=END)fail("release domain",a,BASE);event(2,a,0,0);
 if((current->parameters[10]&2)&&(a==0x21200||a==0x21220||a==0x21240))store_pointer(0x21100,0);
 if(current->parameters[10]&4)D_003F2C44=NULL;
}
void *george_sgi_engine_allocate(u32 size){if(booting)return malloc(size);return func_002AF140(size);}
void george_sgi_engine_release(void *p){if(booting){free(p);return;}func_002AF1E8(p);}
s32 func_00394F68(const char *format,...){
 /* Original allocation diagnostic after failed allocation, with fresh heap. */
 va_list ap;u32 size,align;void *heap;
 equal("diagnostic format",format==D_00447238,1);va_start(ap,format);heap=va_arg(ap,void *);size=va_arg(ap,u32);align=va_arg(ap,u32);va_end(ap);equal("diagnostic exhausted heap",heap==NULL,1);equal("diagnostic align",align,3);event(3,0x447238,size,3);
 return 0;
}
void *func_003935A4(void *d,const void *s,u32 n){return resolver_generic_memmove(d,s,n);}
void resolver_formatter_event(void *out,u32 key){event(5,resolver_address(out),key,0);if(current->parameters[10]&8)*at(0x20118)=15;}
static u32 initial_word(u32 i){return 0xA5870301u^(i*0x10103u);}
static void setup(void){
 u32 i,size=current->parameters[1],capacity=current->parameters[2],nodes=current->parameters[8];
 u32 table=0x20540,bucket_count=current->parameters[15],path=current->parameters[16],key=current->parameters[7],slot=key%bucket_count;
 for(i=0;i<RESOLVER_WORDS;++i)arena[i]=initial_word(i);
 memset(_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE,0,64);
 _ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE=NULL;
 _ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE=NULL;
 _ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE=0;
 booting=1;resolver_construct(pointer(0x20104),pointer(0x20540),pointer(0x20600));booting=0;
 /* Initialized source bytes restored after real object lifetime starts.
  * This explicit GNU/native prefix view is not a universal object model. */
 *at(0x20104)=initial_word((0x20104-BASE)/4);*at(0x20540)=initial_word((0x20540-BASE)/4);
 D_003F2C44=NULL;D_003F2C9C=0;memset(&D_0046A0F0,0,12);
 memset(_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE,0,64);
 _ZN24__default_alloc_templateILb0ELi0EE13_S_start_freeE=NULL;
 _ZN24__default_alloc_templateILb0ELi0EE11_S_end_freeE=NULL;
 _ZN24__default_alloc_templateILb0ELi0EE12_S_heap_sizeE=0;
 _ZN23__malloc_alloc_templateILi0EE26__malloc_alloc_oom_handlerE=oom;
 D_003FD204=(GeorgeHeap *)pointer(0x22C00);D_00405694=NULL;
 *at(0x22C18)=0;*at(0x22C24)=0x10000;*at(0x22C28)=0;
 for(i=0;i<2;++i){u32 t=i?0x20104:0x20540;store_pointer(t+4,0x20700);store_pointer(t+8,0x20700+bucket_count*4);store_pointer(t+12,0x20700+bucket_count*4);*at(t+16)=nodes;}
 for(i=0;i<193;++i)store_pointer(0x20700+i*4,0);
 for(i=0;i<3;++i){u32 n=0x21100+i*32;resolver_node(pointer(n),i==0?key:i,pointer(i!=1?0x21200+i*32:0),pointer(i==0&&path==0?0x21120:0));}
 store_pointer(0x20700+slot*4,0x21100);
 if(path==1&&slot+1<bucket_count)store_pointer(0x20700+(bucket_count-1)*4,0x21140);
 for(i=0;i<80;++i)store_pointer(0x20B00+i*4,0x21200+(i%3)*32);
 store_pointer(0x20600,0x20B00);store_pointer(0x20604,0x20B00+size*4);store_pointer(0x20608,0x20B00+capacity*4);
 store_pointer(0x21600,0x21240);*at(0x20118)=current->parameters[6];
 store_pointer(0x21400,0x21100);store_pointer(0x21404,table);
 store_pointer(0x21304,0x4208A0);store_pointer(0x21500,current->parameters[13]?0:0x21300);
 store_pointer(0x46A0F0,0x21500);store_pointer(0x46A0F4,0x21504);store_pointer(0x46A0F8,0x21510);
 store_pointer(0x3F2C44,nodes?0x20100:0);
 store_pointer(NEW,0);
 if(current->parameters[11]==1||current->parameters[11]==2){store_pointer(0x3F21F8,NEW);store_pointer(0x3F21FC,NEW+(current->parameters[11]==1?320:24));}
 else if(current->parameters[11]==3){u32 n=(size+(size>current->parameters[4]?size:current->parameters[4]))*4;store_pointer(0x3F21B8+(((n+7)/8)-1)*4,NEW);}
 for(i=0;i<RESOLVER_WORDS;++i){initial[i]=initial_typed(BASE+i*4)?resolver_address((void *)arena[i]):arena[i];expected[i]=initial[i];}
 for(i=0;i<current->delta_count;i+=2){u32 off=resolver_delta[current->delta_offset+i];expected[off/4]=resolver_delta[current->delta_offset+i+1];}
 allocations=event_count=0;
}
static void run(void){
 u32 routine=current->parameters[0],result=0,i;void *p;
 switch(routine){
 case 3:func_002BF0F0(pointer(0x20600),pointer(0x20B00+current->parameters[3]*4),current->parameters[4],pointer(current->parameters[5]?0x20B00:0x21600));break;
 case 6:result=resolver_address(func_002BF4B8(pointer(0x21400)));break;
 default:fail("counterpart routine",routine,6);
 }
 equal("returned identity",result,current->result);
 for(i=0;i<RESOLVER_WORDS;++i){u32 value=typed(BASE+i*4)?resolver_address((void *)arena[i]):arena[i];if(value!=expected[i])fail("arena output",BASE+i*4,value);++checks;}
 for(i=0;i<current->global_count;i+=2){u32 a=resolver_global[current->global_offset+i],v=*global(a);if(typed(a))v=resolver_address((void *)v);equal("global output",v,resolver_global[current->global_offset+i+1]);}
 equal("event count",event_count,current->event_count);for(i=0;i<event_count;++i)equal("event lane",events[i],resolver_event[current->event_offset+i]);
 /* Real object destructors run only after detaching all observed storage.
  * Their housekeeping is outside original/native comparison and no award. */
 p=pointer(0x20104);memset((u8 *)p+4,0,16);p=pointer(0x20540);memset((u8 *)p+4,0,16);memset(pointer(0x20600),0,12);
 booting=1;resolver_destroy_empty(pointer(0x20104),pointer(0x20540),pointer(0x20600));booting=0;
}
int main(void){u32 words[12],i;arena=(u32 *)(((u32)backing+65535u)&~65535u);resolver_layout(words);for(i=0;i<12;++i)equal("native layout",words[i],(u32[]){4,12,20,8,12,4,8,0,4,4,8,64}[i]);for(fixture_index=0;fixture_index<sizeof(resolver_fixtures)/sizeof(*resolver_fixtures);++fixture_index){current=&resolver_fixtures[fixture_index];setup();run();}printf("SGI container duplicates: %u checks; %u new initialized clone-PC fixtures\n",checks,fixture_index);return 0;}
