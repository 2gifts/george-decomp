#ifndef GEORGE_ACTOR_STATES2_H
#define GEORGE_ACTOR_STATES2_H

#include "george/actor_states.h"
#include "george/startup.h"
#include "george/matrix_scalar.h"

typedef struct GeorgeActorPointerRange {
    void **field00, **field04, **field08;
} GeorgeActorPointerRange;
typedef struct GeorgeActorEffectRecord {
    u32 field00;
    const u8 *field04;
    void *field08;
} GeorgeActorEffectRecord;
typedef struct GeorgeActorVirtualCommandScalarVector {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, s32, GeorgeMathVec3 *, float *);
} GeorgeActorVirtualCommandScalarVector;
typedef char actor_pointer_range_size[(sizeof(GeorgeActorPointerRange)==12)?1:-1];
typedef char actor_effect_record_size[(sizeof(GeorgeActorEffectRecord)==12)?1:-1];
typedef char actor_command_scalar_target[(offsetof(GeorgeActorVirtualCommandScalarVector, invoke)==4)?1:-1];

void func_0017DD10(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0017E3A0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_0017ED18(GeorgeGoalEntity *entity, s32 mode) GEORGE_SAVE128;
void func_0017EE30(GeorgeGoalEntity *entity, s32 mode) GEORGE_SAVE128;
void func_0017EF88(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00183C58(GeorgeGoalEntity *entity) GEORGE_SAVE128;

#endif
