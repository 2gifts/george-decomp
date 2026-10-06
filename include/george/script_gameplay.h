#ifndef GEORGE_SCRIPT_GAMEPLAY_H
#define GEORGE_SCRIPT_GAMEPLAY_H

#include "george/deimos.h"
#include "george/math_helpers.h"

/* Intrusive goal links are independently visible in func_001D89D8. */
typedef struct GeorgeGameplayGoal {
    u32 unknown00;
    struct GeorgeGameplayGoal *field04;
    struct GeorgeGameplayGoal *field08;
} GeorgeGameplayGoal;

typedef struct GeorgeGameplayGoalQueue {
    GeorgeGameplayGoal *field00;
    u32 unknown04;
    GeorgeGameplayGoal *field08;
} GeorgeGameplayGoalQueue;

/* Partial script actor layout; the owner subobject at +0x34 is opaque. */
typedef struct GeorgeGameplayActor {
    u8 unknown00[0x34];
    u8 field34[0x114];
    u32 field148;
    u8 unknown14C[0x3C];
    GeorgeGameplayGoalQueue field188;
} GeorgeGameplayActor;

#define GAMEPLAY_OFFSET(type, field, offset) \
    typedef char gameplay_offset_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
GAMEPLAY_OFFSET(GeorgeGameplayGoal, field04, 4);
GAMEPLAY_OFFSET(GeorgeGameplayGoal, field08, 8);
GAMEPLAY_OFFSET(GeorgeGameplayGoalQueue, field08, 8);
GAMEPLAY_OFFSET(GeorgeGameplayActor, field34, 0x34);
GAMEPLAY_OFFSET(GeorgeGameplayActor, field148, 0x148);
GAMEPLAY_OFFSET(GeorgeGameplayActor, field188, 0x188);
#undef GAMEPLAY_OFFSET

/* Callback ABI: argument count in $4, optional destination slot in $5. */
void func_001D3A68(s32 argc, s32 destination);
void func_001D3B20(s32 argc, s32 destination);
void func_001D3BB8(s32 argc, s32 destination);
void func_001D44D0(s32 argc, s32 destination);
void func_001D48D0(s32 argc, s32 destination);
void func_001D4968(s32 argc, s32 destination);
void func_001D4A00(s32 argc, s32 destination);
void func_001D4E48(s32 argc, s32 destination);
void func_001D52D8(s32 argc, s32 destination);
void func_001D5378(s32 argc, s32 destination);
void func_001D57F0(s32 argc, s32 destination);
void func_001D5CD0(s32 argc, s32 destination);
void func_001D5D68(s32 argc, s32 destination);
void func_001D5E28(s32 argc, s32 destination);
void func_001D5ED8(s32 argc, s32 destination);
void func_001D5F88(s32 argc, s32 destination);
void func_001D6020(s32 argc, s32 destination);
void func_001D60C0(s32 argc, s32 destination);
void func_001D6648(s32 argc, s32 destination);
void func_001D66D0(s32 argc, s32 destination);
void func_001D6948(s32 argc, s32 destination);
void func_001D6A30(s32 argc, s32 destination);
void func_001D6AB8(s32 argc, s32 destination);
void func_001D6B40(s32 argc, s32 destination);
void func_001D6BC8(s32 argc, s32 destination);
void func_001D89D8(GeorgeGameplayGoalQueue *queue, GeorgeGameplayGoal *goal);

#endif
