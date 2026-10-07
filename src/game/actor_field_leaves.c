#include "george/actor_field_leaves.h"

/* Existing project field conventions; initialized aligned scalar storage only.
 * The (s32)word conversion for values above INT_MAX is implementation-defined:
 * this is the measured two's-complement GNU target/native representation.
 * Arithmetic updates remain unsigned32, with no signed-overflow expression.
 */
#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define LOAD_SIGNED_WORD(object, offset) \
    ((GeorgeActorBits64)(signed long long)(s32)FIELD(object, offset, u32))

s32 george_actor_word198_test_bits(const void *owner, GeorgeActorBits64 mask)
{
    return (LOAD_SIGNED_WORD(owner, 0x198) & mask) != 0;
}

GeorgeActorBits64 george_actor_word198_set_bits(void *owner, GeorgeActorBits64 mask)
{
    GeorgeActorBits64 result = LOAD_SIGNED_WORD(owner, 0x198) | mask;
    FIELD(owner, 0x198, u32) = (u32)result;
    return result;
}

GeorgeActorBits64 george_actor_word198_clear_bits(void *owner, GeorgeActorBits64 mask)
{
    GeorgeActorBits64 result = LOAD_SIGNED_WORD(owner, 0x198) & ~mask;
    FIELD(owner, 0x198, u32) = (u32)result;
    return result;
}

s32 george_actor_word190_test_bits(const void *owner, GeorgeActorBits64 mask)
{
    return (FIELD(owner, 0x190, GeorgeActorBits64) & mask) != 0;
}

s32 george_actor_word360_increment(void *owner)
{
    u32 value = FIELD(owner, 0x360, u32) + 1U;
    FIELD(owner, 0x360, u32) = value;
    return (s32)value;
}

s32 george_actor_word360_decrement(void *owner)
{
    u32 value = FIELD(owner, 0x360, u32) - 1U;
    FIELD(owner, 0x360, u32) = value;
    return (s32)value;
}

s32 george_actor_word038_increment(void *owner)
{
    u32 value = FIELD(owner, 0x38, u32) + 1U;
    FIELD(owner, 0x38, u32) = value;
    return (s32)value;
}

s32 george_actor_word038_decrement(void *owner)
{
    u32 value = FIELD(owner, 0x38, u32) - 1U;
    FIELD(owner, 0x38, u32) = value;
    return (s32)value;
}

GeorgeActorBits64 george_actor_word148_set_bits(void *owner, GeorgeActorBits64 mask)
{
    GeorgeActorBits64 result = LOAD_SIGNED_WORD(owner, 0x148) | mask;
    FIELD(owner, 0x148, u32) = (u32)result;
    return result;
}

GeorgeActorBits64 george_actor_word148_clear_bits(void *owner, GeorgeActorBits64 mask)
{
    GeorgeActorBits64 result = LOAD_SIGNED_WORD(owner, 0x148) & ~mask;
    FIELD(owner, 0x148, u32) = (u32)result;
    return result;
}

GeorgeActorBits64 george_actor_word148_mask_bits(const void *owner, GeorgeActorBits64 mask)
{
    return LOAD_SIGNED_WORD(owner, 0x148) & mask;
}
