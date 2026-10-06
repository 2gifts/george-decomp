#include "george/goal_methods4.h"
#include "george/accessors.h"
#include "george/ee_math.h"

extern GeorgeGoalMapOwner *D_003F8C28;
extern float D_003F8DB8[];
extern GeorgeGoalMember D_003F8DF8[];
extern GeorgeGoalRoad *func_001CCA28(u32 word);
extern GeorgeGenericMap *func_002A7F58(u32 buckets);
extern void func_002A7DC0(GeorgeGenericMap *map, u32 key);
extern void func_002A8070(GeorgeGenericMap *map);
extern u32 func_00297388(const void *resource);
extern void func_00297178(void *resource, u16 *index, float *position,
                            GeorgeMathVec3 *output);
extern void func_002A1C60(const void *matrix, const GeorgeMathVec3 *input,
                            GeorgeMathVec3 *output);
extern void func_002AF560(GeorgeDeimosRing *ring);
extern float func_002A3538(GeorgeMathVec3 *vector);
extern float func_002A35C0(GeorgeMathVec3 *output, const GeorgeMathVec3 *input, float scale);
extern s32 func_0016ECE0(void *object);
extern s32 func_001CF588(void *context, u32 word);
extern s32 func_001D1760(void *context, u32 word, u32 index);
extern GeorgeGoalRouteResult *func_001CF340(void *context);
extern GeorgeGoalRouteWord *func_001CE130(void *context, u8 *index);
extern float func_001D0EE0(u32 word, u32 index, s32 mode, s32 reverse,
                            const GeorgeMathVec3 *position);
extern s32 func_001D11A8(GeorgeGoalIntersectionRecord *first,
                          GeorgeGoalIntersectionRecord *second, s32 mode, s32 reverse);
extern float func_001D0010(GeorgeGoalRoadGeometry *road, u32 word, u32 previous,
                            s32 mode, u8 *output_mode, signed char *output_index,
                            u32 *output_word, u8 *output_reverse);
extern GeorgeMathVec3 D_00436BE8;
extern void func_002A1C30(float *matrix);
extern void func_002A0B00(GeorgeMathVec4 *output, const float *matrix);
extern float func_0029C230(float value);
extern void func_001FF6D0(void *object, const GeorgeMathVec4 *value);
extern void func_001FF6F8(void *object, const GeorgeMathVec3 *value);

#define ADDRESS(type, base, offset) ((type *)((u32)(base) + (u32)(offset)))
#define VIEW(input) ((GeorgeGoalDriveMotion *)(input))
#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define FIELD(type, base, offset) (*ADDRESS(type, base, offset))
#define OWNER_BYTE(owner, offset) FIELD(u8, owner, offset)
#define OWNER_SIGNED_BYTE(owner, offset) FIELD(signed char, owner, offset)
#define ADJUST(object, offset) ((void *)((u32)(object) + (u32)(s32)(offset)))
#define VIRTUAL(type, object, offset) ((const type *)ADDRESS(u8, (object)->field04, offset))

static __inline__ GeorgeGoalRoadGeometry *lookup_road(u32 word)
{
    return (GeorgeGoalRoadGeometry *)func_001CCA28(word);
}

static __inline__ GeorgeGoalIntersectionRecord *record34(GeorgeGoalRoadGeometry *road, u32 word)
{
    return ADDRESS(GeorgeGoalIntersectionRecord, road->field38, (word & 0xFFFFU) * 0x34U);
}

static __inline__ GeorgeGoalRoadEntry *record60(GeorgeGoalRoadGeometry *road, u32 word)
{
    return ADDRESS(GeorgeGoalRoadEntry, road->field3C, (word & 0xFFFFU) * 0x60U);
}

static __inline__ GeorgeGoalRouteQueueEntry *queue_entry(GeorgeGoalDriveMotion *goal, s32 index)
{
    return (GeorgeGoalRouteQueueEntry *)func_002AF208(goal->field40, (u32)index);
}

static __inline__ const GeorgeMathVec3 *resource_position(GeorgeGoalVirtualObject *object)
{
    const GeorgeGoalVirtualVector *method = VIRTUAL(GeorgeGoalVirtualVector, object, 0x28);
    return method->invoke(ADJUST(object, method->adjustment));
}

static __inline__ s32 resource_predicate(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualInt *method = VIRTUAL(GeorgeGoalVirtualInt, object, offset);
    return method->invoke(ADJUST(object, method->adjustment));
}

static __inline__ const GeorgeMathVec3 *resource_vector(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualVector *method = VIRTUAL(GeorgeGoalVirtualVector, object, offset);
    return method->invoke(ADJUST(object, method->adjustment));
}

static __inline__ void resource_word(GeorgeGoalVirtualObject *object, u32 offset, u32 value)
{
    const GeorgeGoalVirtualWord *method = VIRTUAL(GeorgeGoalVirtualWord, object, offset);
    method->invoke(ADJUST(object, method->adjustment), value);
}

static __inline__ void resource_scalar(GeorgeGoalVirtualObject *object, u32 offset, float value)
{
    const GeorgeGoalVirtualFloat *method = VIRTUAL(GeorgeGoalVirtualFloat, object, offset);
    method->invoke(ADJUST(object, method->adjustment), value);
}

static __inline__ float resource_float(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualFloatResult *method = VIRTUAL(GeorgeGoalVirtualFloatResult, object, offset);
    return method->invoke(ADJUST(object, method->adjustment));
}

static __inline__ void resource_void(GeorgeGoalVirtualObject *object, u32 offset)
{
    const GeorgeGoalVirtualVoid *method = VIRTUAL(GeorgeGoalVirtualVoid, object, offset);
    method->invoke(ADJUST(object, method->adjustment));
}

