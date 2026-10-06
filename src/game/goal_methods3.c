#include "george/goal_methods3.h"

typedef unsigned long long GoalBits64;
typedef char goal_methods3_bits64_size[(sizeof(GoalBits64) == 8) ? 1 : -1];

/* Callee entry/use review establishes full words, separate f12 floats, and
 * complete soft-double integer-register operands/results. */
extern GeorgeGoalRoad *func_001CCA28(u32 word);
extern s32 func_001CF7A0(GeorgeMathVec3 *output, GeorgeGoalRoadGeometry *road,
                          u32 word, u32 index, s32 mode, float value);
extern s32 func_001CFBF8(void *output, u32 word, u32 index);
extern u32 func_001D0B30(GeorgeGoalRoadGeometry *road, const GeorgeMathVec3 *position,
                          u32 word, u32 *next, s32 mode, GeorgeMathVec3 *output);
extern u32 func_001D0370(GeorgeGoalRoadGeometry *road, u32 word, u32 next,
                          s32 mode, GeorgeMathVec3 *output, u32 *output_mode,
                          u32 *output_word, u8 *output_index);
extern void func_0018FD00(GeorgeGoalEntity *entity, GoalBits64 flags, s32 mode);
extern s32 func_00177E48(GeorgeGoalEntity *entity, const GeorgeMathVec3 *motion);
extern s32 func_001773F0(GeorgeGoalEntity *entity, u32 object, u32 value0, u32 value1);
extern void func_001BAA68(u32 object, u32 word, s32 byte, s32 mode);
extern void func_002A3390(GeorgeMathVec3 *output, const GeorgeMathVec3 *current,
                           const GeorgeMathVec3 *target, float blend);
extern float func_002A3538(GeorgeMathVec3 *vector);
extern float func_002A35C0(GeorgeMathVec3 *output, const GeorgeMathVec3 *input, float scale);
extern float func_0029C168(float angle);
extern float func_0029C090(float angle);
extern s32 func_00397178(void);
extern GoalBits64 func_00374848(float value);
extern float func_003734F8(GoalBits64 value);
extern s32 func_00373250(GoalBits64 first, GoalBits64 second);
extern GoalBits64 func_00372CC0(GoalBits64 first, GoalBits64 second);
extern GoalBits64 func_00372C68(GoalBits64 first, GoalBits64 second);
extern GoalBits64 func_00372D28(GoalBits64 first, GoalBits64 second);

#define ADDRESS(type, base, offset) ((type *)((u32)(base) + (u32)(offset)))
#define OWNER(goal) ((GeorgeGoalOwner *)(goal)->base.links.unknown00)
#define ROAD_OWNER(goal) ((GeorgeGoalOwner *)(goal)->prefix.base.links.unknown00)
#define ENTITY_POSITION(entity) ((GeorgeMathVec3 *)(entity)->field40)
#define OWNER_BYTE(owner, offset) (*ADDRESS(u8, owner, offset))
#define OWNER_SIGNED_BYTE(owner, offset) (*ADDRESS(signed char, owner, offset))
#define ENTER_MOTION(goal) ((GeorgeGoalMotion *)&(goal)->field44)
#define ROAD_MOTION(goal) ((GeorgeGoalMotion *)&(goal)->prefix.field14)
#define PI_BITS 0x400921FB60000000ULL
#define TAU_BITS 0x401921FB60000000ULL
#define ANGLE_EPSILON_BITS 0x3FB99999A0000000ULL
#define NEGATIVE_ONE_BITS 0xBFF0000000000000ULL

static __inline__ GeorgeGoalIntersectionRecord *intersection_record(
    GeorgeGoalRoadGeometry *road, u32 word)
{
    return ADDRESS(GeorgeGoalIntersectionRecord, road->field38, (word & 0xFFFFU) * 0x34U);
}

GeorgeGoalRoadEntry *func_001D17D8(u32 word)
{
    GeorgeGoalRoad *road = func_001CCA28(word);
    if (road == NULL) return NULL;
    return ADDRESS(GeorgeGoalRoadEntry, road->field3C, (word & 0xFFFFU) * 0x60U);
}

