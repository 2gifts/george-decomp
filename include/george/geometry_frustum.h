#ifndef GEORGE_GEOMETRY_FRUSTUM_H
#define GEORGE_GEOMETRY_FRUSTUM_H

#include "george/geometry.h"

/* Exactly six existing 28-byte face records are written. This numeric view
 * does not establish an original class name or an asset capacity. */
typedef struct GeorgeGeometrySixFaces {
    GeorgeGeometryFace faces[6];
} GeorgeGeometrySixFaces;

void func_002A3DF8(GeorgeGeometrySixFaces *output, const GeorgeGeometryFrame *frame,
                  float near_depth, float far_depth, float width, float height) GEORGE_SAVE128;

typedef char george_six_face_extent[(sizeof(GeorgeGeometrySixFaces) == 168) ? 1 : -1];

#endif
