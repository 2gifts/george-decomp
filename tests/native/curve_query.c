#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/curve_query.h"
#include "curve_query_golden.h"

static unsigned checks;

static void check(int condition, const char *message, unsigned fixture, unsigned word)
{
    ++checks;
    if (!condition) {
        fprintf(stderr, "curve query fixture %u word %u: %s\n", fixture, word, message);
        exit(1);
    }
}

static u32 bits(float value)
{
    union { float value; u32 bits; } data;
    data.value = value;
    return data.bits;
}

static void golden_checks(void)
{
    unsigned i, j;
    for (i = 0; i < sizeof(curve_query_golden) / sizeof(curve_query_golden[0]); ++i) {
        const struct CurveQueryGolden *g = &curve_query_golden[i];
        union { u32 words[64]; float values[64]; } buffer;
        u32 result;
        memcpy(buffer.words, g->initial, sizeof(buffer.words));
        if (g->routine == 0) {
            float *fraction = g->fraction < 0 ? 0 : buffer.values + g->fraction;
            result = bits(func_0029CF28((GeorgeMathVec3 *)(buffer.values + g->a),
                                       (GeorgeMathVec3 *)(buffer.values + g->b),
                                       (GeorgeMathVec3 *)(buffer.values + g->c), fraction));
        } else {
            result = func_0029DD60((GeorgeMathVec4 *)(buffer.values + g->a),
                                  (GeorgePathRay *)(buffer.values + g->b));
        }
        check(result == g->result, "return differs from original finite trace", i, 64);
        for (j = 0; j < 64; ++j)
            check(buffer.words[j] == g->expected[j], "memory differs from original finite trace", i, j);
    }
}

/* Closed-form axis-aligned geometry supplies a second model, independent of
 * the decoded instruction order. Quarter coordinates make these exact cases. */
static void distance_invariants(void)
{
    GeorgeMathVec3 first = {0, 0, 0}, second = {4, 0, 0}, point;
    int xi, yi, zi;
    for (xi = -8; xi <= 24; ++xi) {
        for (yi = -4; yi <= 4; ++yi) {
            for (zi = -3; zi <= 3; ++zi) {
                float x = xi / 4.0f, y = yi / 4.0f, z = zi / 4.0f;
                float nearest_x = x < 0 ? 0 : x > 4 ? 4 : x;
                float residual = x - nearest_x;
                float expected = (residual * residual + y * y) + z * z;
                float fraction, reverse_fraction, result;
                point.x = x; point.y = y; point.z = z;
                result = func_0029CF28(&first, &second, &point, &fraction);
                check(bits(result) == bits(expected), "axis distance identity", 0, 0);
                check(bits(fraction) == bits(nearest_x / 4), "axis fraction identity", 0, 1);
                result = func_0029CF28(&second, &first, &point, &reverse_fraction);
                check(bits(result) == bits(expected), "segment reversal distance", 0, 2);
                check(bits(reverse_fraction) == bits(1 - fraction), "segment reversal fraction", 0, 3);
            }
        }
    }
    point.x = 2; point.y = -3; point.z = 4;
    {
        float fraction = 99;
        check(bits(func_0029CF28(&first, &first, &point, &fraction)) == bits(29),
              "zero-length segment uses first endpoint", 0, 4);
        check(bits(fraction) == 0, "zero-length fraction is zero", 0, 5);
    }
}

static void query_boundaries(void)
{
    GeorgeMathVec4 query = {0, 0, 0, 1};
    GeorgePathRay ray = {{0, 0, 0}, {1, 0, 0}, 2};
    query.x = 0.5f; ray.field18 = -10;
    check(func_0029DD60(&query, &ray) == 1, "strict interior ignores ray extent", 0, 0);
    query.x = 1; ray.field18 = 0;
    check(func_0029DD60(&query, &ray) == 0, "entry equal to endpoint is excluded", 0, 1);
    ray.field18 = 0.25f;
    check(func_0029DD60(&query, &ray) == 1, "entry before endpoint is accepted", 0, 2);
    query.x = -1;
    check(func_0029DD60(&query, &ray) == 0, "boundary behind direction is excluded", 0, 3);
    query.x = 2; query.y = 1; ray.field18 = 2;
    check(func_0029DD60(&query, &ray) == 0, "tangent exactly at endpoint is excluded", 0, 4);
    ray.field18 = 2.25f;
    check(func_0029DD60(&query, &ray) == 1, "tangent before endpoint is accepted", 0, 5);
    query.w = -1;
    check(func_0029DD60(&query, &ray) == 1, "radius sign remains squared", 0, 6);
    query.w = 0; query.x = 0; query.y = 0; ray.field18 = 0;
    check(func_0029DD60(&query, &ray) == 0, "zero-radius endpoint equality remains strict", 0, 7);
    ray.field18 = 1;
    check(func_0029DD60(&query, &ray) == 1, "zero-radius entry can precede endpoint", 0, 8);
    query.x = 2; query.w = 1; ray.field0C.x = 0; ray.field18 = 10;
    check(func_0029DD60(&query, &ray) == 0, "zero direction has no invented normalization", 0, 9);
    ray.field0C.x = 2; ray.field18 = 0.5f;
    check(func_0029DD60(&query, &ray) == 1, "unnormalized direction follows original algebra", 0, 10);
}

int main(void)
{
    golden_checks(); distance_invariants(); query_boundaries();
    printf("curve_query: %u checks passed\n", checks);
    return 0;
}