s32 func_001D1820(GeorgeMathVec3 *output, u32 word, u32 index, s32 mode, float value)
{
    u32 narrowed = (u8)index;
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CCA28(word);
    if (road == NULL) return 0;
    return func_001CF7A0(output, road, word, narrowed, mode, value);
}

s32 func_001D18A8(GeorgeMathVec3 *output, u32 word, u32 index, s32 reverse)
{
    u32 narrowed = (u8)index;
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CCA28(word);
    GeorgeMathVec3 *vector;
    float x, y, z;
    if (road == NULL) return 0;
    vector = ADDRESS(GeorgeMathVec3, road->field48,
        ((u32)intersection_record(road, word)->field10 + narrowed) * 12U);
    /* Interleaved reads/stores permit shifted source/destination aliases.
     * Negation uses each captured scalar rather than rereading the source. */
    x = vector->x; output->x = x;
    y = vector->y; output->y = y;
    z = vector->z; output->z = z;
    if (reverse == 1) {
        output->x = -x; output->y = -y; output->z = -z;
    }
    return 1;
}

/* Four identical angle-completion paths differ only in the observed fields.
 * The owner and target reload occur after the first soft comparison, before
 * the optional absolute-value call. Later entity loads use that captured owner.
 */
#define LOOK_STATE_BODY(initial_angle, target_angle, completion_word, state) \
    GeorgeGoalOwner *owner = OWNER(goal); \
    float initial = goal->initial_angle; \
    float rate = goal->field24; \
    GoalBits64 value; \
    s32 negative; \
    float target; \
    if (owner->field2C == 0) { \
        owner->field34 = initial; \
        owner->field30 = rate; \
        owner = OWNER(goal); \
    } \
    value = func_00374848(owner->field08->field5C - goal->target_angle); \
    negative = func_00373250(value, 0ULL); \
    owner = OWNER(goal); \
    target = goal->target_angle; \
    if (negative < 0) value = func_00372CC0(0ULL, value); \
    if (func_00373250(value, PI_BITS) > 0) { \
        value = func_00374848(owner->field08->field5C - target); \
        if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value); \
        value = func_00372CC0(value, TAU_BITS); \
        if (owner->field08->field5C - target < 0.0f) \
            value = func_00372D28(value, NEGATIVE_ONE_BITS); \
        if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value); \
    } else { \
        value = func_00374848(owner->field08->field5C - target); \
        if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value); \
    } \
    if (func_00373250(value, ANGLE_EPSILON_BITS) < 0) goal->completion_word = state

void func_001D9DC8(GeorgeGoalLook *goal) { LOOK_STATE_BODY(field1C, field1C, field14, 1); }
void func_001D9FC8(GeorgeGoalLook *goal) { LOOK_STATE_BODY(field18, field18, field14, 2); }
void func_001DA1C8(GeorgeGoalLook *goal) { LOOK_STATE_BODY(field20, field20, field14, 3); }
void func_001DA3C8(GeorgeGoalLook *goal) { LOOK_STATE_BODY(field18, field20, field10, 1); }
#undef LOOK_STATE_BODY

static __inline__ float squared_length(const GeorgeMathVec3 *vector)
{
    return vector->x * vector->x + vector->y * vector->y + vector->z * vector->z;
}

/* This helper scales by reciprocal SQUARED length, exactly as retail does. */
static __inline__ void reciprocal_squared_scale(GeorgeMathVec3 *vector)
{
    float length = squared_length(vector);
    if (length > 0.000009999999747378752f) {
        float scale = 1.0f / length;
        vector->z *= scale; vector->x *= scale; vector->y *= scale;
    }
}

