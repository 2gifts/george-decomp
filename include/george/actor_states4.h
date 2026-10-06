#ifndef GEORGE_ACTOR_STATES4_H
#define GEORGE_ACTOR_STATES4_H

#include "george/actor_states3.h"

typedef struct GeorgeActorVirtualWordResult {
    s16 adjustment;
    u16 unknown02;
    s32 (*invoke)(void *, u32);
} GeorgeActorVirtualWordResult;
typedef char actor_word_result_target[(offsetof(GeorgeActorVirtualWordResult, invoke)==4)?1:-1];

void func_00195978(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_001959C0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00187648(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00187990(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00187BA0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195A88(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195B08(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00187EA8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00188290(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195B30(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195B78(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00188590(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00188968(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00188B10(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195BE8(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195C30(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00189310(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195CC0(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195D08(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195D50(GeorgeGoalEntity *entity) GEORGE_SAVE128;

#endif
