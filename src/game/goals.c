#include "george/goals.h"

extern u8 D_00436818[], D_004368A0[], D_00436900[], D_004369A0[];
extern u8 D_00436A70[], D_00436AF8[], D_00436B48[], D_00436B98[];
extern u8 D_00436BF8[], D_00436C48[], D_00436C98[], D_00436CE8[];
extern u8 D_00436DC0[], D_00436E48[], D_00437118[], D_00437168[];
extern u8 D_004371B8[], D_00437340[], D_00437390[], D_004374C8[];
extern u8 D_004375A0[], D_00437628[], D_00437678[], D_00437788[];
extern u8 D_004377D8[], D_0043A2B0[];
extern void *func_002AF5A0(u32 count, u32 element_size);

/* The first word stores the owner address; +4/+8 are the existing queue links. */
GeorgeGameplayGoal *func_0020D2A0(void *storage, void *owner)
{
    GeorgeGoalBase *goal = storage;
    goal->links.unknown00 = (u32)owner;
    goal->links.field04 = 0;
    goal->field0C = D_0043A2B0;
    goal->links.field08 = 0;
    return &goal->links;
}

void func_001DC538(GeorgeGoalTimer *timer, float value)
{
    timer->field00 = 0;
    timer->field04 = 0;
    timer->field08 = 0;
    timer->field0C = 0;
    timer->field14 = 0;
    timer->field10 = 0;
    timer->field24 = 1.0f;
    timer->field18 = 1.0e9f;
    timer->field1C = value;
    timer->field20 = 1.0f;
}

/* Preserve the increment of the separate object's byte and the owner reload. */
GeorgeGameplayGoal *func_001E84D0(void *storage, void *owner, u32 object,
                                u32 word1, u32 word2, u32 word3)
{
    GeorgeGoalEnterVehicle *goal = storage;
    GeorgeGoalReferencedObject *referenced = (GeorgeGoalReferencedObject *)object;
    GeorgeGoalOwner *reloaded_owner;
    func_0020D2A0(storage, owner);
    goal->field10 = object;
    goal->base.field0C = D_004375A0;
    referenced->field05 = (u8)(referenced->field05 + 1);
    goal->field40 = (u8)word1;
    goal->field41 = (u8)word2;
    goal->field42 = (u8)word3;
    goal->field14 = 0;
    goal->field3D = 0;
    goal->field3E = 0;
    goal->field3F = 0;
    func_001DC538(&goal->field44, 0.5f);
    reloaded_owner = (GeorgeGoalOwner *)goal->base.links.unknown00;
    reloaded_owner->field24 |= 0x40;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E87C0(void *storage, void *actor, u32 word)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, ((GeorgeGameplayActor *)actor)->field34);
    goal->field10 = (u32)actor;
    goal->field14 = word;
    goal->base.field0C = D_00437628;
    return &goal->base.links;
}

/* These two constructors have the same three stores after base construction. */
#define WORD14_BODY(vtable, word) \
    GeorgeGoalWords *goal = storage; \
    func_0020D2A0(storage, owner); \
    goal->field14 = word; \
    goal->base.field0C = vtable; \
    goal->field10 = 0; \
    return &goal->base.links

GeorgeGameplayGoal *func_001E1E78(void *storage, void *owner, u32 word)
{
    WORD14_BODY(D_00437168, word);
}

GeorgeGameplayGoal *func_001DC650(void *storage, void *owner, s32 value)
{
    WORD14_BODY(D_00436AF8, (u32)value);
}

GeorgeGameplayGoal *func_001E5A58(void *storage, void *owner, u32 object)
{
    GeorgeGoalWords *goal = storage;
    GeorgeGoalReferencedObject *referenced = (GeorgeGoalReferencedObject *)object;
    func_0020D2A0(storage, owner);
    goal->field10 = object;
    goal->base.field0C = D_00437390;
    referenced->field05 = (u8)(referenced->field05 + 1);
    return &goal->base.links;
}

