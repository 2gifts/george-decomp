#ifndef GEORGE_VECTOR_TRANSFORM_H
#define GEORGE_VECTOR_TRANSFORM_H

#include "george/rotation.h"

/* Numeric original entry names. a0 addresses 16-byte aligned matrix rows,
 * a1/a2 address three scalar components. The point entry reads four rows;
 * the direction entry reads three. Exactly Z, X, Y are written to a2.
 * Ordinary C represents the reviewed finite normal/zero value model, not
 * VU extended accumulator precision, flags or exceptional-value behavior.
 * Existing reviewed callers use the output memory and ignore f0. */
void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);
void func_002A1D78(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);

#endif
