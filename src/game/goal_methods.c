#include "george/goals.h"
#include "george/deimos_calls.h"

extern u8 D_00436818[], D_004368A0[], D_00436DC0[], D_00436E48[];
extern u8 D_00436CE8[], D_004371B8[], D_00437390[], D_004375A0[];
extern u8 D_0043A2B0[], D_0045C640[];
extern void *func_003936A0(void *destination, s32 value, u32 size);
extern s32 func_00397178(void);
extern s32 func_001868F8(GeorgeGoalEntity *entity);
extern s32 func_00173818(GeorgeGoalEntity *entity);
extern s32 func_00174500(GeorgeGoalEntity *entity);
extern s32 func_00177E48(GeorgeGoalEntity *entity, const GeorgeMathVec3 *vector);
extern s32 func_001915D8(GeorgeGoalEntity *entity, u32 word, s32 mode, float value);
extern void func_00192368(GeorgeGoalEntity *entity);
extern s32 func_001DC468(void *context, u32 *word18, u32 *word14, u32 *word20);
extern GeorgeGoalRoad *func_001CCA28(u32 word);
extern void func_001DFEB0(GeorgeGoalWalkRoad *goal);
extern s32 func_00211BA8(void);
extern void *func_00239FD8(u32 word);
extern void *func_00238BA0(void *object, u32 hash);
extern void func_001779F8(GeorgeGoalEntity *entity, void *object, u32 word);
extern void func_001B67C0(void *context, GeorgeGoalEntity *entity, u32 word0,
                           u32 word1, s32 flag0, s32 flag1);
extern void func_001F6148(void *object);
extern s32 func_0018FCF0(GeorgeGoalEntity *entity, u32 flags);
extern void func_001BAAD8(u32 word, s32 index);
extern void func_002AF100(void *storage);
extern void func_001CB180(GeorgeGoalReferencedObject *object, u32 flags);
extern void func_002AD748(void *allocator, void *storage);
extern const GeorgeMathVec3 *func_00120B68(void *object);

typedef struct GeorgeGoalInteractionNode {
    u8 unknown00[0x88];
    u32 field88;
} GeorgeGoalInteractionNode;

#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define BASE_OWNER(goal) ((GeorgeGoalOwner *)(goal)->links.unknown00)
#define ADJUST_THIS(object, entry) \
    ((void *)((u32)(object) + (u32)(s32)(entry)->adjustment))

/* Each action destructor snapshots its owner before changing its table. */
#define ACTION_DESTRUCTOR_BODY(table) \
    GeorgeGoalOwner *owner = BASE_OWNER(goal); \
    goal->field0C = table; \
    owner->field24 &= 0xFFF7; \
    func_0020D2C0(goal, flags)

void func_001D9970(GeorgeGoalBase *goal, u32 flags)
{
    ACTION_DESTRUCTOR_BODY(D_00436818);
}

void func_001D9AD0(GeorgeGoalBase *goal, u32 flags)
{
    ACTION_DESTRUCTOR_BODY(D_004368A0);
}

void func_001E0F18(GeorgeGoalBase *goal, u32 flags)
{
    ACTION_DESTRUCTOR_BODY(D_00436DC0);
}

void func_001E10C8(GeorgeGoalBase *goal, u32 flags)
{
    ACTION_DESTRUCTOR_BODY(D_00436E48);
}

/* These operations happen only after the corresponding action gate succeeds. */
#define START_ACTION_BODY() \
    float value = goal->field14; \
    GeorgeGoalOwner *owner = OWNER(goal); \
    goal->field10 = value; \
    owner->field24 |= 8; \
    goal->field18 = 0

void func_001D99A8(GeorgeGoalTimedAction *goal)
{
    if (func_001868F8(OWNER(goal)->field08) != 0) {
        START_ACTION_BODY();
    }
}

void func_001D9B08(GeorgeGoalTimedAction *goal)
{
    GeorgeMathVec3 vector;
    func_003936A0(&vector, 0, 12);
    func_00177E48(OWNER(goal)->field08, &vector);
    if (func_00173818(OWNER(goal)->field08) != 0) {
        START_ACTION_BODY();
    }
}

