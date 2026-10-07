#ifndef GEORGE_MATRIX_TO_QUATERNION_H
#define GEORGE_MATRIX_TO_QUATERNION_H

#include "george/rotation.h"

/* Undefined writable numeric binding only. Three observed initialized words
 * do not establish the full declared object extent or a runtime invariant.
 */
extern s32 D_003FC940[];

/* A distinct effects-only convention for the consumed 16/64-byte prefixes.
 * Original class/prototype/return identity and old numeric ISO-C interface
 * unification remain unresolved. Indexed views and shifted aliases are an
 * explicitly bounded GNU initialized-scalar contract, not general portability.
 */
void george_matrix_to_quaternion(GeorgeMathVec4 *output,
                                 const GeorgeRotationMatrix *matrix);

#endif
