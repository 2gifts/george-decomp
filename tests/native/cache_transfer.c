#include <stdio.h>
#include <string.h>
#include "george/cache_transfer.h"

u32 D_003FD24C, D_003FD250;
GeorgeCacheTransferRecord D_00469E00[10];
static unsigned checks, failures, copies;
static u32 copied_address, copied_size;
static const void *copied_source;
static int mutation;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)

static const void *key(unsigned index) { return (const void *)(0x1000u + index * 32u); }

void *func_003934F8(void *destination, const void *source, u32 size)
{
    ++copies;
    copied_address = (u32)destination;
    copied_source = source;
    copied_size = size;
    if (mutation) {
        CHECK(D_003FD250 == 3 && D_003FD24C == 0x80);
        CHECK(D_00469E00[3].source == NULL);
        D_003FD250 = 7;
        D_003FD24C = 0x200;
        D_00469E00[6].source = key(9);
        D_00469E00[6].offset = 0x220;
        D_00469E00[6].size = 16;
    }
    /* The controlled substitute records calls; it does not dereference the
     * observed numeric destination or model physical memory semantics. */
    return (void *)0xDEADBEEFu;
}

static void reset(void)
{
    copies = 0; mutation = 0;
    copied_address = copied_size = 0; copied_source = NULL;
    D_003FD24C = D_003FD250 = 0;
    memset(D_00469E00, 0, sizeof D_00469E00);
}

static void wrapper(void)
{
    static const struct { u32 offset, size, address, result; } cases[] = {
        {0, 0, 0x11000000, 0}, {0xB00, 0x270, 0x11000B00, 0xD70},
        {0xFFFFFFFFu, 2, 0x10FFFFFF, 1},
        {0xF0000000u, 3, 0x01000000, 0xF0000003u},
        {1, 0xFFFFFFFFu, 0x11000001, 0},
    };
    unsigned index;
    for (index = 0; index < sizeof cases / sizeof cases[0]; ++index) {
        reset();
        CHECK(func_002B3150(cases[index].offset, key(8), cases[index].size) == cases[index].result);
        CHECK(copies == 1 && copied_address == cases[index].address);
        CHECK(copied_source == key(8) && copied_size == cases[index].size);
    }
    reset(); mutation = 1; D_003FD250 = 3; D_003FD24C = 0x80;
    CHECK(func_002B3150(0x80, key(4), 65) == 0xC1);
    CHECK(D_003FD250 == 7 && D_003FD24C == 0x200);
    CHECK(copied_address == 0x11000080 && copied_size == 65);
}

static void hits_and_holes(void)
{
    unsigned cursor, position, index;
    GeorgeCacheTransferRecord before[10];
    for (cursor = 0; cursor < 10; ++cursor) {
        for (position = 1; position <= 4; ++position) {
            reset();
            D_003FD250 = cursor; D_003FD24C = 0x180;
            for (index = 1; index <= 4; ++index) {
                unsigned slot = (cursor + 10 - index) % 10;
                D_00469E00[slot].source = key(index);
                D_00469E00[slot].offset = index * 64;
                D_00469E00[slot].size = 32;
            }
            memcpy(before, D_00469E00, sizeof before);
            /* A matching pointer suppresses copying even if size differs. */
            func_002B2F40(key(position), 999);
            CHECK(copies == 0);
            CHECK(D_003FD250 == cursor && D_003FD24C == 0x180);
            CHECK(memcmp(before, D_00469E00, sizeof before) == 0);
        }
    }
    reset(); D_003FD250 = 3; D_003FD24C = 0x180;
    D_00469E00[1].source = key(6);
    D_00469E00[1].offset = 0x180; D_00469E00[1].size = 16;
    func_002B2F40(key(6), 16);
    CHECK(copies == 1 && copied_source == key(6));
    CHECK(D_00469E00[3].source == key(6) && D_00469E00[1].source == key(6));
    CHECK(D_003FD250 == 4 && D_003FD24C == 0x1C0);
}

static void allocation_rounding(void)
{
    static const u32 offsets[] = {0, 1, 63, 64, 4095};
    static const u32 sizes[] = {0, 1, 63, 64, 65, 127, 128, 257, 4095};
    unsigned cursor, offset_index, size_index, record;
    for (cursor = 0; cursor < 10; ++cursor) {
        for (offset_index = 0; offset_index < sizeof offsets / sizeof offsets[0]; ++offset_index) {
            for (size_index = 0; size_index < sizeof sizes / sizeof sizes[0]; ++size_index) {
                u32 offset = offsets[offset_index], size = sizes[size_index];
                u32 expected = (offset + ((size + 63) / 64) * 64) % 4096;
                reset(); D_003FD250 = cursor; D_003FD24C = offset;
                func_002B2F40(key(30), size);
                CHECK(copies == 1 && copied_address == 0x11000000 + offset);
                CHECK(copied_source == key(30) && copied_size == size);
                CHECK(D_003FD250 == (cursor + 1) % 10 && D_003FD24C == expected);
                for (record = 0; record < 10; ++record) {
                    if (record == cursor) {
                        CHECK(D_00469E00[record].source == key(30));
                        CHECK(D_00469E00[record].offset == offset && D_00469E00[record].size == size);
                    } else {
                        CHECK(D_00469E00[record].source == NULL);
                        CHECK(D_00469E00[record].offset == 0 && D_00469E00[record].size == 0);
                    }
                }
            }
        }
    }
    reset(); D_003FD250 = 9; D_003FD24C = 127;
    func_002B2F40(NULL, 1);
    CHECK(copied_source == NULL && D_003FD250 == 0 && D_003FD24C == 191);
    CHECK(D_00469E00[9].source == NULL && D_00469E00[9].size == 1);
}