static __inline__ float dot(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second)
{
    return (first->x * second->x + first->y * second->y) + first->z * second->z;
}

/* The original uses reciprocal squared length, not reciprocal square root. */
static __inline__ void reciprocal_squared(GeorgeMathVec3 *vector)
{
    float x = vector->x, y = vector->y, z = vector->z;
    float squared = (x * x + y * y) + z * z;
    if (0.000009999999747378752f < squared) {
        float inverse = 1.0f / squared;
        vector->z = z * inverse; vector->x = x * inverse; vector->y = y * inverse;
    }
}

static __inline__ float record_weight(GeorgeGoalIntersectionRecord *record, s32 mode)
{
    s32 adjusted = mode + 4;
    s32 index = adjusted / 2;
    float first = FIELD(float, record, 0x20U + (u32)index * 4U);
    if ((adjusted & 1) == 0) return first;
    return (first + FIELD(float, record, 0x20U + (u32)(index + 1) * 4U)) * 0.5f;
}

void func_001CCAA8(u32 word, u32 key)
{
    GeorgeGoalMapOwner *manager = D_003F8C28;
    GeorgeGenericMap *map = (GeorgeGenericMap *)func_002A7C08(manager->field300, key);
    if (map != NULL) {
        if (func_002A7C08(map, word) == NULL)
            func_002A7CD0(map, word, (void *)word);
    } else {
        map = func_002A7F58(10);
        func_002A7CD0(map, word, (void *)word);
        func_002A7CD0(manager->field300, key, map);
    }
}

void func_001CCB58(u32 word, u32 key)
{
    GeorgeGoalMapOwner *manager = D_003F8C28;
    GeorgeGenericMap *map = (GeorgeGenericMap *)func_002A7C08(manager->field300, key);
    if (map != NULL) {
        /* Retail makes this lookup even though its result is unused. */
        func_002A7C08(map, word);
        func_002A7DC0(map, word);
        if (func_002A8128(map) == 0) {
            func_002A8070(map);
            func_002A7DC0(manager->field300, key);
        }
    }
}

GeorgeGenericMap *func_001CCBE8(u32 key)
{
    GeorgeGoalMapOwner *manager = D_003F8C28;
    return (GeorgeGenericMap *)func_002A7C08(manager->field300, key);
}

s32 func_001CFB20(GeorgeMathVec3 *output, u32 word, u32 index)
{
    u32 narrowed = (u8)index;
    GeorgeGoalRoadGeometry *road = lookup_road(word);
    GeorgeGoalIntersectionRecord *record;
    GeorgeMathVec3 *first, *second;
    float ax, ay, az, bx, by, bz;
    if (road == NULL) return 0;
    record = record34(road, word);
    first = ADDRESS(GeorgeMathVec3, road->field48,
                    (FIELD(u16, record, 0x12) + narrowed) * 12U);
    second = ADDRESS(GeorgeMathVec3, road->field48,
                     (record->field10 + narrowed) * 12U);
    ax = first->x; ay = first->y; az = first->z;
    bx = second->x; by = second->y; bz = second->z;
    output->y = bz * ax - bx * az;
    output->z = bx * ay - by * ax;
    output->x = by * az - bz * ay;
    return 1;
}

s32 func_001D1A30(GeorgeGoalRoadGeometry *road, u32 word, const u8 *selector)
{
    GeorgeGoalRoadEntry *record = record60(road, word);
    u8 byte;
    u32 high, shift;
    void *block, *first, *second;
    if (FIELD(u32, record, 0x5C) != 0) return 1;
    byte = *selector;
    high = (u8)((s32)(signed char)byte >> 4);
    shift = (high * 5U + (byte & 15U)) & 31U;
    block = ADDRESS(u8, road->field40, FIELD(u32, record, 0x44));
    first = FIELD(void *, block, 0xC0);
    second = FIELD(void *, first, 0);
    return (FIELD(u32, second, 4) & (1U << shift)) != 0;
}

s32 func_001D1AA0(GeorgeGoalRoadGeometry *road, u32 word, u32 *output,
                   const signed char *index)
{
    GeorgeGoalRoadEntry *record = record60(road, word);
    void *block = ADDRESS(u8, road->field40, FIELD(u32, record, 0x44));
    u8 *cursor = ADDRESS(u8, block, 0xC4);
    u32 count = 0;
    if (FIELD(u8, block, 5) != 0) do {
        if (count == (u32)(s32)*index) {
            *output = FIELD(u32, cursor, 0);
            return 1;
        }
        {
            u32 size = func_00297388(FIELD(void *, cursor, 0));
            u32 limit = FIELD(u8, block, 5);
            count = (u8)(count + 1U);
            cursor = ADDRESS(u8, cursor, size + 8U);
            if (count >= limit) break;
        }
    } while (1);
    return 0;
}

void func_001DCAA8(GeorgeGoalDrive *input, GeorgeGoalRoadGeometry *road)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalRouteQueueEntry *entry = queue_entry(goal, goal->field32);
    u32 word = entry->field04 & 0xFFFFU;
    u32 index;
    GeorgeGoalIntersectionRecord *record;
    GeorgeMathVec3 *position, *direction;
    float scalar, x, y, z;
    if (road == NULL) {
        road = lookup_road(entry->field04);
        if (road == NULL) goal->field45 = 2;
        /* The original continues here, including the failing lookup path. */
    }
    index = entry->field0A == 0 ? (u8)(goal->field30 + 1U) : goal->field30;
    record = record34(road, word);
    position = ADDRESS(GeorgeMathVec3, road->field48,
                       (FIELD(u16, record, 6) + index) * 12U);
    direction = ADDRESS(GeorgeMathVec3, road->field48,
                        (FIELD(u16, record, 0x12) + index) * 12U);
    scalar = (float)entry->field08;
    scalar += scalar;
    x = position->x + scalar * direction->x;
    z = position->z + scalar * direction->z;
    y = position->y + scalar * direction->y;
    goal->field24.x = x; goal->field24.z = z; goal->field24.y = y;
}

