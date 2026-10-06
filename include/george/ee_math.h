#ifndef GEORGE_EE_MATH_H
#define GEORGE_EE_MATH_H

#include "george/types.h"

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

#endif
