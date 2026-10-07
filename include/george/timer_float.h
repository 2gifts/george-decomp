#ifndef GEORGE_TIMER_FLOAT_H
#define GEORGE_TIMER_FLOAT_H

#include "george/timer_registry.h"

/* Natural numeric scalar candidates. No original class/prototype or discovered
 * invocation root is asserted. Ordinary float supports only the reviewed exact
 * representability observation domain; general EE precision/FCR31 differs. */
float func_002BD770(u32 *timer, s32 reset) GEORGE_SAVE128;
float func_002BDA78(u32 unused, u32 value) GEORGE_SAVE128;

#endif
