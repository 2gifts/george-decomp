#ifndef GEORGE_ACTOR_STATES3_H
#define GEORGE_ACTOR_STATES3_H

#include "george/actor_states2.h"
#include "george/script_object.h"
#include "george/deimos_calls.h"
#include "george/accessors.h"

/* Observed callback arguments for the attachment API; distinct from the
 * three-argument actor request callback used by animation requests. */
typedef void (*GeorgeActorAttachmentCallback)(GeorgeGoalEntity *, void *);
/* SDL/SDR copy an eight-byte position snapshot into unaligned handle+0x14. */
typedef struct __attribute__((packed)) GeorgeActorUnalignedPosition {
    GeorgeActorBits64 field00;
    u32 field08;
} GeorgeActorUnalignedPosition;
typedef char actor_unaligned_position_size[(sizeof(GeorgeActorUnalignedPosition)==12)?1:-1];

void func_00185178(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001852C0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00186328(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001867D8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00186AC0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00186DF8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194F08(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00194F50(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00194FD8(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195068(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_001950C0(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001950E8(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195140(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195168(GeorgeGoalEntity *entity, void *object) GEORGE_SAVE128;
void func_001951C0(GeorgeGoalEntity *entity, u32 word, float height) GEORGE_SAVE128;
void func_00195260(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_001952D8(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195330(GeorgeGoalEntity *entity) GEORGE_SAVE128;
void func_00195358(GeorgeGoalEntity *entity, void *reference) GEORGE_SAVE128;
void func_001957D0(u32 unused, GeorgeGoalEntity *entity, void *source) GEORGE_SAVE128;
void func_00195828(GeorgeGoalEntity *entity) GEORGE_SAVE128;

#endif
