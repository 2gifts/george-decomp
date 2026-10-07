#ifndef GEORGE_PLANE_INTERSECTION_H
#define GEORGE_PLANE_INTERSECTION_H

#include "george/vector_math.h"

/* Observed 16-byte planes use XYZ as the supplied normal and W as offset.
 * No unit-normal or original class identity is assumed. Outputs are XYZ.
 * The first entry publishes a point and that point plus the raw cross normal;
 * the second entry intersects the infinite line defined by two endpoints. */
u32 func_0029F370(const GeorgeMathVec4 *, const GeorgeMathVec4 *,
                  GeorgeMathVec3 *, GeorgeMathVec3 *) GEORGE_SAVE128;
u32 func_0029F630(const GeorgeMathVec3 *, const GeorgeMathVec3 *,
                  const GeorgeMathVec4 *, GeorgeMathVec3 *) GEORGE_SAVE128;

#endif