void func_001DB968(GeorgeGoalOwner *owner, GeorgeGoalMotion *motion)
{
    GeorgeGoalEntity *entity = owner->field08;
    GeorgeMathVec3 *direction = ADDRESS(GeorgeMathVec3, owner, 0x90);
    const GeorgeMathVec3 *position;
    GeorgeMathVec3 local;
    float x, y, z, length;
    u16 flags;
    if (entity->field0C != 0x20) func_0018FD00(entity, 0x80000000000ULL, 0);
    position = ENTITY_POSITION(owner->field08);
    x = motion->field0C.x - position->x;
    z = motion->field0C.z - position->z;
    y = motion->field0C.y - position->y;
    direction->x = x; direction->z = z; direction->y = y;
    length = func_002A3538(direction);
    motion->field18 = length * length;
    flags = owner->field24;
    if ((flags & 0x20) != 0 && (flags & 0x40) == 0) {
        const GeorgeMathVec3 *normal = ADDRESS(GeorgeMathVec3, owner->field08, 0x88);
        const GeorgeMathVec3 *input = ADDRESS(GeorgeMathVec3, owner, 0x54);
        float nx = normal->x, ny = normal->y, nz = normal->z;
        float ix = input->x, iy = input->y, iz = input->z;
        float dot = nx * ix + ny * iy + nz * iz;
        local.z = iz - dot * nz;
        local.x = ix - dot * nx;
        local.y = iy - dot * ny;
    } else {
        func_002A3390(&local, &motion->field00, direction, 1.0f);
        reciprocal_squared_scale(&local);
    }
    if (squared_length(&local) < 0.0010000000474974513f || (owner->field24 & 0x80) != 0) {
        /* Original writes integer zero words, not floating operations. */
        GeorgeGoalZeroUnit *zero = (GeorgeGoalZeroUnit *)&local;
        zero->field00 = 0; *(u32 *)&zero->field04 = 0; zero->field08 = 0;
        func_00177E48(owner->field08, &local);
    } else {
        func_002A35C0(&local, &local, motion->field1C * motion->field24);
        func_00177E48(owner->field08, &local);
    }
    x = local.x; y = local.y; z = local.z;
    motion->field00.x = x; motion->field00.z = z; motion->field00.y = y;
}

float func_001DC200(float origin, float target, float blend)
{
    GoalBits64 weight, start, value;
    float delta;
    if (target > 3.1415927410125732f) target -= 6.2831854820251465f;
    else if (target < -3.1415927410125732f) target += 6.2831854820251465f;
    if (origin > 3.1415927410125732f) origin -= 6.2831854820251465f;
    else if (origin < -3.1415927410125732f) origin += 6.2831854820251465f;
    delta = target - origin;
    weight = func_00374848(george_ee_minimum(blend, 1.0f));
    start = func_00374848(origin);
    value = func_00374848(delta);
    if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value);
    if (func_00373250(value, PI_BITS) > 0) {
        value = func_00374848(delta);
        if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value);
        value = func_00372CC0(value, TAU_BITS);
        if (delta < 0.0f) weight = func_00372D28(weight, NEGATIVE_ONE_BITS);
    } else {
        value = func_00374848(delta);
    }
    value = func_00372D28(value, weight);
    value = func_00372C68(value, start);
    return func_003734F8(value);
}

/* Target components are all evaluated before the x/z/y stores; indexed
 * source vectors can overlap that target inside the goal allocation. */
static __inline__ void intersection_target(GeorgeGoalIntersectionState *goal,
                                          const GeorgeMathVec3 *source)
{
    float scale = goal->field18;
    float x = source->x + scale * goal->field44.x;
    float y = (source->y + scale * goal->field44.y) + 0.10000000149011612f;
    float z = source->z + scale * goal->field44.z;
    goal->field1C.field0C.x = x;
    goal->field1C.field0C.z = z;
    goal->field1C.field0C.y = y;
}

void func_001DF728(GeorgeGoalIntersectionState *goal)
{
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CCA28(OWNER(goal)->field18.bits);
    GeorgeGoalOwner *owner;
    u32 count;
    float x, z, scale;
    if (road == NULL) { goal->field11 = 2; return; }
    owner = OWNER(goal);
    count = func_001D0B30(road, ENTITY_POSITION(owner->field08), owner->field18.bits,
                         &owner->field14, OWNER_SIGNED_BYTE(owner, 0x22), &goal->field5C);
    goal->field13 = (u8)count;
    if ((u8)count == 0) return;
    x = func_0029C168((float)(func_00397178() % 10001) * 0.00062831852119416f);
    z = func_0029C090((float)(func_00397178() % 10001) * 0.00062831852119416f);
    goal->field44.x = x;
    *(u32 *)&goal->field44.y = 0;
    goal->field44.z = z;
    scale = (float)(func_00397178() % 10001) * 0.00014999999257270247f;
    goal->field12 = 0;
    goal->field18 = scale;
    intersection_target(goal, &goal->field5C);
    goal->field10 = 1;
    goal->field12 = (u8)(goal->field12 + 1);
}

