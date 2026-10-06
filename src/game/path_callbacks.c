#include "george/path_callbacks.h"

extern float func_0029CF28(const GeorgeMathVec3 *, const GeorgeMathVec3 *,
                          const GeorgeMathVec3 *, float *);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define THIRD 0.3333333432674407958984375f
#define PACK16_SCALE 0.000030518509447574615478515625f
#define PACK8_SCALE 0.0078740157186985015869140625f
#if __GNUC__ >= 3
#define CALLBACK_INLINE static __inline__ __attribute__((always_inline))
#else
#define CALLBACK_INLINE static __inline__
#endif

/* Each complete component block reads fresh endpoint fields before its store.
 * Reusing the scalar arithmetic does not snapshot the next component early. */
CALLBACK_INLINE float polynomial_component(const void *first, const void *second,
                                            u32 component, float time,
                                            float square, float cube)
{
    float current = FIELD(first, component + 4U, float);
    float previous = FIELD(first, component + 0x10U, float);
    float next = FIELD(second, component + 4U, float);
    float following = FIELD(second, component + 0x1CU, float);
    float quadratic = ((previous + previous) - current * 5.0f) + next * 4.0f;
    float cubic = ((-previous + current * 3.0f) - next * 3.0f) + following;
    float linear = (-previous + next) * time;
    quadratic = (quadratic - following) * square;
    cubic *= cube;
    return (((cubic + quadratic) + linear) + (current + current)) * 0.5f;
}

void func_002C03E8(const void *first, const void *second, void *output, float time)
{
    float square = time * time;
    float cube = square * time;
    FIELD(output, 0, float) = polynomial_component(first, second, 0, time, square, cube);
    FIELD(output, 4, float) = polynomial_component(first, second, 4, time, square, cube);
    FIELD(output, 8, float) = polynomial_component(first, second, 8, time, square, cube);
}

CALLBACK_INLINE float polynomial_slope_component(const void *first, const void *second,
                                                  u32 component, float time, float square)
{
    float previous = FIELD(first, component + 0x10U, float);
    float current = FIELD(first, component + 4U, float);
    float next = FIELD(second, component + 4U, float);
    float following = FIELD(second, component + 0x1CU, float);
    float linear = (next - previous) * 0.5f;
    float first_order = ((previous * 4.0f - current * 10.0f) + next * 8.0f) - (following + following);
    float second_order = ((previous * -3.0f + current * 9.0f) - next * 9.0f) + following * 3.0f;
    return (linear + first_order * time) + second_order * square;
}

void func_002C0560(const void *first, const void *second, void *output, float time)
{
    float square = time * time;
    FIELD(output, 0, float) = polynomial_slope_component(first, second, 0, time, square);
    FIELD(output, 4, float) = polynomial_slope_component(first, second, 4, time, square);
    /* Retail's final Z expression overwrites output X, leaving output Z alone. */
    FIELD(output, 0, float) = polynomial_slope_component(first, second, 8, time, square);
}

CALLBACK_INLINE void decode_packed(const void *input, GeorgeMathVec4 *output)
{
    u32 combined;
    s16 first = FIELD(input, 0, s16);
    u32 first_bits = FIELD(input, 0, u8) & 3U;
    u32 last = FIELD(input, 6, u8) << 8;
    u32 second_bits = (FIELD(input, 2, u8) & 3U) << 4;
    u32 third_bits = (FIELD(input, 4, u8) & 3U) << 2;
    s16 second = FIELD(input, 2, s16);
    s16 third = FIELD(input, 4, s16);
    combined = last | (first_bits << 6);
    combined |= second_bits;
    combined |= third_bits;
    output->x = (float)first * PACK16_SCALE;
    output->y = (float)second * PACK16_SCALE;
    output->z = (float)third * PACK16_SCALE;
    output->w = (float)(s16)(u16)combined * PACK16_SCALE;
}

