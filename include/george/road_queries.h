#ifndef GEORGE_ROAD_QUERIES_H
#define GEORGE_ROAD_QUERIES_H

#include "george/spatial_queries.h"
#include "george/resource_registry.h"

/* Alternate observed prefixes, not original classes or format capacities. */
typedef struct GeorgeRoadQueryCell {
    u16 field00;
    u8 unknown02[2];
    u16 *field04;
    u8 *field08; /* Entries below add a byte offset before the second LHU. */
    u8 field0C;
    u8 unknown0D[3];
} GeorgeRoadQueryCell;

typedef struct GeorgeRoadQueryRoad {
    u32 field00;
    u8 unknown04[2];
    u16 field06;
    u8 unknown08[0x1C];
    u32 *field24;
    u8 unknown28[8];
    u8 *field30;
    GeorgeRegistryIndex *field34;
    u8 *field38;
    u8 unknown3C[12];
    const GeorgeMathVec3 *field48;
} GeorgeRoadQueryRoad;

typedef struct GeorgeRoadQueryHeader {
    u32 field00;
    GeorgeBounds field04;
    u8 unknown1C[16];
    GeorgeGoalRoad *field2C;
} GeorgeRoadQueryHeader;

#define ROAD_QUERY_OFFSET(type,member,n) \
    typedef char road_query_##type##_##member[(offsetof(type,member)==(n))?1:-1]
ROAD_QUERY_OFFSET(GeorgeRoadQueryCell,field04,4);
ROAD_QUERY_OFFSET(GeorgeRoadQueryCell,field08,8);
ROAD_QUERY_OFFSET(GeorgeRoadQueryCell,field0C,12);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field06,6);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field24,0x24);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field30,0x30);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field34,0x34);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field38,0x38);
ROAD_QUERY_OFFSET(GeorgeRoadQueryRoad,field48,0x48);
ROAD_QUERY_OFFSET(GeorgeRoadQueryHeader,field04,4);
ROAD_QUERY_OFFSET(GeorgeRoadQueryHeader,field2C,0x2C);
#undef ROAD_QUERY_OFFSET
typedef char road_query_cell_size[(sizeof(GeorgeRoadQueryCell)==16)?1:-1];

void *func_001CC710(const GeorgeMathVec3 *point) GEORGE_SAVE128;
float func_001CE620(const GeorgeMathVec3 *point, GeorgeGoalRoad *road,
                   s32 index, s32 count, u8 *vertex, s32 *mode,
                   float threshold) GEORGE_SAVE128;
/* Compatible with the published collision caller's numeric u16 result view. */
u16 *func_001CEA80(const GeorgeMathVec3 *point, GeorgeGoalRoad **road,
                 u8 *vertex, s32 *mode, float radius) GEORGE_SAVE128;

#endif