void func_001DF8D8(GeorgeGoalIntersectionState *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    GeorgeGoalRoadGeometry *road;
    GeorgeGoalIntersectionRecord *record;
    GeorgeGoalRoadEntry *old_record;
    u32 mode = 0, word = 0xFFFFFFFFU, count;
    u8 flags, chosen_mode;
    goal->field1C.field20 = george_ee_minimum(owner->field60 * 45.0f, 1.0f);
    func_001DB968(owner, &goal->field1C);
    if (!(goal->field1C.field18 < 1.0f)) return;
    if (goal->field12 < goal->field13) {
        intersection_target(goal, ADDRESS(GeorgeMathVec3, &goal->field5C, (u32)goal->field12 * 12U));
        goal->field12 = (u8)(goal->field12 + 1);
        return;
    }
    road = (GeorgeGoalRoadGeometry *)func_001CCA28(OWNER(goal)->field18.bits);
    if (road == NULL) { goal->field11 = 2; return; }
    owner = OWNER(goal);
    count = func_001D0370(road, owner->field18.bits, owner->field14,
                         OWNER_SIGNED_BYTE(owner, 0x22), &goal->field5C,
                         &mode, &word, &goal->field14);
    goal->field13 = (u8)count;
    if ((u8)count != 0 && goal->field14 != 0xFF) {
        goal->field12 = 0;
        intersection_target(goal, &goal->field5C);
        goal->field12 = (u8)(goal->field12 + 1);
        return;
    }
    road = (GeorgeGoalRoadGeometry *)func_001CCA28(word);
    if (road == NULL) { goal->field11 = 2; return; }
    old_record = func_001D17D8(OWNER(goal)->field18.bits);
    /* The record address is computed after the helper returns; its containing
     * road's field38 may have changed during that call. */
    record = intersection_record(road, word);
    if (*ADDRESS(u32, old_record, 0x40) != record->field18) {
        OWNER_BYTE(OWNER(goal), 0x21) = 1;
        OWNER_BYTE(OWNER(goal), 0x20) = (u8)(record->field05 - 1);
    } else {
        OWNER_BYTE(OWNER(goal), 0x21) = 0;
        OWNER_BYTE(OWNER(goal), 0x20) = 0;
    }
    OWNER(goal)->field14 = word;
    flags = record->field04;
    if ((flags & 4) != 0) chosen_mode = (u8)-2;
    else if ((flags & 8) != 0) chosen_mode = 2;
    else chosen_mode = (u8)mode;
    OWNER_BYTE(OWNER(goal), 0x22) = chosen_mode;
    owner = OWNER(goal);
    func_001D1820(&goal->field1C.field0C, owner->field14, OWNER_BYTE(owner, 0x20),
                 OWNER_SIGNED_BYTE(owner, 0x22), owner->field1C);
    owner = OWNER(goal);
    func_001D18A8(&goal->field50, owner->field14, OWNER_BYTE(owner, 0x20), OWNER_BYTE(owner, 0x21));
    {
        float x = goal->field1C.field0C.x + goal->field50.x;
        float y = goal->field1C.field0C.y + goal->field50.y;
        float z = goal->field1C.field0C.z + goal->field50.z;
        goal->field1C.field0C.x = x;
        goal->field1C.field0C.y = y;
        goal->field1C.field0C.z = z;
    }
    goal->field10 = 2;
}