s32 func_001DCBD8(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeMathVec3 delta;
    const GeorgeMathVec3 *position;
    float distance, length;
    func_001DDA70(input);
    position = resource_position(goal->field14);
    delta.y = position->y - goal->field18.y;
    delta.z = position->z - goal->field18.z;
    delta.x = position->x - goal->field18.x;
    distance = func_002A3538(&delta);
    delta.y = goal->field24.y - goal->field18.y;
    delta.z = goal->field24.z - goal->field18.z;
    delta.x = goal->field24.x - goal->field18.x;
    length = func_002A3538(&delta);
    if (length < 0.5f) return 0;
    if (distance < 10.0f) {
        float blend = george_ee_minimum(goal->field3C * OWNER(goal)->field60, 1.0f);
        float x = goal->field18.x + blend * delta.x;
        float z = goal->field18.z + blend * delta.z;
        float y = goal->field18.y + blend * delta.y;
        goal->field18.x = x; goal->field18.z = z; goal->field18.y = y;
        return 2;
    }
    return 1;
}

void func_001DCD48(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalRouteQueueEntry *entry;
    GeorgeGoalRoadGeometry *road;
    u32 word, action;
    if (func_001DCBD8(input) != 0) return;
    entry = queue_entry(goal, goal->field32);
    word = entry->field04 & 0xFFFFU;
    road = lookup_road(entry->field04);
    if (road == NULL) { goal->field45 = 2; return; }
    if (entry->field0A == 0) {
        u32 index = goal->field30;
        if ((s32)index < (s32)record34(road, word)->field05 - 2) {
            goal->field30 = (u8)(index + 1U); action = 1;
        } else {
            goal->field32 = (signed char)((u8)goal->field32 + 1U); action = 3;
        }
    } else if (goal->field30 != 0) {
        goal->field30 = (u8)(goal->field30 - 1U); action = 1;
    } else {
        goal->field32 = (signed char)((u8)goal->field32 + 1U); action = 3;
    }
    if ((s32)goal->field32 >= (s32)goal->field40->count) {
        FIELD(u32, goal, 0x34) = 0; goal->field31 = 1;
        FIELD(u32, goal, 0x38) = 0; FIELD(u32, goal, 0x3C) = 0;
        return;
    }
    if (action == 1) {
        u32 index, packed, nibble;
        goal->field31 = 0;
        index = goal->field30;
        packed = *ADDRESS(u8, FIELD(void *, road, 0x2C),
                    FIELD(u16, record34(road, word), 0x14) + (index >> 1));
        nibble = (index & 1U) == 0 ? packed >> 4 : packed & 15U;
        {
            float speed = D_003F8DB8[nibble];
            goal->field38 = speed;
            goal->field34 = speed;
        }
    } else {
        entry = queue_entry(goal, goal->field32);
        if (entry->field0B == 0) {
            goal->field31 = 2; goal->field38 = 10.0f; goal->field34 = 10.0f;
            return;
        }
        road = lookup_road(entry->field04);
        if (road == NULL) { goal->field45 = 2; return; }
        if (entry->field0A == 0) goal->field30 = 0;
        else goal->field30 = (u8)(record34(road, entry->field04)->field05 - 2);
        goal->field31 = 0;
        goal->field38 = goal->field34;
    }
    func_001DCAA8(input, road);
}

void func_001DF290(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalRouteQueueEntry *entry = queue_entry(goal, goal->field32);
    GeorgeGoalRoadGeometry *road = lookup_road(entry->field04);
    if (road != NULL && func_001D1A30(road, entry->field04, (u8 *)&entry->field0A) != 0) {
        GeorgeGoalRoadEntry *record;
        func_001D1AA0(road, entry->field04, (u32 *)&goal->field60, &entry->field08);
        record = record60(road, entry->field04);
        goal->field31 = 4; goal->field38 = goal->field34;
        goal->field4A = 0; FIELD(u32, goal, 0x4C) = 0;
        goal->field64 = record;
    } else goal->field31 = 3;
}

void func_001DCF50(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalRouteQueueEntry *entry;
    GeorgeGoalRoadGeometry *road;
    void *resource, *last;
    float end, start, position;
    u32 stride, count;
    if (func_001DCBD8(input) == 1) return;
    entry = queue_entry(goal, goal->field32);
    if (lookup_road(entry->field04) == NULL) { goal->field45 = 2; return; }
    goal->field4C += goal->field3C * OWNER(goal)->field60;
    func_00297178(goal->field60, &goal->field4A, &goal->field4C, &goal->field24);
    func_002A1C60(goal->field64, &goal->field24, &goal->field24);
    resource = goal->field60;
    count = FIELD(u16, resource, 4);
    stride = FIELD(u8, resource, 1);
    last = ADDRESS(u8, resource, (count - 1U) * stride * 4U);
    if (FIELD(u8, resource, 0) == 0x10)
        end = (float)FIELD(u8, last, 0xF) * 160.0f;
    else end = FIELD(float, last, 8);
    resource = goal->field60;
    position = goal->field4C;
    if (FIELD(u8, resource, 0) == 0x10)
        start = (float)FIELD(u8, resource, 0xF) * 160.0f;
    else start = FIELD(float, resource, 8);
    if (end - start <= position) {
        s32 index = (signed char)((u8)goal->field32 + 1U);
        goal->field32 = index;
        entry = queue_entry(goal, index);
        road = lookup_road(entry->field04);
        if (road == NULL) { goal->field45 = 2; return; }
        if (entry->field0A == 0) goal->field30 = 0;
        else goal->field30 = (u8)(record34(road, entry->field04)->field05 - 2);
        func_001DCAA8(input, NULL);
        goal->field31 = 0; goal->field60 = NULL;
    }
}

