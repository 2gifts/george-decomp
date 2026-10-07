#include "george/matrix_basis.h"

#if __GNUC__ >= 3
#define BASIS_INLINE static __inline__ __attribute__((always_inline))
#else
#define BASIS_INLINE static __inline__
#endif

/* Ordinary nominal float expressions retain each original multiplicand and
 * broadcast role. Distinct products and grouped diagonal subtractions are
 * deliberate; this source does not assert the VU's actual arithmetic bits. */
BASIS_INLINE void publish_basis(GeorgeRotationMatrix *output,
                                const GeorgeMathVec4 *coefficients)
{
    float w = coefficients->w;
    float y = coefficients->y;
    float z = coefficients->z;
    float x = coefficients->x;
    float dx = x + x, dy = y + y, dz = z + z, dw = w + w;
    float xw = x * dw, yw = y * dw, zw = z * dw;
    float xx = x * dx, yx = y * dx, zx = z * dx;
    float xy = x * dy, yy = y * dy, zy = z * dy;
    float xz = x * dz, yz = y * dz, zz = z * dz;
    float row01 = yx - zw;
    float row10 = xy + zw;
    float row20 = xz - yw;
    float row00 = 1.0f - yy;
    float row11 = 1.0f - xx;
    float row22 = 1.0f - xx;
    float row02 = zx + yw;
    float row12 = zy - xw;
    float row21 = yz + xw;
    row00 = row00 - zz;
    row11 = row11 - zz;
    row22 = row22 - yy;

    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
    output->element[15] = 1.0f;
    output->element[0] = row00;
    output->element[1] = row01;
    output->element[2] = row02;
    output->element[3] = 0.0f;
    output->element[4] = row10;
    output->element[5] = row11;
    output->element[6] = row12;
    output->element[7] = 0.0f;
    output->element[8] = row20;
    output->element[9] = row21;
    output->element[10] = row22;
    output->element[11] = 0.0f;
}

void func_002A1E78(GeorgeRotationMatrix *output,
                   const GeorgeMathVec4 *coefficients)
{
    publish_basis(output, coefficients);
}

void func_002A1F18(GeorgeRotationMatrix *output,
                   const GeorgeMathVec4 *coefficients,
                   const GeorgeMathVec3 *position)
{
    float captured_z;
    publish_basis(output, coefficients);
    output->element[12] = position->x;
    output->element[13] = position->y;
    captured_z = position->z;
    output->element[15] = 1.0f;
    output->element[14] = captured_z;
}

#undef BASIS_INLINE
