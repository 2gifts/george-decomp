#ifndef GEORGE_GOALS_H
#define GEORGE_GOALS_H

#include "george/script_gameplay.h"
#include "george/compiler.h"

/* These are observed prefixes and fields, not recovered original classes. */
typedef struct GeorgeGoalBase {
    GeorgeGameplayGoal links;
    const void *field0C;
} GeorgeGoalBase;

typedef struct GeorgeGoalWords {
    GeorgeGoalBase base;
    u32 field10;
    u32 field14;
} GeorgeGoalWords;

typedef struct GeorgeGoalPosition {
    GeorgeGoalBase base;
    u32 field10;
    GeorgeMathVec3 field14;
} GeorgeGoalPosition;

typedef struct GeorgeGoalInteractionPosition {
    GeorgeGoalBase base;
    u32 unknown10;
    u32 field14;
    GeorgeMathVec3 field18;
} GeorgeGoalInteractionPosition;

typedef struct GeorgeGoalTimedAction {
    GeorgeGoalBase base;
    float field10;
    float field14;
    u8 field18;
    u8 field19;
} GeorgeGoalTimedAction;

typedef struct GeorgeGoalLook {
    GeorgeGoalBase base;
    u32 field10;
    u32 field14;
    float field18;
    float field1C;
    float field20;
    float field24;
} GeorgeGoalLook;

typedef struct GeorgeGoalRestart {
    GeorgeGoalBase base;
    s16 field10;
    s16 field12;
} GeorgeGoalRestart;

typedef struct GeorgeGoalWait {
    GeorgeGoalBase base;
    float field10;
    u32 field14;
} GeorgeGoalWait;

/* Zeroed words are kept as words: retail uses sw, not float stores. */
typedef struct GeorgeGoalTimer {
    u32 field00;
    u32 field04;
    u32 field08;
    u32 field0C;
    u32 field10;
    u32 field14;
    float field18;
    float field1C;
    float field20;
    float field24;
} GeorgeGoalTimer;

typedef struct GeorgeGoalWalkIntersection {
    GeorgeGoalBase base;
    u8 field10;
    u8 field11;
    u8 field12;
    u8 unknown13;
    u8 field14;
    u8 unknown15[7];
    GeorgeGoalTimer field1C;
} GeorgeGoalWalkIntersection;

typedef struct GeorgeGoalWalkRoad {
    GeorgeGoalBase base;
    u8 field10;
    u8 field11;
    u8 field12;
    u8 unknown13;
    GeorgeGoalTimer field14;
} GeorgeGoalWalkRoad;

typedef struct GeorgeGoalEnterVehicle {
    GeorgeGoalBase base;
    u32 field10;
    u32 field14;
    u8 unknown18[0x25];
    u8 field3D;
    u8 field3E;
    u8 field3F;
    u8 field40;
    u8 field41;
    u8 field42;
    u8 unknown43;
    GeorgeGoalTimer field44;
} GeorgeGoalEnterVehicle;

typedef struct GeorgeGoalIdle {
    GeorgeGoalBase base;
    u32 field10;
    u32 field14;
    u32 field18;
    u32 field1C;
    u8 unknown20[0x70];
    u32 field90;
    u32 field94;
    u32 field98;
    u8 field9C;
    u8 field9D;
    u8 field9E;
    u8 field9F;
} GeorgeGoalIdle;

typedef struct GeorgeGoalDrive {
    GeorgeGoalBase base;
    u32 field10;
    u32 field14;
    u8 unknown18[0x28];
    void *field40;
    u8 unknown44[2];
    u8 field46;
    u8 field47;
    u8 field48;
    u8 field49;
    u8 unknown4A[6];
    u32 field50;
    u32 field54;
    u8 unknown58[8];
    u32 field60;
    u32 field64;
} GeorgeGoalDrive;