void func_001E0F50(GeorgeGoalTimedAction *goal)
{
    if (func_00174500(OWNER(goal)->field08) != 0) {
        START_ACTION_BODY();
    }
}

void func_001E1100(GeorgeGoalTimedAction *goal)
{
    GeorgeMathVec3 vector;
    GeorgeGoalEntity *entity;
    func_003936A0(&vector, 0, 12);
    func_00177E48(OWNER(goal)->field08, &vector);
    entity = OWNER(goal)->field08;
    if (func_001915D8(entity, entity->field18->field3FC, 0, 5.0f) != 0) {
        START_ACTION_BODY();
    }
}

void func_001DA650(GeorgeGoalLook *goal)
{
    GeorgeMathVec3 vector;
    GeorgeGoalOwner *owner;
    float value0, value1, angle;
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.0f;
    func_00177E48(OWNER(goal)->field08, &vector);
    owner = OWNER(goal);
    value0 = goal->field1C;
    value1 = goal->field20;
    angle = owner->field08->field5C;
    /* Exact retail factor 0x3C8EFA36, including its one-ULP rounding choice. */
    value0 = value0 * 0.01745329424738884f;
    value1 = value1 * 0.01745329424738884f;
    value0 = angle + value0;
    goal->field18 = angle;
    value1 = angle + value1;
    goal->field1C = value0;
    goal->field20 = value1;
    if (3.1415927410125732f < value0) {
        goal->field1C = value0 - 6.2831854820251465f;
    }
    if (goal->field1C < -3.1415927410125732f) {
        goal->field1C = goal->field1C + 6.2831854820251465f;
    }
    if (3.1415927410125732f < goal->field20) {
        goal->field20 = goal->field20 - 6.2831854820251465f;
    }
    if (goal->field20 < -3.1415927410125732f) {
        goal->field20 = goal->field20 + 6.2831854820251465f;
    }
    goal->field10 = 0;
    goal->field14 = 0;
}

void func_001DA790(GeorgeGoalLook *goal)
{
    float angle = goal->field18;
    GeorgeGoalOwner *owner = OWNER(goal);
    if (owner->field2C == 0) {
        owner->field34 = angle;
        owner->field30 = 6.2831854820251465f;
    }
}

/* Verified complete 32-byte GCC 3.2.3 match, including branch delay slots. */
void func_001DC5F8(GeorgeGoalRestart *goal)
{
    if (goal->field12 != 0) {
        goal->field10 = (s16)((u16)goal->field10 - 1);
    }
}

/* Verified 32-byte match; reload entity through the saved owner after the store. */
void func_001DC698(GeorgeGoalWords *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    u32 value = goal->field14;
    u32 previous = owner->field08->field368;
    goal->field10 = previous;
    owner->field08->field368 = value;
}

void func_001DC7E0(GeorgeGoalBase *goal, GeorgeGoalOutput *output)
{
    GeorgeGoalOwner *owner;
    output->field10 = 0;
    output->field00 = 1;
    owner = BASE_OWNER(goal);
    output->field04.x = owner->field3C.x;
    output->field04.y = owner->field3C.y;
    output->field04.z = owner->field3C.z;
}

void func_001DF548(GeorgeGoalWalkIntersection *goal)
{
    GeorgeMathVec3 *position = (GeorgeMathVec3 *)&goal->field1C;
    GeorgeGoalEntity *entity = OWNER(goal)->field08;
    GeorgeGoalOwner *owner;
    GeorgeGoalRoad *road;
    s32 result;
    position->x = entity->fieldD0.x;
    position->y = entity->fieldD0.y;
    position->z = entity->fieldD0.z;
    owner = OWNER(goal);
    result = func_001DC468(owner->field08->field40, &owner->field18.bits,
                           &owner->field14, &owner->field20);
    if (result == 1) {
        goal->field11 = 1;
    } else if (result == 0) {
        road = func_001CCA28(OWNER(goal)->field18.bits);
        if (road != 0) {
            owner = OWNER(goal);
            owner->field8C = road->field3C[owner->field18.halfwords.low].field34;
            owner = OWNER(goal);
            owner->field80.field00 = 0;
            owner->field80.field04 = 1.0f;
            owner->field80.field08 = 0;
            goal->field11 = 0;
            goal->field10 = 0;
        } else {
            goal->field11 = 2;
        }
    } else {
        goal->field11 = 2;
    }
}