void func_002C0C18(const void *first, const void *second, void *output, float time)
{
    GeorgeMathVec4 left, right;
    decode_packed(first, &left);
    decode_packed(second, &right);
    func_002A2780((GeorgeMathVec4 *)output, &left, &right, time);
}

void func_002C19A8(const void *first, const void *second, void *output, float time)
{
    const void *chosen = time == 1.0f ? second : first;
    FIELD(output, 0, float) = FIELD(chosen, 4, float);
    FIELD(output, 4, float) = FIELD(chosen, 8, float);
    FIELD(output, 8, float) = FIELD(chosen, 12, float);
}

void func_002C1AD0(const void *first, const void *second, void *output, float time)
{
    float remaining = 1.0f - time;
    float span = (FIELD(second, 0, float) - FIELD(first, 0, float)) * THIRD;
    float right_control = time * (remaining * 6.0f);
    float right_square = time * (time * 3.0f);
    float left_square = remaining * (remaining * 3.0f);
    float right_start = right_control * span - right_square * span;
    float left_control = left_square * span - right_control * span;
    float left_end = time * (remaining * -6.0f);
    s32 component;
    for (component = 0; component < 3; ++component) {
        u32 offset = (u32)component << 2;
        float a = left_end * FIELD(first, offset + 0x1CU, float);
        float b = left_control * FIELD(first, offset + 0x10U, float);
        float c = right_start * FIELD(second, offset + 4U, float);
        float d = right_control * FIELD(second, offset + 0x1CU, float);
        FIELD(output, offset, float) = ((a + b) + c) + d;
    }
}

void func_002C1BB0(const void *first, const void *second, void *output, float time)
{
    float span = (FIELD(second, 0, float) - FIELD(first, 0, float)) * THIRD;
    float remaining = 1.0f - time;
    float six_time = time * 6.0f;
    float left_end = remaining * -6.0f + six_time;
    float right_control = remaining * 6.0f - six_time;
    float right_start = remaining * (span * 6.0f) - time * (span * 12.0f);
    float left_control = remaining * (span * -12.0f) + time * (span * 6.0f);
    s32 component;
    for (component = 0; component < 3; ++component) {
        u32 offset = (u32)component << 2;
        float a = left_end * FIELD(first, offset + 0x1CU, float);
        float b = left_control * FIELD(first, offset + 0x10U, float);
        float c = right_start * FIELD(second, offset + 4U, float);
        float d = right_control * FIELD(second, offset + 0x1CU, float);
        FIELD(output, offset, float) = ((a + b) + c) + d;
    }
}

void func_002C1E20(const void *first, const void *second, void *output, float time)
{
    float left_start = time * -6.0f + 6.0f;
    float left_control = time * 18.0f - 12.0f;
    float right_control = 6.0f - time * 18.0f;
    u32 offset;
    for (offset = 0; offset < 12; offset += 4) {
        float a = left_start * FIELD(first, offset + 4U, float);
        float b = left_control * FIELD(first, offset + 0x1CU, float);
        float c = right_control * FIELD(second, offset + 0x10U, float);
        float d = FIELD(second, offset + 4U, float) * 6.0f;
        d *= time;
        FIELD(output, offset, float) = ((a + b) + c) + d;
    }
}

void func_002C1EF8(const void *first, const void *second, void *output, float time)
{
    float x = FIELD(first, 4, float), z = FIELD(first, 12, float), y = FIELD(first, 8, float);
    float dx = FIELD(second, 4, float) - x;
    float dz = FIELD(second, 12, float) - z;
    float dy = FIELD(second, 8, float) - y;
    z += time * dz;
    x += time * dx;
    y += time * dy;
    FIELD(output, 8, float) = z;
    FIELD(output, 0, float) = x;
    FIELD(output, 4, float) = y;
}