void func_001DF360(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    if (resource_predicate(goal->field14, 0x88) != 0) {
        const GeorgeGoalVirtualWord *method;
        if (goal->field47 != 0) { goal->field45 = 2; return; }
        func_002AF560(goal->field40);
        method = ADDRESS(GeorgeGoalVirtualWord, goal->base.field0C, 0x10);
        method->invoke(ADJUST(goal, method->adjustment), 0);
        goal->field46 = 0; FIELD(u32, goal, 0x3C) = 0;
    }
}

void func_001DDA70(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    float target = goal->field38, current = goal->field3C;
    if (current < target) {
        float next, bound;
        FIELD(u32, goal->field14, 0x12C) = 0;
        current = goal->field3C;
        {
            float delta = OWNER(goal)->field60;
            next = current + (delta + delta);
        }
        bound = goal->field38;
        goal->field48 &= 0xFD;
        goal->field3C = next;
        if (bound < next) goal->field3C = bound;
    } else if (target < current) {
        float rate = 5.0f;
        if (target < current - rate) FIELD(u32, goal->field14, 0x12C) = 1;
        if ((goal->field48 & 2) != 0) rate = 20.0f;
        goal->field3C = goal->field3C - OWNER(goal)->field60 * rate;
        if (goal->field3C < goal->field38) goal->field3C = goal->field38;
    }
    {
        GeorgeGoalVirtualObject *object = goal->field14;
        const GeorgeGoalVirtualFloatResult *method = VIRTUAL(GeorgeGoalVirtualFloatResult, object, 0x190);
        float value = method->invoke(ADJUST(object, method->adjustment));
        current = goal->field3C;
        goal->field54 = current * (current * 0.20000000298023224f) + 5.0f + value;
    }
}

void func_001DDBB8(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalOwner *owner;
    if (resource_predicate(goal->field14, 0x138) != 0 && goal->field46 == 0) {
        GeorgeGoalRouteQueueEntry *entry = queue_entry(goal, 0);
        GeorgeGoalVirtualObject *object;
        const GeorgeGoalVirtualPointer *pointer;
        const GeorgeGoalVirtualWord *word;
        const GeorgeGoalVirtualFloat *scalar;
        u32 key = entry->field04;
        if (entry->field0B == 0) key |= 0x80000000U;
        func_001CCB58((u32)goal, key);
        owner = OWNER(goal);
        goal->field58 = 0.5f; goal->field48 &= 0xFE;
        owner->field24 |= 4;
        object = goal->field14;
        pointer = VIRTUAL(GeorgeGoalVirtualPointer, object, 0x80);
        object = pointer->invoke(ADJUST(object, pointer->adjustment));
        goal->field10 = object;
        goal->field49 = (u8)(goal->field49 + 1U);
        word = VIRTUAL(GeorgeGoalVirtualWord, object, 0x30);
        word->invoke(ADJUST(object, word->adjustment), 1);
        object = goal->field10;
        scalar = VIRTUAL(GeorgeGoalVirtualFloat, object, 0x20);
        scalar->invoke(ADJUST(object, scalar->adjustment), 0.0f);
        goal->field44 = 1; goal->field5C = 5.0f; goal->field46 = 1;
    }
    owner = OWNER(goal);
    if ((owner->field24 & 3U) != 0) {
        FIELD(u32, goal, 0x38) = 0;
        goal->field48 |= 2; goal->field58 = 0.5f;
        owner->field24 &= 0xFFFE;
    }
    if (goal->field58 < 0.0f) {
        goal->field58 = 0.0f; goal->field38 = goal->field34;
    } else if (0.0f < goal->field58) goal->field58 -= OWNER(goal)->field60;
}

void func_001DD128(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    const GeorgeGoalMember *entry = ADDRESS(GeorgeGoalMember, D_003F8DF8,
                                           (u32)(s32)goal->field31 * 8U);
    s32 selector = entry->selector;
    if (selector != 0) {
        unsigned long long packed;
        void (*invoke)(void *);
        s32 adjustment;
        if (selector > 0) {
            const u8 *table = FIELD(const u8 *, goal, entry->target.vtable_offset);
            packed = FIELD(unsigned long long, table, (u32)selector * 8U - 8U);
            invoke = (void (*)(void *))(u32)(packed >> 32);
        } else invoke = entry->target.direct;
        entry = ADDRESS(GeorgeGoalMember, D_003F8DF8, (u32)(s32)goal->field31 * 8U);
        adjustment = entry->adjustment;
        if (selector > 0) adjustment += (s16)(u16)packed;
        invoke(ADJUST(goal, adjustment));
    }
    func_001DE228(input);
}