void func_001E0530(GeorgeGoalWalkRoad *goal)
{
    GeorgeMathVec3 *position = (GeorgeMathVec3 *)&goal->field14;
    GeorgeGoalEntity *entity;
    GeorgeGoalOwner *owner;
    s32 result;
    goal->field12 = 0;
    entity = OWNER(goal)->field08;
    position->x = entity->fieldD0.x;
    position->y = entity->fieldD0.y;
    position->z = entity->fieldD0.z;
    owner = OWNER(goal);
    result = func_001DC468(owner->field08->field40, &owner->field18.bits,
                           &owner->field14, &owner->field20);
    if (result == 1) {
        func_001DFEB0(goal);
    } else if (result == 0) {
        goal->field11 = 1;
    } else {
        goal->field11 = 2;
    }
}

void func_001E05C8(GeorgeGoalBase *goal)
{
    s32 value = func_00397178();
    BASE_OWNER(goal)->field1C = (float)(value % 10001) * 0.00009999999747378752f;
}

void func_001E0CE0(GeorgeGoalBase *goal, u32 flags)
{
    goal->field0C = D_00436CE8;
    func_00192368(BASE_OWNER(goal)->field08);
    func_0020D2C0(goal, flags);
}

void func_001E1EC0(GeorgeGoalWords *goal)
{
    void *object;
    GeorgeGoalInteractionNode *node;
    GeorgeGoalOwner *owner;
    if (func_00211BA8() == 0) {
        goal->field10 = 2;
    }
    object = func_00239FD8(goal->field14);
    node = func_00238BA0(object, 0x21F5A0EA);
    owner = OWNER(goal);
    func_001779F8(owner->field08, node, node->field88);
}

void func_001E1F98(GeorgeGoalWords *goal, u32 flags)
{
    goal->base.field0C = D_004371B8;
    func_002CD130((GeorgeDeimosPoolNode *)goal->field10);
    goal->field10 = 0;
    func_0020D2C0(&goal->base, flags);
}

/* Verified complete 36-byte linked GCC 3.2.3 match. */
void func_001E1FE8(GeorgeGoalWords *goal)
{
    func_002D02A8((GeorgeDeimosPoolNode *)goal->field10, 0, 0);
}

void func_001E59E0(GeorgeGoalWords *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    u32 word = goal->field10;
    GeorgeGoalEntity *entity = owner->field08;
    u32 context_word = owner->field7C;
    func_001B67C0(entity->field40, entity, context_word, word, 0, 1);
}

void func_001E5B08(GeorgeGoalWords *goal, u32 flags)
{
    goal->base.field0C = D_00437390;
    if (goal->field10 != 0) {
        func_001CAF88((GeorgeGoalReferencedObject *)goal->field10);
        goal->field10 = 0;
    }
    func_0020D2C0(&goal->base, flags);
}

void func_001E5BA0(GeorgeGoalPosition *goal, GeorgeGoalOutput *output)
{
    GeorgeGoalReferencedObject *object;
    output->field00 = 1;
    output->field10 = goal->field10;
    object = (GeorgeGoalReferencedObject *)goal->field10;
    if (object != 0) {
        if ((object->field06 & 0x40) != 0 || object->field20.word != 0) {
            const GeorgeMathVec3 *position = func_001CAFE0((GeorgeGoalReferencedObject *)goal->field10);
            output->field04.x = position->x;
            output->field04.y = position->y;
            output->field04.z = position->z;
        }
    } else {
        output->field04.x = goal->field14.x;
        output->field04.y = goal->field14.y;
        output->field04.z = goal->field14.z;
    }
}