void func_002C1F50(const void *first, const void *second, void *output, float time)
{
    float x = FIELD(second, 4, float) - FIELD(first, 4, float);
    float z = FIELD(second, 12, float) - FIELD(first, 12, float);
    float y = FIELD(second, 8, float) - FIELD(first, 8, float);
    (void)time;
    FIELD(output, 0, float) = x;
    FIELD(output, 8, float) = z;
    FIELD(output, 4, float) = y;
}

/* Each complete compressed-wrapper body snapshots both decoded quaternions
 * before the actual shared quaternion call; its return value is ignored. */
#define COMPRESSED_ENTRY(name, width, step, scale) \
void name(const void *first, const void *second, void *output, float time) \
{ \
    GeorgeMathVec4 left, right; \
    left.x = (float)FIELD(first, 4, width) * (scale); \
    left.y = (float)FIELD(first, 4 + (step), width) * (scale); \
    left.z = (float)FIELD(first, 4 + 2 * (step), width) * (scale); \
    left.w = (float)FIELD(first, 4 + 3 * (step), width) * (scale); \
    right.x = (float)FIELD(second, 4, width) * (scale); \
    right.y = (float)FIELD(second, 4 + (step), width) * (scale); \
    right.z = (float)FIELD(second, 4 + 2 * (step), width) * (scale); \
    right.w = (float)FIELD(second, 4 + 3 * (step), width) * (scale); \
    func_002A2780((GeorgeMathVec4 *)output, &left, &right, time); \
}
COMPRESSED_ENTRY(func_002C1F90, s16, 2, PACK16_SCALE)
COMPRESSED_ENTRY(func_002C2088, signed char, 1, PACK8_SCALE)
#undef COMPRESSED_ENTRY

void func_002C21B0(const void *first, const void *second, void *output, float time)
{
    float x, y, z;
    func_002A2780((GeorgeMathVec4 *)output, (GeorgeMathVec4 *)ADDRESS(first, 4),
                  (GeorgeMathVec4 *)ADDRESS(second, 4), time);
    x = FIELD(first, 0x14, float);
    z = FIELD(first, 0x1C, float);
    y = FIELD(first, 0x18, float);
    x += time * (FIELD(second, 0x14, float) - x);
    z += time * (FIELD(second, 0x1C, float) - z);
    y += time * (FIELD(second, 0x18, float) - y);
    FIELD(output, 0x10, float) = x;
    FIELD(output, 0x18, float) = z;
    FIELD(output, 0x14, float) = y;
}

void func_002C2300(const void *first, const void *second, void *output, float time)
{
    float square = time * time;
    float span = (FIELD(second, 0, float) - FIELD(first, 0, float)) * THIRD;
    float remaining = 1.0f - time;
    float left = FIELD(first, 12, float);
    float right = FIELD(second, 12, float);
    float left_control = span * FIELD(first, 8, float) + left;
    float right_control = span * FIELD(second, 4, float) + right;
    float value = remaining * left;
    value += (time * 3.0f) * left_control;
    value *= remaining;
    value += (square * 3.0f) * right_control;
    value *= remaining;
    FIELD(output, 0, float) = value + (time * square) * right;
}

#define PROJECTION_ENTRY(name, offset) \
void name(const GeorgeMathVec3 *reference, const void *first, const void *second, \
          float *distance, float *position) \
{ \
    float left, right, fraction; \
    *distance = func_0029CF28((GeorgeMathVec3 *)ADDRESS(first, offset), \
                             (GeorgeMathVec3 *)ADDRESS(second, offset), reference, position); \
    left = FIELD(first, 0, float); \
    right = FIELD(second, 0, float); \
    fraction = *position; \
    *position = left + fraction * (right - left); \
}
PROJECTION_ENTRY(func_002C23C8, 0x1C)
PROJECTION_ENTRY(func_002C2440, 4)
#undef PROJECTION_ENTRY

#undef CALLBACK_INLINE
#undef PACK8_SCALE
#undef PACK16_SCALE
#undef THIRD
#undef FIELD
#undef ADDRESS
