#ifndef GEORGE_ACTOR_CAMERA_STATE_H
#define GEORGE_ACTOR_CAMERA_STATE_H

#include "george/actor_pose_controller.h"
#include "george/deimos.h"
#include "george/vector_math.h"

/* Command strings identify rail-camera use; original class and complete
 * object layouts remain unknown. Numeric fields stay opaque. */
void *func_001670A8(void *, const GeorgeRotationMatrix *) GEORGE_SAVE128;
void func_001671D8(void *, float) GEORGE_SAVE128;
GeorgeDeimosPoolNode *func_001678E0(void) GEORGE_SAVE128;
void func_00167B98(s32, s32) GEORGE_SAVE128;
void func_00167CC0(s32, s32) GEORGE_SAVE128;
void func_00167F98(s32, s32) GEORGE_SAVE128;
void func_00168290(void *);
void func_00168398(s32, s32) GEORGE_SAVE128;
void func_001683F0(s32, s32) GEORGE_SAVE128;
void func_001684D0(s32, s32) GEORGE_SAVE128;
void func_001684F8(s32, s32) GEORGE_SAVE128;
void func_00168550(s32, s32) GEORGE_SAVE128;
void func_001685A8(s32, s32) GEORGE_SAVE128;
void func_00168630(s32, s32) GEORGE_SAVE128;
void func_00168660(s32, s32) GEORGE_SAVE128;

#endif
