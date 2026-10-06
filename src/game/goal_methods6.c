#include "george/goal_methods6.h"
#include "george/pool_slots.h"

extern u8 D_003F8EC0[], D_00437700[], D_00437828[], D_00437878[];
extern GeorgeSlotPool D_0045C680;
extern void func_001F6138(GeorgeGoalVirtualObject *vehicle);
extern s32 func_00177270(GeorgeGoalEntity *entity, const GeorgeMathVec3 *target,
                         u32 word, float height);
extern void func_001E1028(GeorgeGoalTimedAction *goal);

#define ADDRESS(type, base, offset) ((type *)((u32)(base) + (u32)(offset)))
#define FIELD(type, base, offset) (*ADDRESS(type, base, offset))
#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define VEHICLE(goal) ((GeorgeGoalVirtualObject *)(goal)->field14)
#define CANDIDATE(goal, index) ADDRESS(GeorgeMathVec3, goal, 0x18U + (u32)(index) * 12U)
#define ADJUST(object, entry) ADDRESS(void, object, (s32)(entry)->adjustment)

void func_001D9C88(GeorgeGoalBase *goal)
{
    const GeorgeGoalVirtualWord *entry = ADDRESS(GeorgeGoalVirtualWord, goal->field0C, 8);
    entry->invoke(ADJUST(goal, entry), 2);
    func_002AD748(&D_0045C680, goal);
}

static __inline__ const GeorgeMathVec3 *vehicle_vector(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualVector *entry = ADDRESS(GeorgeGoalVirtualVector, object->field04, offset);
    return entry->invoke(ADJUST(object, entry));
}

static __inline__ float vehicle_scalar(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualFloatResult *entry = ADDRESS(GeorgeGoalVirtualFloatResult, object->field04, offset);
    return entry->invoke(ADJUST(object, entry));
}

static __inline__ float dot(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second)
{
    return (first->x * second->x + first->y * second->y) + first->z * second->z;
}

/* Each repeated retail path captures the output index before its first call,
 * reloads vehicle after every call, and reads both returned vectors only after
 * the scalar callback. Keeping the pointers, rather than early vector copies,
 * preserves mutations and aliases between callback outputs. */
static __inline__ void vehicle_candidate_base(GeorgeGoalEnterVehicle *goal,
    GeorgeGoalVirtualObject *first_vehicle, u32 index, s32 forward)
{
    const GeorgeMathVec3 *first = vehicle_vector(first_vehicle, 0x98);
    const GeorgeMathVec3 *second = vehicle_vector(VEHICLE(goal), 0x98);
    float distance = forward ? vehicle_scalar(VEHICLE(goal), 0x190) + 2.0f
                             : vehicle_scalar(VEHICLE(goal), 0x198) - 2.0f;
    const GeorgeMathVec3 *origin = ADDRESS(GeorgeMathVec3, first, 0x30);
    const GeorgeMathVec3 *direction = ADDRESS(GeorgeMathVec3, second, 0x20);
    float x = origin->x + distance * direction->x;
    float z = origin->z + distance * direction->z;
    float y = origin->y + distance * direction->y;
    GeorgeMathVec3 *output = CANDIDATE(goal, index);
    output->x = x; output->z = z; output->y = y;
}

static __inline__ void vehicle_candidate_lateral(GeorgeGoalEnterVehicle *goal, s32 negative)
{
    GeorgeGoalVirtualObject *object = VEHICLE(goal);
    u32 index = goal->field3F;
    const GeorgeMathVec3 *direction = vehicle_vector(object, 0x98);
    float distance = vehicle_scalar(VEHICLE(goal), 0x188) * 0.5f + 2.0f;
    GeorgeMathVec3 *output = CANDIDATE(goal, index);
    float x, z, y;
    if (negative) distance = -distance;
    x = output->x + distance * direction->x;
    z = output->z + distance * direction->z;
    y = output->y + distance * direction->y;
    output->z = z; output->x = x; output->y = y;
}

