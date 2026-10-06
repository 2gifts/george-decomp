#ifndef GEORGE_ACTOR_CONSTRUCTION_H
#define GEORGE_ACTOR_CONSTRUCTION_H

#include "george/actor_core.h"

/* The +00 table view is shared with the reviewed actor control object.
 * The control's +1C context has a separate layout from actor +18 data. */
void func_0016CF30(GeorgeActorControlObject *, const GeorgeMathVec3 *) GEORGE_SAVE128;
void func_0016EA40(void *, u32) GEORGE_SAVE128;
void func_0016EE80(void *, u32) GEORGE_SAVE128;
GeorgeGoalEntity *func_0016EEB0(GeorgeGoalEntity *) GEORGE_SAVE128;

/* Eight integer-register arguments, seven EABI64 stack slots, separate f12.
 * Numerical argument roles retain the widths observed in the complete body. */
void func_0016EF18(GeorgeGoalEntity *, const GeorgeMathVec3 *, GeorgeGoalEntityData *,
                  u32, u32, u32, u32, void *, s32, u32, void *, u32, u32, u32,
                  u32, float) GEORGE_SAVE128;

#endif
