#ifndef GEORGE_MATRIX_KERNELS_H
#define GEORGE_MATRIX_KERNELS_H

/* Numeric memory-effect conventions: three pointer GPRs, not an original
 * declaration/class or a claim about incidental GPR/FPR return lanes.
 * Storage consists of initialized flat float arrays. No restrict qualifier:
 * full and shifted overlaps are supported by captures before all stores.
 * General EE/VU ACC precision/flags and exceptional values are unproved. */
void func_002A1DF0(const float matrix[16], const float input[4], float output[4]);
void func_002A2200(float output[16], const float left[16], const float right[16]);

#endif
