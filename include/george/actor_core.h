#ifndef GEORGE_ACTOR_CORE_H
#define GEORGE_ACTOR_CORE_H

#include "george/actor_substates.h"

/* Original local output regions overlap: frame at +00, Vec4 at +10, frame at
 * +20, Vec3 at +30/+40 and frame at +50/+90. Keep these observed views together
 * without claiming an original source local type. Unwritten scratch is output
 * storage for the engine calls; it is not initialized by the caller. */
typedef union GeorgeActorCoreScratch {
    u32 word[52];
    float scalar[52];
    GeorgeRotationMatrix alignment;
} GeorgeActorCoreScratch __attribute__((aligned(16)));
typedef char actor_core_scratch_size[(sizeof(GeorgeActorCoreScratch)==0xD0)?1:-1];

/* Observed numerical entries only. The complete actor class remains opaque. */
void func_0016FC98(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_001703F0(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00170600(GeorgeGoalEntity *, float) GEORGE_SAVE128;
void func_00171FB8(GeorgeGoalEntity *, GeorgeRotationMatrix *) GEORGE_SAVE128;
s32 func_00172550(GeorgeGoalEntity *) GEORGE_SAVE128;

#endif
