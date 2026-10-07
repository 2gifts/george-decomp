#include "george/matrix_kernels.h"

#if __GNUC__ >= 3
#define MATRIX_INLINE static __inline__ __attribute__((always_inline))
#else
#define MATRIX_INLINE static __inline__
#endif

/* Adapt the complete published scalar basis_component expression with a
 * fourth captured homogeneous coefficient. Each product precedes its
 * left-associated add; this is ordinary C, not a VU precision assertion. */
MATRIX_INLINE float component4(const float *matrix, unsigned component,
                               float x, float y, float z, float w)
{
    float result = matrix[component] * x;
    float product = matrix[component + 4U] * y;
    result = result + product;
    product = matrix[component + 8U] * z;
    result = result + product;
    product = matrix[component + 12U] * w;
    return result + product;
}

void func_002A1DF0(const float matrix[16], const float input[4], float output[4])
{
    float w = input[3];
    float y = input[1];
    float z = input[2];
    float x = input[0];
    float captured[16];
    float result[4];
    unsigned i;
    for (i = 0; i < 16; ++i)
        captured[i] = matrix[i];
    for (i = 0; i < 4; ++i)
        result[i] = component4(captured, i, x, y, z, w);
    output[3] = result[3];
    output[0] = result[0];
    output[1] = result[1];
    output[2] = result[2];
}

void func_002A2200(float output[16], const float left[16], const float right[16])
{
    float captured_left[16];
    float captured_right[16];
    float result[16];
    unsigned i, j;
    for (i = 0; i < 16; ++i) {
        captured_left[i] = left[i];
        captured_right[i] = right[i];
    }
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            result[4U * i + j] = component4(captured_right, j,
                captured_left[4U * i], captured_left[4U * i + 1U],
                captured_left[4U * i + 2U], captured_left[4U * i + 3U]);
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            output[4U * i + j] = result[4U * i + j];
}

#undef MATRIX_INLINE
