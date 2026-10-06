#ifndef GEORGE_CAMERA_BASIS_H
#define GEORGE_CAMERA_BASIS_H

#include "george/camera_transform.h"

s32 func_00299BE0(GeorgeCameraMotionTransform *transform) GEORGE_SAVE128;
void func_00299D68(GeorgeCameraMotionTransform *transform,
                  GeorgeRotationMatrix *inverse,
                  GeorgeRotationMatrix *forward) GEORGE_SAVE128;

#endif