void func_001DD1E0(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    s32 count = func_0016ECE0(goal->field10);
    const GeorgeMathVec3 *first = resource_vector(goal->field14, 0x38);
    const GeorgeMathVec3 *second = resource_vector(goal->field14, 0x38);
    float length = george_ee_square_root(dot(first, second));
    float remaining = goal->field5C - OWNER(goal)->field60;
    GeorgeGoalOwner *owner;
    void *context;
    u32 word;
    s32 action;
    goal->field5C = remaining;
    if (!(length < 0.20000000298023224f) || count < 3 || !(remaining < 0.0f)) return;
    goal->field47 = 0;
    context = (void *)resource_position(goal->field14);
    owner = OWNER(goal);
    word = owner->field18.bits;
    if (word != 0xFFFFFFFFU && func_001CF588(context, word) != 0) action = 0;
    else {
        word = owner->field14;
        if (word != 0xFFFFFFFFU && func_001D1760(context, word, OWNER_BYTE(owner, 0x20)) != 0)
            action = 0;
        else if (func_001CF340(context) != NULL) action = 1;
        else {
            GeorgeGoalRouteWord *route = func_001CE130(context, ADDRESS(u8, owner, 0x20));
            action = 3;
            if (route != NULL && (FIELD(u8, route, 4) & 2U) == 0) {
                action = 2; owner->field14 = route->field00;
            }
        }
    }
    if (action == 2) {
        func_002AF560(goal->field40);
        func_001DE928(input);
        FIELD(u32, goal, 0x3C) = 0;
    }
    if (action == 3) {
        owner = OWNER(goal);
        goal->field47 = 1; owner->field24 &= 0xFFFB;
        resource_void(goal->field14, 0x90);
        goal->field44 = 3; goal->field10 = NULL;
        goal->field49 = (u8)(goal->field49 - 1U);
    } else {
        GeorgeMathVec3 delta;
        const GeorgeMathVec3 *position = resource_position(goal->field14);
        const GeorgeMathVec3 *basis;
        delta.y = goal->field80.y - position->y;
        delta.x = goal->field80.x - position->x;
        delta.z = goal->field80.z - position->z;
        reciprocal_squared(&delta);
        basis = resource_vector(goal->field14, 0x98);
        if (0.9900000095367432f < dot(&delta, ADDRESS(GeorgeMathVec3, basis, 0x20))) {
            OWNER(goal)->field24 &= 0xFFFB;
            resource_void(goal->field14, 0x90);
            goal->field44 = 3; goal->field10 = NULL;
            goal->field49 = (u8)(goal->field49 - 1U);
        } else goal->field44 = 2;
    }
}

void func_001DD580(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeMathVec3 delta;
    const GeorgeMathVec3 *position = resource_position(goal->field14), *basis, *first, *second;
    s32 count;
    float side, forward, motion_forward, speed, control;
    delta.y = goal->field80.y - position->y;
    delta.x = goal->field80.x - position->x;
    delta.z = goal->field80.z - position->z;
    reciprocal_squared(&delta);
    count = func_0016ECE0(goal->field10);
    basis = resource_vector(goal->field14, 0x98);
    side = dot(&delta, basis);
    basis = resource_vector(goal->field14, 0x98);
    forward = dot(&delta, ADDRESS(GeorgeMathVec3, basis, 0x20));
    first = resource_vector(goal->field14, 0x38);
    basis = resource_vector(goal->field14, 0x98);
    motion_forward = dot(first, ADDRESS(GeorgeMathVec3, basis, 0x20));
    first = resource_vector(goal->field14, 0x38);
    second = resource_vector(goal->field14, 0x38);
    speed = george_ee_square_root(dot(first, second));
    if (0.0f < goal->field38) {
        if (speed < 2.799999952316284f) {
            resource_word(goal->field10, 0x68, 1);
            resource_word(goal->field10, 0x30, 0);
            control = 1.0f;
        } else if (3.200000047683716f < speed && 0.0f < motion_forward) {
            resource_word(goal->field10, 0x30, 1); control = -1.0f;
        } else {
            resource_word(goal->field10, 0x30, 0); control = 0.0f;
        }
    } else {
        resource_word(goal->field10, 0x30, 1);
        control = 0.0f < speed && 0.0f < motion_forward ? -1.0f : 0.0f;
    }
    resource_scalar(goal->field10, 0x20, control);
    if (0.949999988079071f < forward) {
        if (0.9990000128746033f < forward && count >= 3) {
            OWNER(goal)->field24 &= 0xFFFB;
            resource_void(goal->field14, 0x90);
            goal->field44 = 3; goal->field10 = NULL;
            goal->field49 = (u8)(goal->field49 - 1U);
            return;
        }
        control = side * 10.0f;
        if (-1.0f <= control) control = george_ee_minimum(control, 1.0f);
        else control = -1.0f;
        resource_scalar(goal->field10, 0x28, control);
    } else resource_scalar(goal->field10, 0x28, 0.0f < side ? 1.0f : -1.0f);
}

