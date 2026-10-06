#include "george/goal_methods2.h"

/* The software-double helpers pass complete 64-bit values in integer registers. */
typedef unsigned long long GeorgeGoalBits64;
typedef char goal_bits64_size[(sizeof(GeorgeGoalBits64) == 8) ? 1 : -1];

extern GeorgeGoalMember D_003F8D50[], D_003F8DD8[], D_003F8E38[];
extern GeorgeGoalMember D_003F8E68[], D_003F8EE0[];
extern u32 D_003F8DD0;
extern float func_0029B940(float value0, float value1);
extern s32 func_001CF588(void *context, u32 word);
extern s32 func_001D1760(void *context, u32 word, u32 byte);
extern GeorgeGoalRouteResult *func_001CF340(void *context);
extern GeorgeGoalRouteWord *func_001CE130(void *context, u8 *byte);
extern void func_001DDD70(GeorgeGoalDrive *goal);
extern void func_001DE6E0(GeorgeGoalDrive *goal);
extern void func_001DDBB8(GeorgeGoalDrive *goal);
extern s32 func_00177C40(GeorgeGoalEntity *entity, u32 word0, u32 word1,
                           u32 word2, u32 word3, s32 enabled, s32 byte,
                           float value);
extern s32 func_00397178(void);
extern s32 func_001B69E8(void *context);
extern s32 func_00211BA8(void);
extern void *func_00239FD8(u32 word);
extern void *func_00238BA0(void *object, u32 hash);
extern void func_00121A80(void *object, GeorgeMathVec3 *position,
                           GeorgeMathVec3 *scratch);
extern GeorgeGoalOwner *func_001BA610(u32 word);
extern GeorgeGoalBits64 func_00374848(float value);
extern s32 func_00373250(GeorgeGoalBits64 value0, GeorgeGoalBits64 value1);
extern GeorgeGoalBits64 func_00372CC0(GeorgeGoalBits64 value0,
                                       GeorgeGoalBits64 value1);

#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define BASE_OWNER(goal) ((GeorgeGoalOwner *)(goal)->links.unknown00)
#define ENTITY_POSITION(entity) ((GeorgeMathVec3 *)(entity)->field40)
#define ENTITY_MOTION(entity) ((GeorgeMathVec3 *)((entity)->field40 + 0xC))
#define ADJUST_THIS(object, adjustment) \
    ((void *)((u32)(object) + (u32)(s32)(adjustment)))

/* All four complete 136-byte timer functions match with the GCC 2.9 profile.
 * Their switch and XOR expressions reflect the original branch structure. */
#define ACTION_TIMER_BODY(wait_condition) \
    if (goal->field18 == 0) { \
        u32 phase = goal->field19; \
        switch (phase) { \
        case 0: { \
            GeorgeGoalEntity *entity = OWNER(goal)->field08; \
            if (wait_condition) goal->field19 = 1; \
            break; \
        } \
        case 1: { \
            GeorgeGoalOwner *owner = OWNER(goal); \
            float remaining = goal->field10 - owner->field60; \
            goal->field10 = remaining; \
            if (remaining < 0.0f) { \
                goal->field18 = phase; \
                owner->field24 &= 0xFFF7; \
            } \
            break; \
        } \
        } \
    } \
    return goal->field18

u32 func_001D99F8(GeorgeGoalTimedAction *goal)
{
    ACTION_TIMER_BODY((entity->field0C ^ 0x12u) != 0);
}

u32 func_001D9B78(GeorgeGoalTimedAction *goal)
{
    ACTION_TIMER_BODY((entity->field0C ^ 0x23u) != 0);
}

u32 func_001E0FA0(GeorgeGoalTimedAction *goal)
{
    ACTION_TIMER_BODY((entity->field2D8 ^ 1u) != 0);
}

u32 func_001E1188(GeorgeGoalTimedAction *goal)
{
    ACTION_TIMER_BODY((entity->field0C ^ 0x20u) != 0);
}

/* Scalar loads and 32-bit wrapping addresses mirror the old member-call ABI.
 * The adjustment sum remains 32-bit; m2c's draft narrowed it incorrectly.
 * index is evaluated again when loading the final adjustment. Drive passes a
 * field expression, while look/walk/enter pass their captured index. */
