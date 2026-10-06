#ifndef GEORGE_CAMERA_TRANSFORM_H
#define GEORGE_CAMERA_TRANSFORM_H

#include "george/camera_motion.h"
#include "george/geometry_bounds.h"

/* Established scalar view of the previously opaque +10..+2F prefix. Keep the
 * reviewed controller prefix unchanged; the union describes the same storage. */
typedef struct GeorgeCameraPose {
    u32 field00;
    GeorgeMathVec3 field04, field10;
    GeorgeMathVec4 field1C;
    float field2C, field30, field34, field38;
} GeorgeCameraPose;

typedef struct GeorgeCameraTransform {
    union { GeorgeCameraMotionTransform prefix; GeorgeCameraPose fields; } pose;
    float field3C, field40, field44, field48;
    u32 field4C, field50;
    GeorgeMathVec3 field54;
    u32 field60, field64;
} GeorgeCameraTransform;

#define CAMERA_TRANSFORM_OFFSET(type, field, offset) \
    typedef char camera_transform_##type##_##field[(offsetof(type,field)==(offset))?1:-1]
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field10,0x10);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field1C,0x1C);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field2C,0x2C);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field30,0x30);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field34,0x34);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraPose,field38,0x38);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraTransform,field3C,0x3C);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraTransform,field48,0x48);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraTransform,field54,0x54);
CAMERA_TRANSFORM_OFFSET(GeorgeCameraTransform,field64,0x64);
typedef char camera_transform_pose_size[(sizeof(GeorgeCameraPose)==0x3C)?1:-1];
typedef char camera_transform_size[(sizeof(GeorgeCameraTransform)==0x68)?1:-1];
#undef CAMERA_TRANSFORM_OFFSET

GeorgeCameraTransform *func_0029A100(float x,float y,float z) GEORGE_SAVE128;
void func_0029A188(GeorgeCameraTransform *transform,float x,float y,float z) GEORGE_SAVE128;
void func_0029A3A0(const GeorgeCameraTransform *transform,GeorgeRotationMatrix *output) GEORGE_SAVE128;
void func_0029A498(GeorgeCameraTransform *transform,const GeorgeMathVec3 *delta);
/* Same proven prefix-pointer/matrix-pair ABI, with the original wide saves
 * enabled by the established candidate compiler profile for this definition. */
void func_0029A308(GeorgeCameraMotionTransform *transform,
                  GeorgeRotationMatrix *inverse,GeorgeRotationMatrix *forward) GEORGE_SAVE128;

#endif