/* Only these offsets of the two separate pointees are established here. */
typedef struct GeorgeGoalReferencedObject {
    u8 unknown00[4];
    signed char field04;
    u8 field05;
    u8 field06;
    u8 unknown07[0x19];
    union {
        u32 word;
        GeorgeMathVec3 position;
    } field20;
} GeorgeGoalReferencedObject;

typedef union GeorgeGoalOwnerWord {
    u32 bits;
    struct {
        u16 low;
        u16 high;
    } halfwords;
} GeorgeGoalOwnerWord;

typedef struct GeorgeGoalEntityData {
    u8 unknown00[0x3FC];
    u32 field3FC;
} GeorgeGoalEntityData;

typedef struct GeorgeGoalEntity {
    u8 unknown00[0xC];
    u32 field0C;
    u8 unknown10[8];
    GeorgeGoalEntityData *field18;
    u8 unknown1C[0x24];
    u8 field40[0x1C];
    float field5C;
    u8 unknown60[0x70];
    GeorgeMathVec3 fieldD0;
    u8 unknownDC[0x288];
    u32 field364;
    u32 field368;
    u8 unknown36C[0x394];
    u32 field700;
    u8 unknown704[0x34];
    signed char field738;
} GeorgeGoalEntity;

typedef struct GeorgeGoalZeroUnit {
    u32 field00;
    float field04;
    u32 field08;
} GeorgeGoalZeroUnit;

typedef struct GeorgeGoalOwner {
    u32 unknown00;
    u32 field04;
    GeorgeGoalEntity *field08;
    u8 unknown0C[8];
    u32 field14;
    GeorgeGoalOwnerWord field18;
    float field1C;
    u32 field20;
    u16 field24;
    u8 unknown26[6];
    u32 field2C;
    float field30;
    float field34;
    u8 unknown38[4];
    GeorgeMathVec3 field3C;
    u8 unknown48[0x34];
    u32 field7C;
    GeorgeGoalZeroUnit field80;
    float field8C;
} GeorgeGoalOwner;

typedef struct GeorgeGoalOutput {
    u32 field00;
    GeorgeMathVec3 field04;
    u32 field10;
} GeorgeGoalOutput;

typedef struct GeorgeGoalRoadEntry {
    u8 unknown00[0x34];
    float field34;
    u8 unknown38[0x28];
} GeorgeGoalRoadEntry;

typedef struct GeorgeGoalRoad {
    u8 unknown00[0x3C];
    GeorgeGoalRoadEntry *field3C;
} GeorgeGoalRoad;

/* The retained virtual ABI uses a signed 16-bit this adjustment before a pointer. */
typedef struct GeorgeGoalVirtualObject {
    u32 unknown00;
    const u8 *field04;
} GeorgeGoalVirtualObject;
typedef struct GeorgeGoalVirtualInt {
    s16 adjustment;
    u16 unknown02;
    s32 (*invoke)(void *adjusted_this);
} GeorgeGoalVirtualInt;
typedef struct GeorgeGoalVirtualEntity {
    s16 adjustment;
    u16 unknown02;
    GeorgeGoalEntity *(*invoke)(void *adjusted_this, s32 index);
} GeorgeGoalVirtualEntity;
typedef struct GeorgeGoalVirtualVector {
    s16 adjustment;
    u16 unknown02;
    const GeorgeMathVec3 *(*invoke)(void *adjusted_this);
} GeorgeGoalVirtualVector;

#define GOAL_OFFSET(type, member, offset) \
    typedef char goal_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
