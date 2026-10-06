#include <stdio.h>
#include <string.h>
#include "george/byte_order.h"

static unsigned checks, failures;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)

typedef union FloatWord { float scalar; u32 bits; } FloatWord;

/* Byte-array reference is deliberately different from the source's shifts. */
static u32 reversed(u32 value, unsigned width)
{
    unsigned index;
    u32 result = 0;
    for (index = 0; index < width; ++index) {
        result = result * 256u + value % 256u;
        value /= 256u;
    }
    return result;
}

static int finite_word(u32 bits)
{
    return (bits & 0x7F800000u) != 0x7F800000u;
}

static void scalars(void)
{
    unsigned index;
    u32 value = 0x719382ABu;
    for (index = 0; index < 65536; ++index)
        CHECK(func_002B2430((u16)index) == reversed(index, 2));
    for (index = 0; index < 20000; ++index) {
        u32 expected;
        FloatWord floating, result;
        value = value * 1664525u + 1013904223u;
        expected = reversed(value, 4);
        CHECK(func_002B2448(value) == expected);
        CHECK(func_002B24A8(value) == expected);
        CHECK(func_002B2448(func_002B24A8(value)) == value);
        /* Host float argument/return ABI cannot prove EE special values.
         * Test only finite encodings in both directions for this entry. */
        if (finite_word(value) && finite_word(expected)) {
            floating.bits = value;
            result.scalar = func_002B2470(floating.scalar);
            CHECK(result.bits == expected);
        }
    }
    for (index = 0; index < 32; ++index) {
        value = 1u << index;
        CHECK(func_002B2448(value) == reversed(value, 4));
        CHECK(func_002B24A8(~value) == reversed(~value, 4));
    }
    {
        static const u32 exceptional[] = {0, 0x80000000u, 1, 0x80000001u,
            0x7F800000u, 0xFF800000u, 0x7FC12345u, 0xFFC12345u,
            0x7F812345u, 0xFF812345u, 0xFFFFFFFFu};
        for (index = 0; index < sizeof exceptional / sizeof exceptional[0]; ++index) {
            value = exceptional[index];
            CHECK(func_002B2448(value) == reversed(value, 4));
            CHECK(func_002B24A8(value) == reversed(value, 4));
        }
    }
}

static void in_place(void)
{
    unsigned length, index, method;
    u16 halves[24], halves_expected[24];
    u32 words[24], words_expected[24], floats[24], float_expected[24];
    static const u32 encodings[] = {0, 0x80000000u, 1, 0x80000001u,
        0x7F800000u, 0xFF800000u, 0x7FC12345u, 0xFFC12345u,
        0x7F812345u, 0xFF812345u, 0x01234567u, 0xFFFFFFFFu};
    void (*fixed[3])(float *) = {func_002B25E0, func_002B2600, func_002B2620};
    unsigned fixed_count[3] = {16, 6, 15};
    for (length = 0; length <= 20; ++length) {
        for (index = 0; index < 24; ++index) {
            halves[index] = halves_expected[index] = (u16)(index * 1739u + 329u);
            words[index] = words_expected[index] = encodings[index % 12];
            /* Float destinations are finite so native x87 loads/stores cannot
             * quiet a signaling NaN instead of reproducing target bit moves. */
            floats[index] = float_expected[index] = 0x01234567u + index * 271u;
        }
        for (index = 2; index < length + 2; ++index) {
            halves_expected[index] = (u16)reversed(halves_expected[index], 2);
            words_expected[index] = reversed(words_expected[index], 4);
            float_expected[index] = reversed(float_expected[index], 4);
            CHECK(finite_word(float_expected[index]));
        }
        func_002B26A8(halves + 2, (s32)length);
        func_002B2730(words + 2, (s32)length);
        func_002B2640((float *)(floats + 2), (s32)length);
        CHECK(memcmp(halves, halves_expected, sizeof halves) == 0);
        CHECK(memcmp(words, words_expected, sizeof words) == 0);
        CHECK(memcmp(floats, float_expected, sizeof floats) == 0);
    }
    for (method = 0; method < 3; ++method) {
        for (index = 0; index < 24; ++index)
            floats[index] = float_expected[index] = 0x01234567u + index * 271u;
        for (index = 0; index < fixed_count[method]; ++index)
            float_expected[index + 2] = reversed(float_expected[index + 2], 4);
        fixed[method]((float *)(floats + 2));
        CHECK(memcmp(floats, float_expected, sizeof floats) == 0);
    }
    for (index = 0; index < 24; ++index)
        floats[index] = float_expected[index] = 0x01234567u + index * 271u;
    for (index = 2; index < 5; ++index)
        float_expected[index] = reversed(float_expected[index], 4);
    func_002B24D0((float *)(floats + 2));
    CHECK(memcmp(floats, float_expected, sizeof floats) == 0);
    func_002B26A8(NULL, 0); func_002B26A8(NULL, -1);
    func_002B2730(NULL, 0); func_002B2730(NULL, (s32)0x80000000u);
    func_002B2640(NULL, 0); func_002B2640(NULL, -99);
}

