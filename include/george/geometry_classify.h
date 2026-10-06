#ifndef GEORGE_GEOMETRY_CLASSIFY_H
#define GEORGE_GEOMETRY_CLASSIFY_H

#include "george/geometry_bounds.h"

/* The routines consume five or six existing 28-byte face records. */
u32 func_002A3CA0(const GeorgeGeometryFace *faces, const GeorgeBounds *bounds);
u32 func_002A4520(const GeorgeGeometryFace *faces, const GeorgeBounds *bounds);
u32 func_002A4678(const GeorgeGeometryFace *faces, const GeorgeGeometryFrame *frame) GEORGE_SAVE128;
u32 func_002A4760(const GeorgeGeometryFace *faces, const GeorgeMathVec4 *sphere);
u32 func_0029E098(const GeorgeGeometryFrame *frame, const GeorgeGeometryFace *face) GEORGE_SAVE128;

#endif