void func_001DFBD8(GeorgeGoalIntersectionState *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    const GeorgeMathVec3 *position;
    GeorgeMathVec3 delta;
    goal->field1C.field20 = george_ee_minimum(owner->field60 * 45.0f, 1.0f);
    func_001DB968(owner, &goal->field1C);
    if (!(goal->field1C.field18 < 1.0f)) return;
    position = ENTITY_POSITION(OWNER(goal)->field08);
    delta.x = position->x - goal->field1C.field0C.x;
    delta.y = position->y - goal->field1C.field0C.y;
    delta.z = position->z - goal->field1C.field0C.z;
    reciprocal_squared_scale(&delta);
    if (goal->field50.x * delta.x + goal->field50.y * delta.y + goal->field50.z * delta.z > 0.0f)
        goal->field11 = 1;
}

void func_001DFEB0(GeorgeGoalRoadState *goal)
{
    GeorgeGoalOwner *owner = ROAD_OWNER(goal);
    u32 index = OWNER_BYTE(owner, 0x20);
    u32 reverse = OWNER_BYTE(owner, 0x21);
    GeorgeGoalMotion *motion = ROAD_MOTION(goal);
    if (reverse == 0) index = (u8)(index + 1);
    func_001D18A8(&goal->field3C, owner->field14, index, (s32)reverse);
    owner = ROAD_OWNER(goal);
    func_001CFBF8(&owner->field80, owner->field14, OWNER_BYTE(owner, 0x20));
    owner = ROAD_OWNER(goal);
    if (func_001D1820(&motion->field0C, owner->field14, index,
                     OWNER_SIGNED_BYTE(owner, 0x22), owner->field1C) != 0) {
        const GeorgeMathVec3 *position;
        float x = motion->field0C.x + goal->field3C.x * 0.10000000149011612f;
        float y = motion->field0C.y + goal->field3C.y * 0.10000000149011612f;
        float z = motion->field0C.z + goal->field3C.z * 0.10000000149011612f;
        motion->field0C.x = x; motion->field0C.y = y; motion->field0C.z = z;
        goal->prefix.field10 = 1;
        goal->prefix.field11 = 0;
        *(u32 *)&goal->field54 = 0;
        position = ENTITY_POSITION(ROAD_OWNER(goal)->field08);
        goal->field48.x = position->x;
        goal->field48.y = position->y;
        goal->field48.z = position->z;
    } else goal->prefix.field11 = 2;
}

void func_001DFFE0(GeorgeGoalRoadState *goal)
{
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CCA28(ROAD_OWNER(goal)->field14);
    GeorgeGoalOwner *owner = ROAD_OWNER(goal);
    GeorgeGoalIntersectionRecord *record;
    u32 index;
    s32 next;
    if (road == NULL) { goal->prefix.field11 = 2; return; }
    record = intersection_record(road, owner->field14);
    if (OWNER_BYTE(owner, 0x21) == 0) {
        index = OWNER_BYTE(owner, 0x20);
        if ((s32)index < (s32)record->field05 - 2) {
            OWNER_BYTE(owner, 0x20) = (u8)(index + 1);
            goal->prefix.field10 = 1;
        } else {
            next = (s32)record->field1C;
            if (next >= 0) {
                road = (GeorgeGoalRoadGeometry *)func_001CCA28((u32)next);
                if (road != NULL) {
                    u32 word = record->field1C;
                    s32 mode;
                    GeorgeGoalIntersectionRecord *new_record;
                    owner = ROAD_OWNER(goal);
                    mode = OWNER_SIGNED_BYTE(owner, 0x22);
                    new_record = intersection_record(road, word);
                    if ((mode == 2 && (new_record->field04 & 4) != 0) ||
                        (mode == -2 && (new_record->field04 & 8) != 0)) {
                        OWNER_BYTE(ROAD_OWNER(goal), 0x21) = 1;
                    } else {
                        owner->field14 = word;
                        OWNER_BYTE(ROAD_OWNER(goal), 0x20) = 0;
                        goal->prefix.field10 = 1;
                    }
                } else OWNER_BYTE(ROAD_OWNER(goal), 0x21) = 1;
            } else if (next == -1) OWNER_BYTE(owner, 0x21) = 1;
            else {
                owner->field18.bits = (u32)next;
                goal->prefix.field11 = 1;
                return;
            }
        }
    } else {
        index = OWNER_BYTE(owner, 0x20);
        if (index != 0) {
            OWNER_BYTE(owner, 0x20) = (u8)(index - 1);
            goal->prefix.field10 = 1;
        } else {
            next = (s32)record->field18;
            if (next >= 0) {
                road = (GeorgeGoalRoadGeometry *)func_001CCA28((u32)next);
                if (road != NULL) {
                    u32 word = record->field18;
                    s32 mode;
                    GeorgeGoalIntersectionRecord *new_record;
                    owner = ROAD_OWNER(goal);
                    mode = OWNER_SIGNED_BYTE(owner, 0x22);
                    new_record = intersection_record(road, word);
                    if ((mode == 2 && (new_record->field04 & 4) != 0) ||
                        (mode == -2 && (new_record->field04 & 8) != 0)) {
                        OWNER_BYTE(owner, 0x21) = 0;
                    } else {
                        owner->field14 = word;
                        owner = ROAD_OWNER(goal);
                        OWNER_BYTE(owner, 0x20) = (u8)(intersection_record(road, owner->field14)->field05 - 2);
                        goal->prefix.field10 = 1;
                    }
                } else OWNER_BYTE(ROAD_OWNER(goal), 0x21) = 0;
            } else if (next == -1) OWNER_BYTE(owner, 0x21) = 0;
            else {
                owner->field18.bits = (u32)next;
                goal->prefix.field11 = 1;
                return;
            }
        }
    }
    func_001DFEB0(goal);
}

