#ifndef GEORGE_GOAL_METHODS5_H
#define GEORGE_GOAL_METHODS5_H

#include "george/goal_methods3.h"

/* Numeric storage views, established from accesses rather than class names. */
typedef union GeorgeGoalTargetValue {
    float angle;
    GeorgeGoalReferencedObject *reference;
    GeorgeMathVec3 position;
} GeorgeGoalTargetValue;

typedef struct GeorgeGoalAngleTarget {
    GeorgeGoalBase base;
    u32 field10;
    GeorgeGoalTargetValue field14;
    float field20, field24;
    u32 field28;
} GeorgeGoalAngleTarget;

typedef struct GeorgeGoalReferenceCandidates {
    GeorgeGoalBase base;
    GeorgeMathVec3 field10[5];
    s32 field4C[5];
    float field60;
    GeorgeGoalReferencedObject *field64;
    GeorgeMathVec3 field68;
} GeorgeGoalReferenceCandidates;

typedef struct GeorgeGoalEndpoint {
    GeorgeGoalBase base;
    s16 field10, field12;
    GeorgeGoalReferencedObject *field14;
    GeorgeMathVec3 field18, field24;
} GeorgeGoalEndpoint;

typedef struct GeorgeGoalRouteAction {
    GeorgeGoalBase base;
    u8 field10, field11;
    u8 unknown12[2];
    float field14;
    u32 unknown18;
    u32 field1C, field20;
    u8 field24;
    u8 unknown25[3];
    u32 field28;
    s32 field2C;
    u32 unknown30;
    GeorgeGoalMotion field34;
    GeorgeMathVec3 field5C, field68;
} GeorgeGoalRouteAction;

typedef struct GeorgeGoalRay {
    GeorgeMathVec3 field00, field0C;
    float field18;
} GeorgeGoalRay;

#define GM5_OFFSET(type, member, offset) \
    typedef char gm5_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
GM5_OFFSET(GeorgeGoalAngleTarget, field14, 0x14);
GM5_OFFSET(GeorgeGoalAngleTarget, field28, 0x28);
GM5_OFFSET(GeorgeGoalReferenceCandidates, field4C, 0x4C);
GM5_OFFSET(GeorgeGoalReferenceCandidates, field60, 0x60);
GM5_OFFSET(GeorgeGoalReferenceCandidates, field68, 0x68);
GM5_OFFSET(GeorgeGoalEndpoint, field24, 0x24);
GM5_OFFSET(GeorgeGoalRouteAction, field34, 0x34);
GM5_OFFSET(GeorgeGoalRouteAction, field5C, 0x5C);
GM5_OFFSET(GeorgeGoalRouteAction, field68, 0x68);
GM5_OFFSET(GeorgeGoalRay, field18, 0x18);
typedef char gm5_angle_size[(sizeof(GeorgeGoalAngleTarget) == 0x2C) ? 1 : -1];
typedef char gm5_candidates_size[(sizeof(GeorgeGoalReferenceCandidates) == 0x74) ? 1 : -1];
typedef char gm5_endpoint_size[(sizeof(GeorgeGoalEndpoint) == 0x30) ? 1 : -1];
typedef char gm5_route_size[(sizeof(GeorgeGoalRouteAction) == 0x74) ? 1 : -1];
#undef GM5_OFFSET

GeorgeGameplayGoal *func_001E56E8(void *storage, void *owner, float angle,
                                 float value20, float value24) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E57C8(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object,
                                 float value20, float value24) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E5858(void *storage, void *owner,
                                 const GeorgeMathVec3 *position,
                                 float value20, float value24) GEORGE_SAVE128;
void func_001E58F8(GeorgeGoalAngleTarget *goal, u32 flags) GEORGE_SAVE128;
s32 func_001E5C48(GeorgeGoalReferenceCandidates *goal) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E6318(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object,
                                 float value) GEORGE_SAVE128;
void func_001E6378(GeorgeGoalReferenceCandidates *goal, u32 flags) GEORGE_SAVE128;
void func_001E63D0(GeorgeGoalReferenceCandidates *goal, GeorgeGoalOutput *output);
s32 func_001E6400(GeorgeGoalEndpoint *goal) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E6688(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object, s32 value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E6700(void *storage, void *owner,
                                 const GeorgeMathVec3 *position, s32 value) GEORGE_SAVE128;
void func_001E6778(GeorgeGoalEndpoint *goal, u32 flags) GEORGE_SAVE128;
void func_001E67D0(GeorgeGoalEndpoint *goal, GeorgeGoalOutput *output);
void func_001E6908(GeorgeGoalRouteAction *goal) GEORGE_SAVE128;
u32 func_001E6DA0(GeorgeGoalRouteAction *goal) GEORGE_SAVE128;
void func_001E6ED8(GeorgeGoalRouteAction *goal, GeorgeGoalRoadEntry *record) GEORGE_SAVE128;
void func_001E71E0(GeorgeGoalRouteAction *goal) GEORGE_SAVE128;
void func_001E73C8(GeorgeGoalRouteAction *goal) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E7520(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object,
                                 float value, float time) GEORGE_SAVE128;
void func_001E75F8(GeorgeGoalRouteAction *goal) GEORGE_SAVE128;
s32 func_001CFBF8(void *output, u32 word, u32 index) GEORGE_SAVE128;

#endif