static void reference_copy16(u16 *data, unsigned destination, unsigned source, unsigned length)
{
    unsigned index;
    for (index = 0; index < length; ++index) {
        u16 old = data[source + index];
        data[destination + index] = (u16)reversed(old, 2);
    }
}

static void reference_copy32(u32 *data, unsigned destination, unsigned source, unsigned length)
{
    unsigned index;
    for (index = 0; index < length; ++index) {
        u32 old = data[source + index];
        data[destination + index] = reversed(old, 4);
    }
}

static void copies(void)
{
    unsigned destination, length, index;
    u16 halves[48], halves_expected[48];
    u32 words[48], words_expected[48];
    for (destination = 4; destination <= 28; ++destination) {
        for (length = 0; length <= 16; ++length) {
            for (index = 0; index < 48; ++index) {
                halves[index] = halves_expected[index] = (u16)(index * 1739u + 329u);
                words[index] = words_expected[index] = index * 0x7391245u + 0x819A4302u;
            }
            reference_copy16(halves_expected, destination, 16, length);
            reference_copy32(words_expected, destination, 16, length);
            func_002B26E0(halves + destination, halves + 16, 1, (s32)length);
            func_002B2790(words + destination, words + 16, 1, (s32)length);
            CHECK(memcmp(halves, halves_expected, sizeof halves) == 0);
            CHECK(memcmp(words, words_expected, sizeof words) == 0);
        }
        for (length = 0; length <= 5; ++length) {
            for (index = 0; index < 48; ++index)
                words[index] = words_expected[index] = index * 0x7391245u + 0x819A4302u;
            reference_copy32(words_expected, destination, 16, length * 3);
            func_002B2568(words + destination, words + 16, (s32)length);
            CHECK(memcmp(words, words_expected, sizeof words) == 0);
        }
    }
    {
        static const struct { s32 rows, columns; unsigned result; } cases[] = {
            {-1, -1, 1}, {-2, -3, 6}, {(s32)0x80000001u, 2, 2},
            {(s32)0x80000000u, 2, 0}, {(s32)0x7FFFFFFFu, 2, 0},
            {0x55555556, 3, 2}, {0, -3, 0}, {1, -1, 0},
        };
        unsigned case_index;
        for (case_index = 0; case_index < sizeof cases / sizeof cases[0]; ++case_index) {
            for (index = 0; index < 48; ++index) {
                halves[index] = halves_expected[index] = (u16)(index * 1739u + 329u);
                words[index] = words_expected[index] = index * 0x7391245u + 0x819A4302u;
            }
            reference_copy16(halves_expected, 18, 16, cases[case_index].result);
            reference_copy32(words_expected, 18, 16, cases[case_index].result);
            func_002B26E0(halves + 18, halves + 16, cases[case_index].rows, cases[case_index].columns);
            func_002B2790(words + 18, words + 16, cases[case_index].rows, cases[case_index].columns);
            CHECK(memcmp(halves, halves_expected, sizeof halves) == 0);
            CHECK(memcmp(words, words_expected, sizeof words) == 0);
        }
    }
    for (length = 1; length <= 2; ++length) {
        s32 triples = length == 1 ? (s32)0xAAAAAAABu : 0x55555556;
        for (index = 0; index < 48; ++index)
            words[index] = words_expected[index] = index * 0x7391245u + 0x819A4302u;
        reference_copy32(words_expected, 18, 16, length);
        func_002B2568(words + 18, words + 16, triples);
        CHECK(memcmp(words, words_expected, sizeof words) == 0);
    }
    func_002B2568(NULL, NULL, 0); func_002B2568(NULL, NULL, (s32)0x80000000u);
    func_002B26E0(NULL, NULL, (s32)0x80000000u, 2);
    func_002B2790(NULL, NULL, -1, 1);
}

int main(void)
{
    scalars(); in_place(); copies();
    printf("byte_order: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
