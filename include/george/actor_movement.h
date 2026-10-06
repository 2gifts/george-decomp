#ifndef GEORGE_ACTOR_MOVEMENT_H
#define GEORGE_ACTOR_MOVEMENT_H

#include "george/goal_methods6.h"
#include "george/vector_math.h"

/* Only accessed numeric storage is asserted. State/class names are unresolved. */
typedef unsigned long long GeorgeActorBits64;
typedef struct GeorgeActorStateRecord {
    GeorgeGoalMember field00, field08, field10;
    u32 field18;
} GeorgeActorStateRecord;
typedef struct GeorgeActorCollisionRecord {
    u32 field00;
    u8 unknown04[0x10];
} GeorgeActorCollisionRecord;
typedef struct GeorgeActorVirtualConfigure {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, GeorgeGoalEntity *, s32);
} GeorgeActorVirtualConfigure;
typedef struct GeorgeActorVirtualCommandVector {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, s32, GeorgeMathVec3 *, GeorgeMathVec3 *);
} GeorgeActorVirtualCommandVector;
typedef struct GeorgeActorVirtualIndex {
    s16 adjustment;
    u16 unknown02;
    GeorgeGoalVirtualObject *(*invoke)(void *, u32);
} GeorgeActorVirtualIndex;

typedef char actor_state_stride[(sizeof(GeorgeActorStateRecord) == 0x1C) ? 1 : -1];
typedef char actor_collision_stride[(sizeof(GeorgeActorCollisionRecord) == 0x14) ? 1 : -1];
typedef char actor_bits64_size[(sizeof(GeorgeActorBits64) == 8) ? 1 : -1];
typedef char actor_configure_target[(offsetof(GeorgeActorVirtualConfigure, invoke) == 4) ? 1 : -1];
typedef char actor_command_target[(offsetof(GeorgeActorVirtualCommandVector, invoke) == 4) ? 1 : -1];
typedef char actor_index_target[(offsetof(GeorgeActorVirtualIndex, invoke) == 4) ? 1 : -1];

void func_00170538(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00173648(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00173720(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00173818(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_001739A0(GeorgeGoalEntity *entity, const GeorgeMathVec3 *vector) GEORGE_SAVE128;
s32 func_00173B20(GeorgeGoalEntity *entity, s32 value) GEORGE_SAVE128;
void func_00174770(GeorgeGoalEntity *entity, s32 value, float adjustment) GEORGE_SAVE128;
void func_001765D0(GeorgeGoalEntity *entity, float step) GEORGE_SAVE128;
s32 func_00176E10(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00176F58(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00177160(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00177270(GeorgeGoalEntity *entity, const GeorgeMathVec3 *target,
                 u32 word, float height) GEORGE_SAVE128;
s32 func_001773F0(GeorgeGoalEntity *entity, u32 object, u32 value0, u32 value1) GEORGE_SAVE128;
s32 func_001775A8(GeorgeGoalEntity *entity, s32 command) GEORGE_SAVE128;
s32 func_001778C8(GeorgeGoalEntity *entity, u32 object, s32 value) GEORGE_SAVE128;
s32 func_001779F8(GeorgeGoalEntity *entity, void *reference, s32 word) GEORGE_SAVE128;
void func_00177B80(GeorgeGoalEntity *entity) GEORGE_SAVE128;
s32 func_00177C40(GeorgeGoalEntity *entity, u32 word0, u32 word1,
                 u32 word2, u32 word3, u32 word4, u32 override_word,
                 float value) GEORGE_SAVE128;
s32 func_00177E48(GeorgeGoalEntity *entity, const GeorgeMathVec3 *input) GEORGE_SAVE128;
s32 func_00179070(GeorgeGoalEntity *entity, float threshold) GEORGE_SAVE128;

#endif