GOAL_OFFSET(GeorgeGoalBase, field0C, 0xC);
GOAL_OFFSET(GeorgeGoalWords, field10, 0x10);
GOAL_OFFSET(GeorgeGoalWords, field14, 0x14);
GOAL_OFFSET(GeorgeGoalPosition, field14, 0x14);
GOAL_OFFSET(GeorgeGoalInteractionPosition, field18, 0x18);
GOAL_OFFSET(GeorgeGoalTimedAction, field14, 0x14);
GOAL_OFFSET(GeorgeGoalTimedAction, field18, 0x18);
GOAL_OFFSET(GeorgeGoalTimedAction, field19, 0x19);
GOAL_OFFSET(GeorgeGoalLook, field1C, 0x1C);
GOAL_OFFSET(GeorgeGoalLook, field24, 0x24);
GOAL_OFFSET(GeorgeGoalRestart, field12, 0x12);
GOAL_OFFSET(GeorgeGoalWait, field14, 0x14);
GOAL_OFFSET(GeorgeGoalTimer, field0C, 0xC);
GOAL_OFFSET(GeorgeGoalTimer, field24, 0x24);
GOAL_OFFSET(GeorgeGoalWalkIntersection, field14, 0x14);
GOAL_OFFSET(GeorgeGoalWalkIntersection, field1C, 0x1C);
GOAL_OFFSET(GeorgeGoalWalkRoad, field14, 0x14);
GOAL_OFFSET(GeorgeGoalEnterVehicle, field3D, 0x3D);
GOAL_OFFSET(GeorgeGoalEnterVehicle, field40, 0x40);
GOAL_OFFSET(GeorgeGoalEnterVehicle, field44, 0x44);
GOAL_OFFSET(GeorgeGoalIdle, field90, 0x90);
GOAL_OFFSET(GeorgeGoalIdle, field9F, 0x9F);
GOAL_OFFSET(GeorgeGoalDrive, field40, 0x40);
GOAL_OFFSET(GeorgeGoalDrive, field46, 0x46);
GOAL_OFFSET(GeorgeGoalDrive, field50, 0x50);
GOAL_OFFSET(GeorgeGoalDrive, field64, 0x64);
GOAL_OFFSET(GeorgeGoalReferencedObject, field05, 5);
GOAL_OFFSET(GeorgeGoalReferencedObject, field20, 0x20);
GOAL_OFFSET(GeorgeGoalOwner, field08, 8);
GOAL_OFFSET(GeorgeGoalOwner, field18, 0x18);
GOAL_OFFSET(GeorgeGoalOwner, field24, 0x24);
GOAL_OFFSET(GeorgeGoalOwner, field3C, 0x3C);
GOAL_OFFSET(GeorgeGoalOwner, field7C, 0x7C);
GOAL_OFFSET(GeorgeGoalOwner, field8C, 0x8C);
GOAL_OFFSET(GeorgeGoalEntity, field40, 0x40);
GOAL_OFFSET(GeorgeGoalEntity, fieldD0, 0xD0);
GOAL_OFFSET(GeorgeGoalEntity, field364, 0x364);
GOAL_OFFSET(GeorgeGoalEntity, field368, 0x368);
GOAL_OFFSET(GeorgeGoalEntity, field700, 0x700);
GOAL_OFFSET(GeorgeGoalEntity, field738, 0x738);
GOAL_OFFSET(GeorgeGoalEntityData, field3FC, 0x3FC);
GOAL_OFFSET(GeorgeGoalOutput, field10, 0x10);
GOAL_OFFSET(GeorgeGoalRoad, field3C, 0x3C);
GOAL_OFFSET(GeorgeGoalVirtualInt, invoke, 4);
GOAL_OFFSET(GeorgeGoalVirtualEntity, invoke, 4);
GOAL_OFFSET(GeorgeGoalVirtualVector, invoke, 4);
typedef char goal_road_entry_stride[(sizeof(GeorgeGoalRoadEntry) == 0x60) ? 1 : -1];
#undef GOAL_OFFSET