void func_001E0218(GeorgeGoalRoadState *goal)
{
    GeorgeGoalOwner *owner = ROAD_OWNER(goal);
    u32 state = owner->field08->field0C;
    GeorgeGoalMotion *motion = ROAD_MOTION(goal);
    GeorgeMathVec3 delta;
    const GeorgeMathVec3 *position;
    if (state == 2 || state == 0) {
        float timer = goal->field54 + owner->field60;
        goal->field54 = timer;
        if (timer > 1.2000000476837158f) {
            position = ENTITY_POSITION(owner->field08);
            delta.y = position->y - goal->field48.y;
            delta.x = position->x - goal->field48.x;
            delta.z = position->z - goal->field48.z;
            if (squared_length(&delta) < 1.0f) {
                goal->field54 = goal->prefix.field12 != 0 ? 1.2000000476837158f : 5.0f;
                goal->prefix.field10 = 2;
                goal->prefix.field12 = (u8)(goal->prefix.field12 + 1);
                return;
            }
            *(u32 *)&goal->field54 = 0;
            position = ENTITY_POSITION(owner->field08);
            goal->field48.x = position->x;
            goal->field48.y = position->y;
            goal->field48.z = position->z;
        }
    } else {
        *(u32 *)&goal->field54 = 0;
        position = ENTITY_POSITION(owner->field08);
        goal->field48.x = position->x;
        goal->field48.y = position->y;
        goal->field48.z = position->z;
    }
    owner = ROAD_OWNER(goal);
    motion->field20 = george_ee_minimum(owner->field60 * 45.0f, 1.0f);
    func_001DB968(owner, motion);
    ROAD_OWNER(goal)->field18.bits = 0xFFFFFFFFU;
    position = ENTITY_POSITION(ROAD_OWNER(goal)->field08);
    delta.x = position->x - motion->field0C.x;
    delta.y = position->y - motion->field0C.y;
    delta.z = position->z - motion->field0C.z;
    reciprocal_squared_scale(&delta);
    if (goal->field3C.x * delta.x + goal->field3C.y * delta.y + goal->field3C.z * delta.z >= 0.0f ||
        motion->field18 < 0.009999999776482582f)
        goal->prefix.field10 = 0;
}

