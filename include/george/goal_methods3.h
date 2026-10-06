#ifndef GEORGE_GOAL_METHODS3_H
#define GEORGE_GOAL_METHODS3_H

#include "george/goal_methods2.h"

/* Alternate observed view of the word-zeroed timer's vector fields. */
typedef struct GeorgeGoalMotion {
    GeorgeMathVec3 field00;
    GeorgeMathVec3 field0C;
    float field18, field1C, field20, field24;
} GeorgeGoalMotion;

typedef struct GeorgeGoalIntersectionState {
    GeorgeGoalBase base;
    u8 field10, field11, field12, field13, field14;
    u8 unknown15[3];
    float field18;
    GeorgeGoalMotion field1C;
    GeorgeMathVec3 field44, field50;
    /* First element only; the allocation's capacity is not yet established. */
    GeorgeMathVec3 field5C;
} GeorgeGoalIntersectionState;

typedef struct GeorgeGoalRoadState {
    GeorgeGoalWalkRoad prefix;
    GeorgeMathVec3 field3C, field48;
    float field54;
} GeorgeGoalRoadState;

typedef struct GeorgeGoalIntersectionRecord {
    u8 unknown00[4];
    u8 field04, field05;
    u8 unknown06[0xA];
    u16 field10;
    u8 unknown12[6];
    u32 field18, field1C;
    u8 unknown20[0x14];
} GeorgeGoalIntersectionRecord;

typedef struct GeorgeGoalRoadGeometry {
    u8 unknown00[0x38];
    GeorgeGoalIntersectionRecord *field38;
    GeorgeGoalRoadEntry *field3C;
    void *field40;
    u32 unknown44;
    GeorgeMathVec3 *field48;
} GeorgeGoalRoadGeometry;

/* EE MIN.S orders signed encodings, reversing when both are negative.
 * This independently expressed value model also retains negative NaN encodings
 * and selects -0 over +0. It does not model FCR cause flags. See goal_methods3.md.
 */
static __inline__ float george_ee_minimum(float first, float second)
{
    union { float scalar; s32 signed_bits; u32 bits; } left, right;
    left.scalar = first;
    right.scalar = second;
    if ((left.bits & right.bits & 0x80000000U) != 0)
        return left.signed_bits > right.signed_bits ? first : second;
    return left.signed_bits < right.signed_bits ? first : second;
}

#define GM3_OFFSET(type, member, offset) \
    typedef char gm3_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
GM3_OFFSET(GeorgeGoalMotion, field0C, 0xC);
GM3_OFFSET(GeorgeGoalMotion, field24, 0x24);
GM3_OFFSET(GeorgeGoalIntersectionState, field18, 0x18);
GM3_OFFSET(GeorgeGoalIntersectionState, field1C, 0x1C);
GM3_OFFSET(GeorgeGoalIntersectionState, field44, 0x44);
GM3_OFFSET(GeorgeGoalIntersectionState, field50, 0x50);
GM3_OFFSET(GeorgeGoalIntersectionState, field5C, 0x5C);
GM3_OFFSET(GeorgeGoalRoadState, field3C, 0x3C);
GM3_OFFSET(GeorgeGoalRoadState, field48, 0x48);
GM3_OFFSET(GeorgeGoalRoadState, field54, 0x54);
GM3_OFFSET(GeorgeGoalIntersectionRecord, field10, 0x10);
GM3_OFFSET(GeorgeGoalIntersectionRecord, field18, 0x18);
GM3_OFFSET(GeorgeGoalIntersectionRecord, field1C, 0x1C);
GM3_OFFSET(GeorgeGoalRoadGeometry, field38, 0x38);
GM3_OFFSET(GeorgeGoalRoadGeometry, field48, 0x48);
typedef char gm3_record_stride[(sizeof(GeorgeGoalIntersectionRecord) == 0x34) ? 1 : -1];
typedef char gm3_motion_size[(sizeof(GeorgeGoalMotion) == sizeof(GeorgeGoalTimer)) ? 1 : -1];
#undef GM3_OFFSET

GeorgeGoalRoadEntry *func_001D17D8(u32 word) GEORGE_SAVE128;
s32 func_001D1820(GeorgeMathVec3 *output, u32 word, u32 index, s32 mode, float value) GEORGE_SAVE128;
s32 func_001D18A8(GeorgeMathVec3 *output, u32 word, u32 index, s32 reverse) GEORGE_SAVE128;
void func_001D9DC8(GeorgeGoalLook *goal) GEORGE_SAVE128;
void func_001D9FC8(GeorgeGoalLook *goal) GEORGE_SAVE128;
void func_001DA1C8(GeorgeGoalLook *goal) GEORGE_SAVE128;
void func_001DA3C8(GeorgeGoalLook *goal) GEORGE_SAVE128;
void func_001DB968(GeorgeGoalOwner *owner, GeorgeGoalMotion *motion) GEORGE_SAVE128;
float func_001DC200(float origin, float target, float blend) GEORGE_SAVE128;
void func_001DF728(GeorgeGoalIntersectionState *goal) GEORGE_SAVE128;
void func_001DF8D8(GeorgeGoalIntersectionState *goal) GEORGE_SAVE128;
void func_001DFBD8(GeorgeGoalIntersectionState *goal) GEORGE_SAVE128;
void func_001DFEB0(GeorgeGoalRoadState *goal) GEORGE_SAVE128;
void func_001DFFE0(GeorgeGoalRoadState *goal) GEORGE_SAVE128;
void func_001E0218(GeorgeGoalRoadState *goal) GEORGE_SAVE128;
void func_001E0630(GeorgeGoalRoadState *goal) GEORGE_SAVE128;
void func_001E8390(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;
void func_001E8608(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;
void func_001E86A8(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;
void func_001E8708(GeorgeGoalEnterVehicle *goal) GEORGE_SAVE128;

#endif