void func_001DE928(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    u32 original_word = OWNER(goal)->field14 & 0xFFFFU;
    GeorgeGoalRoadGeometry *road = lookup_road(OWNER(goal)->field14);
    GeorgeGoalOwner *owner;
    GeorgeGoalIntersectionRecord *record;
    GeorgeGoalRouteQueueEntry entry;
    u32 index;
    s32 mode, reverse;
    const GeorgeMathVec3 *position;
    GeorgeMathVec3 *a, *b;
    float scalar, x, y, z;
    if (road == NULL) { goal->field45 = 2; return; }
    owner = OWNER(goal);
    record = record34(road, owner->field14);
    entry.field00 = (u32)goal;
    FIELD(u32, goal, 0x50) = 0; FIELD(u32, goal, 0x54) = 0;
    goal->field44 = 0; goal->field45 = 0;
    entry.field04 = owner->field14;
    entry.field08 = OWNER_BYTE(owner, 0x22);
    entry.field09 = OWNER_BYTE(owner, 0x23);
    entry.field0A = OWNER_BYTE(owner, 0x21); entry.field0B = 1;
    index = OWNER_BYTE(owner, 0x20);
    mode = OWNER_SIGNED_BYTE(owner, 0x22);
    reverse = OWNER_SIGNED_BYTE(owner, 0x21);
    position = resource_position(goal->field14);
    entry.field0C = func_001D0EE0(owner->field14, index, mode, reverse, position);
    entry.field10 = 30.0f;
    entry.field14 = record_weight(record, OWNER_SIGNED_BYTE(OWNER(goal), 0x22));
    func_002AF470(goal->field40, &entry);
    func_001DEC18(input);
    position = resource_position(goal->field14);
    goal->field18.x = position->x;
    goal->field18.y = position->y;
    goal->field18.z = position->z;
    index = OWNER_BYTE(OWNER(goal), 0x20);
    goal->field31 = 0; goal->field32 = 0; goal->field30 = index;
    goal->field3C = 7.5f; goal->field34 = 7.5f; goal->field38 = 7.5f;
    func_001DCAA8(input, NULL);
    owner = OWNER(goal);
    if (OWNER_BYTE(owner, 0x21) == 0) index = (u8)(OWNER_BYTE(owner, 0x20) + 1U);
    else index = OWNER_BYTE(owner, 0x20);
    record = record34(road, original_word);
    a = ADDRESS(GeorgeMathVec3, road->field48, (FIELD(u16, record, 6) + index) * 12U);
    b = ADDRESS(GeorgeMathVec3, road->field48, (FIELD(u16, record, 0x12) + index) * 12U);
    scalar = (float)OWNER_SIGNED_BYTE(OWNER(goal), 0x22); scalar += scalar;
    x = a->x + scalar * b->x; z = a->z + scalar * b->z; y = a->y + scalar * b->y;
    goal->field80.x = x; goal->field80.z = z; goal->field80.y = y;
    owner = OWNER(goal);
    func_001D18A8(&goal->field74, owner->field14, OWNER_BYTE(owner, 0x20), OWNER_BYTE(owner, 0x21));
    owner = OWNER(goal);
    func_001CFB20(&goal->field68, owner->field14, OWNER_BYTE(owner, 0x20));
}

float func_001DEC18(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeGoalRouteQueueEntry previous, entry;
    GeorgeGoalRoadGeometry *road;
    GeorgeGoalIntersectionRecord *record, *next_record;
    s32 next;
    float total = 0.0f;
    func_002AF398(goal->field40, &previous);
    road = lookup_road(previous.field04);
    if (road == NULL) { goal->field45 = 2; return 100000000.0f; }
    record = record34(road, previous.field04);
    next = previous.field0A == 0 ? (s32)record->field1C : (s32)record->field18;
    if (next == -1) return 100000000.0f;
    road = lookup_road((u32)next);
    if (road == NULL) { goal->field45 = 2; return 100000000.0f; }
    if (next >= 0) {
        s32 mode;
        float weight;
        next_record = record34(road, (u32)next);
        if ((next_record->field04 & 2U) != 0) return 100000000.0f;
        entry.field04 = (u32)next; entry.field00 = (u32)goal;
        mode = (signed char)func_001D11A8(record, next_record, previous.field08, previous.field0A);
        entry.field09 = previous.field08; entry.field0A = previous.field0A;
        entry.field0B = 1; entry.field08 = mode;
        FIELD(u32, &entry, 0xC) = 0; FIELD(u32, &entry, 0x10) = 0;
        weight = record_weight(next_record, mode);
        total = total + weight; entry.field14 = weight;
        func_002AF470(goal->field40, &entry);
        return total;
    } else {
        u8 mode, reverse;
        signed char index = -100;
        u32 output_word = 0xFFFFFFFFU;
        s32 selector;
        u32 packed, shift, direction = 0x7F;
        float distance = func_001D0010(road, (u32)next, previous.field04,
                                       previous.field08, &mode, &index, &output_word, &reverse);
        if (output_word == 0xFFFFFFFFU) return 100000000.0f;
        entry.field04 = (u32)next; entry.field09 = mode; entry.field0A = reverse;
        total = total + distance;
        entry.field00 = (u32)goal; entry.field08 = mode; entry.field0B = 0;
        FIELD(u32, &entry, 0xC) = 0; FIELD(u32, &entry, 0x10) = 0;
        entry.field14 = distance;
        func_002AF470(goal->field40, &entry);
        road = lookup_road(output_word);
        if (road == NULL) { goal->field45 = 2; return 100000000.0f; }
        next_record = record34(road, output_word);
        if ((FIELD(u16, next_record, 0x16) & 0x8000U) != 0) {
            if (index < 0) index = (signed char)(index + 1);
            else if (index > 0) index = (signed char)(index - 1);
        }
        selector = index;
        packed = FIELD(u16, next_record, 0x16);
        if ((selector & 1) != 0) {
            if (selector < 0) selector = (signed char)(selector - 1);
            else if (selector > 0) selector = (signed char)(selector + 1);
        }
        shift = (u32)(selector / 2 + 2) * 3U;
        selector = (packed >> (shift & 31U)) & 7U;
        if (selector == 1) direction = 0;
        else if (selector == 2) direction = 1;
        entry.field0A = direction; entry.field04 = output_word; entry.field0B = 1;
        entry.field00 = (u32)goal; entry.field08 = index; entry.field09 = index;
        FIELD(u32, &entry, 0xC) = 0; FIELD(u32, &entry, 0x10) = 0;
        entry.field14 = record_weight(next_record, index);
        total = total + entry.field14;
        func_002AF470(goal->field40, &entry);
        return total;
    }
}

