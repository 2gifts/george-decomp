#ifndef GEORGE_PATH_CURVES_H
#define GEORGE_PATH_CURVES_H

#include "george/path_sampling.h"
#include "george/vector_math.h"

/* Only the observed record prefix and numeric callback contracts are known.
 * No format-capacity guard or original class identity is inferred here. */
typedef void (*GeorgePathCurveCallback)(const void *, const void *, void *, float);
typedef void (*GeorgePathProjectionCallback)(const GeorgeMathVec3 *,
                                              const void *, const void *,
                                              float *, float *);
typedef struct GeorgePathRay {
    GeorgeMathVec3 field00, field0C;
    float field18;
} GeorgePathRay;
typedef char path_ray_size[(sizeof(GeorgePathRay) == 28) ? 1 : -1];

u32 func_00295EB8(void *, u32, float *, float *, void **, void **) GEORGE_SAVE128;
u32 func_00296560(void *, const GeorgeMathVec4 *, float *, float *) GEORGE_SAVE128;
u32 func_002966F0(void *, const GeorgeMathVec3 *, u16 *, s32,
                 float *, float *) GEORGE_SAVE128;
float func_00297178(void *, u16 *, float *, GeorgeMathVec3 *) GEORGE_SAVE128;
float func_00297208(void *, u16 *, void *, float) GEORGE_SAVE128;
float func_00297290(void *, u16 *, void *, float) GEORGE_SAVE128;
float func_00297318(void *, u16 *, GeorgeMathVec3 *, float) GEORGE_SAVE128;

#endif
