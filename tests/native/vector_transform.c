#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/vector_transform.h"
#include "vector_transform_golden.h"

static unsigned checks, current_fixture;
static union { u32 words[64]; float values[64]; } arena __attribute__((aligned(16)));

static void check(int condition, const char *message, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr, "vector transform fixture %u index %u: %s\n",
                current_fixture, index, message);
        exit(1);
    }
}
static u32 bits(float value)
{
    union { float scalar; u32 bits; } data;
    data.scalar = value;
    return data.bits;
}
static GeorgeMathVec3 *vector(unsigned offset)
{
    check(offset <= sizeof(arena) - 12 && !(offset & 3), "fixture vector range", offset);
    return (GeorgeMathVec3 *)((char *)&arena + offset);
}
static void run_fixtures(void)
{
    unsigned i, j;
    for (i = 0; i < sizeof(transform_golden) / sizeof(transform_golden[0]); ++i) {
        const struct TransformGolden *test = &transform_golden[i];
        GeorgeMathVec3 *input, *output;
        current_fixture = i;
        memcpy(arena.words, test->initial, sizeof(arena));
        input = vector(test->input_offset);
        output = vector(test->output_offset);
        if (test->routine == 0)
            func_002A1C60(&arena, input, output);
        else {
            check(test->routine == 1, "selected complete routine", test->routine);
            func_002A1D78(&arena, input, output);
        }
        for (j = 0; j < 64; ++j)
            check(arena.words[j] == test->expected[j], "complete arena word", j);
    }
}
static void independent_cases(void)
{
    GeorgeRotationMatrix matrix = {{2,0.5f,-1,2, -3,1,0.25f,0,
                                    0.75f,-2,4,1, 2,3,-1,2}};
    GeorgeMathVec3 input = {2,-3,4}, output;
    struct { GeorgeMathVec3 vector; u32 sentinel; } observed;
    current_fixture = 0xFFFFFFFFu;
    observed.sentinel = 0xFEDCBA98u;
    func_002A1C60(&matrix, &input, &observed.vector);
    check(observed.vector.x == 18, "known point x", 0);
    check(observed.vector.y == -7, "known point y", 1);
    check(observed.vector.z == 12.25f, "known point z", 2);
    check(observed.sentinel == 0xFEDCBA98u, "fourth output word untouched", 3);
    func_002A1D78(&matrix, &input, &output);
    check(output.x == 16 && output.y == -10 && output.z == 13.25f,
          "direction excludes translation", 0);
    func_002A1C60(&matrix, &input, &input);
    check(input.x == 18 && input.y == -7 && input.z == 12.25f,
          "same input/output snapshot", 0);
    /* Separately rounded product then addition cancels to +0. A fused or
     * double-precision expression would retain the small positive residual. */
    memset(&matrix, 0, sizeof(matrix));
    matrix.element[0] = -(1.0f + 0.0000002384185791015625f);
    matrix.element[4] = 1.0f + 0.00000011920928955078125f;
    input.x = 1; input.y = matrix.element[4]; input.z = 0;
    func_002A1D78(&matrix, &input, &output);
    check(bits(output.x) == 0, "product rounding before accumulator add", 0);
}
int main(void)
{
    check(sizeof(void *) == 4 && sizeof(float) == 4, "native target widths", 0);
    check(sizeof(GeorgeMathVec3) == 12 && sizeof(GeorgeRotationMatrix) == 64,
          "reused layout sizes", 0);
    run_fixtures();
    independent_cases();
    printf("vector transform: %u checks passed\n", checks);
    return 0;
}
