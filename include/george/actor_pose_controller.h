#ifndef GEORGE_ACTOR_POSE_CONTROLLER_H
#define GEORGE_ACTOR_POSE_CONTROLLER_H

#include "george/camera_transform.h"
#include "george/script_object.h"
#include "george/goal_methods4.h"

/* Observed controller offsets stay opaque. Its +8 points to the independently
 * reviewed camera-transform prefix, rather than an actor entity. */
void *func_00166CA8(void *, const GeorgeRotationMatrix *) GEORGE_SAVE128;
void *func_00166DD0(void *) GEORGE_SAVE128;
void *func_00166EB8(void *, const void *) GEORGE_SAVE128;
void func_00166F58(void *, u32) GEORGE_SAVE128;
void *func_0016B948(void *, const GeorgeRotationMatrix *) GEORGE_SAVE128;
void func_0016BDA0(void *, float) GEORGE_SAVE128;

#endif
