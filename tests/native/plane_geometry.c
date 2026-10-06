#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/plane_geometry.h"
#include "plane_geometry_golden.h"

static unsigned checks;

static void check(int condition, const char *message, unsigned fixture, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr, "plane geometry fixture %u word %u: %s\n", fixture, index, message);
        exit(1);
    }
}

static u32 bits(float value)
{
    union { float value; u32 bits; } data;
    data.value = value;
    return data.bits;
}

static float scalar(u32 value)
{
    union { float value; u32 bits; } data;
    data.bits = value;
    return data.value;
}

/* The separate published vector TU introduces these uncalled engine hooks.
 * A surprise invocation must fail, rather than hide an invented substitute. */
static float unexpected(void)
{
    fprintf(stderr, "unexpected engine trigonometric call\n");
    exit(1);
    return 0;
}
float func_0029B940(float y, float x) { (void)y; (void)x; return unexpected(); }
float func_0029C090(float angle) { (void)angle; return unexpected(); }
float func_0029C168(float angle) { (void)angle; return unexpected(); }
float func_0029C230(float angle) { (void)angle; return unexpected(); }

static void golden_checks(void)
{
    unsigned i, j;
    for (i = 0; i < sizeof(plane_geometry_golden) / sizeof(plane_geometry_golden[0]); ++i) {
        const struct PlaneGeometryGolden *g = &plane_geometry_golden[i];
        union { u32 words[128]; float values[128]; } buffer;
        GeorgeMathVec3 *a, *b, *c, *d;
        u32 result = 0;
        memcpy(buffer.words, g->initial, sizeof(buffer.words));
        a = g->offsets[0] < 0 ? 0 : (GeorgeMathVec3 *)(buffer.values + g->offsets[0]);
        b = g->offsets[1] < 0 ? 0 : (GeorgeMathVec3 *)(buffer.values + g->offsets[1]);
        c = g->offsets[2] < 0 ? 0 : (GeorgeMathVec3 *)(buffer.values + g->offsets[2]);
        d = g->offsets[3] < 0 ? 0 : (GeorgeMathVec3 *)(buffer.values + g->offsets[3]);
        switch (g->routine) {
        case 0:
            check(func_0029C6C0((GeorgeMathVec4 *)a, b, c, d) == (GeorgeMathVec4 *)a,
                  "normal-plane returns original output", i, 128);
            break;
        case 1:
            check(func_0029C7B0(a, b, c, d) == a, "normal returns original output", i, 128);
            break;
        case 2: result = func_0029CA28(a, b, c, scalar(g->radius)); break;
        case 3: result = func_0029CB20(a, b); break;
        case 4: result = bits(func_0029D048(a, b, c, (float *)d)); break;
        case 5: func_0029E448((GeorgeBounds *)a, b); break;
        case 6: result = func_0029E720((GeorgeBounds *)a, (GeorgeMathVec4 *)b); break;
        default: check(0, "unknown fixture routine", i, 128);
        }
        check(result == g->result, "return differs from finite original instructions", i, 128);
        for (j = 0; j < 128; ++j)
            check(buffer.words[j] == g->expected[j], "memory differs from finite original instructions", i, j);
    }
}

/* Independent closed-form checks use exact quarter/integer axis geometry. */
static void line_invariants(void)
{
    GeorgeMathVec3 origin = {0, 0, 0}, direction = {1, 0, 0}, query;
    int xi, yi, zi;
    for (xi = -12; xi <= 12; ++xi) {
        for (yi = -8; yi <= 8; ++yi) {
            for (zi = -5; zi <= 5; ++zi) {
                float parameter = 99, expected;
                query.x = xi / 4.0f; query.y = yi / 4.0f; query.z = zi / 4.0f;
                expected = query.y * query.y + query.z * query.z;
                check(bits(func_0029D048(&origin, &direction, &query, &parameter)) == bits(expected), "axis line distance", 0, 0);
                check(bits(parameter) == bits(query.x), "line projection is unbounded", 0, 1);
                check(bits(func_0029D048(&origin, &direction, &query, 0)) == bits(expected), "nullable line parameter", 0, 2);
            }
        }
    }
    direction.x = 2; query.x = 3; query.y = 1; query.z = -2;
    check(bits(func_0029D048(&origin, &direction, &query, 0)) == bits(86), "direction is used without normalization", 0, 3);
}

static void triangle_invariants(void)
{
    GeorgeMathVec3 triangle[3] = {{0, 0, 0}, {4, 0, 0}, {0, 4, 0}};
    GeorgeMathVec3 reversed[3] = {{0, 0, 0}, {0, 4, 0}, {4, 0, 0}}, query;
    GeorgeMathVec4 plane;
    int xi, yi;
    func_0029C6C0(&plane, triangle, triangle + 1, triangle + 2);
    check(plane.x == 0 && plane.y == 0 && plane.z == 1 && bits(plane.w) == 0x80000000U, "axis plane and NEG.S signed zero", 0, 0);
    for (xi = -4; xi <= 20; ++xi) {
        for (yi = -4; yi <= 20; ++yi) {
            int expected = xi > 0 && yi > 0 && xi + yi < 16;
            query.x = xi / 4.0f; query.y = yi / 4.0f; query.z = 0;
            check(func_0029CB20(triangle, &query) == expected, "strict triangle interior", 0, 1);
            check(func_0029CB20(reversed, &query) == expected, "reversed triangle interior", 0, 2);
        }
    }
    query.x = 1; query.y = 1; query.z = 8;
    check(func_0029CB20(triangle, &query) == 1, "triangle test does not invent plane-distance gate", 0, 3);
    triangle[1] = triangle[0]; triangle[2] = triangle[0];
    check(func_0029CB20(triangle, &query) == 0, "degenerate triangle strict zero cross", 0, 4);
    func_0029C6C0(&plane, triangle, triangle + 1, triangle + 2);
    check(plane.x == 1 && plane.y == 0 && plane.z == 0, "real normalization zero fallback", 0, 5);
}

static void bounds_invariants(void)
{
    GeorgeBounds bounds = {{-1, -2, -3}, {1, 2, 3}};
    GeorgeMathVec4 sphere;
    int xi, yi, zi, ri;
    for (xi = -3; xi <= 3; ++xi) {
        for (yi = -4; yi <= 4; ++yi) {
            for (zi = -5; zi <= 5; ++zi) {
                int x = xi < -1 ? -1 - xi : xi > 1 ? xi - 1 : 0;
                int y = yi < -2 ? -2 - yi : yi > 2 ? yi - 2 : 0;
                int z = zi < -3 ? -3 - zi : zi > 3 ? zi - 3 : 0;
                int squared = x * x + y * y + z * z;
                sphere.x = (float)xi; sphere.y = (float)yi; sphere.z = (float)zi;
                for (ri = -3; ri <= 3; ++ri) {
                    sphere.w = (float)ri;
                    check(func_0029E720(&bounds, &sphere) == (squared <= ri * ri), "sphere/bounds tangent and radius sign", 0, 0);
                }
            }
        }
    }
    {
        GeorgeMathVec3 point = {-4, 5, 0};
        func_0029E448(&bounds, &point);
        check(bounds.lower.x == -4 && bounds.lower.y == -2 && bounds.lower.z == -3,
              "bounds lower extension", 0, 1);
        check(bounds.upper.x == 1 && bounds.upper.y == 5 && bounds.upper.z == 3,
              "bounds upper extension", 0, 2);
    }
}

int main(void)
{
    golden_checks(); line_invariants(); triangle_invariants(); bounds_invariants();
    printf("plane_geometry: %u checks passed\n", checks);
    return 0;
}
