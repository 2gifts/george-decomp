#ifndef GEORGE_ACTOR_ROUTE_H
#define GEORGE_ACTOR_ROUTE_H

#include "george/goal_methods5.h"
#include "george/road_queries.h"

/* Field-only views; neither a complete allocation nor an original class. */
typedef struct GeorgeActorRouteActor {
    u8 reserved00[0x34];
    GeorgeGoalOwner field34;
} GeorgeActorRouteActor;

typedef struct GeorgeActorRouteState {
    u8 reserved00[0x10];
    void *field10;
    u8 reserved14[0x2A];
    u8 field3E,field3F;
    u8 reserved40;
    u8 field41;
    u8 reserved42[4];
    u16 field46;
    u32 field48,field4C;
    u8 reserved50[0xC0];
    GeorgeGoalReferencedObject *field110;
    GeorgeMathVec3 field114;
} GeorgeActorRouteState;

typedef struct GeorgeActorRouteVirtualEntry {
    s16 adjustment;
    u8 reserved02[2];
    void *(*invoke)(void *);
} GeorgeActorRouteVirtualEntry;

#define ACTOR_ROUTE_OFFSET(type,member,n) \
    typedef char actor_route_##type##_##member[(offsetof(type,member)==(n))?1:-1]
ACTOR_ROUTE_OFFSET(GeorgeActorRouteActor,field34,0x34);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field10,0x10);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field3E,0x3E);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field3F,0x3F);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field41,0x41);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field46,0x46);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field48,0x48);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field4C,0x4C);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field110,0x110);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteState,field114,0x114);
ACTOR_ROUTE_OFFSET(GeorgeActorRouteVirtualEntry,invoke,4);
#undef ACTOR_ROUTE_OFFSET

void func_001B7AB8(GeorgeActorRouteActor *,const GeorgeMathVec3 *) GEORGE_SAVE128;
void func_001E2D88(GeorgeActorRouteState *,const GeorgeMathVec3 *) GEORGE_SAVE128;
s32 func_001CC628(void *object);

/* Supporting-only numeric bodies: no source award for these two entries.
 * The setup caller supplies seven GPR lanes; it supplies no float argument. */
GeorgeGoalOwner *func_001BA610(u32 actor);
void func_0020D080(void *owner,void *array,u32 kind,u32 first,u32 second,
                   const GeorgeMathVec3 *point,const GeorgeMathVec3 *target);

#endif
