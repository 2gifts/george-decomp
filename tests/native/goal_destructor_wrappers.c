#include "george/goal_destructor_wrappers.h"
#include "george/goals.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

struct GoalForwardArena {
    GeorgeGoalBase first;
    u32 unused10[4];
    GeorgeGoalBase second;
    u32 canaries30[20];
};
typedef char forward_arena128[(sizeof(struct GoalForwardArena)==128)?1:-1];
typedef char forward_second32[(offsetof(struct GoalForwardArena,second)==32)?1:-1];
static struct GoalForwardArena arena;
static unsigned char old_table[16], base_table[16];
static u32 calls, frees, seen_flags, seen_offset;
static u32 event_count, event_kind[2], event_pointer[2], event_value[2];
static unsigned long checks;

/* This exact-typed native helper is a disclosed finite observer seam.
 * It does not execute/award the published destructor or allocator body. */
void func_0020D2C0(GeorgeGoalBase *storage, u32 flags)
{
    ++calls;
    seen_offset=(u32)((unsigned char *)storage-(unsigned char *)&arena);
    seen_flags=flags;
    event_kind[event_count]=1U;event_pointer[event_count]=seen_offset;
    event_value[event_count++]=flags;
    storage->field0C=base_table;
    if (flags&1U) {
        if(storage->field0C!=base_table) {fprintf(stderr,"publication before free\n");return;}
        event_kind[event_count]=2U;event_pointer[event_count]=seen_offset;
        event_value[event_count++]=0x43A2B0U;++frees;
    }
}

#include "goal_destructor_wrappers_golden.h"

static u32 read_word(const unsigned char *p)
{
    return (u32)p[0]|((u32)p[1]<<8)|((u32)p[2]<<16)|((u32)p[3]<<24);
}
static void write_word(unsigned char *p,u32 v)
{
    p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);
    p[2]=(unsigned char)(v>>16);p[3]=(unsigned char)(v>>24);
}
int main(void)
{
    unsigned c,i;
    for(c=0;c<sizeof(goal_forward_golden)/sizeof(goal_forward_golden[0]);++c) {
        const struct GoalForwardGolden *g=&goal_forward_golden[c];
        unsigned char *bytes=(unsigned char *)&arena;
        for(i=0;i<32;++i) write_word(bytes+4*i,g->initial[i]);
        arena.first.field0C=old_table;arena.second.field0C=old_table;
        calls=frees=seen_flags=seen_offset=event_count=0;
        george_goal_base_destructor_forward(bytes+g->offset,g->flags);
        ++checks;if(calls!=1U||frees!=(g->flags&1U)||seen_flags!=g->flags||seen_offset!=g->offset||
            event_count!=1U+(g->flags&1U)||event_kind[0]!=1U||event_pointer[0]!=g->offset||event_value[0]!=g->flags||
            ((g->flags&1U)&&(event_kind[1]!=2U||event_pointer[1]!=g->offset||event_value[1]!=0x43A2B0U))) {
            fprintf(stderr,"forwarding fixture%u event mismatch\n",c);return 1;
        }
        for(i=0;i<32;++i) {
            u32 actual=read_word(bytes+4*i), expected=g->expected[i];
            if(i==3||i==11) {
                const void *p=(i==3)?arena.first.field0C:arena.second.field0C;
                if(p==base_table) actual=0x43A2B0U;
                else if(p==old_table) actual=g->initial[i];
                else {fprintf(stderr,"forwarding fixture%u invalid typed pointer\n",c);return 1;}
            }
            ++checks;if(actual!=expected) {fprintf(stderr,"forwarding fixture%u word%u\n",c,i);return 1;}
        }
    }
    printf("PASS %lu forwarding checks; pointer%lu u32%lu base%lu field0C%lu arena%lu second%lu\n",checks,
        (unsigned long)sizeof(void *),(unsigned long)sizeof(u32),(unsigned long)sizeof(GeorgeGoalBase),
        (unsigned long)offsetof(GeorgeGoalBase,field0C),(unsigned long)sizeof(arena),
        (unsigned long)offsetof(struct GoalForwardArena,second));return 0;
}
