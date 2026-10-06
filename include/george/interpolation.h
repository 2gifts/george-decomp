#ifndef GEORGE_INTERPOLATION_H
#define GEORGE_INTERPOLATION_H

#include "george/types.h"

/* Each record contains its key, followed by component_count floats. */
void func_002ADC80(s32 key_count, const float *records, s32 component_count,
                   float *output, float key);

#endif
