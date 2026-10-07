#ifndef GEORGE_MATRIX_BASIS_H
#define GEORGE_MATRIX_BASIS_H

#include "george/rotation.h"

/* Compatible with the existing camera and property caller declarations.
 * The source preserves captured coefficients and live position reads through
 * overlapping storage. No original class, normalization, incidental return
 * lanes or general EE/VU arithmetic identity is established. */
void func_002A1E78(GeorgeRotationMatrix *output,
                   const GeorgeMathVec4 *coefficients);
void func_002A1F18(GeorgeRotationMatrix *output,
                   const GeorgeMathVec4 *coefficients,
                   const GeorgeMathVec3 *position);

#endif
