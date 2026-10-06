#ifndef GEORGE_ACTOR_STATES_H
#define GEORGE_ACTOR_STATES_H

#include "george/actor_movement.h"

/* The actor's field20 object has its table at offset 0. Vehicle objects use
 * the already observed field04 table. These views assert only accessed bytes. */
typedef struct GeorgeActorControlObject {
    const u8 *field00;
} GeorgeActorControlObject;
typedef struct GeorgeActorVirtualPredicate {
    s16 adjustment;
    u16 unknown02;
    s32 (*invoke)(void *, float, float);
} GeorgeActorVirtualPredicate;
typedef struct GeorgeActorVirtualVectorInput {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, const GeorgeMathVec3 *);
} GeorgeActorVirtualVectorInput;
typedef struct GeorgeActorVirtualEntity {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, GeorgeGoalEntity *);
} GeorgeActorVirtualEntity;
typedef void (*GeorgeActorRequestCallback)(u32, GeorgeGoalEntity *, void *);

typedef char actor_predicate_target[(offsetof(GeorgeActorVirtualPredicate, invoke) == 4) ? 1 : -1];
typedef char actor_vector_input_target[(offsetof(GeorgeActorVirtualVectorInput, invoke) == 4) ? 1 : -1];
typedef char actor_entity_target[(offsetof(GeorgeActorVirtualEntity, invoke) == 4) ? 1 : -1];

void func_0017D908(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0017EB20(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0017EBF0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0017F588(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00182B98(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00183380(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00183730(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001839D0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194B70(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00194BB8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194C08(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194C98(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00194CE0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194D38(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194D60(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00194DA8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194DE0(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00194E28(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194EA8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
u32 func_00195D88(GeorgeGoalEntity *entity, u32 key) GEORGE_SAVE128;

#endif
