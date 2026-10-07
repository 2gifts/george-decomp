#ifndef GEORGE_ACTOR_COLLISION_H
#define GEORGE_ACTOR_COLLISION_H

#include "george/spatial_queries.h"
#include "george/pool_slots.h"

/* Only the accessed 32-byte allocation and numeric state view are known. */
typedef struct GeorgeActorCollisionNode {
    GeorgeMathVec3 field00;
    float field0C, field10;
    struct GeorgeActorCollisionNode *field14, *field18, *field1C;
} GeorgeActorCollisionNode;

typedef char george_actor_collision_node_size[
    (sizeof(GeorgeActorCollisionNode) == 32) ? 1 : -1];
typedef char george_actor_collision_parent_offset[
    (offsetof(GeorgeActorCollisionNode, field14) == 0x14) ? 1 : -1];

GeorgeActorCollisionNode *func_001E2EC8(
    void *state, const GeorgeMathVec3 *point, const GeorgeMathVec3 *direction,
    u32 depth, s32 slot, u32 vertex, s32 flag, s32 control,
    float scalar0, float scalar1) GEORGE_SAVE128;
void func_001E52E8(void *state, GeorgeActorCollisionNode *node);

#endif
