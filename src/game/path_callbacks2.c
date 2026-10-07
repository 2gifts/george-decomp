#include "george/path_callbacks2.h"

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define THIRD 0.3333333432674407958984375f
#if __GNUC__ >= 3
#define CURVE_INLINE static __inline__ __attribute__((always_inline))
#else
#define CURVE_INLINE static __inline__
#endif

/* Original MULA/MADDA/MADD order, represented by separate single products and
 * left-associated additions. Finite normal/zero host arithmetic is reviewed;
 * this does not model EE FCR flags, exceptional values or precision quirks. */
CURVE_INLINE float weighted_component(float a, float b, float c, float d,
                                      float wa, float wb, float wc, float wd)
{
    float result = wa * a;
    result += wb * b;
    result += wc * c;
    return result + wd * d;
}

/* The complete original bodies capture all three component results before
 * the first output store, including when output overlaps either endpoint. */
CURVE_INLINE void weighted_vector(const void *first, const void *second,
                                  void *output, u32 left_a, u32 left_b,
                                  u32 right_a, u32 right_b,
                                  float wa, float wb, float wc, float wd)
{
    float z = weighted_component(FIELD(first, left_a + 8U, float),
                                 FIELD(first, left_b + 8U, float),
                                 FIELD(second, right_a + 8U, float),
                                 FIELD(second, right_b + 8U, float), wa, wb, wc, wd);
    float x = weighted_component(FIELD(first, left_a, float),
                                 FIELD(first, left_b, float),
                                 FIELD(second, right_a, float),
                                 FIELD(second, right_b, float), wa, wb, wc, wd);
    float y = weighted_component(FIELD(first, left_a + 4U, float),
                                 FIELD(first, left_b + 4U, float),
                                 FIELD(second, right_a + 4U, float),
                                 FIELD(second, right_b + 4U, float), wa, wb, wc, wd);
    FIELD(output, 8, float) = z;
    FIELD(output, 0, float) = x;
    FIELD(output, 4, float) = y;
}

void func_002C19F0(const void *first, const void *second, void *output, float time)
{
    float square = time * time;
    float remaining = 1.0f - time;
    float cube = time * square;
    float three_square = square * 3.0f;
    float remaining_square = remaining * remaining;
    float left_control = remaining_square * (time * 3.0f);
    float right_control = remaining * three_square;
    float right_weight = right_control + cube;
    float span = (FIELD(second, 0, float) - FIELD(first, 0, float)) * THIRD;
    float left_weight = remaining_square * remaining + left_control;
    left_control *= span;
    right_control *= span;
    weighted_vector(first, second, output, 0x1CU, 0x10U, 4U, 0x1CU,
                    left_weight, left_control, right_control, right_weight);
}

void func_002C1CA0(const void *first, const void *second, void *output, float time)
{
    float square = time * time;
    float remaining = 1.0f - time;
    float left_weight = remaining * remaining;
    float left_control = time * (remaining * 3.0f);
    float right_control = time * (time * 3.0f);
    left_weight *= remaining;
    left_control *= remaining;
    right_control *= remaining;
    weighted_vector(first, second, output, 4U, 0x1CU, 0x10U, 4U,
                    left_weight, left_control, right_control, square * time);
}

void func_002C1D60(const void *first, const void *second, void *output, float time)
{
    float remaining = 1.0f - time;
    float right_weight = time * (time * 3.0f);
    float mixed = time * (remaining * 6.0f);
    float left_square = (remaining * remaining) * 3.0f;
    weighted_vector(first, second, output, 4U, 0x1CU, 0x10U, 4U,
                    -left_square, left_square - mixed,
                    mixed - right_weight, right_weight);
}

/* Both complete wrappers differ only in their endpoint storage offset. */
#define QUATERNION_ENTRY(name, offset) \
void name(const void *first, const void *second, void *output, float time) \
{ \
    func_002A2780((GeorgeMathVec4 *)output, \
                  (const GeorgeMathVec4 *)ADDRESS(first, offset), \
                  (const GeorgeMathVec4 *)ADDRESS(second, offset), time); \
}
QUATERNION_ENTRY(func_002C2180, 4U)
QUATERNION_ENTRY(func_002C2390, 0x18U)
#undef QUATERNION_ENTRY
#undef CURVE_INLINE
#undef THIRD
#undef FIELD
#undef ADDRESS
