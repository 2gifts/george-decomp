#ifndef GEORGE_ACTOR_STATES5_H
#define GEORGE_ACTOR_STATES5_H

#include "george/actor_states4.h"

/* Observed object+4 virtual pair: inline point input, vector and float output. */
typedef struct GeorgeActorVirtualPointVectorScalar {
    s16 adjustment;
    u16 unknown02;
    s32 (*invoke)(void *, const GeorgeMathVec3 *, GeorgeMathVec3 *, float *);
} GeorgeActorVirtualPointVectorScalar;
typedef char actor_point_vector_scalar_target[(offsetof(GeorgeActorVirtualPointVectorScalar,invoke)==4)?1:-1];

void func_00195ED8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00189E88(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195F10(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195E88(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_0018ACC8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018AE08(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195F68(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195FB0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196030(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018B528(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195FE8(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00196050(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001960E8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018B7E8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196088(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00196120(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018BC28(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018BDA0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196210(GeorgeGoalEntity *entity) GEORGE_SAVE128;

#endif
