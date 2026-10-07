#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/buffer_manager.h"
#include "george/accessors.h"
#include "george/string_algorithms.h"
#include "buffer_manager_golden.h"

#define BUFFER 0x20000U
#define END 0x22000U
#define MANAGER 0x20040U
#define COMMANDS 0x20500U
#define ALT_COMMANDS 0x20520U
#define COMMAND_RECORDS 0x20600U
#define NEW_TEXT 0x21101U
static u32 arena[MANAGER_WORDS] __attribute__((aligned(256)));
static u32 expected[MANAGER_WORDS],events[160],invocations[8];
static unsigned checks,current,event_count,callback_count,mutation;

static void check(int v,const char *why,unsigned index)
{
    ++checks;
    if (!v) {fprintf(stderr,"buffer manager fixture %u: %s %u\n",current,why,index);exit(1);}
}
static void *physical(u32 value)
{
    check(value>=BUFFER && value<END,"bounded authored pointer",value);
    return (u8 *)arena+value-BUFFER;
}
static u32 canonical(u32 value)
{
    if (value>=(u32)arena && value-(u32)arena<sizeof(arena))
        return BUFFER+value-(u32)arena;
    return value;
}
static void event(u32 kind,u32 a,u32 b,u32 c)
{
    check(event_count+4<=160,"bounded events",event_count);
    events[event_count++]=kind;events[event_count++]=canonical(a);
    events[event_count++]=canonical(b);events[event_count++]=canonical(c);
}
static void command_effect(GeorgeBufferManager *manager,char *text,u32 data,u32 kind)
{
    ++callback_count;
    check(canonical((u32)manager)==MANAGER,"three-GPR callback manager",0);
    check(canonical((u32)text)>=BUFFER && canonical((u32)text)<END,"three-GPR callback text",0);
    event(kind,(u32)manager,(u32)text,data);
    if(callback_count==1) {
        if(mutation==1)manager->commands=physical(ALT_COMMANDS);
        else if(mutation==2)*(u32 *)physical(COMMANDS+8)=1;
        else if(mutation==3)((GeorgeBufferLine *)physical(0x20260U))->data=physical(NEW_TEXT);
        else if(mutation==4)manager->current=physical(0x20300U);
        else if(mutation==5) {manager->commands=physical(ALT_COMMANDS);*(u32 *)physical(ALT_COMMANDS+8)=3;}
        else if(mutation==6) {
            GeorgeBufferCommandPrefix *command=physical(COMMAND_RECORDS+0x40);
            extern void second_command(GeorgeBufferManager *,char *,u32);
            command->data=0xABCDEF01U;command->callback=second_command;
        } else if(mutation==7) {manager->history_count=-1;manager->navigation=(s32)0x80000000U;}
    }
}
static void first_command(GeorgeBufferManager *m,char *t,u32 d) {command_effect(m,t,d,8);}
void second_command(GeorgeBufferManager *m,char *t,u32 d) {command_effect(m,t,d,9);}

/* Genuine published functions/unchanged licensed sources are separate TUs.
 * These transparent bridges expose only their actual argument lanes/events. */
extern u32 manager_real_length(const signed char *);
extern u32 manager_real_count(const void *);
extern char *manager_real_strstr(const char *,const char *);
extern char *manager_real_strncpy(char *,const char *,u32);
u32 func_00295050(const signed char *text)
{++invocations[3];event(3,(u32)text,0,0);return manager_real_length(text);}
u32 func_002AAF88(const void *array)
{++invocations[5];event(5,(u32)array,0,0);return manager_real_count(array);}
char *func_00398628(const char *text,const char *pattern)
{++invocations[6];event(6,(u32)text,(u32)pattern,0);return manager_real_strstr(text,pattern);}
char *func_00394010(char *destination,const char *source,u32 count)
{
    u32 d=canonical((u32)destination),s=canonical((u32)source);
    check(count<=64 && (count<8 || ((d|s)&7)!=0),"original byte-copy path",0);
    check(!count || d+count<=s || s+count<=d,"supporting strncpy nonoverlap",0);
    ++invocations[7];event(7,(u32)destination,(u32)source,count);
    return manager_real_strncpy(destination,source,count);
}
/* Explicit bounded low-word observer of fourteen instructions at2AAF50.
 * This is not recovered helper source and receives no production award. */