#define MEMBER_CALL(object, table, index) do { \
    const GeorgeGoalMember *entry = \
        (const GeorgeGoalMember *)((u32)(table) + (u32)(index) * 8u); \
    s32 selector = entry->selector; \
    if (selector != 0) { \
        GeorgeGoalBits64 packed; \
        void (*invoke)(void *); \
        s32 adjustment; \
        if (selector > 0) { \
            const u8 *vtable = *(const u8 **) \
                ADJUST_THIS(object, entry->target.vtable_offset); \
            packed = *(const GeorgeGoalBits64 *) \
                ((u32)vtable + (u32)selector * 8u - 8u); \
            invoke = (void (*)(void *))(u32)(packed >> 32); \
        } else { \
            invoke = entry->target.direct; \
        } \
        entry = (const GeorgeGoalMember *) \
            ((u32)(table) + (u32)(index) * 8u); \
        adjustment = entry->adjustment; \
        if (selector > 0) adjustment += (s16)(u16)packed; \
        invoke(ADJUST_THIS(object, adjustment)); \
    } \
} while (0)

u32 func_001D9D10(GeorgeGoalLook *goal)
{
    if (goal->field10 == 0) {
        u32 index = goal->field14;
        MEMBER_CALL(goal, D_003F8D50, index);
    }
    return goal->field10;
}

s32 func_001DB818(GeorgeGoalBase *goal)
{
    GeorgeGoalOwner *owner = BASE_OWNER(goal);
    GeorgeGoalEntity *entity = owner->field08;
    float x = ENTITY_MOTION(entity)->x;
    float z = ENTITY_MOTION(entity)->z;
    if (0.009999999776482582f < x * x + z * z) {
        entity = owner->field08;
        if ((u32)(entity->field0C - 3u) >= 2u) {
            GeorgeMathVec3 target;
            float angle;
            target.x = ENTITY_POSITION(entity)->x + ENTITY_MOTION(entity)->x;
            target.z = ENTITY_POSITION(entity)->z + ENTITY_MOTION(entity)->z;
            target.y = ENTITY_POSITION(entity)->y + ENTITY_MOTION(entity)->y;
            angle = func_001DC178(ENTITY_POSITION(owner->field08), &target);
            owner = BASE_OWNER(goal);
            if (owner->field2C == 0) {
                owner->field34 = angle;
                owner->field30 = 6.2831854820251465f;
            }
        }
    }
    return 0;
}

float func_001DC178(const GeorgeMathVec3 *origin, const GeorgeMathVec3 *target)
{
    GeorgeMathVec3 difference;
    float squared;
    difference.x = target->x - origin->x;
    difference.y = 0.0f;
    difference.z = target->z - origin->z;
    squared = difference.x * difference.x + difference.z * difference.z;
    if (0.000009999999747378752f < squared) {
        float inverse = 1.0f / squared;
        difference.z *= inverse;
        difference.x *= inverse;
    }
    return func_0029B940(difference.z, difference.x);
}

s32 func_001DC468(void *context, u32 *word18, u32 *word14, u32 *word20)
{
    u32 word = *word18;
    GeorgeGoalRouteResult *route;
    GeorgeGoalRouteWord *intersection;
    if (word != 0xFFFFFFFFu && func_001CF588(context, word) != 0)
        return 0;
    word = *word14;
    if (word != 0xFFFFFFFFu &&
        func_001D1760(context, word, *(u8 *)word20) != 0)
        return 1;
    route = func_001CF340(context);
    if (route != NULL) {
        *word18 = route->field40;
        return 0;
    }
    intersection = func_001CE130(context, (u8 *)word20);
    if (intersection == NULL) return 2;
    *word14 = intersection->field00;
    return 1;
}

s32 func_001DC618(GeorgeGoalRestart *goal)
{
    return goal->field10 < 0 ? 1 : 5;
}

void func_001DC6B8(GeorgeGoalWords *goal)
{
    /* Complete 20-byte match with GCC 3.2.3. */
    GeorgeGoalOwner *owner = OWNER(goal);
    u32 word = goal->field10;
    owner->field08->field368 = word;
}

