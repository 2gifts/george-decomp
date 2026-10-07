#include <stddef.h>
#include <stdio.h>
#include "george/actor_field_leaves.h"

/* A constructed typed storage counterpart, not an original actor class. */
typedef struct ActorFieldFixture {
    u8 pad000[0x38];
    u32 word038;
    u8 pad03C[0x10C];
    u32 word148;
    u8 pad14C[0x44];
    GeorgeActorBits64 word190;
    u32 word198;
    u8 pad19C[0x1C4];
    u32 word360;
    u32 sentinel364;
} ActorFieldFixture;
typedef char fixture_038[(offsetof(ActorFieldFixture, word038) == 0x38) ? 1 : -1];
typedef char fixture_148[(offsetof(ActorFieldFixture, word148) == 0x148) ? 1 : -1];
typedef char fixture_190[(offsetof(ActorFieldFixture, word190) == 0x190) ? 1 : -1];
typedef char fixture_198[(offsetof(ActorFieldFixture, word198) == 0x198) ? 1 : -1];
typedef char fixture_360[(offsetof(ActorFieldFixture, word360) == 0x360) ? 1 : -1];
typedef char fixture_size[(sizeof(ActorFieldFixture) == 0x368) ? 1 : -1];
typedef char native_pointer32[(sizeof(void *) == 4) ? 1 : -1];
#include "actor_field_leaves_golden.h"

static u32 initial_word(u32 index, u32 salt)
{
    return 0xC63A8107U ^ (index * 0x1020311U) ^ salt;
}

static GeorgeActorBits64 invoke(u32 routine, ActorFieldFixture *owner, GeorgeActorBits64 mask)
{
    switch (routine) {
    case 0: return (GeorgeActorBits64)(signed long long)george_actor_word198_test_bits(owner, mask);
    case 1: return george_actor_word198_set_bits(owner, mask);
    case 2: return george_actor_word198_clear_bits(owner, mask);
    case 3: return (GeorgeActorBits64)(signed long long)george_actor_word190_test_bits(owner, mask);
    case 4: return (GeorgeActorBits64)(signed long long)george_actor_word360_increment(owner);
    case 5: return (GeorgeActorBits64)(signed long long)george_actor_word360_decrement(owner);
    case 6: return (GeorgeActorBits64)(signed long long)george_actor_word038_increment(owner);
    case 7: return (GeorgeActorBits64)(signed long long)george_actor_word038_decrement(owner);
    case 8: return george_actor_word148_set_bits(owner, mask);
    case 9: return george_actor_word148_clear_bits(owner, mask);
    case 10: return george_actor_word148_mask_bits(owner, mask);
    default: return 0;
    }
}

int main(void)
{
    static const u32 boundary[] = {0, 0x7FFFFFFFU, 0x80000000U, 0xFFFFFFFFU};
    static const GeorgeActorBits64 signed_boundary[] = {
        0, 0x7FFFFFFFULL, 0xFFFFFFFF80000000ULL, 0xFFFFFFFFFFFFFFFFULL
    };
    unsigned long checks = 0;
    unsigned int i, j;
    for (i = 0; i < 4; ++i) {
        if ((GeorgeActorBits64)(signed long long)(s32)boundary[i] != signed_boundary[i]) {
            fprintf(stderr, "GNU signed conversion failed at boundary %u\n", i);
            return 1;
        }
        ++checks;
    }
    for (i = 0; i < sizeof(actor_field_golden) / sizeof(actor_field_golden[0]); ++i) {
        const struct ActorFieldGolden *golden = &actor_field_golden[i];
        ActorFieldFixture owner;
        u8 *bytes = (u8 *)&owner;
        GeorgeActorBits64 result;
        for (j = 0; j < sizeof(owner); ++j)
            bytes[j] = (u8)(initial_word(j / 4, golden->salt) >> (8 * (j % 4)));
        owner.word038 = owner.word148 = owner.word198 = owner.word360 = golden->word;
        owner.word190 = ((GeorgeActorBits64)golden->high << 32) | golden->word;
        result = invoke(golden->routine, &owner, golden->mask);
        if (result != golden->result) {
            fprintf(stderr, "fixture %u routine %u lower64 result %016llX != %016llX\n",
                    i, golden->routine, result, golden->result);
            return 1;
        }
        ++checks;
        for (j = 0; j < sizeof(owner); ++j) {
            u8 expected = (u8)(golden->expected[j / 4] >> (8 * (j % 4)));
            if (bytes[j] != expected) {
                fprintf(stderr, "fixture %u routine %u byte %X value %02X != %02X\n",
                        i, golden->routine, j, bytes[j], expected);
                return 1;
            }
            ++checks;
        }
    }
    printf("actor field leaves: %lu checks\n", checks);
    return 0;
}
