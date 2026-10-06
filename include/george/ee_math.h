#ifndef GEORGE_EE_MATH_H
#define GEORGE_EE_MATH_H

#include "george/types.h"

/* EE MIN.S/MAX.S select signed encodings, reversing their ordering when both
 * operands are negative. This value model preserves signed zero and nonfinite
 * encodings, but does not model FCR cause flags. See docs/pad_input.md and the
 * pinned public PCSX2 fp_min/fp_max evidence cited there. */
static __inline__ float george_ee_minimum(float first, float second)
{
    union { float scalar; s32 signed_bits; u32 bits; } left, right;
    left.scalar = first;
    right.scalar = second;
    if ((left.bits & right.bits & 0x80000000U) != 0)
        return left.signed_bits > right.signed_bits ? first : second;
    return left.signed_bits < right.signed_bits ? first : second;
}

static __inline__ float george_ee_maximum(float first, float second)
{
    union { float scalar; s32 signed_bits; u32 bits; } left, right;
    left.scalar = first;
    right.scalar = second;
    if ((left.bits & right.bits & 0x80000000U) != 0)
        return left.signed_bits < right.signed_bits ? first : second;
    return left.signed_bits > right.signed_bits ? first : second;
}

/* R5900 SQRT.S consumes ft, unlike the standard MIPS fs encoding. This narrow
 * primitive also preserves the hardware operation without a libm/errno path.
 * GNU's sqrt builtin adds such a path unless global fast-math is enabled.
 * Native builds provide only a host arithmetic model for finite test cases. */
static __inline__ float george_ee_square_root(float value)
{
#if defined(__mips__) || defined(__mips) || defined(R5900) || defined(_R5900) || defined(__R5900__)
    float result;
    __asm__("sqrt.s %0, %1" : "=f" (result) : "f" (value));
    return result;
#else
    return __builtin_sqrtf(value);
#endif
}

/* RSQRT.S divides fs by sqrt(ft) as one EE operation. Keep its target
 * instruction instead of a libm call or a pair of target operations. */
static __inline__ float george_ee_reciprocal_square_root(float numerator,
                                                       float radicand)
{
#if defined(__mips__) || defined(__mips) || defined(R5900) || defined(_R5900) || defined(__R5900__)
    float result;
    __asm__("rsqrt.s %0, %1, %2" : "=f" (result) : "f" (numerator), "f" (radicand));
    return result;
#else
    return numerator / __builtin_sqrtf(radicand);
#endif
}

#endif