void func_001E0630(GeorgeGoalRoadState *goal)
{
    GeorgeGoalZeroUnit zero;
    GeorgeGoalOwner *owner;
    float remaining;
    zero.field00 = 0;
    *(u32 *)&zero.field04 = 0;
    zero.field08 = 0;
    func_00177E48(ROAD_OWNER(goal)->field08, (const GeorgeMathVec3 *)&zero);
    owner = ROAD_OWNER(goal);
    remaining = goal->field54 - owner->field60;
    goal->field54 = remaining;
    if (remaining < 0.0f) {
        if (goal->prefix.field12 < 2) goal->prefix.field10 = 1;
        else {
            goal->prefix.field10 = 0;
            goal->prefix.field12 = 0;
            if (OWNER_BYTE(owner, 0x21) != 0) OWNER_BYTE(owner, 0x21) = 0;
            else OWNER_BYTE(owner, 0x21) = 1;
        }
        goal->field54 = 0.6000000238418579f;
    }
}

void func_001E8390(GeorgeGoalEnterVehicle *goal)
{
    GeorgeGoalVirtualObject *object = (GeorgeGoalVirtualObject *)goal->field14;
    GeorgeGoalOwner *owner;
    if (object != NULL) {
        const GeorgeGoalVirtualVector *method = (const GeorgeGoalVirtualVector *)(object->field04 + 0x38);
        const GeorgeMathVec3 *first = method->invoke(ADDRESS(void, object, (s32)method->adjustment));
        const GeorgeMathVec3 *second;
        object = (GeorgeGoalVirtualObject *)goal->field14;
        method = (const GeorgeGoalVirtualVector *)(object->field04 + 0x38);
        second = method->invoke(ADDRESS(void, object, (s32)method->adjustment));
        if (first->x * second->x + first->y * second->y + first->z * second->z > 1.0f)
            goal->field3E = 4;
        if ((signed char)goal->field40 != 0) {
            const GeorgeGoalVirtualInt *state;
            object = (GeorgeGoalVirtualObject *)goal->field14;
            state = (const GeorgeGoalVirtualInt *)(object->field04 + 0x1A0);
            if (state->invoke(ADDRESS(void, object, (s32)state->adjustment)) != 1) goal->field3E = 2;
        }
    }
    owner = OWNER(goal);
    ENTER_MOTION(goal)->field20 = george_ee_minimum(owner->field60 * 45.0f, 1.0f);
    func_001DB968(owner, ENTER_MOTION(goal));
    if (ENTER_MOTION(goal)->field18 < 1.0f) {
        if (goal->field3F != 0) goal->field3D = 0;
        else goal->field3D = 2;
    }
}

void func_001E8608(GeorgeGoalEnterVehicle *goal)
{
    GeorgeMathVec3 *target = &ENTER_MOTION(goal)->field0C;
    const GeorgeMathVec3 *source;
    GeorgeGoalOwner *owner;
    float angle;
    u8 index = (u8)(goal->field3F - 1);
    goal->field3F = index;
    source = ADDRESS(GeorgeMathVec3, goal, 0x18U + (u32)index * 12U);
    target->x = source->x; target->y = source->y; target->z = source->z;
    angle = func_001DC178(ENTITY_POSITION(OWNER(goal)->field08), target);
    owner = OWNER(goal);
    if (owner->field2C == 0) {
        owner->field34 = angle;
        owner->field30 = 6.2831854820251465f;
    }
    goal->field3D = 1;
}

void func_001E86A8(GeorgeGoalEnterVehicle *goal)
{
    u32 byte = *ADDRESS(u8, goal, 0x3C);
    if (func_001773F0(OWNER(goal)->field08, goal->field14, byte, byte) != 0) {
        GeorgeGoalOwner *owner = OWNER(goal);
        owner->field24 |= 8;
        goal->field3D = 3;
    } else goal->field3E = 2;
}

void func_001E8708(GeorgeGoalEnterVehicle *goal)
{
    GeorgeGoalOwner *owner = OWNER(goal);
    GeorgeGoalEntity *entity;
    if ((owner->field08->field0C ^ 0xEU) != 0) {
        owner->field24 &= 0xFFF7;
        goal->field3E = 4;
    }
    entity = OWNER(goal)->field08;
    if (entity->field0C == 0xE && *ADDRESS(u32, entity, 0x750) != 0) {
        func_001BAA68(entity->field364, goal->field14, (signed char)goal->field42, 0);
        owner = OWNER(goal);
        owner->field24 &= 0xFFF7;
        goal->field3E = 1;
    }
}
