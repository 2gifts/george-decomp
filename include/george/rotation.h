#ifndef GEORGE_ROTATION_H
#define GEORGE_ROTATION_H

#include "george/vector_math.h"

typedef struct GeorgeRotationMatrix { float element[16]; } GeorgeRotationMatrix;

void func_002A2658(GeorgeRotationMatrix *output, const GeorgeMathVec3 *input);
void func_002A26C8(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right);
void func_002A2780(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right, float blend) GEORGE_SAVE128;
void func_002A2990(GeorgeMathVec4 *output, const GeorgeMathVec4 *previous, const GeorgeMathVec4 *current, const GeorgeMathVec4 *next) GEORGE_SAVE128;
void func_002A2C48(const GeorgeMathVec4 *rotation, const GeorgeMathVec3 *input, GeorgeMathVec3 *output) GEORGE_SAVE128;
void func_002A2CF0(GeorgeMathVec4 *output, const GeorgeMathVec4 *axis_angle) GEORGE_SAVE128;
void func_002A2D70(GeorgeMathVec4 *output, float angle) GEORGE_SAVE128;
void func_002A2DC8(GeorgeMathVec4 *output, float angle) GEORGE_SAVE128;
void func_002A2E20(GeorgeMathVec4 *output, float angle) GEORGE_SAVE128;
void func_002A2E78(GeorgeMathVec4 *output, float first, float second, float third) GEORGE_SAVE128;
void func_002A2F68(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right, float blend);
void func_002A2FE0(GeorgeMathVec4 *output, const GeorgeMathVec4 *first_left, const GeorgeMathVec4 *second_left, const GeorgeMathVec4 *second_right, const GeorgeMathVec4 *first_right, float blend) GEORGE_SAVE128;

#endif