s32 func_001DC750(GeorgeGoalWait *goal)
{
    /* Constructor writes this slot with sw; this method uses lwc1/swc1. */
    float remaining = *(float *)&goal->field14;
    if (0.0f < remaining) {
        GeorgeGoalOwner *owner = OWNER(goal);
        *(float *)&goal->field14 = remaining - owner->field60;
        return 0;
    }
    return 1;
}

u32 func_001DC988(GeorgeGoalDrive *goal)
{
    GeorgeGoalVirtualObject *object = (GeorgeGoalVirtualObject *)goal->field14;
    const GeorgeGoalVirtualInt *entry =
        (const GeorgeGoalVirtualInt *)(object->field04 + 0x138);
    if (entry->invoke(ADJUST_THIS(object, entry->adjustment)) == 0) {
        goal->field8C = 0;
    } else {
        if (goal->field8C == 0) D_003F8DD0 += 1u;
        goal->field8C = 1;
    }
    if (goal->field45 == 0) {
        func_001DDD70(goal);
        func_001DE6E0(goal);
        func_001DDBB8(goal);
        MEMBER_CALL(goal, D_003F8DD8, goal->field44);
    }
    return goal->field45;
}

u32 func_001DF630(GeorgeGoalWalkIntersection *goal)
{
    if (goal->field11 == 0) {
        u32 index = goal->field10;
        GeorgeGoalOwner *owner;
        float angle;
        MEMBER_CALL(goal, D_003F8E38, index);
        angle = func_001DC178(ENTITY_POSITION(OWNER(goal)->field08),
            (const GeorgeMathVec3 *)((u8 *)goal + 0x28));
        owner = OWNER(goal);
        if (owner->field2C == 0) {
            owner->field34 = angle;
            owner->field30 = 6.2831854820251465f;
        }
    }
    return goal->field11;
}

u32 func_001DFDB8(GeorgeGoalWalkRoad *goal)
{
    if (goal->field11 == 0) {
        u32 index = goal->field10;
        GeorgeGoalOwner *owner;
        float angle;
        MEMBER_CALL(goal, D_003F8E68, index);
        angle = func_001DC178(ENTITY_POSITION(OWNER(goal)->field08),
            (const GeorgeMathVec3 *)((u8 *)goal + 0x20));
        owner = OWNER(goal);
        if (owner->field2C == 0) {
            owner->field34 = angle;
            owner->field30 = 6.2831854820251465f;
        }
    }
    return goal->field11;
}

/* Each inline idle record is four words, indexed with a wrapping word shift. */
#define IDLE_RECORD(goal) ((const u32 *)((u32)(goal) + \
    (u32)(goal)->field98 * 16u + 0x10u))
#define IDLE_INVOKE(entity, goal, record) \
    func_00177C40(entity, (record)[0], (record)[1], (record)[2], (record)[3], \
        1, (signed char)(goal)->field9C, 5.0f)
#define IDLE_DECREMENT(goal) do { \
    if ((s32)(goal)->field94 > 0) (goal)->field94 -= 1u; \
    if ((goal)->field94 == 0) (goal)->field9F = 1; \
} while (0)

s32 func_001E07E0(GeorgeGoalIdle *goal)
{
    s32 state = (signed char)goal->field9E;
    if (state == 0) {
        const u32 *record = IDLE_RECORD(goal);
        if (IDLE_INVOKE(OWNER(goal)->field08, goal, record) == 0)
            goal->field9F = 2;
        goal->field9E = 3;
    } else if (state == 1) {
        GeorgeGoalEntity *entity = OWNER(goal)->field08;
        if (entity->field0C != 0xC) {
            const u32 *record = IDLE_RECORD(goal);
            if (IDLE_INVOKE(entity, goal, record) == 0) goal->field9F = 4;
            goal->field9E = 3;
        }
    } else if (state == 2) {
        if (OWNER(goal)->field08->field0C != 0xB) goal->field9E = 0;
    } else if (state == 3) {
        if (OWNER(goal)->field08->field0C != 0xC) {
            s32 count = (s32)goal->field90;
            if (count < 2) {
                IDLE_DECREMENT(goal);
            } else {
                s32 mode = (signed char)goal->field9D;
                if (mode == 0) {
                    if (goal->field98 == (u32)count - 1u) IDLE_DECREMENT(goal);
                    goal->field98 = (s32)(goal->field98 + 1u) %
                        (s32)goal->field90;
                } else if (mode == 1) {
                    s32 random = func_00397178();
                    goal->field98 = random % (s32)goal->field90;
                    IDLE_DECREMENT(goal);
                }
            }
            goal->field9E = 0;
        }
    }
    return (signed char)goal->field9F;
}

