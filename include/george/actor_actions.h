#ifndef GEORGE_ACTOR_ACTIONS_H
#define GEORGE_ACTOR_ACTIONS_H

#include "george/actor_controls.h"
#include "george/deimos_calls.h"
#include "george/script_object.h"

/* Only the observed signed adjustment and two-argument vector call are asserted. */
typedef struct GeorgeActorVirtualVec4Input {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, const GeorgeMathVec4 *);
} GeorgeActorVirtualVec4Input;
typedef char actor_vec4_input_target[(offsetof(GeorgeActorVirtualVec4Input, invoke)==4)?1:-1];

void func_001784D0(GeorgeGoalEntity *, u32, float) GEORGE_SAVE128;
void func_001787B0(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00178D10(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00178E80(GeorgeGoalEntity *, u32, float) GEORGE_SAVE128;
void func_00179168(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00190FC0(u32 unused, GeorgeGoalEntity *, void *) GEORGE_SAVE128;
void func_00191E88(GeorgeGoalEntity *, u32, float) GEORGE_SAVE128;
void func_00191F78(GeorgeGoalEntity *, float) GEORGE_SAVE128;
void func_00191FF0(GeorgeGoalEntity *, float) GEORGE_SAVE128;
void func_00192078(GeorgeGoalEntity *, u32, float) GEORGE_SAVE128;
void func_00192180(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00192B28(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00192BC0(u32 unused, GeorgeGoalEntity *, void *) GEORGE_SAVE128;
s32 func_00195850(GeorgeGoalEntity *, u32, u32, u32) GEORGE_SAVE128;
void *func_00195DC8(GeorgeGoalEntity *, GeorgeMathVec3 *, GeorgeMathVec3 *,
                   u32, float) GEORGE_SAVE128;

#endif
