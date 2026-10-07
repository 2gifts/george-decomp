#include "george/route_setup.h"
#include <stdio.h>
#include <string.h>
#include "route_setup_golden.h"

static u32 arena[1536];

static u32 initial_word(u32 index, u32 seed)
{
    int integer = (int)(((unsigned long long)index * 7 + seed) % 17) - 8;
    float value = (float)integer;
    u32 result;
    if (integer == 0 && (index & 1)) return 0x80000000U;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    u32 n, i, checks = 0;
    for (n = 0; n < sizeof(setup_golden) / sizeof(setup_golden[0]); ++n) {
        const struct SetupGolden *g = &setup_golden[n];
        u32 start = 64 + (g->entry == 0 ? 0xECC : 0xCB0) / 4;
        u32 count = g->entry == 0 ? 26 : 23;
        for (i = 0; i < 1536; ++i) arena[i] = initial_word(i, g->seed);
        if (g->entry == 0)
            func_0020BEA8(arena + 64, (void *)g->state, g->kind, g->first, g->second,
                           (const GeorgeMathVec3 *)(arena + g->point),
                           (const GeorgeMathVec3 *)(arena + g->target));
        else
            func_0020D080(arena + 64, (void *)g->state, g->kind, g->first, g->second,
                           (const GeorgeMathVec3 *)(arena + g->point),
                           (const GeorgeMathVec3 *)(arena + g->target));
        for (i = 0; i < 1536; ++i) {
            u32 expected = i == 64 ? g->status :
                (i >= start && i < start + count ? g->expected[i - start] : initial_word(i, g->seed));
            ++checks;
            if (arena[i] != expected) {
                fprintf(stderr, "route setup fixture %u word %u: %08x != %08x\n",
                        n, i, (unsigned)arena[i], (unsigned)expected);
                return 1;
            }
        }
    }
    printf("%u route setup checks passed\n", (unsigned)checks);
    return 0;
}
