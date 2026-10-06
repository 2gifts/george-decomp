/* Independent finite face-distance oracle and controlled frame-call contract. */
#include <stdio.h>
#include <string.h>
#include "george/geometry_classify6.h"

static GeorgeGeometryFace faces[6];
static GeorgeGeometryFrame frame;
static s32 returns[6];
static u32 calls, checks, failures;

#define CHECK(value) do { ++checks; if (!(value)) { \
    if (failures < 12) printf("geometry_classify6:%u failed\n",__LINE__); \
    ++failures; } } while (0)

u32 func_0029E098(const GeorgeGeometryFrame *input, const GeorgeGeometryFace *face)
{
    u32 index = calls++;
    CHECK(input == &frame && index < 6);
    CHECK(face == faces + index);
    return (u32)returns[index];
}

static void frame_cases(void)
{
    u32 code, index;
    for (code = 0; code < 4096; ++code) {
        u32 digits = code, expected = 1, expected_calls = 6;
        calls = 0;
        for (index = 0; index < 6; ++index) {
            returns[index] = (s32)(digits & 3) - 1;
            digits >>= 2;
        }
        for (index = 0; index < 6; ++index) {
            if (returns[index] == 2) { expected = 0; expected_calls = index + 1; break; }
            if (returns[index] == 0 && index < 5) expected = 2;
        }
        CHECK(func_002A4808(faces, &frame) == expected);
        CHECK(calls == expected_calls);
    }
}

static u32 sphere_oracle(const GeorgeMathVec4 *sphere)
{
    u32 index, crossing = 0;
    for (index = 0; index < 6; ++index) {
        const GeorgeGeometryFace *face = faces + index;
        double distance = (double)sphere->x * face->normal.x
                        + (double)sphere->y * face->normal.y
                        + (double)sphere->z * face->normal.z - face->distance;
        if (distance > sphere->w) return 0;
        if (distance > -sphere->w && index < 5) crossing = 1;
    }
    return crossing ? 2 : 1;
}

static u32 random_word(u32 *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static float dyadic(u32 *state)
{
    return ((s32)(random_word(state) % 17) - 8) * 0.25f;
}

static void sphere_cases(void)
{
    u32 seed, index, axis;
    GeorgeMathVec4 sphere;
    for (seed = 1; seed <= 4096; ++seed) {
        u32 state = seed;
        sphere.x = dyadic(&state); sphere.y = dyadic(&state); sphere.z = dyadic(&state);
        sphere.w = (random_word(&state) % 9) * 0.25f;
        for (index = 0; index < 6; ++index) {
            faces[index].normal.x = dyadic(&state);
            faces[index].normal.y = dyadic(&state);
            faces[index].normal.z = dyadic(&state);
            faces[index].distance = dyadic(&state);
        }
        CHECK(func_002A48F0(faces, &sphere) == sphere_oracle(&sphere));
    }
    memset(&sphere, 0, sizeof sphere); sphere.w = 1;
    for (index = 0; index < 6; ++index) {
        for (axis = 0; axis < 5; ++axis) {
            u32 i;
            memset(faces, 0, sizeof faces);
            for (i = 0; i < 6; ++i) faces[i].distance = 2;
            faces[index].distance = (float)((s32)axis - 2);
            CHECK(func_002A48F0(faces, &sphere) == sphere_oracle(&sphere));
        }
    }
}

int main(void)
{
    CHECK(sizeof(void *) == 4 && sizeof(GeorgeGeometryFace) == 28);
    frame_cases(); sphere_cases();
    printf("geometry_classify6: %u checks, %u failures\n",checks,failures);
    return failures ? 1 : 0;
}