void func_001E7690(GeorgeGoalEnterVehicle *goal)
{
    GeorgeGoalReferencedObject *reference = (GeorgeGoalReferencedObject *)goal->field10;
    GeorgeGoalVirtualObject *object = (GeorgeGoalVirtualObject *)reference->field20.word;
    GeorgeGoalEntity *entity;
    const GeorgeMathVec3 *position, *basis;
    GeorgeMathVec3 delta, scratch;
    u32 side = 1, index, found = 0;
    if (object == NULL) { goal->field3E = 4; return; }
    goal->field14 = (u32)object;
    func_001F6138(object);
    object = VEHICLE(goal);
    entity = OWNER(goal)->field08;
    position = vehicle_vector(object, 0x28);
    delta.x = FIELD(float, entity, 0x40) - position->x;
    delta.z = FIELD(float, entity, 0x48) - position->z;
    delta.y = FIELD(float, entity, 0x44) - position->y;
    basis = vehicle_vector(VEHICLE(goal), 0x98);
    if (1.0f < dot(&delta, basis)) side = 0;
    index = FIELD(signed char, goal, 0x41) == 0 ? 1U : 0U;
    do {
        u8 *command = D_003F8EC0 + side * 16U + index * 4U;
        const GeorgeGoalVirtualCommand *test;
        object = VEHICLE(goal);
        test = ADDRESS(GeorgeGoalVirtualCommand, object->field04, 0x1B8);
        if (test->invoke(ADJUST(object, test), *(signed char *)command) == 0) {
            const GeorgeGoalVirtualCommandVector *get;
            u32 count = goal->field3F;
            u8 selected = *command;
            FIELD(u8, goal, 0x3C) = selected;
            object = VEHICLE(goal);
            get = ADDRESS(GeorgeGoalVirtualCommandVector, object->field04, 0xD8);
            get->invoke(ADJUST(object, get), selected, CANDIDATE(goal, count), &scratch);
            goal->field3F = (u8)(goal->field3F + 1U);
            selected = FIELD(u8, goal, 0x3C);
            found = 1;
            if ((side == 0 && (selected == 1 || selected == 3)) ||
                (side == 1 && (selected == 0 || selected == 2))) {
                s32 forward;
                GeorgeGoalVirtualObject *next;
                basis = vehicle_vector(VEHICLE(goal), 0x98);
                forward = 1.0f < dot(&delta, ADDRESS(GeorgeMathVec3, basis, 0x20));
                next = VEHICLE(goal);
                count = goal->field3F;
                vehicle_candidate_base(goal, next, count, forward);
                vehicle_candidate_lateral(goal, side == 0);
                count = (u8)(goal->field3F + 1U);
                next = VEHICLE(goal);
                goal->field3F = (u8)count;
                vehicle_candidate_base(goal, next, count, forward);
                vehicle_candidate_lateral(goal, side != 0);
                goal->field3F = (u8)(goal->field3F + 1U);
            }
        }
        index = (u32)(s32)(signed char)(index + 1U);
    } while ((index & 0xFFU) < 4U && found == 0);
    if (found == 0) goal->field3E = 2;
}

void func_001E8988(GeorgeGoalBase *goal)
{
    GeorgeGoalVirtualObject *vehicle = (GeorgeGoalVirtualObject *)
        ((GeorgeGoalOwner *)goal->links.unknown00)->unknown00;
    const GeorgeGoalVirtualFloat *stop = ADDRESS(GeorgeGoalVirtualFloat, vehicle->field04, 0x20);
    const GeorgeGoalVirtualWord *mode;
    stop->invoke(ADJUST(vehicle, stop), -1.0f);
    mode = ADDRESS(GeorgeGoalVirtualWord, vehicle->field04, 0x30);
    mode->invoke(ADJUST(vehicle, mode), 1);
}

