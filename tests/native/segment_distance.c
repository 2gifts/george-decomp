#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/segment_distance.h"
#include "segment_distance_golden.h"

typedef unsigned long long Bits64;
static unsigned checks, call_counts[4], event_count;
static u32 events[64];
static float *outputs[2];
static unsigned fixture;

static u32 bits(float value)
{
    union { float value; u32 bits; } data;
    data.value = value;
    return data.bits;
}

static void check(int condition, const char *message, unsigned index)
{
    ++checks;
    if (!condition) {
        fprintf(stderr, "segment distance fixture %u word %u: %s\n", fixture, index, message);
        exit(1);
    }
}

static void event(unsigned index, Bits64 a, Bits64 b, float value)
{
    check(event_count + 8 <= 64, "event capacity", event_count);
    ++call_counts[index];
    events[event_count++] = index;
    events[event_count++] = (u32)a;
    events[event_count++] = (u32)(a >> 32);
    events[event_count++] = (u32)b;
    events[event_count++] = (u32)(b >> 32);
    events[event_count++] = bits(value);
    events[event_count++] = outputs[0] ? bits(*outputs[0]) : 0;
    events[event_count++] = outputs[1] ? bits(*outputs[1]) : 0;
}

/* Explicit finite normal/zero native models of the already pinned source
 * soft-call ABI. These do not implement EE denormal/FCR/exception behavior. */
Bits64 func_00374848(float value)
{
    union { double value; Bits64 bits; } data;
    event(0, 0, 0, value);
    data.value = (double)value;
    return data.bits;
}

s32 func_00373250(Bits64 left, Bits64 right)
{
    union { double value; Bits64 bits; } a, b;
    event(1, left, right, 0.0f);
    a.bits = left; b.bits = right;
    return (a.value > b.value) - (a.value < b.value);
}

Bits64 func_00372CC0(Bits64 left, Bits64 right)
{
    union { double value; Bits64 bits; } a, b;
    event(2, left, right, 0.0f);
    a.bits = left; b.bits = right;
    a.value -= b.value;
    return a.bits;
}

float func_003734F8(Bits64 value)
{
    union { double value; Bits64 bits; } data;
    event(3, value, 0, 0.0f);
    data.bits = value;
    return (float)data.value;
}

static void golden_checks(void)
{
    unsigned i, j;
    for (i = 0; i < sizeof(segment_distance_golden) / sizeof(segment_distance_golden[0]); ++i) {
        const struct SegmentDistanceGolden *g = &segment_distance_golden[i];
        union { u32 words[64]; float values[64]; } buffer;
        float result;
        fixture = i;
        memcpy(buffer.words, g->initial, sizeof(buffer.words));
        memset(call_counts, 0, sizeof(call_counts));
        memset(events, 0, sizeof(events));
        event_count = 0;
        outputs[0] = g->offsets[4] < 0 ? 0 : buffer.values + g->offsets[4];
        outputs[1] = g->offsets[5] < 0 ? 0 : buffer.values + g->offsets[5];
        result = func_0029D5D0((GeorgeMathVec3 *)(buffer.values + g->offsets[0]),
                                (GeorgeMathVec3 *)(buffer.values + g->offsets[1]),
                                (GeorgeMathVec3 *)(buffer.values + g->offsets[2]),
                                (GeorgeMathVec3 *)(buffer.values + g->offsets[3]),
                                outputs[0], outputs[1]);
        check(bits(result) == g->result, "return differs from original finite trace", 64);
        for (j = 0; j < 64; ++j)
            check(buffer.words[j] == g->expected[j], "memory differs from original finite trace", j);
        check(event_count == g->event_count, "soft event extent", 65);
        for (j = 0; j < 4; ++j)
            check(call_counts[j] == g->calls[j], "soft call count", j);
        for (j = 0; j < event_count; ++j)
            check(events[j] == g->events[j], "actual soft operands/publication order", j);
    }
}

/* Independent closed-form perpendicular axes model exercises clipping in
 * both parameters without using the reused nine-region algorithm. */
static void geometry_invariants(void)
{
    GeorgeMathVec3 a = {0, 0, 0}, u = {2, 0, 0}, b, v = {0, 2, 0};
    int xi, yi, zi;
    outputs[0] = outputs[1] = 0;
    for (xi = -8; xi <= 16; ++xi) {
        for (yi = -16; yi <= 8; ++yi) {
            for (zi = -4; zi <= 4; ++zi) {
                float x = xi / 4.0f, y = yi / 4.0f, z = zi / 4.0f;
                float near_x = x < 0 ? 0 : x > 2 ? 2 : x;
                float near_y = y > 0 ? y : y < -2 ? y + 2 : 0;
                float residual_x = x - near_x;
                float expected = (residual_x * residual_x + near_y * near_y) + z * z;
                float s, t, result;
                b.x = x; b.y = y; b.z = z;
                event_count = 0;
                result = func_0029D5D0(&a, &u, &b, &v, &s, &t);
                check(bits(result) == bits(expected), "perpendicular distance model", 0);
                check(bits(s) == bits(near_x / 2), "first clipped fraction model", 1);
                check(t == (y > 0 ? 0 : y < -2 ? 1 : -y / 2), "second clipped fraction model", 2);
            }
        }
    }
}

int main(void)
{
    golden_checks();
    geometry_invariants();
    printf("segment_distance: %u checks passed\n", checks);
    return 0;
}
