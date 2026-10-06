#ifndef GEORGE_MATRIX_RIGID_H
#define GEORGE_MATRIX_RIGID_H

#include "george/rotation.h"

/* Transpose the basis and negate its translation products. The original
 * performs sequential loads/stores; overlapping matrices stay live. */
void func_002A1098(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input);

#endif
