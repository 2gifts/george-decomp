#ifndef GEORGE_TIMER_REGISTRY_H
#define GEORGE_TIMER_REGISTRY_H

#include "george/actor_states2.h"
#include "george/tree_updates.h"

/* Numeric field views only. The first allocation's word at +0 is unwritten;
 * the constant and divider do not establish hardware frequency or time units. */
typedef struct GeorgeTimerRate {
    u32 unknown00;
    u32 field04;
    u32 field08;
} GeorgeTimerRate;

typedef char george_timer_rate_width[(sizeof(GeorgeTimerRate) == 12) ? 1 : -1];
typedef char george_timer_rate_constant[(offsetof(GeorgeTimerRate, field04) == 4) ? 1 : -1];
typedef char george_timer_rate_divider[(offsetof(GeorgeTimerRate, field08) == 8) ? 1 : -1];

/* Start has an effects-only convention: selected original callers do not use
 * its incidental Count result. Only the first timer word is accessed. */
void func_002BD3C8(u32 *timer) GEORGE_SAVE128;
u32 func_002BD618(u32 *timer, s32 reset) GEORGE_SAVE128;
u32 func_002BD928(u32 *timer, s32 reset) GEORGE_SAVE128;

#endif