u32 func_001E1F20(GeorgeGoalWords *goal)
{
    /* Complete 40-byte match with GCC 2.9. */
    if ((OWNER(goal)->field08->field0C ^ 6u) != 0) goal->field10 = 1;
    return goal->field10;
}

s32 func_001E5A18(GeorgeGoalWords *goal)
{
    s32 result = 1;
    if (goal->field14 != 0 &&
        func_001B69E8(OWNER(goal)->field08->field40) != 0) result = 0;
    return result;
}

s32 func_001E5B60(GeorgeGoalWords *goal)
{
    GeorgeGoalReferencedObject *object = (GeorgeGoalReferencedObject *)goal->field10;
    s32 result = 1;
    if (object != NULL) {
        s32 available = 0;
        if ((object->field06 & 0x40) != 0 || object->field20.word != 0)
            available = 1;
        result = 4;
        if (available != 0) result = 1;
    }
    return result;
}

s32 func_001E6870(GeorgeGoalInteractionPosition *goal)
{
    if (func_00211BA8() != 0) {
        void *object = func_00238BA0(func_00239FD8(goal->field14), 0x21F5A0EA);
        if (object != NULL) {
            GeorgeMathVec3 scratch;
            func_00121A80(object, &goal->field18, &scratch);
            return 1;
        }
        return 2;
    }
    return 2;
}

u32 func_001E82D8(GeorgeGoalEnterVehicle *goal)
{
    if (goal->field3E == 0) {
        u32 index = goal->field3D;
        MEMBER_CALL(goal, D_003F8EE0, index);
    }
    return goal->field3E;
}

s32 func_001E8900(GeorgeGoalWords *goal)
{
    /* Complete 44-byte linked match with GCC 2.9. */
    return func_001BA610(goal->field10)->field08->field0C != 0xE;
}

s32 func_001E89E0(GeorgeGoalBase *goal)
{
    GeorgeGoalVehicle *vehicle = (GeorgeGoalVehicle *)BASE_OWNER(goal)->unknown00;
    GeorgeGoalBits64 value = func_00374848(vehicle->field18);
    if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value);
    if (func_00373250(value, 0x3FF0000000000000ULL) < 0) {
        const GeorgeGoalVirtualFloat *entry =
            (const GeorgeGoalVirtualFloat *)(vehicle->field04 + 0x20);
        entry->invoke(ADJUST_THIS(vehicle, entry->adjustment), 0.0f);
        return 1;
    }
    return 0;
}

s32 func_001E8DE0(GeorgeGoalWords *goal)
{
    /* Complete 60-byte match with GCC 3.2.3. */
    GeorgeGoalVehicle *vehicle = (GeorgeGoalVehicle *)goal->field10;
    GeorgeGoalVirtualObject *object = vehicle->field38;
    if (object != NULL) {
        const GeorgeGoalVirtualVoid *entry =
            (const GeorgeGoalVirtualVoid *)(object->field04 + 0x1E0);
        entry->invoke(ADJUST_THIS(object, entry->adjustment));
    }
    return 1;
}

void func_001E8EC0(GeorgeGoalWords *goal)
{
    /* Complete 44-byte match with GCC 2.9. */
    u32 saved = goal->field14;
    GeorgeGoalOwner *owner = OWNER(goal);
    if (saved != 0) owner->field24 |= 0x40;
    else owner->field24 &= 0xFFBF;
}

#undef IDLE_DECREMENT
#undef IDLE_INVOKE
#undef IDLE_RECORD
#undef MEMBER_CALL
#undef ACTION_TIMER_BODY
#undef ADJUST_THIS
#undef ENTITY_MOTION
#undef ENTITY_POSITION
#undef BASE_OWNER
#undef OWNER
