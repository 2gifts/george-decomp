#ifndef GEORGE_ACTOR_CONTROLS_H
#define GEORGE_ACTOR_CONTROLS_H

#include "george/actor_states8.h"

/* Complete numerical entries; only observed call signatures are asserted. */
void func_00185EA0(GeorgeGoalEntity *, void *, u32, u32, const GeorgeMathVec3 *) GEORGE_SAVE128;
void func_0018D8A0(GeorgeGoalEntity *, const GeorgeMathVec3 *) GEORGE_SAVE128;
s32 func_0018B710(GeorgeGoalEntity *, const GeorgeMathVec3 *, const GeorgeMathVec3 *) GEORGE_SAVE128;
void func_0018FD30(GeorgeGoalEntity *, GeorgeActorBits64) GEORGE_SAVE128;
void func_0018FD40(GeorgeGoalEntity *, GeorgeActorBits64) GEORGE_SAVE128;
void func_00190E00(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00190EB0(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00190F70(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_001911F0(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_001912C8(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_001913A0(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_001913E0(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00191670(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00191D08(GeorgeGoalEntity *) GEORGE_SAVE128;
void func_00191DC8(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
void func_00191E50(GeorgeGoalEntity *) GEORGE_SAVE128;
GeorgeRotationMatrix *func_00192748(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
GeorgeRotationMatrix *func_00192818(GeorgeGoalEntity *, u32) GEORGE_SAVE128;
s32 func_00192C58(GeorgeGoalEntity *, u32, GeorgeActorRequestCallback,
                  GeorgeGoalEntity *, float) GEORGE_SAVE128;

#endif
