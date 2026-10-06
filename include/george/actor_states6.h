#ifndef GEORGE_ACTOR_STATES6_H
#define GEORGE_ACTOR_STATES6_H

#include "george/actor_states5.h"
#include "george/geometry.h"

/* Observed virtual pair at control table+0x100. Basis and translation are
 * separate pointers into a complete local 64-byte frame. */
typedef struct GeorgeActorVirtualFrameParts {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, const GeorgeMathVec4 *, const GeorgeMathVec4 *);
} GeorgeActorVirtualFrameParts;
typedef struct GeorgeActorInteractionCallback {
    const u8 *field00;
    GeorgeGoalEntity *field04;
    u32 field08;
    u32 field0C;
    u8 unknown10[0x28];
} GeorgeActorInteractionCallback;
typedef char actor_frame_parts_target[(offsetof(GeorgeActorVirtualFrameParts,invoke)==4)?1:-1];
typedef char actor_interaction_callback_size[(sizeof(GeorgeActorInteractionCallback)==0x38)?1:-1];

void func_0018C0C8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001962E0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196368(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196280(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00196748(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018CE18(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018D648(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196700(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00196468(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196500(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196608(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196420(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_0018C708(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0018C8A0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001966C0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00196680(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_0018E5A8(GeorgeGoalEntity *entity, const GeorgeMathVec3 *lower,
                     const GeorgeMathVec3 *upper, u32 word, u32 count) GEORGE_SAVE128;
void func_00196980(GeorgeGoalEntity *entity) GEORGE_SAVE128;

#endif
