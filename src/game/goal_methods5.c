#include "george/goal_methods5.h"

typedef unsigned long long GeorgeGoalBits64;
typedef char gm5_bits64_size[(sizeof(GeorgeGoalBits64) == 8) ? 1 : -1];

extern u8 D_004372F0[], D_004373F0[], D_00437478[], D_00437518[];
extern GeorgeGoalMember D_003F8EB0[];
extern GeorgeMathVec3 D_004373E0;
extern GeorgeGoalRoad *func_001CCA28(u32 word);
extern GeorgeGoalIntersectionRecord *func_001D1608(u32 word);
extern float func_002A3538(GeorgeMathVec3 *vector);
extern s32 func_00397178(void);
extern s32 func_001CD2A8(const GeorgeMathVec3 *position, void **record,
                         void **resource, u32 unused);
extern s32 func_001CD728(const GeorgeGoalRay *ray, u32 *word, float *fraction);
extern float func_001CDA18(u32 word, const GeorgeMathVec3 *start,
                           const GeorgeMathVec3 *end);
extern s32 func_001CDEF8(const void *record, const GeorgeMathVec3 *start,
                         const GeorgeMathVec3 *end, const void *resource);

#define ADDRESS(type, base, offset) ((type *)((u32)(base) + (u32)(offset)))
#define FIELD(type, base, offset) (*ADDRESS(type, base, offset))
#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define BYTE(owner, offset) FIELD(u8, owner, offset)
#define SIGNED_BYTE(owner, offset) FIELD(signed char, owner, offset)
#define POSITION(entity) ADDRESS(GeorgeMathVec3, entity, 0x40)

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

static __inline__ float dot(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second)
{
    return (first->x * second->x + first->y * second->y) + first->z * second->z;
}

static __inline__ void reciprocal_squared(GeorgeMathVec3 *vector)
{
    float x = vector->x, y = vector->y, z = vector->z;
    float squared = (x * x + y * y) + z * z;
    if (0.000009999999747378752f < squared) {
        float inverse = 1.0f / squared;
        vector->z = z * inverse; vector->x = x * inverse; vector->y = y * inverse;
    }
}

