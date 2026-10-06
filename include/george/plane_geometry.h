#ifndef GEORGE_PLANE_GEOMETRY_H
#define GEORGE_PLANE_GEOMETRY_H

#include "george/geometry_bounds.h"
#include "george/vector_math.h"

/* Only observed storage and numerical operations are named. These entries do
 * not establish original C++ classes or an ownership contract for the inputs. */
GeorgeMathVec4 *func_0029C6C0(GeorgeMathVec4 *output,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *middle,
    const GeorgeMathVec3 *last) GEORGE_SAVE128;
GeorgeMathVec3 *func_0029C7B0(GeorgeMathVec3 *output,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *middle,
    const GeorgeMathVec3 *last) GEORGE_SAVE128;
s32 func_0029CA28(const GeorgeMathVec3 *query,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *second,
    float radius) GEORGE_SAVE128;
s32 func_0029CB20(const GeorgeMathVec3 *triangle,
    const GeorgeMathVec3 *query) GEORGE_SAVE128;
float func_0029D048(const GeorgeMathVec3 *origin,
    const GeorgeMathVec3 *direction, const GeorgeMathVec3 *query,
    float *parameter);
void func_0029E448(GeorgeBounds *bounds, const GeorgeMathVec3 *point);
s32 func_0029E720(const GeorgeBounds *bounds, const GeorgeMathVec4 *sphere);

#endif