GeorgeGameplayGoal *func_001E8A78(void *storage, void *owner,
    const GeorgeMathVec3 *position, u32 word, float height, float time)
{
    GeorgeGoalJumpTarget *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field20 = word; goal->field24 = height; goal->field2C = time;
    goal->base.field0C = D_00437700;
    goal->field30 = 2; goal->field31 = 0;
    goal->field10.x = position->x;
    goal->field10.y = position->y;
    goal->field10.z = position->z;
    goal->field1C = NULL;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E8B18(void *storage, void *owner,
    GeorgeGoalReferencedObject *object, u32 word, float height, float time)
{
    GeorgeGoalJumpTarget *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field20 = word; goal->field24 = height; goal->field2C = time;
    goal->field1C = object; goal->base.field0C = D_00437700;
    goal->field30 = 2; goal->field31 = 0;
    object->field05 = (u8)(object->field05 + 1U);
    return &goal->base.links;
}

void func_001E8BA8(GeorgeGoalJumpTarget *goal, u32 flags)
{
    GeorgeGoalReferencedObject *object;
    goal->base.field0C = D_00437700;
    object = goal->field1C;
    if (object != NULL) { func_001CAF88(object); goal->field1C = NULL; }
    OWNER(goal)->field24 &= 0xFFF7U;
    func_0020D2C0(&goal->base, flags);
}

void func_001E8C10(GeorgeGoalJumpTarget *goal)
{
    const GeorgeMathVec3 *target = NULL;
    GeorgeGoalReferencedObject *object = goal->field1C;
    float difference;
    if (object == NULL) target = &goal->field10;
    else if ((object->field06 & 0x40U) != 0 || object->field20.word != 0)
        target = func_001CAFE0(goal->field1C);
    if (target == NULL) { goal->field30 = 2; return; }
    difference = target->y - FIELD(float, OWNER(goal)->field08, 0x44);
    if (0.0f < difference) goal->field24 = difference + goal->field24;
    if (func_00177270(OWNER(goal)->field08, target, goal->field20, goal->field24) != 0) {
        GeorgeGoalOwner *owner = OWNER(goal);
        goal->field28 = goal->field2C;
        owner->field24 |= 8U;
        goal->field30 = 0;
    }
}

u32 func_001E8CF8(GeorgeGoalJumpTarget *goal)
{
    u32 phase;
    if (goal->field30 != 0) return goal->field30;
    phase = goal->field31;
    if (phase == 0) {
        if (OWNER(goal)->field08->field0C != 4U) goal->field31 = 1;
    } else if (phase == 1) {
        float time = goal->field28 - OWNER(goal)->field60;
        goal->field28 = time;
        if (time < 0.0f) goal->field30 = (u8)phase;
    }
    return goal->field30;
}

GeorgeGameplayGoal *func_001E8EF8(void *storage, void *owner,
    GeorgeGoalBase *first, GeorgeGoalBase *second)
{
    GeorgeGoalPair *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = first; goal->field14 = second;
    goal->base.field0C = D_00437828;
    goal->field1A = 1; goal->field18 = 1;
    return &goal->base.links;
}

void func_001E8F58(GeorgeGoalPair *goal, u32 flags)
{
    GeorgeGoalBase *second;
    goal->base.field0C = D_00437828;
    func_001D9C88(goal->field10);
    second = goal->field14;
    goal->field10 = NULL; goal->field18 = 0;
    func_001D9C88(second);
    goal->field14 = NULL; goal->field1A = 0;
    func_0020D2C0(&goal->base, flags);
}

/* These four pairs have identical complete instruction bodies; reuse one
 * typed template while retaining separate entry symbols and byte checks. */
#define PAIR_WORD(name) \
void name(GeorgeGoalPair *goal, u32 value) \
{ \
    GeorgeGoalBase *child = goal->field10; \
    const GeorgeGoalVirtualWord *entry = ADDRESS(GeorgeGoalVirtualWord, child->field0C, 0x10); \
    entry->invoke(ADJUST(child, entry), value); \
    child = goal->field14; \
    entry = ADDRESS(GeorgeGoalVirtualWord, child->field0C, 0x10); \
    entry->invoke(ADJUST(child, entry), value); \
}
PAIR_WORD(func_001E8FC0)
PAIR_WORD(func_001E92B8)
#undef PAIR_WORD

#define PAIR_OUTPUT(name) \
void name(GeorgeGoalPair *goal, GeorgeGoalOutput *output) \
{ \
    GeorgeGoalBase *child = goal->field10; \
    const GeorgeGoalVirtualOutput *entry = ADDRESS(GeorgeGoalVirtualOutput, child->field0C, 0x18); \
    entry->invoke(ADJUST(child, entry), output); \
    child = goal->field14; \
    entry = ADDRESS(GeorgeGoalVirtualOutput, child->field0C, 0x18); \
    entry->invoke(ADJUST(child, entry), output); \
}
PAIR_OUTPUT(func_001E9020)
PAIR_OUTPUT(func_001E9318)
#undef PAIR_OUTPUT

#define PAIR_STOP(name) \
void name(GeorgeGoalPair *goal) \
{ \
    GeorgeGoalBase *child = goal->field10; \
    const GeorgeGoalVirtualVoid *entry = ADDRESS(GeorgeGoalVirtualVoid, child->field0C, 0x20); \
    entry->invoke(ADJUST(child, entry)); \
    child = goal->field14; \
    entry = ADDRESS(GeorgeGoalVirtualVoid, child->field0C, 0x20); \
    entry->invoke(ADJUST(child, entry)); \
}
PAIR_STOP(func_001E9080)
PAIR_STOP(func_001E9378)
#undef PAIR_STOP

s32 func_001E90D0(GeorgeGoalPair *goal)
{
    s32 first = 0, second = 0;
    const GeorgeGoalVirtualInt *entry;
    GeorgeGoalBase *child;
    if (goal->field18 != 0) {
        child = goal->field10;
        entry = ADDRESS(GeorgeGoalVirtualInt, child->field0C, 0x28);
        first = entry->invoke(ADJUST(child, entry));
    }
    if (goal->field1A != 0) {
        child = goal->field14;
        entry = ADDRESS(GeorgeGoalVirtualInt, child->field0C, 0x28);
        second = entry->invoke(ADJUST(child, entry));
    }
    if (first != 0) goal->field18 = 0;
    if (second != 0) goal->field1A = 0;
    return first;
}

#define PAIR_ACTIVE(name) \
void name(GeorgeGoalPair *goal) \
{ \
    GeorgeGoalBase *child; \
    const GeorgeGoalVirtualVoid *entry; \
    if (goal->field18 != 0) { \
        child = goal->field10; \
        entry = ADDRESS(GeorgeGoalVirtualVoid, child->field0C, 0x30); \
        entry->invoke(ADJUST(child, entry)); \
    } \
    if (goal->field1A != 0) { \
        child = goal->field14; \
        entry = ADDRESS(GeorgeGoalVirtualVoid, child->field0C, 0x30); \
        entry->invoke(ADJUST(child, entry)); \
    } \
}
PAIR_ACTIVE(func_001E9168)
PAIR_ACTIVE(func_001E9468)
#undef PAIR_ACTIVE

GeorgeGameplayGoal *func_001E91D0(void *storage, void *owner,
    GeorgeGoalBase *first, GeorgeGoalBase *second)
{
    GeorgeGoalPair *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field10 = first; goal->field14 = second;
    goal->base.field0C = D_00437878;
    goal->field1A = 1; goal->field18 = 1;
    OWNER(goal)->field24 |= 0x200U;
    return &goal->base.links;
}

void func_001E9240(GeorgeGoalPair *goal, u32 flags)
{
    GeorgeGoalBase *second;
    GeorgeGoalOwner *owner;
    goal->base.field0C = D_00437878;
    func_001D9C88(goal->field10);
    second = goal->field14;
    goal->field10 = NULL; goal->field18 = 0;
    func_001D9C88(second);
    owner = OWNER(goal);
    goal->field14 = NULL; goal->field1A = 0;
    owner->field24 &= 0xFDFFU;
    func_0020D2C0(&goal->base, flags);
}

s32 func_001E93C8(GeorgeGoalPair *goal)
{
    s32 first = 0, second = 0;
    const GeorgeGoalVirtualInt *entry;
    GeorgeGoalBase *child;
    if (goal->field18 != 0) {
        child = goal->field10;
        entry = ADDRESS(GeorgeGoalVirtualInt, child->field0C, 0x28);
        first = entry->invoke(ADJUST(child, entry));
    }
    if (goal->field1A != 0) {
        child = goal->field14;
        entry = ADDRESS(GeorgeGoalVirtualInt, child->field0C, 0x28);
        second = entry->invoke(ADJUST(child, entry));
    }
    if (first != 0) goal->field18 = 0;
    if (second != 0) func_001E1028((GeorgeGoalTimedAction *)goal->field14);
    return first;
}
