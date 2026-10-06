#ifndef GEORGE_GOAL_METHODS4_H
#define GEORGE_GOAL_METHODS4_H

#include "george/goal_methods3.h"
#include "george/deimos_tables.h"
#include "george/ring.h"

/* Alternate numeric views of previously observed opaque storage. */
typedef struct GeorgeGoalDriveMotion {
    GeorgeGoalBase base;
    GeorgeGoalVirtualObject *field10, *field14;
    GeorgeMathVec3 field18, field24;
    u8 field30;
    signed char field31, field32;
    u8 unknown33;
    float field34, field38, field3C;
    GeorgeDeimosRing *field40;
    u8 field44, field45, field46, field47, field48, field49;
    u16 field4A;
    float field4C, field50, field54, field58, field5C;
    void *field60;
    GeorgeGoalRoadEntry *field64;
    GeorgeMathVec3 field68, field74, field80;
    u32 field8C;
} GeorgeGoalDriveMotion;

typedef struct GeorgeGoalRouteQueueEntry {
    u32 field00, field04;
    signed char field08, field09, field0A, field0B;
    float field0C, field10, field14;
} GeorgeGoalRouteQueueEntry;

typedef struct GeorgeGoalMapOwner {
    u8 unknown00[0x300];
    GeorgeGenericMap *field300;
} GeorgeGoalMapOwner;

typedef struct GeorgeGoalVirtualWord {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *adjusted_this, u32 word);
} GeorgeGoalVirtualWord;
typedef struct GeorgeGoalVirtualFloatResult {
    s16 adjustment;
    u16 unknown02;
    float (*invoke)(void *adjusted_this);
} GeorgeGoalVirtualFloatResult;
typedef struct GeorgeGoalVirtualPointer {
    s16 adjustment;
    u16 unknown02;
    GeorgeGoalVirtualObject *(*invoke)(void *adjusted_this);
} GeorgeGoalVirtualPointer;

#define GM4_OFFSET(type, member, offset) \
    typedef char gm4_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
GM4_OFFSET(GeorgeGoalDriveMotion, field18, 0x18);
GM4_OFFSET(GeorgeGoalDriveMotion, field30, 0x30);
GM4_OFFSET(GeorgeGoalDriveMotion, field34, 0x34);
GM4_OFFSET(GeorgeGoalDriveMotion, field40, 0x40);
GM4_OFFSET(GeorgeGoalDriveMotion, field4A, 0x4A);
GM4_OFFSET(GeorgeGoalDriveMotion, field60, 0x60);
GM4_OFFSET(GeorgeGoalDriveMotion, field80, 0x80);
GM4_OFFSET(GeorgeGoalDriveMotion, field8C, 0x8C);
GM4_OFFSET(GeorgeGoalRouteQueueEntry, field08, 8);
GM4_OFFSET(GeorgeGoalRouteQueueEntry, field14, 0x14);
GM4_OFFSET(GeorgeGoalMapOwner, field300, 0x300);
typedef char gm4_drive_view_size[(sizeof(GeorgeGoalDriveMotion) == sizeof(GeorgeGoalDrive)) ? 1 : -1];
typedef char gm4_route_queue_size[(sizeof(GeorgeGoalRouteQueueEntry) == 0x18) ? 1 : -1];
#undef GM4_OFFSET

void func_001CCAA8(u32 word, u32 key) GEORGE_SAVE128;
void func_001CCB58(u32 word, u32 key) GEORGE_SAVE128;
GeorgeGenericMap *func_001CCBE8(u32 key);
s32 func_001CFB20(GeorgeMathVec3 *output, u32 word, u32 index) GEORGE_SAVE128;
s32 func_001D1A30(GeorgeGoalRoadGeometry *road, u32 word, const u8 *selector);
s32 func_001D1AA0(GeorgeGoalRoadGeometry *road, u32 word, u32 *output,
                   const signed char *index) GEORGE_SAVE128;
void func_001DCAA8(GeorgeGoalDrive *input, GeorgeGoalRoadGeometry *road) GEORGE_SAVE128;
s32 func_001DCBD8(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DCD48(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DCF50(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DD128(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DD1E0(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DD580(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DDA70(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DDBB8(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DDD70(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DE228(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DE6E0(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DE928(GeorgeGoalDrive *input) GEORGE_SAVE128;
float func_001DEC18(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DF290(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DF360(GeorgeGoalDrive *input) GEORGE_SAVE128;
void func_001DF420(GeorgeGenericMap *map, u32 key, void *value,
                    void *context) GEORGE_SAVE128;

#endif