/* Scalar copies preserve the read/store sequence if input overlaps storage. */
GeorgeGameplayGoal *func_001E5AA8(void *storage, void *owner,
                                const GeorgeMathVec3 *position)
{
    GeorgeGoalPosition *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = 0;
    goal->base.field0C = D_00437390;
    goal->field14.x = position->x;
    goal->field14.y = position->y;
    goal->field14.z = position->z;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E6828(void *storage, void *owner, u32 word)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field14 = word;
    goal->base.field0C = D_004374C8;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001DFD38(void *storage, void *owner, float value)
{
    GeorgeGoalWalkIntersection *goal = storage;
    func_0020D2A0(storage, owner);
    goal->base.field0C = D_00436C48;
    func_001DC538(&goal->field1C, value);
    goal->field1C.field0C = 0;
    goal->field1C.field14 = 0;
    goal->field1C.field10 = 0;
    goal->field1C.field20 = 0.75f;
    goal->field14 = 0xFF;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E04C8(void *storage, void *owner, float value)
{
    GeorgeGoalWalkRoad *goal = storage;
    func_0020D2A0(storage, owner);
    goal->base.field0C = D_00436C98;
    func_001DC538(&goal->field14, value);
    goal->field14.field0C = 0;
    goal->field14.field14 = 0;
    goal->field14.field10 = 0;
    goal->field14.field20 = 0.75f;
    return &goal->base.links;
}

/* Four action constructors differ only in their observed vtable address. */
#define TIMED_ACTION_BODY(vtable) \
    GeorgeGoalTimedAction *goal = storage; \
    func_0020D2A0(storage, owner); \
    goal->field14 = value; \
    goal->base.field0C = vtable; \
    goal->field18 = 2; \
    goal->field19 = 0; \
    return &goal->base.links

GeorgeGameplayGoal *func_001D9A80(void *storage, void *owner, float value)
{
    TIMED_ACTION_BODY(D_004368A0);
}

GeorgeGameplayGoal *func_001E0EC8(void *storage, void *owner, float value)
{
    TIMED_ACTION_BODY(D_00436DC0);
}

GeorgeGameplayGoal *func_001E1078(void *storage, void *owner, float value)
{
    TIMED_ACTION_BODY(D_00436E48);
}

GeorgeGameplayGoal *func_001D9920(void *storage, void *owner, float value)
{
    TIMED_ACTION_BODY(D_00436818);
}

GeorgeGameplayGoal *func_001E8E40(void *storage, void *owner, u32 word)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = word;
    goal->base.field0C = D_004377D8;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001DA5E8(void *storage, void *owner,
                                float value0, float value1, float value2)
{
    GeorgeGoalLook *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field1C = value0;
    goal->field20 = value1;
    goal->field24 = value2;
    goal->base.field0C = D_00436900;
    return &goal->base.links;
}

/* The positivity test uses the original 32-bit input, before narrowing. */
GeorgeGameplayGoal *func_001DC5A0(void *storage, void *owner, s32 value)
{
    GeorgeGoalRestart *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = (s16)value;
    goal->field12 = 0;
    goal->base.field0C = D_00436A70;
    if (value > 0) {
        goal->field12 = 1;
    }
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E1F48(void *storage, void *owner, u32 word)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = word;
    goal->base.field0C = D_004371B8;
    func_002CD0B8((GeorgeDeimosPoolNode *)word);
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E5988(void *storage, void *owner,
                                u32 word0, u32 word1)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = word0;
    goal->field14 = word1;
    goal->base.field0C = D_00437340;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001DC6F8(void *storage, void *owner, float value)
{
    GeorgeGoalWait *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = value;
    goal->base.field0C = D_00436B48;
    goal->field14 = 0;
    return &goal->base.links;
}

#define BASE_ONLY_BODY(vtable) \
    GeorgeGoalBase *goal = storage; \
    func_0020D2A0(storage, owner); \
    goal->field0C = vtable; \
    return &goal->links

GeorgeGameplayGoal *func_001E8950(void *storage, void *owner)
{
    BASE_ONLY_BODY(D_00437678);
}

GeorgeGameplayGoal *func_001DC7A8(void *storage, void *owner)
{
    BASE_ONLY_BODY(D_00436B98);
}

GeorgeGameplayGoal *func_001DB930(void *storage, void *owner)
{
    BASE_ONLY_BODY(D_004369A0);
}

GeorgeGameplayGoal *func_001E0C38(void *storage, void *owner,
                                u32 value0, u32 value1, u32 value2,
                                u32 value3, u32 value4)
{
    GeorgeGoalIdle *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = value0;
    goal->field14 = value1;
    goal->field18 = value2;
    goal->field1C = value3;
    goal->field9C = (u8)value4;
    goal->base.field0C = D_00436CE8;
    goal->field94 = 1;
    goal->field98 = 0xFFFFFFFFU;
    goal->field9E = 3;
    goal->field90 = 1;
    goal->field9D = 0;
    goal->field9F = 0;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001DF028(void *storage, void *owner)
{
    GeorgeGoalDrive *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field48 = 0;
    goal->field49 = 0;
    goal->base.field0C = D_00436BF8;
    goal->field46 = 0;
    goal->field47 = 0;
    goal->field50 = 0;
    goal->field54 = 0;
    goal->field60 = 0;
    goal->field64 = 0;
    goal->field10 = 0;
    goal->field14 = ((GeorgeGoalOwner *)goal->base.links.unknown00)->field04;
    goal->field40 = func_002AF5A0(5, 0x18);
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E1DF0(void *storage, void *owner, void *actor)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = (u32)actor;
    goal->base.field0C = D_00437118;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E8D98(void *storage, void *actor)
{
    GeorgeGoalWords *goal = storage;
    func_0020D2A0(storage, ((GeorgeGameplayActor *)actor)->field34);
    goal->field10 = (u32)actor;
    goal->base.field0C = D_00437788;
    return &goal->base.links;
}

#undef BASE_ONLY_BODY
#undef TIMED_ACTION_BODY
#undef WORD14_BODY