void func_001E68D8(GeorgeGoalInteractionPosition *goal, GeorgeGoalOutput *output)
{
    output->field00 = 1;
    output->field04.x = goal->field18.x;
    output->field04.y = goal->field18.y;
    output->field04.z = goal->field18.z;
    output->field10 = 0;
}

void func_001E8580(GeorgeGoalEnterVehicle *goal, u32 flags)
{
    GeorgeGoalOwner *owner;
    goal->base.field0C = D_004375A0;
    if (goal->field10 != 0) {
        func_001CAF88((GeorgeGoalReferencedObject *)goal->field10);
        goal->field10 = 0;
    }
    if (goal->field14 != 0) {
        func_001F6148((void *)goal->field14);
        goal->field14 = 0;
    }
    owner = OWNER(goal);
    owner->field24 &= 0xFFBF;
    func_0020D2C0(&goal->base, flags);
}

/* Count is refreshed by a virtual call each iteration; only its argument narrows. */
void func_001E8818(GeorgeGoalWords *goal)
{
    if (goal->field14 != 0) {
        u32 index = 0;
        for (;;) {
            GeorgeGoalVirtualObject *object = (GeorgeGoalVirtualObject *)OWNER(goal)->field04;
            const GeorgeGoalVirtualInt *count_entry = (const GeorgeGoalVirtualInt *)(object->field04 + 0x1A0);
            s32 count = count_entry->invoke(ADJUST_THIS(object, count_entry));
            const GeorgeGoalVirtualEntity *entry;
            GeorgeGoalEntity *entity;
            if (!((s32)index < count)) {
                break;
            }
            object = (GeorgeGoalVirtualObject *)OWNER(goal)->field04;
            entry = (const GeorgeGoalVirtualEntity *)(object->field04 + 0x1B8);
            entity = entry->invoke(ADJUST_THIS(object, entry), (signed char)index);
            if (entity->field0C == 14 && func_0018FCF0(entity, 0x10000000) == 0) {
                func_001BAAD8(entity->field364, entity->field738);
            }
            index += 1;
        }
    } else {
        GeorgeGoalOwner *owner = OWNER(goal);
        u32 word = goal->field10;
        func_001BAAD8(word, owner->field08->field738);
    }
}

/* Verified complete 56-byte GCC 2.9 match; reload flags after the goal store. */
void func_001E8E88(GeorgeGoalWords *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    u32 enabled = goal->field10;
    u32 old = owner->field24 & 0x40;
    goal->field14 = old;
    if (enabled != 0) {
        owner->field24 |= 0x40;
    } else {
        owner->field24 &= 0xFFBF;
    }
}

void func_0020D2C0(GeorgeGoalBase *goal, u32 flags)
{
    goal->field0C = D_0043A2B0;
    if ((flags & 1) != 0) {
        func_002AF100(goal);
    }
}

void func_001CAF88(GeorgeGoalReferencedObject *object)
{
    u8 references = (u8)(object->field05 - 1);
    object->field05 = references;
    if (references == 0) {
        object->field06 |= 4;
        func_001CB180(object, 2);
        func_002AD748(D_0045C640, object);
    }
}

const GeorgeMathVec3 *func_001CAFE0(GeorgeGoalReferencedObject *object)
{
    s32 type = object->field04;
    if (type == 1) {
        return (const GeorgeMathVec3 *)(object->field20.word + 0x40U);
    } else if (type == 2) {
        GeorgeGoalVirtualObject *target = (GeorgeGoalVirtualObject *)object->field20.word;
        const GeorgeGoalVirtualVector *entry = (const GeorgeGoalVirtualVector *)(target->field04 + 0x28);
        return entry->invoke(ADJUST_THIS(target, entry));
    } else if (type == 3) {
        return &object->field20.position;
    } else if (type == 5) {
        return func_00120B68((void *)object->field20.word);
    }
    return 0;
}

#undef START_ACTION_BODY
#undef ACTION_DESTRUCTOR_BODY
#undef BASE_OWNER
#undef OWNER
#undef ADJUST_THIS