void func_001DE6E0(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    s32 index = 0;
    u32 next_index = 0x01000000U;
    if ((s32)goal->field40->count > 0) {
        if (!(0.0f < goal->field54)) return;
        do {
            GeorgeGoalRouteQueueEntry *entry = queue_entry(goal, index);
            GeorgeGenericMap *map;
            float value;
            if (index == 0) entry->field0C += goal->field50;
            value = goal->field54;
            {
                float progress = entry->field0C;
                float length = entry->field14;
                entry->field10 = value;
                goal->field54 = (progress + value) - length;
            }
            if (entry->field0B != 0) {
                map = func_001CCBE8(entry->field04);
                if (map != NULL) func_002A8130(map, func_001DF420, entry);
            } else {
                GeorgeGoalRoadGeometry *road;
                map = func_001CCBE8(entry->field04 | 0x80000000U);
                if (map != NULL) func_002A8130(map, func_001DF420, entry);
                if (entry->field10 < 2.0f) {
                    road = lookup_road(entry->field04);
                    if (road == NULL) { goal->field45 = 2; return; }
                    if (func_001D1A30(road, entry->field04, (u8 *)&entry->field0A) == 0) {
                        FIELD(u32, goal, 0x38) = 0;
                        goal->field58 = 0.5f; goal->field48 &= 0xFD;
                    }
                }
            }
            index = (s32)next_index >> 24;
            next_index += 0x01000000U;
            if (index >= (s32)goal->field40->count) break;
        } while (0.0f < goal->field54);
    }
    if (0.0f < goal->field54 && (s32)goal->field40->count < 3) do {
        float length = func_001DEC18(input);
        float remaining = goal->field54 - length;
        goal->field54 = remaining;
        if (!(0.0f < remaining)) break;
    } while ((s32)goal->field40->count < 3);
}

void func_001DF420(GeorgeGenericMap *map, u32 key, void *value, void *context)
{
    GeorgeGoalDriveMotion *other = (GeorgeGoalDriveMotion *)value;
    GeorgeGoalRouteQueueEntry *entry = (GeorgeGoalRouteQueueEntry *)context;
    GeorgeGoalRouteQueueEntry previous;
    float end, start;
    float radius;
    GeorgeGoalDriveMotion *owner;
    (void)map; (void)key;
    if ((u32)other == entry->field00) return;
    func_002AF358(other->field40, &previous);
    if (previous.field08 != entry->field08 && previous.field09 != entry->field08) return;
    radius = resource_float(other->field14, 0x190);
    end = previous.field0C + radius;
    owner = (GeorgeGoalDriveMotion *)entry->field00;
    radius = resource_float(owner->field14, 0x190);
    start = entry->field0C - radius;
    if (!(start < end)) return;
    radius = resource_float(other->field14, 0x190);
    start = previous.field0C - radius;
    if (!(start < entry->field0C + entry->field10)) return;
    owner = (GeorgeGoalDriveMotion *)entry->field00;
    {
        u32 flags = owner->field48;
        owner->field58 = 0.5f;
        FIELD(u32, owner, 0x38) = 0;
        owner->field48 = flags & 0xFD;
    }
}

void func_001DE228(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeMathVec3 motion, direction, normal, side;
    GeorgeMathVec4 orientation;
    float matrix[16] __attribute__((aligned(16)));
    const GeorgeMathVec3 *position, *basis;
    float scale, distance, threshold;
    func_002A1C30(matrix);
    position = resource_position(goal->field14);
    scale = goal->field3C * 0.30000001192092896f;
    motion.z = goal->field18.z - position->z;
    motion.y = goal->field18.y - position->y;
    motion.x = goal->field18.x - position->x;
    scale = 0.5f <= scale ? george_ee_minimum(scale, 5.0f) : 0.5f;
    motion.x *= scale; motion.z *= scale; motion.y *= scale;
    direction = motion;
    distance = func_002A3538(&direction);
    threshold = goal->field38 * 0.30000001192092896f;
    threshold = 0.5f <= threshold ? george_ee_minimum(threshold, 5.0f) : 0.5f;
    if (threshold < distance && 0.0f < goal->field3C) {
        float projection, angle, cosine;
        normal = D_00436BE8;
        if ((goal->field48 & 1U) == 0) normal = goal->field68;
        projection = dot(&direction, &normal);
        direction.x -= projection * normal.x;
        direction.y -= projection * normal.y;
        direction.z -= projection * normal.z;
        reciprocal_squared(&direction);
        basis = resource_vector(goal->field14, 0x98);
        cosine = dot(ADDRESS(GeorgeMathVec3, basis, 0x20), &direction);
        angle = func_0029C230(cosine <= 1.0f ? cosine : 1.0f);
        angle = george_ee_minimum(angle * 5.0f, 0.7853981852531433f);
        basis = resource_vector(goal->field14, 0x98);
        if (0.0f < dot(basis, &direction)) angle = -angle;
        FIELD(float, goal->field14, 0x928) = angle;
        side.x = normal.y * direction.z - normal.z * direction.y;
        side.y = normal.z * direction.x - normal.x * direction.z;
        side.z = normal.x * direction.y - normal.y * direction.x;
        reciprocal_squared(&side);
        matrix[0] = side.x; matrix[1] = side.y; matrix[2] = side.z; matrix[3] = 0.0f;
        matrix[4] = normal.x; matrix[5] = normal.y; matrix[7] = 0.0f; matrix[6] = normal.z;
        matrix[8] = direction.x; matrix[9] = direction.y; matrix[11] = 0.0f; matrix[10] = direction.z;
        func_002A0B00(&orientation, matrix);
        func_001FF6D0(goal->field14, &orientation);
        scale = goal->field3C;
        if (0.0f < scale && scale < distance) {
            distance = scale;
            func_002A35C0(&motion, &motion, distance);
        }
        goal->field50 = distance * OWNER(goal)->field60;
    } else {
        FIELD(u32, &motion, 0) = 0; FIELD(u32, &motion, 4) = 0; FIELD(u32, &motion, 8) = 0;
        FIELD(u32, goal, 0x50) = 0;
    }
    func_001FF6F8(goal->field14, &motion);
}

