#ifndef GEORGE_ACTOR_FIELD_LEAVES_H
#define GEORGE_ACTOR_FIELD_LEAVES_H

#include "george/actor_movement.h"

/* Semantic symbols avoid changing existing incompatible numeric declarations.
 * Offsets name consumed storage only; original classes/prototypes are unknown.
 * Masks and raw results occupy the lower 64 bits. Word stores commit 32 bits.
 */
s32 george_actor_word198_test_bits(const void *owner, GeorgeActorBits64 mask);
GeorgeActorBits64 george_actor_word198_set_bits(void *owner, GeorgeActorBits64 mask);
GeorgeActorBits64 george_actor_word198_clear_bits(void *owner, GeorgeActorBits64 mask);
s32 george_actor_word190_test_bits(const void *owner, GeorgeActorBits64 mask);
s32 george_actor_word360_increment(void *owner);
s32 george_actor_word360_decrement(void *owner);
s32 george_actor_word038_increment(void *owner);
s32 george_actor_word038_decrement(void *owner);
GeorgeActorBits64 george_actor_word148_set_bits(void *owner, GeorgeActorBits64 mask);
GeorgeActorBits64 george_actor_word148_clear_bits(void *owner, GeorgeActorBits64 mask);
GeorgeActorBits64 george_actor_word148_mask_bits(const void *owner, GeorgeActorBits64 mask);

#endif