GeorgeGameplayGoal *func_0020D2A0(void *storage, void *owner);
void func_001DC538(GeorgeGoalTimer *timer, float value);
GeorgeGameplayGoal *func_001E84D0(void *storage, void *owner, u32 object, u32 word1, u32 word2, u32 word3) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E87C0(void *storage, void *actor, u32 word) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E1E78(void *storage, void *owner, u32 word) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E5A58(void *storage, void *owner, u32 object) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E5AA8(void *storage, void *owner, const GeorgeMathVec3 *position) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E6828(void *storage, void *owner, u32 word) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DFD38(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E04C8(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001D9A80(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E0EC8(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E1078(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001D9920(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E8E40(void *storage, void *owner, u32 word) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DA5E8(void *storage, void *owner, float value0, float value1, float value2) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DC5A0(void *storage, void *owner, s32 value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DC650(void *storage, void *owner, s32 value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E1F48(void *storage, void *owner, u32 word) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E5988(void *storage, void *owner, u32 word0, u32 word1) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DC6F8(void *storage, void *owner, float value) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E8950(void *storage, void *owner) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E0C38(void *storage, void *owner, u32 value0, u32 value1, u32 value2, u32 value3, u32 value4) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DC7A8(void *storage, void *owner) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DF028(void *storage, void *owner) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001DB930(void *storage, void *owner) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E1DF0(void *storage, void *owner, void *actor) GEORGE_SAVE128;
GeorgeGameplayGoal *func_001E8D98(void *storage, void *actor) GEORGE_SAVE128;

void func_001D9970(GeorgeGoalBase *goal, u32 flags);
void func_001D99A8(GeorgeGoalTimedAction *goal) GEORGE_SAVE128;
void func_001D9AD0(GeorgeGoalBase *goal, u32 flags);
void func_001D9B08(GeorgeGoalTimedAction *goal) GEORGE_SAVE128;
void func_001DA650(GeorgeGoalLook *goal) GEORGE_SAVE128;
void func_001DA790(GeorgeGoalLook *goal);
void func_001DC5F8(GeorgeGoalRestart *goal);
void func_001DC698(GeorgeGoalWords *goal);
void func_001DC7E0(GeorgeGoalBase *goal, GeorgeGoalOutput *output);
void func_001DF548(GeorgeGoalWalkIntersection *goal) GEORGE_SAVE128;
void func_001E0530(GeorgeGoalWalkRoad *goal) GEORGE_SAVE128;
void func_001E05C8(GeorgeGoalBase *goal) GEORGE_SAVE128;
void func_001E0CE0(GeorgeGoalBase *goal, u32 flags) GEORGE_SAVE128;
void func_001E0F18(GeorgeGoalBase *goal, u32 flags);
void func_001E0F50(GeorgeGoalTimedAction *goal) GEORGE_SAVE128;
void func_001E10C8(GeorgeGoalBase *goal, u32 flags);
void func_001E1100(GeorgeGoalTimedAction *goal) GEORGE_SAVE128;
void func_001E1EC0(GeorgeGoalWords *goal) GEORGE_SAVE128;
void func_001E1F98(GeorgeGoalWords *goal, u32 flags) GEORGE_SAVE128;
void func_001E1FE8(GeorgeGoalWords *goal);
void func_001E59E0(GeorgeGoalWords *goal);
void func_001E5B08(GeorgeGoalWords *goal, u32 flags) GEORGE_SAVE128;
void func_001E5BA0(GeorgeGoalPosition *goal, GeorgeGoalOutput *output) GEORGE_SAVE128;
void func_001E68D8(GeorgeGoalInteractionPosition *goal, GeorgeGoalOutput *output);
void func_001E8580(GeorgeGoalEnterVehicle *goal, u32 flags) GEORGE_SAVE128;
void func_001E8818(GeorgeGoalWords *goal) GEORGE_SAVE128;
void func_001E8E88(GeorgeGoalWords *goal);
void func_0020D2C0(GeorgeGoalBase *goal, u32 flags);
void func_001CAF88(GeorgeGoalReferencedObject *object) GEORGE_SAVE128;
const GeorgeMathVec3 *func_001CAFE0(GeorgeGoalReferencedObject *object);

#endif