static void intervals(void)
{
    unsigned begin, old_begin, length, old_length;
    for (begin = 0; begin <= 96; begin += 16) {
        for (old_begin = 0; old_begin <= 128; old_begin += 16) {
            for (length = 16; length <= 64; length += 16) {
                for (old_length = 16; old_length <= 64; old_length += 16) {
                    /* Positive, nonwrapping intervals use an independent
                     * max-start/min-end geometric intersection reference. */
                    unsigned lower = begin > old_begin ? begin : old_begin;
                    unsigned upper = begin + length < old_begin + old_length
                        ? begin + length : old_begin + old_length;
                    reset(); D_003FD250 = 5; D_003FD24C = begin;
                    D_00469E00[4].source = key(2);
                    D_00469E00[4].offset = old_begin;
                    D_00469E00[4].size = old_length;
                    func_002B2F40(key(3), length);
                    CHECK(D_00469E00[4].source == (lower < upper ? NULL : key(2)));
                    CHECK(D_00469E00[4].offset == old_begin && D_00469E00[4].size == old_length);
                }
            }
        }
    }
    {
        static const struct { u32 begin, size, old_begin, old_size; int clear; } cases[] = {
            {20, 0, 10, 20, 1}, {10, 30, 20, 0, 1}, {20, 0, 20, 0, 0},
            {0, 10, 10, 1, 0}, {10, 1, 0, 10, 0},
            {0xFFFFFFF0u, 32, 0, 8, 0}, {0, 16, 0xFFFFFFF0u, 32, 0},
            {0xFFFFFFF0u, 32, 0, 0xFFFFFFFFu, 1},
            {16, 32, 32, 0xFFFFFFFFu, 1},
        };
        unsigned index;
        for (index = 0; index < sizeof cases / sizeof cases[0]; ++index) {
            reset(); D_003FD250 = 5; D_003FD24C = cases[index].begin;
            D_00469E00[4].source = key(2);
            D_00469E00[4].offset = cases[index].old_begin;
            D_00469E00[4].size = cases[index].old_size;
            func_002B2F40(key(3), cases[index].size);
            CHECK(D_00469E00[4].source == (cases[index].clear ? NULL : key(2)));
            CHECK(D_00469E00[4].offset == cases[index].old_begin && D_00469E00[4].size == cases[index].old_size);
        }
    }
}

static void callback_and_run(void)
{
    reset(); D_003FD250 = 7; D_003FD24C = 64;
    D_00469E00[6].source = key(4); D_00469E00[6].offset = 70; D_00469E00[6].size = 30;
    D_00469E00[5].source = key(5); D_00469E00[5].offset = 200; D_00469E00[5].size = 30;
    D_00469E00[3].source = key(6); D_00469E00[3].offset = 70; D_00469E00[3].size = 30;
    func_002B2F40(key(7), 64);
    CHECK(D_00469E00[6].source == NULL && D_00469E00[6].offset == 70 && D_00469E00[6].size == 30);
    CHECK(D_00469E00[5].source == key(5) && D_00469E00[3].source == key(6));
    reset(); mutation = 1; D_003FD250 = 3; D_003FD24C = 0x80;
    func_002B2F40(key(7), 65);
    CHECK(copies == 1 && copied_address == 0x11000080 && copied_size == 65);
    CHECK(D_00469E00[3].source == NULL);
    CHECK(D_00469E00[7].source == key(7) && D_00469E00[7].offset == 0x200 && D_00469E00[7].size == 65);
    CHECK(D_00469E00[6].source == NULL && D_00469E00[6].offset == 0x220 && D_00469E00[6].size == 16);
    CHECK(D_003FD250 == 8 && D_003FD24C == 0x280);
    reset(); D_003FD24C = 127;
    func_002B2F40(key(8), 0xFFFFFFC1u);
    CHECK(D_003FD24C == 127 && D_00469E00[0].size == 0xFFFFFFC1u);
    reset(); D_003FD24C = 127;
    func_002B2F40(key(8), 0xFFFFFF80u);
    CHECK(D_003FD24C == 4095);
}

int main(void)
{
    wrapper(); hits_and_holes(); allocation_rounding(); intervals(); callback_and_run();
    printf("cache_transfer: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
