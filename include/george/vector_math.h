#ifndef GEORGE_VECTOR_MATH_H
#define GEORGE_VECTOR_MATH_H

#include "george/math_helpers.h"
#include "george/compiler.h"

void func_002A3080(GeorgeMathVec4 *output, const GeorgeMathVec4 *input) GEORGE_SAVE128;
void func_002A3138(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right) GEORGE_SAVE128;
void func_002A3220(GeorgeMathVec4 *output, const GeorgeMathVec3 *input) GEORGE_SAVE128;
void func_002A32E0(GeorgeMathVec4 *value, const GeorgeMathVec4 *reference);
void func_002A3390(GeorgeMathVec3 *output, const GeorgeMathVec3 *current, const GeorgeMathVec3 *target, float blend) GEORGE_SAVE128;
float func_002A3538(GeorgeMathVec3 *value);
float func_002A35C0(GeorgeMathVec3 *output, const GeorgeMathVec3 *input, float scale);

#endif
