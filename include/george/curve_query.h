#ifndef GEORGE_CURVE_QUERY_H
#define GEORGE_CURVE_QUERY_H

#include "george/path_curves.h"

/* Original numeric entries; the optional fraction may overlap any input.
 * The ray direction is used verbatim. No normalization/length guard exists. */
float func_0029CF28(const GeorgeMathVec3 *, const GeorgeMathVec3 *,
                    const GeorgeMathVec3 *, float *);
u32 func_0029DD60(const GeorgeMathVec4 *, const GeorgePathRay *);

#endif