u32 *func_002AAF50(void *array,s32 index)
{
    const u32 *words=array;
    ++invocations[4];event(4,(u32)array,(u32)index,0);
    if(index<0 || !(index<(s32)words[2]))return NULL;
    return (u32 *)(words[3]+(u32)index*words[0]);
}

static u32 initial_word(u32 index) {return 0xA5000001U^(index*0x103U);}
static int pointer_cell(const struct ManagerGolden *g,u32 index)
{
    u32 i;
    for(i=0;i<g->pointer_count;++i)if(g->pointers[i]==index)return 1;
    return 0;
}
static int function_cell(const struct ManagerGolden *g,u32 index)
{
    u32 i;
    for(i=0;i<g->function_count;++i)if(g->functions[i]==index)return 1;
    return 0;
}
int main(void)
{
    unsigned n,i;
    check(sizeof(void *)==4,"native32 observer",0);
    check(((u32)arena&255U)==0,"low-byte pointer alias storage alignment",0);
    check((u32)arena<0x80000000U && sizeof(arena)<0x80000000U-(u32)arena,"host address extent",0);
    for(n=0;n<sizeof(manager_golden)/sizeof(manager_golden[0]);++n) {
        const struct ManagerGolden *g=manager_golden+n;u32 result=0;
        current=n;mutation=g->mutation;callback_count=event_count=0;
        memset(invocations,0,sizeof(invocations));
        for(i=0;i<MANAGER_WORDS;++i)arena[i]=expected[i]=initial_word(i);
        for(i=0;i<g->initial_count;++i) {
            check(g->initial[i].index<MANAGER_WORDS,"initial word",i);
            arena[g->initial[i].index]=expected[g->initial[i].index]=g->initial[i].value;
        }
        for(i=0;i<g->change_count;++i) {
            check(g->changes[i].index<MANAGER_WORDS,"changed word",i);
            expected[g->changes[i].index]=g->changes[i].value;
        }
        for(i=0;i<g->pointer_count;++i) {
            u32 index=g->pointers[i],value=arena[index];check(index<MANAGER_WORDS,"pointer cell",i);
            if(value>=BUFFER && value<END)arena[index]=(u32)physical(value);
        }
        for(i=0;i<g->function_count;++i) {
            u32 index=g->functions[i];check(index<MANAGER_WORDS,"callback cell",i);
            check(arena[index]==0xF0000010U || arena[index]==0xF0000020U,"controlled callback encoding",i);
            arena[index]=(u32)(arena[index]==0xF0000010U?first_command:second_command);
        }
        invocations[g->routine]=1;
        if(g->routine==0)func_002A4B70(physical(MANAGER),(signed char)g->character);
        else if(g->routine==1)func_002A4DB8(physical(MANAGER),physical(0x21001U));
        else {check(g->routine==2,"selected routine",0);result=func_002A56C0(physical(MANAGER));}
        check(result==g->result,"observed return",0);
        for(i=0;i<MANAGER_WORDS;++i) {
            u32 value=arena[i];
            if(function_cell(g,i)) {
                if(value==(u32)first_command)value=0xF0000010U;
                else if(value==(u32)second_command)value=0xF0000020U;
            } else if(pointer_cell(g,i))value=canonical(value);
            check(value==expected[i],"full authored memory",i);
        }
        check(event_count==g->event_count,"actual helper/callback event count",0);
        for(i=0;i<event_count;++i)check(events[i]==g->events[i],"actual helper/callback argument",i);
        for(i=0;i<8;++i)check(invocations[i]==g->invocations[i],"whole helper entry invocation",i);
    }
    printf("buffer manager: %u checks passed\n",checks);return 0;
}
