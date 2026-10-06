#ifndef GEORGE_GEOMETRY_BOUNDS_H
#define GEORGE_GEOMETRY_BOUNDS_H

#include "george/geometry.h"
#include "george/rotation.h"

/* Storage views for the observed six floats and count/16-byte plane records. */
typedef struct GeorgeBounds { GeorgeMathVec3 lower, upper; } GeorgeBounds;
typedef struct GeorgePlaneArray { u32 count; GeorgeMathVec4 plane[1]; } GeorgePlaneArray;
typedef char bounds_upper_offset[(offsetof(GeorgeBounds, upper)==12)?1:-1];
typedef char bounds_size[(sizeof(GeorgeBounds)==24)?1:-1];
typedef char plane_array_offset[(offsetof(GeorgePlaneArray, plane)==4)?1:-1];

void func_0029E7F0(GeorgeGeometryFrame *output, const GeorgeMathVec3 *lower,
                  const GeorgeMathVec3 *upper) GEORGE_SAVE128;
void func_002A0048(GeorgeBounds *bounds, float amount);
u32 func_002A00A0(const GeorgeBounds *bounds, const GeorgeMathVec3 *point);
u32 func_002A0138(const GeorgeBounds *left, const GeorgeBounds *right);
u32 func_002A01D8(void);
GeorgePlaneArray *func_002A01E0(u32 count) GEORGE_SAVE128;
GeorgeMathVec4 *func_002A0238(GeorgePlaneArray *array, u32 index);
u32 func_002A0248(const GeorgePlaneArray *planes, u32 count, const GeorgeMathVec3 *points);
u32 func_002A02E0(const GeorgePlaneArray *planes, const GeorgeMathVec4 *sphere);
void func_002A0370(GeorgeGeometryFrame *output, const GeorgeBounds *bounds);
u32 func_002A0390(const GeorgeMathVec3 *point, const GeorgeMathVec4 *planes, s32 count);
u32 func_002A0410(const GeorgeMathVec3 *point, const GeorgeMathVec4 *planes,
                  s32 count, float *closest, float tolerance) GEORGE_SAVE128;
void func_002A0E20(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input);

#endif