GeorgeGameplayGoal *func_001E56E8(void *storage, void *owner, float angle,
                                 float value20, float value24)
{
    GeorgeGoalAngleTarget *goal = storage;
    GeorgeGoalOwner *reloaded_owner;
    func_0020D2A0(storage, owner);
    goal->base.field0C = D_004372F0;
    if (3.1415927410125732f < angle)
        goal->field14.angle = angle - 6.2831854820251465f;
    else {
        goal->field14.angle = angle;
        if (angle < -3.1415927410125732f)
            goal->field14.angle = angle + 6.2831854820251465f;
    }
    goal->field20 = value20; goal->field24 = value24;
    goal->field28 = 1; goal->field10 = 0;
    reloaded_owner = OWNER(goal);
    reloaded_owner->field24 |= 0x200;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E57C8(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object,
                                 float value20, float value24)
{
    GeorgeGoalAngleTarget *goal = storage;
    GeorgeGoalOwner *reloaded_owner;
    func_0020D2A0(storage, owner);
    goal->field14.reference = object;
    goal->base.field0C = D_004372F0;
    object->field05 = (u8)(object->field05 + 1U);
    goal->field10 = 1; goal->field20 = value20; goal->field24 = value24;
    goal->field28 = 0;
    reloaded_owner = OWNER(goal);
    reloaded_owner->field24 |= 0x200;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E5858(void *storage, void *owner,
                                 const GeorgeMathVec3 *position,
                                 float value20, float value24)
{
    GeorgeGoalAngleTarget *goal = storage;
    GeorgeGoalOwner *reloaded_owner;
    func_0020D2A0(storage, owner);
    goal->base.field0C = D_004372F0;
    goal->field14.position.x = position->x;
    goal->field14.position.y = position->y;
    goal->field14.position.z = position->z;
    goal->field20 = value20; goal->field24 = value24;
    goal->field10 = 2; goal->field28 = 1;
    reloaded_owner = OWNER(goal);
    reloaded_owner->field24 |= 0x200;
    return &goal->base.links;
}

void func_001E58F8(GeorgeGoalAngleTarget *goal, u32 flags)
{
    GeorgeGoalOwner *owner;
    goal->base.field0C = D_004372F0;
    owner = OWNER(goal);
    owner->field24 &= 0xFDFF;
    if (goal->field10 == 1) {
        func_001CAF88(goal->field14.reference);
        goal->field14.reference = NULL;
    }
    func_0020D2C0(&goal->base, flags);
}

GeorgeGameplayGoal *func_001E6318(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object, float value)
{
    GeorgeGoalReferenceCandidates *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field64 = object; goal->base.field0C = D_004373F0;
    object->field05 = (u8)(object->field05 + 1U);
    goal->field60 = value;
    return &goal->base.links;
}

void func_001E6378(GeorgeGoalReferenceCandidates *goal, u32 flags)
{
    goal->base.field0C = D_004373F0;
    func_001CAF88(goal->field64);
    goal->field64 = NULL;
    func_0020D2C0(&goal->base, flags);
}

void func_001E63D0(GeorgeGoalReferenceCandidates *goal, GeorgeGoalOutput *output)
{
    output->field10 = 0; output->field00 = 1;
    output->field04.x = goal->field68.x;
    output->field04.y = goal->field68.y;
    output->field04.z = goal->field68.z;
}

GeorgeGameplayGoal *func_001E6688(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object, s32 value)
{
    GeorgeGoalEndpoint *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field14 = object; goal->base.field0C = D_00437478;
    object->field05 = (u8)(object->field05 + 1U);
    FIELD(u32, &goal->field18, 0) = 0;
    FIELD(u32, &goal->field18, 8) = 0;
    FIELD(u32, &goal->field18, 4) = 0;
    goal->field12 = (s16)value; goal->field10 = 0;
    return &goal->base.links;
}

GeorgeGameplayGoal *func_001E6700(void *storage, void *owner,
                                 const GeorgeMathVec3 *position, s32 value)
{
    GeorgeGoalEndpoint *goal = storage;
    func_0020D2A0(storage, owner);
    goal->field14 = NULL; goal->base.field0C = D_00437478;
    goal->field18.x = position->x; goal->field18.y = position->y;
    goal->field18.z = position->z;
    goal->field12 = (s16)value; goal->field10 = 1;
    return &goal->base.links;
}

void func_001E6778(GeorgeGoalEndpoint *goal, u32 flags)
{
    GeorgeGoalReferencedObject *object;
    goal->base.field0C = D_00437478;
    object = goal->field14;
    if (object != NULL) {
        func_001CAF88(object);
        goal->field14 = NULL;
    }
    func_0020D2C0(&goal->base, flags);
}

void func_001E67D0(GeorgeGoalEndpoint *goal, GeorgeGoalOutput *output)
{
    output->field00 = 1; output->field10 = (u32)goal->field14;
    output->field04.x = goal->field24.x;
    output->field04.y = goal->field24.y;
    output->field04.z = goal->field24.z;
}

/* Captured entity and target pointers survive calls; reload their contents. */
s32 func_001E6400(GeorgeGoalEndpoint *goal)
{
    const GeorgeMathVec3 *target = NULL;
    GeorgeGoalOwner *owner;
    GeorgeGoalEntity *entity;
    GeorgeMathVec3 *position;
    GeorgeMathVec3 delta;
    GeorgeGoalRay ray;
    float fraction, length;
    u32 word = 0xFFFFFFFFU;
    s32 result;
    if (goal->field10 == 0) {
        GeorgeGoalReferencedObject *object = goal->field14;
        if ((object->field06 & 0x40) == 0 && object->field20.word == 0) return 2;
        target = func_001CAFE0(goal->field14);
    } else if (goal->field10 == 1) target = &goal->field18;
    owner = OWNER(goal); entity = owner->field08; position = POSITION(entity);
    if (goal->field12 != 0) {
        delta.x = target->x - position->x; FIELD(u32, &delta, 4) = 0;
        delta.z = target->z - position->z;
        fraction = func_001CDA18(FIELD(u32, owner, 0xC), position, target);
        if (0.0f < fraction && fraction <= 1.0f) {
            float x = position->x + fraction * delta.x;
            float y = position->y + fraction * delta.y;
            float z = position->z + fraction * delta.z;
            goal->field24.x = x; goal->field24.y = y; goal->field24.z = z;
        }
        return 1;
    }
    delta.x = position->x - target->x; delta.y = position->y - target->y;
    delta.z = position->z - target->z;
    length = func_002A3538(&delta);
    ray.field00.x = target->x; ray.field00.y = target->y; ray.field00.z = target->z;
    ray.field0C.x = delta.x; ray.field0C.y = delta.y; ray.field0C.z = delta.z;
    ray.field18 = length;
    result = func_001CD728(&ray, &word, &fraction);
    if (result == 1) {
        goal->field24.x = target->x; goal->field24.y = target->y; goal->field24.z = target->z;
        return 1;
    }
    if (result == 2) {
        float distance = fraction * length;
        float x = target->x + distance * delta.x;
        float z = target->z + distance * delta.z;
        float y = target->y + distance * delta.y;
        GeorgeGoalReferencedObject *object;
        goal->field24.x = x; goal->field24.z = z; goal->field24.y = y;
        object = goal->field14;
        if (object != NULL) {
            func_001CAF88(object); goal->field14 = NULL;
        }
        return 1;
    }
    return 2;
}

GeorgeGameplayGoal *func_001E7520(void *storage, void *owner,
                                 GeorgeGoalReferencedObject *object,
                                 float value, float time)
{
    GeorgeGoalRouteAction *goal = storage;
    const GeorgeMathVec3 *position;
    func_0020D2A0(storage, owner);
    goal->field24 = 0; goal->field28 = 0;
    goal->field20 = 0xFFFFFFFFU; goal->field1C = 0xFFFFFFFFU;
    goal->base.field0C = D_00437518;
    func_001DC538((GeorgeGoalTimer *)&goal->field34, value);
    FIELD(u32, &goal->field34.field0C, 0) = 0;
    FIELD(u32, &goal->field34.field0C, 8) = 0;
    FIELD(u32, &goal->field34.field0C, 4) = 0;
    goal->field34.field20 = 0.75f;
    if (object != NULL) {
        position = func_001CAFE0(object);
        goal->field68.x = position->x; goal->field68.y = position->y;
        goal->field68.z = position->z;
    } else goal->field11 = 2;
    goal->field14 = time;
    return &goal->base.links;
}

void func_001E75F8(GeorgeGoalRouteAction *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    u32 index = BYTE(owner, 0x20);
    u32 reverse = BYTE(owner, 0x21);
    if (reverse == 0) index += 1;
    func_001D18A8(&goal->field5C, owner->field14, BYTE(owner, 0x20), reverse);
    owner = OWNER(goal);
    func_001CFBF8(&owner->field80, owner->field14, BYTE(owner, 0x20));
    owner = OWNER(goal);
    if (func_001D1820(&goal->field34.field0C, owner->field14, (u8)index,
                     SIGNED_BYTE(owner, 0x22), owner->field1C) != 0)
        goal->field10 = 1;
    else goal->field11 = 2;
}

void func_001E73C8(GeorgeGoalRouteAction *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    GeorgeMathVec3 delta;
    const GeorgeMathVec3 *position;
    goal->field34.field20 = george_ee_minimum(owner->field60 * 45.0f, 1.0f);
    func_001DB968(owner, &goal->field34);
    OWNER(goal)->field18.bits = 0xFFFFFFFFU;
    position = POSITION(OWNER(goal)->field08);
    delta.x = position->x - goal->field34.field0C.x;
    delta.y = position->y - goal->field34.field0C.y;
    delta.z = position->z - goal->field34.field0C.z;
    reciprocal_squared(&delta);
    if (0.0f < dot(&goal->field5C, &delta)) goal->field10 = 0;
}

s32 func_001CFBF8(void *output, u32 word, u32 index)
{
    GeorgeGoalRoadGeometry *road = lookup_road(word);
    GeorgeGoalIntersectionRecord *record;
    const GeorgeMathVec3 *first, *second, *anchor;
    float ax, ay, az, bx, by, bz, x, y, z;
    u32 narrowed = (u8)index;
    if (road == NULL) return 0;
    record = record34(road, word);
    first = ADDRESS(GeorgeMathVec3, road->field48,
                    (FIELD(u16, record, 0x12) + narrowed) * 12U);
    second = ADDRESS(GeorgeMathVec3, road->field48,
                     (record->field10 + narrowed) * 12U);
    ax = first->x; ay = first->y; az = first->z;
    bx = second->x; by = second->y; bz = second->z;
    x = by * az - bz * ay; y = bz * ax - bx * az; z = bx * ay - by * ax;
    FIELD(float, output, 4) = y; FIELD(float, output, 0) = x; FIELD(float, output, 8) = z;
    /* Retail reloads both geometry pointers and reads the anchor after stores. */
    record = record34(road, word);
    anchor = ADDRESS(GeorgeMathVec3, road->field48,
                     (FIELD(u16, record, 6) + narrowed) * 12U);
    FIELD(float, output, 0xC) = (x * anchor->x + y * anchor->y) + z * anchor->z
                              + 0.10000000149011612f;
    return 1;
}

void func_001E6908(GeorgeGoalRouteAction *goal)
{
    GeorgeGoalOwner *owner;
    GeorgeGoalRoadGeometry *road;
    const GeorgeMathVec3 *position;
    GeorgeMathVec3 delta;
    if (goal->field11 == 2) return;
    owner = OWNER(goal);
    goal->field2C = func_001DC468(POSITION(owner->field08), &owner->field18.bits,
                                &owner->field14, &owner->field20);
    goal->field10 = 1; goal->field11 = 0;
    position = POSITION(OWNER(goal)->field08);
    delta.x = goal->field68.x - position->x;
    delta.y = goal->field68.y - position->y;
    delta.z = goal->field68.z - position->z;
    reciprocal_squared(&delta);
    if (goal->field2C == 0) {
        u32 *candidate;
        s32 count = 0;
        float best = 1.0f;
        road = lookup_road(OWNER(goal)->field18.bits);
        if (road == NULL) { goal->field11 = 2; return; }
        candidate = ADDRESS(u32, record60(road, OWNER(goal)->field18.bits), 0x48);
        do {
            if (*candidate != 0xFFFFFFFFU) {
                GeorgeGoalIntersectionRecord *record;
                GeorgeMathVec3 candidate_delta;
                const GeorgeMathVec3 *target;
                signed char index;
                u8 reverse;
                float score;
                road = lookup_road(*candidate);
                if (road == NULL) { goal->field11 = 2; return; }
                record = record34(road, *candidate);
                if ((s32)record->field1C < 0) {
                    reverse = 1; index = (signed char)(record->field05 - 1U);
                } else { reverse = 0; index = 0; }
                position = POSITION(OWNER(goal)->field08);
                target = ADDRESS(GeorgeMathVec3, road->field48,
                                  (FIELD(u16, record, 6) + (u8)index) * 12U);
                candidate_delta.x = target->x - position->x;
                candidate_delta.y = target->y - position->y;
                candidate_delta.z = target->z - position->z;
                reciprocal_squared(&candidate_delta);
                score = dot(&delta, &candidate_delta);
                if (score < best && (record->field04 & 0xC) == 0) {
                    OWNER(goal)->field14 = *candidate;
                    BYTE(OWNER(goal), 0x21) = reverse;
                    BYTE(OWNER(goal), 0x20) = (u8)index;
                    if (record->field04 & 4) SIGNED_BYTE(OWNER(goal), 0x22) = -2;
                    else if (record->field04 & 8) SIGNED_BYTE(OWNER(goal), 0x22) = 2;
                    best = score;
                }
            }
            ++count; ++candidate;
        } while (count < 5);
    } else if (goal->field2C == 1) {
        GeorgeGoalIntersectionRecord *record;
        u32 captured_word;
        const GeorgeMathVec3 *direction;
        float side;
        road = lookup_road(OWNER(goal)->field14);
        if (road == NULL) { goal->field11 = 2; return; }
        owner = OWNER(goal); captured_word = owner->field14;
        record = record34(road, captured_word);
        if (0.0f < dot(&owner->field08->fieldD0, &delta))
            BYTE(owner, 0x21) = BYTE(owner, 0x21) != 0 ? 0 : 1;
        owner = OWNER(goal);
        direction = ADDRESS(GeorgeMathVec3, road->field48,
                            (FIELD(u16, record34(road, captured_word), 0x12)
                              + BYTE(owner, 0x20)) * 12U);
        side = dot(direction, &delta);
        if (0.0f < side && SIGNED_BYTE(owner, 0x22) == 2
            && (record->field04 & 8) == 0) SIGNED_BYTE(owner, 0x22) = -2;
        else if (side < 0.0f && SIGNED_BYTE(owner, 0x22) == -2
                 && (record->field04 & 4) == 0) SIGNED_BYTE(owner, 0x22) = 2;
    } else { goal->field11 = 2; return; }
    func_001E75F8(goal);
}

u32 func_001E6DA0(GeorgeGoalRouteAction *goal)
{
    float remaining = goal->field14;
    if (remaining < 0.0f) goal->field11 = 1;
    remaining = goal->field14;
    if (0.0f < remaining) goal->field14 = remaining - OWNER(goal)->field60;
    if (goal->field11 == 0) {
        u32 index = goal->field10;
        const GeorgeGoalMember *entry = ADDRESS(GeorgeGoalMember, D_003F8EB0, index * 8U);
        s32 selector = entry->selector;
        float angle;
        GeorgeGoalOwner *owner;
        if (selector != 0) {
            GeorgeGoalBits64 packed;
            void (*invoke)(void *);
            s32 adjustment;
            if (selector > 0) {
                const u8 *vtable = FIELD(const u8 *, goal, (s32)entry->target.vtable_offset);
                packed = FIELD(GeorgeGoalBits64, vtable, (u32)selector * 8U - 8U);
                invoke = (void (*)(void *))(u32)(packed >> 32);
            } else invoke = entry->target.direct;
            entry = ADDRESS(GeorgeGoalMember, D_003F8EB0, index * 8U);
            adjustment = entry->adjustment;
            if (selector > 0) adjustment += (s16)(u16)packed;
            invoke(ADDRESS(void, goal, adjustment));
        }
        angle = func_001DC178(POSITION(OWNER(goal)->field08), &goal->field34.field0C);
        owner = OWNER(goal);
        if (owner->field2C == 0) {
            owner->field34 = angle; owner->field30 = 6.2831854820251465f;
        }
    }
    return goal->field11;
}

void func_001E6ED8(GeorgeGoalRouteAction *goal, GeorgeGoalRoadEntry *record)
{
    GeorgeGoalIntersectionRecord *candidates[8];
    signed char candidate_index[16], candidate_mode[16];
    u8 candidate_reverse[16], candidate_point[16];
    u32 *words = ADDRESS(u32, record, 0x48);
    u32 packed_index = 0x01000000U;
    s32 index = 0;
    signed char count = 0;
    s32 any = 0;
    do {
        u32 *word = ADDRESS(u32, words, (u32)index * 4U);
        if (*word != 0xFFFFFFFFU && *word != OWNER(goal)->field14) {
            GeorgeGoalRoadGeometry *road;
            GeorgeGoalIntersectionRecord *candidate;
            u32 flags;
            candidate_index[(s32)count] = (signed char)index;
            road = lookup_road(*word);
            candidate = record34(road, *word);
            candidates[(s32)count] = candidate;
            if (candidate->field1C == FIELD(u32, record, 0x40)) {
                candidate_reverse[(s32)count] = 1;
                candidate_point[(s32)count] = (u8)(candidate->field05 - 1U);
            } else {
                candidate_reverse[(s32)count] = 0;
                candidate_point[(s32)count] = 0;
            }
            flags = FIELD(u32, candidates[(s32)count], 4) & 0xC;
            if (flags == 0) {
                s32 random;
                any = 1;
                random = func_00397178() % 10001;
                candidate_mode[(s32)count] =
                    (float)random * 0.00009999999747378752f < 0.5f ? 2 : -2;
                count = (signed char)(count + 1);
            } else if (flags == 4 || flags == 8) {
                candidate_mode[(s32)count] = flags == 4 ? -2 : 2;
                count = (signed char)(count + 1);
                any = 1;
            }
        }
        index = (s32)packed_index >> 24; packed_index += 0x01000000U;
    } while (index < 5);
    if (any != 0) {
        s32 chosen = func_00397178() % (s32)count;
        OWNER(goal)->field14 = FIELD(u32, words, (s32)candidate_index[chosen] * 4);
        SIGNED_BYTE(OWNER(goal), 0x22) = candidate_mode[chosen];
        BYTE(OWNER(goal), 0x21) = candidate_reverse[chosen];
        BYTE(OWNER(goal), 0x20) = candidate_point[chosen];
    } else {
        GeorgeGoalRoadGeometry *road = lookup_road(OWNER(goal)->field14);
        GeorgeGoalOwner *owner = OWNER(goal);
        GeorgeGoalIntersectionRecord *current = record34(road, owner->field14);
        if (SIGNED_BYTE(owner, 0x22) == 2 && (current->field04 & 8) == 0)
            SIGNED_BYTE(owner, 0x22) = -2;
        else if ((current->field04 & 4) == 0) SIGNED_BYTE(OWNER(goal), 0x22) = 2;
        owner = OWNER(goal);
        if (BYTE(owner, 0x21) != 0) {
            BYTE(owner, 0x21) = 0; BYTE(OWNER(goal), 0x20) = 0;
        } else {
            BYTE(owner, 0x21) = 1;
            BYTE(OWNER(goal), 0x20) = (u8)(current->field05 - 1U);
        }
    }
}

void func_001E71E0(GeorgeGoalRouteAction *goal)
{
    GeorgeGoalRoadGeometry *road = lookup_road(OWNER(goal)->field14);
    GeorgeGoalOwner *owner = OWNER(goal);
    GeorgeGoalIntersectionRecord *record;
    u32 *neighbor_field = NULL;
    u32 neighbor;
    s32 needs_selection = 0;
    if (road == NULL) { goal->field11 = 2; return; }
    record = record34(road, owner->field14);
    if (BYTE(owner, 0x21) == 0) {
        u32 index = BYTE(owner, 0x20);
        if ((s32)index < (s32)record->field05 - 2) {
            BYTE(owner, 0x20) = (u8)(index + 1U); goal->field10 = 1;
        } else {
            neighbor = record->field1C;
            if (neighbor == 0xFFFFFFFFU) BYTE(owner, 0x21) = 1;
            else if ((s32)neighbor >= 0) {
                owner->field14 = neighbor; BYTE(OWNER(goal), 0x20) = 0;
                goal->field10 = 1;
            } else { needs_selection = 1; neighbor_field = &record->field1C; }
        }
    } else {
        u32 index = BYTE(owner, 0x20);
        if (index != 0) {
            BYTE(owner, 0x20) = (u8)(index - 1U); goal->field10 = 1;
        } else {
            neighbor = record->field18;
            if (neighbor == 0xFFFFFFFFU) BYTE(owner, 0x21) = 0;
            else if ((s32)neighbor >= 0) {
                GeorgeGoalIntersectionRecord *next;
                owner->field14 = neighbor;
                next = func_001D1608(OWNER(goal)->field14);
                BYTE(OWNER(goal), 0x20) = (u8)(next->field05 - 2U);
                goal->field10 = 1;
            } else { needs_selection = 1; neighbor_field = &record->field18; }
        }
    }
    if (needs_selection != 0) {
        GeorgeGoalRoadEntry *next;
        u32 selected;
        road = lookup_road(neighbor);
        if (road == NULL) { goal->field11 = 2; return; }
        /* Neighbor low halfword is deliberately reloaded after the lookup. */
        selected = *neighbor_field;
        next = record60(road, selected);
        if (next != NULL) func_001E6ED8(goal, next);
        goal->field10 = 1;
    }
    func_001E75F8(goal);
}

/* Each vector observes a fresh owner/entity and captures its source scalars. */
#define CANDIDATE_STORES(index, direction) do { \
    const GeorgeMathVec3 *position = POSITION(OWNER(goal)->field08); \
    float x = position->x + margin * (direction).x; \
    float y = position->y + margin * (direction).y; \
    float z = position->z + margin * (direction).z; \
    goal->field10[index].x = x; goal->field10[index].y = y; \
    goal->field10[index].z = z; \
} while (0)

s32 func_001E5C48(GeorgeGoalReferenceCandidates *goal)
{
    GeorgeGoalReferencedObject *object = goal->field64;
    const GeorgeMathVec3 *target, *position;
    GeorgeMathVec3 delta, normal, cross, combined;
    void *original_record, *original_resource, *record, *resource;
    u32 original_word;
    float squared, margin;
    s32 first_found = 0, second_found = 0;
    u32 candidate, choices[5];
    s32 best = 1, count = 0;
    if ((object->field06 & 0x40) == 0 && object->field20.word == 0) return 2;
    target = func_001CAFE0(goal->field64);
    original_word = (u32)func_001CD2A8(target, &original_record, &original_resource, 1);
    position = POSITION(OWNER(goal)->field08);
    delta.x = position->x - target->x; delta.y = position->y - target->y;
    delta.z = position->z - target->z;
    squared = dot(&delta, &delta);
    margin = goal->field60 - george_ee_square_root(squared);
    if (margin < 2.0f) return 2;
    reciprocal_squared(&delta);
    normal = D_004373E0;
    cross.x = normal.y * delta.z - normal.z * delta.y;
    cross.y = normal.z * delta.x - normal.x * delta.z;
    cross.z = normal.x * delta.y - normal.y * delta.x;
    combined.x = delta.x + cross.x; combined.y = delta.y + cross.y;
    combined.z = delta.z + cross.z;
    reciprocal_squared(&combined);
    CANDIDATE_STORES(0, cross);
    CANDIDATE_STORES(1, combined);
    CANDIDATE_STORES(2, delta);
    cross.x = -cross.x; cross.y = -cross.y; cross.z = -cross.z;
    combined.x = delta.x + cross.x; combined.y = delta.y + cross.y;
    combined.z = delta.z + cross.z;
    reciprocal_squared(&combined);
    CANDIDATE_STORES(3, combined);
    CANDIDATE_STORES(4, cross);
    candidate = 0;
    do {
        GeorgeMathVec3 *point = &goal->field10[candidate];
        s32 *score = &goal->field4C[candidate];
        u32 word;
        *score = 0;
        word = (u32)func_001CD2A8(point, &record, &resource, 1);
        if (record != NULL) {
            func_001CDEB0(point, FIELD(const GeorgeMathVec4 *, record, 0x18));
            if (word == FIELD(u32, OWNER(goal), 0xC))
                *score = (s32)((u32)*score + 4U);
            else if (original_record != NULL && word != original_word) {
                s32 index = 0;
                if (FIELD(u8, record, 0x16) != 0) do {
                    u16 entry = FIELD(u16, FIELD(const u16 *, record, 0x1C), (u32)index * 2U);
                    const void *item = ADDRESS(u8, FIELD(const u8 *, resource, 0x2C), entry * 0x1CU);
                    if (func_001CDEF8(item, point, target, resource) != 0) {
                        first_found = 1; *score = (s32)((u32)*score + 1U); break;
                    }
                    ++index;
                } while (index < (s32)FIELD(u8, record, 0x16));
                index = 0;
                if (FIELD(u8, original_record, 0x16) != 0) do {
                    u16 entry = FIELD(u16, FIELD(const u16 *, original_record, 0x1C), (u32)index * 2U);
                    const void *item = ADDRESS(u8, FIELD(const u8 *, original_resource, 0x2C), entry * 0x1CU);
                    if (func_001CDEF8(item, target, point, original_resource) != 0) {
                        second_found = 1; *score = (s32)((u32)*score + 1U); break;
                    }
                    ++index;
                } while (index < (s32)FIELD(u8, original_record, 0x16));
                /* These flags deliberately persist across candidate iterations. */
                if (first_found == 0 || second_found == 0)
                    *score = (s32)((u32)*score - 100U);
            }
            *score = (s32)((u32)*score + 4U);
        }
        ++candidate;
    } while (candidate < 5U);
    candidate = 0;
    do {
        s32 score = goal->field4C[candidate];
        if (best < score) { best = score; count = 0; }
        if (score == best) choices[count++] = candidate;
        ++candidate;
    } while (candidate < 5U);
    if (count != 0) {
        const GeorgeMathVec3 *chosen = &goal->field10[choices[func_00397178() % count]];
        goal->field68.x = chosen->x; goal->field68.y = chosen->y;
        goal->field68.z = chosen->z;
        return 1;
    }
    return 2;
}
