/* Synthetic complete-original fixtures. Production C is a separate TU.
 * Numeric pointers are compared without dereference in wrap cases. The only
 * controlled engine call zeros bounded authored storage and mutates exposed
 * state afterward; no allocator/MMI/hardware behavior is claimed. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/arena_buffers.h"
#include "arena_buffers_golden.h"

#define BUFFER 0x20000u
#define END 0x20800u
#define LOCAL 0x20040u
#define BASE 0x20440u
#define GLOBAL 0x46A0D8u

GeorgeArenaBuffer D_0046A0D8;
static unsigned char *storage;
static u32 outputs[3],mutation,call_count,event_count,events[30],checks,failures,case_index;

static void check(int condition,const char *label)
{
    ++checks;
    if (!condition && failures++<12) printf("arena case %lu: %s\n",(unsigned long)case_index,label);
}

static u32 native_pointer(u32 value)
{
    if (value>=BUFFER && value<=END) return (u32)storage+value-BUFFER;
    if (value>=GLOBAL && value<GLOBAL+24) return (u32)&D_0046A0D8+value-GLOBAL;
    if (value==0x2B8E88u) return (u32)func_002B8E88;
    return value;
}

static u32 canonical(u32 value)
{
    if (value>=(u32)storage && value<=(u32)storage+END-BUFFER) return BUFFER+value-(u32)storage;
    if (value>=(u32)&D_0046A0D8 && value<(u32)&D_0046A0D8+24) return GLOBAL+value-(u32)&D_0046A0D8;
    if (value==(u32)func_002B8E88) return 0x2B8E88u;
    return value;
}

static u32 read_word(u32 address)
{
    return *(u32 *)native_pointer(address);
}

static void write_word(u32 address,u32 value)
{
    *(u32 *)native_pointer(address)=native_pointer(value);
}

void *func_003936A0(void *destination,u32 value,u32 length)
{
    u32 p=(u32)destination,i;
    check(value==0 && length<=36 && p>=(u32)storage && p<=(u32)storage+(END-BUFFER)-length,"controlled zero ABI/bounds");
    if (value || length>36 || p<(u32)storage || p>(u32)storage+(END-BUFFER)-length) exit(2);
    check(event_count+10<=30,"event capacity");
    events[event_count++]=p;events[event_count++]=value;events[event_count++]=length;
    for (i=0;i<4;i++) events[event_count++]=((u32 *)&D_0046A0D8)[i];
    for (i=0;i<3;i++) events[event_count++]=read_word(outputs[i]);
    ++call_count;
    memset(destination,0,length);
    if (mutation==1) {
        write_word(GLOBAL,BASE+128);write_word(GLOBAL+8,BASE+132);
        write_word(GLOBAL+12,BASE+136);write_word(outputs[0],BASE+140);
    } else if (mutation==2) {
        write_word(outputs[1],BASE+144);write_word(outputs[2],BASE+148);
    }
    return destination;
}

static void initialize_descriptor(GeorgeArenaBuffer *arena,const u32 *words,u32 raw)
{
    u32 *p=(u32 *)arena,i;
    for (i=0;i<6;i++) p[i]=words[i];
    for (i=0;i<6;i++) {
        if (i==1 || (raw && (i==0 || i==2 || i==3))) continue;
        p[i]=native_pointer(p[i]);
    }
}

int main(void)
{
    unsigned char *allocation=(unsigned char *)malloc(0x10800u);
    u32 i,result,descriptor;
    if (!allocation) return 2;
    storage=(unsigned char *)(((u32)allocation+0xFFFFu)&~0xFFFFu);
    /* Preserve the authored low16 alignment bits; all native storage remains
     * below bit31 so large-shift numeric alignment has the same boundary. */
    if ((u32)storage>=0x80000000u-(END-BUFFER)) {free(allocation);return 2;}
    for (case_index=0;case_index<sizeof(arena_golden)/sizeof(arena_golden[0]);case_index++) {
        const struct ArenaGolden *g=&arena_golden[case_index];
        memcpy(storage,g->initial,sizeof(g->initial));
        initialize_descriptor((GeorgeArenaBuffer *)(storage+LOCAL-BUFFER),g->initial+(LOCAL-BUFFER)/4,g->raw_addresses);
        initialize_descriptor(&D_0046A0D8,g->global_initial,g->raw_addresses);
        memcpy(outputs,g->outputs,sizeof(outputs));mutation=g->mutation;
        call_count=event_count=0;memset(events,0,sizeof(events));result=0;
        descriptor=native_pointer(g->descriptor_global?GLOBAL:LOCAL);
        switch (g->routine) {
        case 0: result=(u32)func_002B8E00((GeorgeArenaBuffer *)descriptor,g->argument,g->exponent);break;
        case 1: func_002B8E88((GeorgeArenaBuffer *)descriptor);break;
        case 2: func_002B8EA8((void *)native_pointer(g->global_initial[0]),g->global_initial[1]);break;
        case 3: func_002B8EE8();break;
        case 4: func_002B8EF0(g->exponent);break;
        case 5: result=(u32)func_002B8F28(g->argument,g->exponent);break;
        case 6: result=(u32)func_002B8FB8(g->argument);break;
        case 7: func_002B9040(g->argument,g->exponent,(void **)native_pointer(outputs[0]));break;
        case 8: func_002B9120(g->argument,g->exponent,(void **)native_pointer(outputs[0]),(void **)native_pointer(outputs[1]));break;
        case 9: func_002B9238(g->argument,g->exponent,(void **)native_pointer(outputs[0]),(void **)native_pointer(outputs[1]),(void **)native_pointer(outputs[2]));break;
        case 10:func_002B9388(g->argument,g->exponent,(void **)native_pointer(outputs[0]),(void **)native_pointer(outputs[1]),(void **)native_pointer(outputs[2]));break;
        default:return 2;
        }
        for (i=0;i<512;i++) check(canonical(((u32 *)storage)[i])==g->expected[i],"whole authored buffer");
        for (i=0;i<6;i++) check(canonical(((u32 *)&D_0046A0D8)[i])==g->global_expected[i],"whole global descriptor");
        check(canonical(result)==g->result,"observed pointer result only");
        check(call_count==g->calls,"zero call count");check(event_count==g->event_count,"event extent");
        for (i=0;i<g->event_count;i++) check(canonical(events[i])==g->events[i],"zero call and published-state event");
    }
    printf("arena_buffers: %lu checks, %lu failures\n",(unsigned long)checks,(unsigned long)failures);
    free(allocation);return failures?1:0;
}
