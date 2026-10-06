#ifndef GEORGE_CAMERA_MOTION_H
#define GEORGE_CAMERA_MOTION_H

#include "george/input_state.h"
#include "george/matrix_scalar.h"
#include "george/vector_math.h"
#include "george/ee_math.h"

/* Only the transform prefix touched by these controllers is established.
 * The original transform constructor allocates 0x68 bytes. */
typedef struct GeorgeCameraMotionTransform {
    u32 field00;
    GeorgeMathVec3 field04;
    u8 unknown10[0x20];
    float field30, field34, field38;
} GeorgeCameraMotionTransform;

typedef struct GeorgeCameraMotion {
    s32 mode;
    GeorgeCameraMotionTransform *transform;
    GeorgeInputState *input;
} GeorgeCameraMotion;

#define CAMERA_MOTION_OFFSET(type, field, offset) \
    typedef char camera_motion_##type##_##field[(offsetof(type,field) == (offset)) ? 1 : -1]
CAMERA_MOTION_OFFSET(GeorgeCameraMotionTransform,field04,4);
CAMERA_MOTION_OFFSET(GeorgeCameraMotionTransform,field30,0x30);
CAMERA_MOTION_OFFSET(GeorgeCameraMotionTransform,field34,0x34);
CAMERA_MOTION_OFFSET(GeorgeCameraMotionTransform,field38,0x38);
CAMERA_MOTION_OFFSET(GeorgeCameraMotion,transform,4);
CAMERA_MOTION_OFFSET(GeorgeCameraMotion,input,8);
typedef char camera_motion_size[(sizeof(GeorgeCameraMotion) == 12) ? 1 : -1];
#undef CAMERA_MOTION_OFFSET

void func_002B59F8(GeorgeCameraMotion *motion, float elapsed) GEORGE_SAVE128;
void func_002B5D08(GeorgeCameraMotion *motion, float elapsed) GEORGE_SAVE128;
GeorgeCameraMotion *func_002B5EA8(GeorgeCameraMotionTransform *transform,
                                GeorgeInputState *input) GEORGE_SAVE128;
void func_002B5F10(GeorgeCameraMotion *motion, s32 mode);
void func_002B5F18(GeorgeCameraMotion *motion, float elapsed);

/* Complete original 0029A308 passes its third argument to matrix construction
 * and its second to affine inversion; the fallback 00299D68 likewise writes
 * the third argument as the forward matrix and second as its inverse. */
void func_0029A308(GeorgeCameraMotionTransform *transform,
                  GeorgeRotationMatrix *inverse, GeorgeRotationMatrix *forward);

#endif
