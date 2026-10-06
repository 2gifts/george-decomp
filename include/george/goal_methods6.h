#ifndef GEORGE_GOAL_METHODS6_H
#define GEORGE_GOAL_METHODS6_H

#include "george/goal_methods4.h"

/* Numeric views of accessed storage; original class names remain unresolved. */
typedef struct GeorgeGoalJumpTarget {
    GeorgeGoalBase base;
    GeorgeMathVec3 field10;
    GeorgeGoalReferencedObject *field1C;
    u32 field20;
    float field24, field28, field2C;
    u8 field30, field31;
    u8 unknown32[2];
} GeorgeGoalJumpTarget;

typedef struct GeorgeGoalPair {
    GeorgeGoalBase base;
    GeorgeGoalBase *field10, *field14;
    s16 field18, field1A;
} GeorgeGoalPair;

typedef struct GeorgeGoalVirtualOutput {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *adjusted_this, GeorgeGoalOutput *output);
} GeorgeGoalVirtualOutput;

typedef struct GeorgeGoalVirtualCommand {
    s16 adjustment;
    u16 unknown02;
    s32 (*invoke)(void *adjusted_this, s32 command);
} GeorgeGoalVirtualCommand;

typedef struct GeorgeGoalVirtualCommandVector {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *adjusted_this, u32 command,
                   GeorgeMathVec3 *output, GeorgeMathVec3 *scratch);
} GeorgeGoalVirtualCommandVector;

#define GM6_OFFSET(type, member, offset) \
    typedef char gm6_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
GM6_OFFSET(GeorgeGoalJumpTarget, field1C, 0x1C);
GM6_OFFSET(GeorgeGoalJumpTarget, field24, 0x24);
GM6_OFFSET(GeorgeGoalJumpTarget, field30, 0x30);
GM6_OFFSET(GeorgeGoalPair, field18, 0x18);
GM6_OFFSET(GeorgeGoalVirtualOutput, invoke, 4);
GM6_OFFSET(GeorgeGoalVirtualCommand, invoke, 4);
GM6_OFFSET(GeorgeGoalVirtualCommandVector, invoke, 4);
typedef char gm6_jump_size[(sizeof(GeorgeGoalJumpTarget) == 0x34) ? 1 : -1];
typedef char gm6_pair_size[(sizeof(GeorgeGoalPair) == 0x1C) ? 1 : -1];
#undef GM6_OFFSET

void func_001D9C88(GeorgeGoalBase *goal) GEORGE_SAVE128;
void func_001E7690(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;
void func_001E8988(GeorgeGoalBase *goal) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E8A78(void *storage, void *owner,
    const GeorgeMathVec3 *position, u32 word, float height, float time) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E8B18(void *storage, void *owner,
    GeorgeGoalReferencedObject *object, u32 word, float height, float time) GEORGE_SAVE128;
void func_001E8BA8(GeorgeGoalJumpTarget *goal, u32 flags) GEORGE_SAVE128;
void func_001E8C10(GeorgeGoalJumpTarget *goal) GEORGE_SAVE128;
u32 func_001E8CF8(GeorgeGoalJumpTarget *goal);
GeorgeGameplayGoal *func_001E8EF8(void *storage, void *owner,
    GeorgeGoalBase *first, GeorgeGoalBase *second) GEORGE_SAVE128;
void func_001E8F58(GeorgeGoalPair *goal, u32 flags) GEORGE_SAVE128;
void func_001E8FC0(GeorgeGoalPair *goal, u32 value) GEORGE_SAVE128;
void func_001E9020(GeorgeGoalPair *goal, GeorgeGoalOutput *output) GEORGE_SAVE128;
void func_001E9080(GeorgeGoalPair *goal) GEORGE_SAVE128;
s32 func_001E90D0(GeorgeGoalPair *goal) GEORGE_SAVE128;
void func_001E9168(GeorgeGoalPair *goal) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E91D0(void *storage, void *owner,
    GeorgeGoalBase *first, GeorgeGoalBase *second) GEORGE_SAVE128;
void func_001E9240(GeorgeGoalPair *goal, u32 flags) GEORGE_SAVE128;
void func_001E92B8(GeorgeGoalPair *goal, u32 value) GEORGE_SAVE128;
void func_001E9318(GeorgeGoalPair *goal, GeorgeGoalOutput *output) GEORGE_SAVE128;
void func_001E9378(GeorgeGoalPair *goal) GEORGE_SAVE128;
s32 func_001E93C8(GeorgeGoalPair *goal) GEORGE_SAVE128;
void func_001E9468(GeorgeGoalPair *goal) GEORGE_SAVE128;

#endif
