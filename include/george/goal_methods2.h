#ifndef GEORGE_GOAL_METHODS2_H
#define GEORGE_GOAL_METHODS2_H

#include "george/goals.h"

/* Observed GCC member-pointer encoding: negative selector means direct call;
 * positive selector indexes an 8-byte adjusted-this/function virtual pair. */
typedef struct GeorgeGoalMember {
    s16 adjustment;
    s16 selector;
    union {
        void (*direct)(void *adjusted_this);
        s16 vtable_offset;
        u32 bits;
    } target;
} GeorgeGoalMember;

typedef struct GeorgeGoalVirtualFloat {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *adjusted_this, float value);
} GeorgeGoalVirtualFloat;

typedef struct GeorgeGoalVirtualVoid {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *adjusted_this);
} GeorgeGoalVirtualVoid;

typedef struct GeorgeGoalVehicle {
    u32 unknown00;
    const u8 *field04;
    u8 unknown08[0x10];
    float field18;
    u8 unknown1C[0x1C];
    GeorgeGoalVirtualObject *field38;
} GeorgeGoalVehicle;

typedef struct GeorgeGoalRouteResult {
    u8 unknown00[0x40];
    u32 field40;
} GeorgeGoalRouteResult;

typedef struct GeorgeGoalRouteWord {
    u32 field00;
} GeorgeGoalRouteWord;

typedef char goal_member_stride[(sizeof(GeorgeGoalMember) == 8) ? 1 : -1];
typedef char goal_member_target_offset[(offsetof(GeorgeGoalMember, target) == 4) ? 1 : -1];
typedef char goal_vehicle_scalar_offset[(offsetof(GeorgeGoalVehicle, field18) == 0x18) ? 1 : -1];
typedef char goal_vehicle_horn_offset[(offsetof(GeorgeGoalVehicle, field38) == 0x38) ? 1 : -1];
typedef char goal_route_result_offset[(offsetof(GeorgeGoalRouteResult, field40) == 0x40) ? 1 : -1];

u32 func_001D99F8(GeorgeGoalTimedAction *goal);
u32 func_001D9B78(GeorgeGoalTimedAction *goal);
u32 func_001D9D10(GeorgeGoalLook *goal) GEORGE_SAVE128;
s32 func_001DB818(GeorgeGoalBase *goal) GEORGE_SAVE128;
float func_001DC178(const GeorgeMathVec3 *origin, const GeorgeMathVec3 *target);
s32 func_001DC468(void *context, u32 *word18, u32 *word14, u32 *word20) GEORGE_SAVE128;
s32 func_001DC618(GeorgeGoalRestart *goal);
void func_001DC6B8(GeorgeGoalWords *goal);
s32 func_001DC750(GeorgeGoalWait *goal);
u32 func_001DC988(GeorgeGoalDrive *goal) GEORGE_SAVE128;
u32 func_001DF630(GeorgeGoalWalkIntersection *goal) GEORGE_SAVE128;
u32 func_001DFDB8(GeorgeGoalWalkRoad *goal) GEORGE_SAVE128;
s32 func_001E07E0(GeorgeGoalIdle *goal) GEORGE_SAVE128;
u32 func_001E0FA0(GeorgeGoalTimedAction *goal);
u32 func_001E1188(GeorgeGoalTimedAction *goal);
u32 func_001E1F20(GeorgeGoalWords *goal);
s32 func_001E5A18(GeorgeGoalWords *goal) GEORGE_SAVE128;
s32 func_001E5B60(GeorgeGoalWords *goal);
s32 func_001E6870(GeorgeGoalInteractionPosition *goal) GEORGE_SAVE128;
u32 func_001E82D8(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;
s32 func_001E8900(GeorgeGoalWords *goal);
s32 func_001E89E0(GeorgeGoalBase *goal) GEORGE_SAVE128;
s32 func_001E8DE0(GeorgeGoalWords *goal);
void func_001E8EC0(GeorgeGoalWords *goal);

#endif
