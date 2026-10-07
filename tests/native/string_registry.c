/* Full selected C with published heap/length and authentic licensed strings.
 * Core allocation/free/diagnostic effects are explicit observation controls. */
#include "george/string_registry.h"
#include "george/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string_registry_golden.h"
#define BASE 0x20000u
#define TABLE 0x20100u
#define ALT_TABLE 0x20180u
#define NEW_TABLE 0x20200u
#define KEY 0x20401u
#define PATH 0x20481u
#define NEW_KEY 0x20600u
#define NEW_PATH 0x20700u
#define OUTPUT 0x20800u
#define HEAP 0x20A00u
static u8 arena[STRING_REGISTRY_WORDS*4u];
static u32 expected[STRING_REGISTRY_WORDS],events[80][4],event_count,checks,allocations,small_allocations;
static const struct RegistryGolden *current;
u32 D_003FD1D8,D_003FD1DC;
GeorgeStringRegistryPair *D_003FD1E0;
char D_00469A00[128],D_00469A80[128],D_00469B00[128];
GeorgeHeap *D_003FD204;
const char D_00447238[]="authored diagnostic observer";
extern void *registry_real_memcpy(void *,const void *,size_t);
extern char *registry_real_strcpy(char *,const char *);
extern char *registry_real_strcat(char *,const char *);
extern int registry_real_strcmp(const char *,const char *);
extern char *registry_real_strlwr(char *);
extern char *registry_real_strstr(const char *,const char *);

static void equal(u32 actual,u32 want,const char *name)
{
    ++checks;
    if(actual!=want){printf("registry fixture %u check %u %s: %08X != %08X\n",
      (u32)(current-registry_golden),checks,name,actual,want);exit(1);}
}
static void *host(u32 p)
{
    if(!p)return 0;
    if(p<BASE || p-BASE>=sizeof(arena)){printf("unowned registry native pointer\n");exit(1);}
    return arena+p-BASE;
}
static u32 guest(const void *p){return p?BASE+((u32)p-(u32)arena):0;}
static u32 value(u32 v){return v>=BASE && v-BASE<sizeof(arena)?(u32)host(v):v;}
static void event(u32 kind,u32 a,u32 b,u32 c)
{
    if(event_count>=80){printf("registry native event bound\n");exit(1);}
    events[event_count][0]=kind;events[event_count][1]=a;events[event_count][2]=b;events[event_count++][3]=c;
}
void *func_002ADF60(GeorgeHeap *heap,u32 bytes,u32 alignment)
{
    u32 result;
    equal(guest(heap),HEAP,"allocation heap");equal((u32)alignment,3,"allocation alignment");
    ++allocations;event(1,HEAP,bytes,(u32)alignment);
    if(bytes>=80)result=NEW_TABLE;
    else{++small_allocations;result=small_allocations==1?NEW_KEY:NEW_PATH;}
    if((current->mutation&1) && bytes>=80){D_003FD1D8=2;D_003FD1DC=7;D_003FD1E0=host(ALT_TABLE);}
    return current->failure&(1u<<(allocations-1))?0:host(result);
}
void func_002AE158(void *memory)
{
    event(2,guest(memory),D_003FD1D8,D_003FD1DC);
    if(current->mutation&2)D_003FD1DC=3;
}
s32 func_00394F68(const char *format,...)
{
    va_list args;u32 cursor,bytes,alignment;
    equal(format==D_00447238,1,"authored diagnostic identity");
    va_start(args,format);cursor=va_arg(args,u32);bytes=va_arg(args,u32);alignment=va_arg(args,u32);va_end(args);
    event(3,cursor?guest((void *)cursor):0,bytes,alignment);return 0;
}
void *func_003934F8(void *d,const void *s,u32 n){return registry_real_memcpy(d,s,n);}
char *func_00393B74(char *d,const char *s){return registry_real_strcpy(d,s);}
char *func_00393758(char *d,const char *s){return registry_real_strcat(d,s);}
s32 func_00393A28(const char *a,const char *b){return registry_real_strcmp(a,b);}
char *func_003984D8(char *s){return registry_real_strlwr(s);}
char *func_00398628(const char *s,const char *n){return registry_real_strstr(s,n);}

static int pointer_cell(u32 i)
{
    u32 a=BASE+4u*i;
    static const u32 tables[]={TABLE,ALT_TABLE,NEW_TABLE,NEW_KEY,NEW_PATH};
    u32 t;
    if(a==HEAP+0x18)return 1;
    for(t=0;t<5;++t)if(a>=tables[t] && a<tables[t]+24u)return 1;
    return 0;
}
int main(void)
{
    u32 case_index,i,j,result;char *texts[]={D_00469A00,D_00469A80,D_00469B00};
    for(case_index=0;case_index<sizeof(registry_golden)/sizeof(registry_golden[0]);++case_index){
        current=&registry_golden[case_index];allocations=small_allocations=event_count=0;
        for(i=0;i<STRING_REGISTRY_WORDS;++i)expected[i]=0x8D4A0301u^(i*0x10203u);
        for(i=0;i<current->initial_count;++i)expected[current->initial[i].index]=current->initial[i].value;
        for(i=0;i<STRING_REGISTRY_WORDS;++i)((u32 *)arena)[i]=pointer_cell(i)?value(expected[i]):expected[i];
        for(i=0;i<3;++i)for(j=0;j<128;++j)texts[i][j]=(char)current->texts_initial[i][j];
        D_003FD1D8=current->globals_initial[0];D_003FD1DC=current->globals_initial[1];
        D_003FD1E0=host(current->globals_initial[2]);D_003FD204=host(current->globals_initial[3]);
        result=0;
        switch(current->routine){
        case 0:result=(u32)func_002AB790(host(KEY),host(PATH));break;
        case 1:func_002ABAE8(host(KEY),host(current->output_alias?KEY:OUTPUT));break;
        case 2:func_002ABDE8(current->setter_alias==2?0:current->setter_alias?D_00469A00:host(KEY));break;
        case 3:func_002ABEF8();break;
        default:printf("unknown registry routine\n");return 1;
        }
        equal(result,current->result,"selected result");
        equal(D_003FD1D8,current->globals_expected[0],"fresh global count");
        equal(D_003FD1DC,current->globals_expected[1],"captured/fresh capacity");
        equal(guest(D_003FD1E0),current->globals_expected[2],"published table");
        equal(guest(D_003FD204),current->globals_expected[3],"heap global");
        for(i=0;i<current->change_count;++i)expected[current->changes[i].index]=current->changes[i].value;
        for(i=0;i<STRING_REGISTRY_WORDS;++i)equal(((u32 *)arena)[i],pointer_cell(i)?value(expected[i]):expected[i],"whole arena");
        for(i=0;i<3;++i)for(j=0;j<128;++j)equal((u8)texts[i][j],current->texts_expected[i][j],"whole authored global text window");
        equal(event_count,current->event_count,"actual controlled call count");
        for(i=0;i<event_count;++i)for(j=0;j<4;++j)equal(events[i][j],current->events[i][j],"actual controlled arguments/order");
    }
    printf("%u string registry checks passed\n",checks);return 0;
}
