#ifndef GEORGE_GEOMETRY_H
#define GEORGE_GEOMETRY_H

#include "george/vector_math.h"

typedef struct GeorgeGeometryFrame { GeorgeMathVec4 axis[3], position; } GeorgeGeometryFrame;
typedef struct GeorgeGeometryFace {
    GeorgeMathVec3 normal;
    float distance;
    GeorgeMathVec3 vertex;
} GeorgeGeometryFace;
typedef struct GeorgeGeometryPyramid { GeorgeGeometryFace faces[5]; } GeorgeGeometryPyramid;
typedef char geometry_frame_position[(offsetof(GeorgeGeometryFrame, position)==0x30)?1:-1];
typedef char geometry_face_vertex[(offsetof(GeorgeGeometryFace, vertex)==0x10)?1:-1];
typedef char geometry_face_stride[(sizeof(GeorgeGeometryFace)==0x1C)?1:-1];
typedef char geometry_pyramid_size[(sizeof(GeorgeGeometryPyramid)==0x8C)?1:-1];

void func_002A3640(GeorgeGeometryPyramid *output, const GeorgeGeometryFrame *frame,
    float depth, float width, float height) GEORGE_SAVE128;

#endif
