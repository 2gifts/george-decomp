/* Authored caller-observation contracts, with genuine separate-TU production
 * record/ownership code and published arena helpers. No heap/MMI emulation. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/render_records.h"
#include "george/arena_ownership.h"
#include "george/arena_buffers.h"
#include "arena_ownership_golden.h"

#define BUFFER 0x20000u
#define END 0x20400u
#define RECORD 0x20040u
#define SECOND 0x20100u
#define OWNER 0x3FD3E0u
#define CAPACITY 0x3FD3E4u
#define ARENA 0x46A0D8u

void *D_003FD3E0;
u32 D_003FD3E4;
GeorgeArenaBuffer D_0046A0D8;
static union {u32 alignment;unsigned char bytes[END-BUFFER];} storage_data;
#define storage storage_data.bytes
static u32 allocation,mutation,counts[2],events[20],event_count,checks,failures,case_index;

static void check(int condition,const char *label)
{
    ++checks;
    if (!condition && failures++<12) printf("ownership case %lu: %s\n",(unsigned long)case_index,label);
}

static u32 native_pointer(u32 value)
{
    if (value>=BUFFER && value<END) return (u32)storage+value-BUFFER;
    if (value==0x2B8E88u) return (u32)func_002B8E88;
    return value;
}

static u32 canonical(u32 value)
{
    if (value>=(u32)storage && value<(u32)storage+END-BUFFER) return BUFFER+value-(u32)storage;
    if (value==(u32)func_002B8E88) return 0x2B8E88u;
    return value;
}

static void event(u32 kind,u32 argument)
{
    u32 i;
    check(event_count+10<=20,"controlled event capacity");
    if (event_count+10>20) exit(2);
    ++counts[kind];events[event_count++]=kind;events[event_count++]=argument;
    events[event_count++]=(u32)D_003FD3E0;events[event_count++]=D_003FD3E4;
    for (i=0;i<6;i++) events[event_count++]=((u32 *)&D_0046A0D8)[i];
}

void *func_002AEC28(u32 size)
{
    u32 i;
    event(0,size);
    if (mutation==1) {D_003FD3E4=size+17;D_003FD3E0=(void *)native_pointer(SECOND);}
    else if (mutation==2) {
        *(u32 *)(storage+RECORD-BUFFER+0x2C)=0x89ABCDEFu;
        *(u32 *)(storage+RECORD-BUFFER+0x20)=native_pointer(SECOND);
        D_003FD3E4=0x80000001u;
    } else if (mutation==3) {
        for (i=0;i<6;i++) ((u32 *)&D_0046A0D8)[i]=0x11000000u+i;
        D_003FD3E4=0xFFFFFFFFu;
    } else if (mutation==4) {func_002B8BE0();D_003FD3E4=19;}
    else if (mutation==5) func_002B8BA8();
    return (void *)native_pointer(allocation);
}

void func_002AEE40(void *pointer)
{
    u32 value=(u32)pointer;
    event(1,value);
    if (mutation==1) {D_003FD3E0=(void *)native_pointer(SECOND);D_003FD3E4=99;}
    else if (mutation==2) {
        D_0046A0D8.cursor=(u8 *)native_pointer(SECOND+4);
        D_0046A0D8.high_water=(u8 *)native_pointer(SECOND+8);
    } else if (mutation==3 && value>=(u32)storage && value<=(u32)storage+END-BUFFER-4)
        *(u32 *)value=0x12345678u;
}

void *func_003936A0(void *destination,u32 value,u32 length)
{
    /* Linked published arena bodies require this symbol; none of the two
     * helpers executed in this scope may call it. */
    (void)value;(void)length;check(0,"unexpected published zero call");exit(2);
    return destination;
}

static float input_float(u32 bits)
{
    union {u32 bits;float value;} v;
    v.bits=bits;return v.value;
}

int main(void)
{
    u32 i,result,expected[256];
    for (case_index=0;case_index<sizeof(ownership_golden)/sizeof(ownership_golden[0]);case_index++) {
        const struct OwnershipGolden *g=&ownership_golden[case_index];
        memset(storage,0x5A,sizeof(storage));memset(expected,0x5A,sizeof(expected));
        for (i=0;i<g->change_count;i++) {
            if (g->changes[i][0]>=256) return 2;
            expected[g->changes[i][0]]=g->changes[i][1];
        }
        D_003FD3E0=(void *)native_pointer(g->global_initial[0]);D_003FD3E4=g->global_initial[1];
        for (i=0;i<6;i++) ((u32 *)&D_0046A0D8)[i]=i==1?g->arena_initial[i]:native_pointer(g->arena_initial[i]);
        allocation=g->allocation;mutation=g->mutation;counts[0]=counts[1]=event_count=0;memset(events,0,sizeof(events));result=0;
        switch (g->routine) {
        case 0:result=(u32)func_002B8A68(native_pointer(g->argument20),native_pointer(g->argument24),input_float(g->floats[0]),input_float(g->floats[1]),input_float(g->floats[2]));break;
        case 1:func_002B8AF0((GeorgeRenderRecord *)native_pointer(g->record),native_pointer(g->argument20),native_pointer(g->argument24),input_float(g->floats[0]),input_float(g->floats[1]),input_float(g->floats[2]));break;
        case 2:func_002B8B68(g->capacity);break;
        case 3:func_002B8BA8();break;
        case 4:func_002B8BE0();break;
        default:return 2;
        }
        for (i=0;i<256;i++) check(canonical(((u32 *)storage)[i])==expected[i],"complete authored buffer");
        check(canonical((u32)D_003FD3E0)==g->global_expected[0],"owner pointer");
        check(D_003FD3E4==g->global_expected[1],"capacity raw scalar");
        for (i=0;i<6;i++) check((i==1?((u32 *)&D_0046A0D8)[i]:canonical(((u32 *)&D_0046A0D8)[i]))==g->arena_expected[i],"complete arena descriptor");
        check(canonical(result)==g->result,"only actual constructor pointer result");
        check(counts[0]==g->counts[0] && counts[1]==g->counts[1],"controlled call counts");
        check(event_count==g->event_count,"controlled event extent");
        for (i=0;i<event_count;i++) {
            /* Raw size/capacity fields are not interpreted as addresses. */
            u32 v=events[i];
            if ((i%10==1 && events[i-1]==1) || (i%10!=0 && i%10!=1 && i%10!=3 && i%10!=5))
                v=canonical(v);
            check(v==g->events[i],"controlled argument/published-state event");
        }
    }
    printf("arena_ownership: %lu checks, %lu failures\n",(unsigned long)checks,(unsigned long)failures);
    return failures?1:0;
}