void func_001DDD70(GeorgeGoalDrive *input)
{
    GeorgeGoalDriveMotion *goal = VIEW(input);
    GeorgeMathVec3 delta;
    const GeorgeMathVec3 *position = resource_position(goal->field14);
    GeorgeGoalRouteQueueEntry *entry;
    GeorgeGoalRoadGeometry *road;
    GeorgeGoalIntersectionRecord *record;
    GeorgeGoalOwner *owner;
    u32 word;
    s32 advance = 0;
    signed char index = 0x7F;
    delta.y = goal->field80.y - position->y;
    delta.x = goal->field80.x - position->x;
    delta.z = goal->field80.z - position->z;
    reciprocal_squared(&delta);
    if ((s32)goal->field40->count < 3) func_001DEC18(input);
    if (!(dot(&goal->field74, &delta) < 0.0f)) return;
    entry = queue_entry(goal, 0);
    word = entry->field04 & 0xFFFFU;
    road = lookup_road(entry->field04);
    if (road == NULL) { goal->field45 = 2; return; }
    if (entry->field0B != 0) {
        record = record34(road, word);
        entry->field09 = entry->field08;
        if (entry->field0A == 0) {
            u32 current;
            owner = OWNER(goal);
            current = OWNER_BYTE(owner, 0x20);
            if ((s32)current < (s32)record->field05 - 2)
                OWNER_BYTE(owner, 0x20) = (u8)(current + 1U);
            else advance = 1;
            index = (signed char)(OWNER_BYTE(OWNER(goal), 0x20) + 1U);
        } else {
            u32 current;
            owner = OWNER(goal);
            current = OWNER_BYTE(owner, 0x20);
            if (current != 0) OWNER_BYTE(owner, 0x20) = (u8)(current - 1U);
            else advance = 1;
            index = OWNER_SIGNED_BYTE(OWNER(goal), 0x20);
        }
    } else advance = 1;
    if (advance != 0 && (s32)goal->field40->count >= 2) {
        GeorgeGoalRouteQueueEntry previous;
        u32 key;
        func_002AF3F0(goal->field40, &previous);
        key = previous.field04;
        if (previous.field0B == 0) key |= 0x80000000U;
        func_001CCB58((u32)goal, key);
        entry = queue_entry(goal, 0);
        word = entry->field04 & 0xFFFFU;
        if (goal->field32 > 0) goal->field32 = (signed char)((u8)goal->field32 - 1U);
        if (entry->field0B != 0) {
            goal->field48 &= 0xFE;
            road = lookup_road(entry->field04);
            if (road == NULL) { goal->field45 = 2; return; }
            record = record34(road, word);
            if (entry->field0A == 0) {
                OWNER_BYTE(OWNER(goal), 0x20) = 0;
                index = (signed char)(OWNER_BYTE(OWNER(goal), 0x20) + 1U);
            } else {
                OWNER_BYTE(OWNER(goal), 0x20) = (u8)(record->field05 - 2);
                index = OWNER_SIGNED_BYTE(OWNER(goal), 0x20);
            }
            if (goal->field46 == 0) func_001CCAA8((u32)goal, entry->field04);
        } else {
            goal->field48 |= 1;
            if (goal->field46 == 0) func_001CCAA8((u32)goal, entry->field04 | 0x80000000U);
            entry = queue_entry(goal, 1);
            word = entry->field04 & 0xFFFFU;
            road = lookup_road(entry->field04);
            if (road == NULL) { goal->field45 = 2; return; }
            record = record34(road, word);
            if (entry->field0A == 0) {
                index = 0; OWNER_BYTE(OWNER(goal), 0x20) = 0;
            } else {
                OWNER_BYTE(OWNER(goal), 0x20) = (u8)(record->field05 - 2);
                index = (signed char)(record->field05 - 1);
            }
        }
    }
    {
        GeorgeMathVec3 *a, *b;
        float scalar, x, y, z;
        u32 narrowed = (u8)index;
        record = record34(road, word);
        scalar = (float)entry->field08; scalar += scalar;
        a = ADDRESS(GeorgeMathVec3, road->field48, (FIELD(u16, record, 6) + narrowed) * 12U);
        b = ADDRESS(GeorgeMathVec3, road->field48, (FIELD(u16, record, 0x12) + narrowed) * 12U);
        x = a->x + scalar * b->x; z = a->z + scalar * b->z; y = a->y + scalar * b->y;
        goal->field80.x = x; goal->field80.z = z; goal->field80.y = y;
    }
    func_001D18A8(&goal->field74, entry->field04, OWNER_BYTE(OWNER(goal), 0x20), entry->field0A);
    func_001CFB20(&goal->field68, entry->field04, OWNER_BYTE(OWNER(goal), 0x20));
    OWNER(goal)->field14 = entry->field04;
    owner = OWNER(goal);
    OWNER_BYTE(owner, 0x23) = OWNER_BYTE(owner, 0x22);
    OWNER_BYTE(OWNER(goal), 0x22) = (u8)entry->field08;
    OWNER_BYTE(OWNER(goal), 0x21) = (u8)entry->field0A;
}
