#ifndef GEORGE_MATRIX_SCALAR_H
#define GEORGE_MATRIX_SCALAR_H

#include "george/rotation.h"

void func_002A1CE0(const GeorgeRotationMatrix *matrix, const GeorgeMathVec3 *input, GeorgeMathVec3 *output);
void func_002A1FD8(GeorgeRotationMatrix *output, const GeorgeMathVec4 *axis_angle) GEORGE_SAVE128;
void func_002A20D8(GeorgeRotationMatrix *output, const GeorgeMathVec4 *axis_angle, const GeorgeMathVec3 *position) GEORGE_SAVE128;
void func_002A2278(GeorgeRotationMatrix *matrix, float x, float y, float z);
void func_002A2330(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input);
void func_002A2370(GeorgeRotationMatrix *output, float angle) GEORGE_SAVE128;
void func_002A2400(GeorgeRotationMatrix *output, float angle) GEORGE_SAVE128;
void func_002A2490(GeorgeRotationMatrix *output, float angle) GEORGE_SAVE128;
void func_002A2520(GeorgeRotationMatrix *output, float first, float second, float third) GEORGE_SAVE128;

#endif
