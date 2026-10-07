#ifndef GEORGE_ACTOR_ROUTE_INIT_H
#define GEORGE_ACTOR_ROUTE_INIT_H

#include "george/goals.h"

/* Observed fields only: these views do not establish allocation capacities. */
typedef struct GeorgeActorRouteRecord {
    u32 reserved00;
    GeorgeMathVec3 field04;
    u8 reserved10[4];
    u8 field14;
    u8 reserved15[2];
    u8 field17;
    GeorgeMathVec4 *field18;
} GeorgeActorRouteRecord;

typedef struct GeorgeActorRouteCollection {
    u32 field00;
    u8 reserved04[0x1C];
    u32 *field20;
    u8 *field24;
} GeorgeActorRouteCollection;

typedef struct GeorgeActorRouteQueryHeader {
    u32 field00;
    u8 reserved04[0x20];
    GeorgeActorRouteCollection *field24;
} GeorgeActorRouteQueryHeader;

typedef struct GeorgeActorRouteInitState {
    GeorgeGoalOwner *field00;
    u8 reserved04[0xC];
    void *field10;
    u8 reserved14[0x60];
    u8 field74,field75,field76,field77;
    u8 reserved78[8];
    u8 field80,field81,field82;
    u8 reserved83;
    u32 field84,field88;
    u8 reserved8C[0xC];
    GeorgeMathVec3 field98;
    GeorgeMathVec3 fieldA4;
    u8 reservedB0[8];
    float fieldB8;
    u32 reservedBC;
    GeorgeGoalReferencedObject *fieldC0;
    GeorgeMathVec3 fieldC4;
} GeorgeActorRouteInitState;

typedef struct GeorgeActorRouteCompletion {
    u8 field00;
    u8 reserved01[0xCB3];
    u8 fieldCB4;
} GeorgeActorRouteCompletion;

#define ROUTE_INIT_OFFSET(type,member,n) \
    typedef char route_init_##type##_##member[(offsetof(type,member)==(n))?1:-1]
ROUTE_INIT_OFFSET(GeorgeActorRouteRecord,field04,4);
ROUTE_INIT_OFFSET(GeorgeActorRouteRecord,field14,0x14);
ROUTE_INIT_OFFSET(GeorgeActorRouteRecord,field17,0x17);
ROUTE_INIT_OFFSET(GeorgeActorRouteRecord,field18,0x18);
ROUTE_INIT_OFFSET(GeorgeActorRouteCollection,field20,0x20);
ROUTE_INIT_OFFSET(GeorgeActorRouteCollection,field24,0x24);
ROUTE_INIT_OFFSET(GeorgeActorRouteQueryHeader,field24,0x24);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field10,0x10);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field74,0x74);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field80,0x80);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field82,0x82);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field84,0x84);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field88,0x88);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,field98,0x98);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,fieldA4,0xA4);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,fieldB8,0xB8);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,fieldC0,0xC0);
ROUTE_INIT_OFFSET(GeorgeActorRouteInitState,fieldC4,0xC4);
ROUTE_INIT_OFFSET(GeorgeActorRouteCompletion,fieldCB4,0xCB4);
#undef ROUTE_INIT_OFFSET

u32 func_001CD578(const GeorgeMathVec3 *,u32 *,GeorgeActorRouteRecord **,
                  GeorgeActorRouteCollection **) GEORGE_SAVE128;
void func_001DB250(GeorgeActorRouteInitState *) GEORGE_SAVE128;
u32 func_0020D190(GeorgeActorRouteCompletion *,u8 *);

#endif
