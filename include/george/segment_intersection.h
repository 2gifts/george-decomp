#ifndef GEORGE_SEGMENT_INTERSECTION_H
#define GEORGE_SEGMENT_INTERSECTION_H

#include "george/path_curves.h"

/* Observed numeric entry contracts. Triangle vertices and optional XYZ output
 * may overlap. A computed point is published before triangle containment is
 * tested; false does not imply the output was left untouched. */
u32 func_0029EB98(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second,
                 const GeorgeMathVec3 *a, const GeorgeMathVec3 *b,
                 const GeorgeMathVec3 *c, GeorgeMathVec3 *output) GEORGE_SAVE128;

/* Plane XYZ normal and W offset; ray direction is used without normalization.
 * The fraction output is mandatory and may overlap either input. */
u32 func_0029F080(const GeorgePathRay *ray, const GeorgeMathVec4 *plane,
                 float *fraction) GEORGE_SAVE128;

#endif
