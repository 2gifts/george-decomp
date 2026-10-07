#ifndef GEORGE_SEGMENT_DISTANCE_H
#define GEORGE_SEGMENT_DISTANCE_H

#include "george/vector_math.h"

/* Origin plus unnormalized displacement describes each segment. Nullable
 * fraction outputs may overlap each other or any input; output zero precedes
 * output one. The original adds no finite/degeneracy guard. */
float func_0029D5D0(const GeorgeMathVec3 *origin0,
                    const GeorgeMathVec3 *direction0,
                    const GeorgeMathVec3 *origin1,
                    const GeorgeMathVec3 *direction1,
                    float *fraction0, float *fraction1) GEORGE_SAVE128;

#endif
