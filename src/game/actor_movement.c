#include "george/actor_movement.h"
#include "george/accessors.h"

extern GeorgeActorStateRecord D_003F83F0[];
extern void *func_00239FD8(u32 word);
extern void *func_00238BA0(void *object, u32 key);
extern void func_001A5E90(void *object, void *value);
extern void func_001F6138(GeorgeGoalVirtualObject *object);
extern s32 func_00120EA0(void *reference, s32 word, GeorgeGoalEntity *entity);
extern void func_00121A80(void *reference, GeorgeMathVec3 *position, GeorgeMathVec3 *direction);
extern float func_0029B940(float first, float second);
extern float func_0029C168(float value);
extern float func_0029C090(float value);
extern GeorgeActorBits64 func_00374848(float value);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern s32 func_00373250(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern float func_00192DA8(GeorgeGoalEntity *entity);
extern s32 func_001F6308(s32 command);
extern s32 func_0022BCB0(const GeorgeMathVec3 *start, const GeorgeMathVec3 *end,
                       s32 mode, GeorgeActorCollisionRecord *output, s32 capacity);
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_0018FD40(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_0018D8A0(GeorgeGoalEntity *entity, const GeorgeMathVec3 *vector);
extern void func_00235CD8(void *object, u32 key, u32 value);
extern u32 *func_002AAF50(void *array, s32 index);
extern void func_002AAFD8(void *array);
extern void func_002393F8(u32 word);
extern void func_00272A58(void *object);
extern void func_00196980(GeorgeGoalEntity *entity);
extern void func_00192078(GeorgeGoalEntity *entity, u32 key, float adjustment);
extern s32 func_0018B710(GeorgeGoalEntity *entity, const GeorgeMathVec3 *direction,
                       const GeorgeMathVec3 *position);
extern void func_00191DC8(GeorgeGoalEntity *entity, u32 word);
extern void *func_00192748(GeorgeGoalEntity *entity, u32 index);
extern void func_002A1C60(const void *matrix, const GeorgeMathVec3 *input, GeorgeMathVec3 *output);
extern void func_002A1C30(float *matrix);
extern void func_002A1C08(void *output, const void *input);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))
#define PAIR(object, offset, type) ((const type *)ADDRESS((object)->field04, offset))
#define VEHICLE(entity) FIELD(entity, 0x730, GeorgeGoalVirtualObject *)

/* Binding uses the captured selector/target; this adjustment uses a fresh
 * actor state after the binding loads. Keep the sum at full signed 32 bits. */
static __inline__ void actor_member(GeorgeGoalEntity *entity, u32 binding_state,
                                   u32 member_offset)
{
    GeorgeGoalMember *member = (GeorgeGoalMember *)ADDRESS(D_003F83F0,
                                      binding_state * 0x1CU + member_offset);
    s32 selector = member->selector;
    void (*invoke)(void *);
    GeorgeGoalVirtualVoid pair;
    s32 adjustment;
    if (selector == 0) return;
    if (selector < 0) {
        invoke = member->target.direct;
    } else {
        const GeorgeGoalVirtualVoid *table = FIELD(entity,
                          member->target.vtable_offset, const GeorgeGoalVirtualVoid *);
        pair = *(const GeorgeGoalVirtualVoid *)ADDRESS(table, (u32)selector * 8U - 8U);
        invoke = pair.invoke;
    }
    adjustment = ((GeorgeGoalMember *)ADDRESS(D_003F83F0,
                      entity->field0C * 0x1CU + member_offset))->adjustment;
    if (selector > 0) adjustment += pair.adjustment;
    invoke(ADJUST(entity, adjustment));
}

void func_00170538(GeorgeGoalEntity *entity)
{
    u32 state = entity->field0C;
    GeorgeGoalMember *member = (GeorgeGoalMember *)ADDRESS(D_003F83F0,
                                                          state * 0x1CU + 8U);
    if (member->selector != 0) {
        actor_member(entity, state, 8);
        state = entity->field0C;
    }
    entity->field0C = 0xFFFFFFFFU;
    FIELD(entity, 0x10, u32) = state;
}

s32 func_00173648(GeorgeGoalEntity *entity)
{
    if (entity->field0C != 0 && entity->field0C != 2) return 0;
    func_00170538(entity);
    entity->field0C = 7;
    actor_member(entity, 7, 0);
    return 1;
}

s32 func_00173720(GeorgeGoalEntity *entity)
{
    if (entity->field0C != 0 && entity->field0C != 2 && entity->field0C != 23) return 0;
    func_00170538(entity);
    entity->field0C = 8;
    actor_member(entity, 8, 0);
    FIELD(entity, 0x45C, u32) = 0;
    return 1;
}

s32 func_00173818(GeorgeGoalEntity *entity)
{
    if (entity->field0C == 12) {
        func_00170538(entity);
        entity->field0C = 0;
        actor_member(entity, 0, 0);
    } else if (entity->field0C != 0 && entity->field0C != 2) return 0;
    func_00170538(entity);
    entity->field0C = 35;
    actor_member(entity, 35, 0);
    return 1;
}

s32 func_001739A0(GeorgeGoalEntity *entity, const GeorgeMathVec3 *vector)
{
    float x, z, length;
    if (entity->field0C != 4) return 0;
    x = FIELD(entity, 0x4C, float);
    z = FIELD(entity, 0x54, float);
    length = george_ee_square_root(x * x + z * z);
    if (length < FIELD(entity->field18, 0x4C, float)) return 0;
    if (FIELD(entity, 0x42C, float) == vector->x &&
        FIELD(entity, 0x430, float) == vector->y &&
        FIELD(entity, 0x434, float) == vector->z) return 0;
    func_00170538(entity);
    FIELD(entity, 0x42C, float) = vector->x;
    FIELD(entity, 0x430, float) = vector->y;
    FIELD(entity, 0x434, float) = vector->z;
    entity->field0C = 33;
    actor_member(entity, 33, 0);
    return 1;
}

s32 func_00173B20(GeorgeGoalEntity *entity, s32 value)
{
    if (value != 0) {
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 1ULL) != 0) return 1;
        func_0018FD30(entity, 1ULL);
    } else func_0018FD40(entity, 1ULL);
    if (value == 0) return 0;
    if (entity->field0C != 0 && entity->field0C != 2 && entity->field0C != 12) return 0;
    func_00170538(entity);
    entity->field0C = 34;
    actor_member(entity, 34, 0);
    return 1;
}

void func_00174770(GeorgeGoalEntity *entity, s32 value, float adjustment)
{
    GeorgeMathVec3 direction, position;
    float scale, x, y, z;
    if (entity->field0C == 13) return;
    /* Caller 0x001903E8 supplies live f12, consumed by 0x00192078. */
    func_00192078(entity, 0xEC789588U, adjustment);
    direction.x = entity->fieldD0.x;
    direction.y = entity->fieldD0.y + 1.0f;
    direction.z = entity->fieldD0.z;
    if (value == 0) {
        direction.x = -direction.x;
        direction.y = -direction.y;
        direction.z = -direction.z;
    }
    scale = FIELD(entity->field18, 0x10, float);
    func_002A35C0(&position, VECTOR(entity, 0x4C), scale + scale);
    x = FIELD(entity, 0x40, float);
    z = FIELD(entity, 0x48, float);
    y = FIELD(entity, 0x44, float);
    position.x += x;
    position.y += y;
    position.z += z;
    func_0018B710(entity, &direction, &position);
}

/* All three collections reload their actor pointer after every callback. */
#define STEP_COLLECTION(offset) do { \
    GeorgeGoalVirtualObject *collection = FIELD(entity, offset, GeorgeGoalVirtualObject *); \
    const GeorgeGoalVirtualFloat *begin; \
    const GeorgeGoalVirtualInt *count_pair; \
    u32 count, index; \
    if (collection != 0) { \
        begin = PAIR(collection, 0x28, GeorgeGoalVirtualFloat); \
        begin->invoke(ADJUST(collection, begin->adjustment), step); \
        collection = FIELD(entity, offset, GeorgeGoalVirtualObject *); \
        count_pair = PAIR(collection, 0x38, GeorgeGoalVirtualInt); \
        count = (u32)count_pair->invoke(ADJUST(collection, count_pair->adjustment)); \
        for (index = 0; index < count; ++index) { \
            const GeorgeActorVirtualIndex *entry_pair; \
            const GeorgeGoalVirtualFloat *update; \
            GeorgeGoalVirtualObject *entry; \
            collection = FIELD(entity, offset, GeorgeGoalVirtualObject *); \
            entry_pair = PAIR(collection, 0x40, GeorgeActorVirtualIndex); \
            entry = entry_pair->invoke(ADJUST(collection, entry_pair->adjustment), index); \
            update = PAIR(entry, 0x48, GeorgeGoalVirtualFloat); \
            update->invoke(ADJUST(entry, update->adjustment), step); \
        } \
    } \
} while (0)

void func_001765D0(GeorgeGoalEntity *entity, float step)
{
    STEP_COLLECTION(0x29C);
    STEP_COLLECTION(0x2A0);
    STEP_COLLECTION(0x2A4);
}
#undef STEP_COLLECTION

s32 func_00176E10(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x3BC, u32) != 0x7F9000CFU && entity->field0C != 13 &&
        (FIELD(entity, 0x190, GeorgeActorBits64) & 0x20ULL) == 0) {
        GeorgeGoalVirtualObject *vehicle = entity->field0C == 14 ? VEHICLE(entity) : 0;
        func_00170538(entity);
        FIELD(entity, 0x724, GeorgeGoalVirtualObject *) = vehicle;
        if (vehicle != 0) func_001F6138(vehicle);
        entity->field0C = 13;
        actor_member(entity, 13, 0);
    }
    FIELD(entity, 0x448, u32) = 0;
    FIELD(entity, 0x450, u32) = 0;
    FIELD(entity, 0x44C, u32) = 0;
    FIELD(entity, 0x43C, u32) = 0;
    FIELD(entity, 0x444, u32) = 0;
    FIELD(entity, 0x440, u32) = 0;
    return entity->field0C == 13;
}

/* State selection from the complete 27-entry retail table at 0x0042C480. */
s32 func_00176F58(GeorgeGoalEntity *entity)
{
    u32 state;
    float value;
    if (entity->field2D8 == 7) return 0;
    state = entity->field0C;
    if (state == 4) {
        FIELD(entity, 0x52E, u8) = 1;
        return 1;
    }
    if (state == 25) {
        if (FIELD(entity, 0x880, u32) != 200) return 0;
        func_00170538(entity);
        FIELD(entity, 0x52C, u8) = 0;
        value = FIELD(entity->field18, 0x248, float);
        FIELD(entity, 0x538, u32) = 13;
        FIELD(entity, 0x534, float) = value;
        FIELD(entity, 0x53C, u32) = 14;
    } else {
        if (state != 0 && state != 2 && state != 12 && state != 26) return 0;
        func_00170538(entity);
        FIELD(entity, 0x538, u32) = 13;
        FIELD(entity, 0x53C, u32) = 14;
        FIELD(entity, 0x52C, u8) = 0;
        FIELD(entity, 0x530, u32) = 0;
        value = FIELD(entity->field18, 0x3C, float);
        FIELD(entity, 0x540, u32) = 0;
        FIELD(entity, 0x534, float) = value;
        FIELD(entity, 0x548, u32) = 0;
        FIELD(entity, 0x544, u32) = 0;
        entity->field0C = 4;
        actor_member(entity, 4, 0);
        return 1;
    }
    FIELD(entity, 0x530, u32) = 0;
    FIELD(entity, 0x540, u32) = 0;
    FIELD(entity, 0x548, u32) = 0;
    FIELD(entity, 0x544, u32) = 0;
    entity->field0C = 4;
    actor_member(entity, 4, 0);
    return 1;
}

void func_00177160(GeorgeGoalEntity *entity)
{
    u32 word = FIELD(entity->field18, 0x11C, u32);
    GeorgeMathVec3 position;
    float matrix[16];
    void *object;
    GeorgeGoalEntityData *data;
    float scale, x, y, z;
    if (word == 0) return;
    func_00191DC8(entity, word);
    object = func_00192748(entity, 41);
    func_002A1C60(ADDRESS(entity, 0xF0), VECTOR(object, 0x30), &position);
    data = entity->field18;
    scale = FIELD(data, 0x124, float);
    z = FIELD(entity, 0x118, float) * scale;
    y = FIELD(entity, 0x114, float) * scale;
    x = FIELD(entity, 0x110, float) * scale;
    position.y += y;
    position.x += x;
    position.z += z;
    position.y += FIELD(data, 0x120, float);
    func_002A1C30(matrix);
    matrix[12] = position.x;
    matrix[13] = position.y;
    matrix[15] = 1.0f;
    matrix[14] = position.z;
    object = FIELD(entity, 0x298, void *);
    func_002A1C08(ADDRESS(object, 0x10), matrix);
    FIELD(object, 0xA0, u32) |= 0x100U;
    func_00235CD8(FIELD(entity, 0x298, void *), 0x9F79558FU, 1);
}

s32 func_00177270(GeorgeGoalEntity *entity, const GeorgeMathVec3 *target,
                 u32 word, float height)
{
    if (word != 0) {
        void *value = func_00238BA0(func_00239FD8(word), 0xDE01EBFEU);
        if (FIELD(entity->field18, 0x1D8, u32) == 0x45A78000U) {
            FIELD(FIELD(entity, 0x20, void *), 0x34, u32) = 0;
            func_001A5E90(FIELD(entity, 0x20, void *), value);
        }
    }
    func_00170538(entity);
    FIELD(entity, 0x52C, u8) = 1;
    FIELD(entity, 0x514, float) = target->x;
    FIELD(entity, 0x518, float) = target->y;
    FIELD(entity, 0x51C, float) = target->z;
    FIELD(entity, 0x520, float) = height;
    FIELD(entity, 0x538, u32) = 13;
    FIELD(entity, 0x53C, u32) = 14;
    FIELD(entity, 0x530, u32) = 0;
    {
        float value = FIELD(entity->field18, 0x3C, float);
        FIELD(entity, 0x540, u32) = 0;
        FIELD(entity, 0x534, float) = value;
    }
    FIELD(entity, 0x548, u32) = 0;
    FIELD(entity, 0x544, u32) = 0;
    entity->field0C = 4;
    FIELD(entity, 0x558, u32) = 0;
    actor_member(entity, 4, 0);
    return 1;
}

s32 func_001773F0(GeorgeGoalEntity *entity, u32 object, u32 value0, u32 value1)
{
    GeorgeGoalVirtualObject *vehicle = (GeorgeGoalVirtualObject *)object;
    const GeorgeGoalVirtualCommand *predicate;
    const GeorgeActorVirtualConfigure *configure;
    s32 narrow;
    if (value1 < 2U) {
        if (value0 - 2U < 2U) return 0;
    } else if (value0 < 2U) return 0;
    if (entity->field0C == 14) return 0;
    predicate = PAIR(vehicle, 0x1B8, GeorgeGoalVirtualCommand);
    if (predicate->invoke(ADJUST(vehicle, predicate->adjustment), (signed char)value0) != 0) return 0;
    predicate = PAIR(vehicle, 0x1B8, GeorgeGoalVirtualCommand);
    narrow = (signed char)value1;
    if (predicate->invoke(ADJUST(vehicle, predicate->adjustment), narrow) != 0) return 0;
    func_00170538(entity);
    VEHICLE(entity) = vehicle;
    func_001F6138(vehicle);
    vehicle = VEHICLE(entity);
    configure = PAIR(vehicle, 0x1A8, GeorgeActorVirtualConfigure);
    configure->invoke(ADJUST(vehicle, configure->adjustment), entity, narrow);
    FIELD(entity, 0x734, u32) = value0;
    FIELD(entity, 0x738, u32) = value1;
    entity->field0C = 14;
    FIELD(entity, 0x73C, u32) = 0;
    actor_member(entity, 14, 0);
    return 1;
}

static __inline__ const GeorgeMathVec3 *actor_vehicle_vector(GeorgeGoalEntity *entity)
{
    GeorgeGoalVirtualObject *vehicle = VEHICLE(entity);
    const GeorgeGoalVirtualVector *pair = PAIR(vehicle, 0x38, GeorgeGoalVirtualVector);
    return pair->invoke(ADJUST(vehicle, pair->adjustment));
}

static __inline__ void actor_command_vector(GeorgeGoalEntity *entity, s32 command,
                                           GeorgeMathVec3 *output)
{
    GeorgeGoalVirtualObject *vehicle = VEHICLE(entity);
    const GeorgeActorVirtualCommandVector *pair = PAIR(vehicle, 0xD8, GeorgeActorVirtualCommandVector);
    pair->invoke(ADJUST(vehicle, pair->adjustment), command, output, 0);
}

static __inline__ s32 actor_other_collision(GeorgeGoalEntity *entity,
                      GeorgeActorCollisionRecord *records, s32 count)
{
    s32 other = 0;
    if (count > 0) do {
        GeorgeGoalVirtualObject *vehicle = VEHICLE(entity);
        const GeorgeGoalVirtualInt *pair = PAIR(vehicle, 0xA0, GeorgeGoalVirtualInt);
        u32 identity;
        --count;
        identity = (u32)pair->invoke(ADJUST(vehicle, pair->adjustment));
        if (records->field00 != identity) other = 1;
        ++records;
    } while (count != 0);
    return other;
}

s32 func_001775A8(GeorgeGoalEntity *entity, s32 command)
{
    const GeorgeMathVec3 *first, *second;
    const float *basis;
    GeorgeGoalVirtualObject *vehicle;
    const GeorgeGoalVirtualVector *basis_pair;
    GeorgeActorBits64 value;
    GeorgeMathVec3 start, end;
    GeorgeActorCollisionRecord records[2];
    s32 mode, count;
    float dot, height;
    if (entity->field0C != 14 || FIELD(entity, 0x750, u32) == 0) return 0;
    first = actor_vehicle_vector(entity);
    second = actor_vehicle_vector(entity);
    dot = (first->x * second->x + first->y * second->y) + first->z * second->z;
    if (!(dot < 1.0f)) return 0;
    vehicle = VEHICLE(entity);
    basis_pair = PAIR(vehicle, 0x98, GeorgeGoalVirtualVector);
    basis = (const float *)basis_pair->invoke(ADJUST(vehicle, basis_pair->adjustment));
    value = func_00374848(basis[5]);
    if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value);
    /* Exact 64-bit immediate assembled by the original shifts, not decimal rounding. */
    if (func_00373250(value, 0x3FEE666660000000ULL) <= 0) return 0;
    start.x = FIELD(entity, 0x40, float);
    start.y = FIELD(entity, 0x44, float);
    start.z = FIELD(entity, 0x48, float);
    height = func_00192DA8(entity);
    start.y += height;
    if (command == -1) command = FIELD(entity, 0x734, s32);
    actor_command_vector(entity, command, &end);
    height = func_00192DA8(entity);
    end.y += height;
    mode = (FIELD(entity, 0x190, GeorgeActorBits64) & 0x10000000ULL) != 0 ? 18 : 19;
    count = func_0022BCB0(&start, &end, mode, records, 2);
    if (actor_other_collision(entity, records, count)) {
        command = func_001F6308(command);
        actor_command_vector(entity, command, &end);
        height = func_00192DA8(entity);
        end.y += height;
        count = func_0022BCB0(&start, &end, mode, records, 2);
        if (actor_other_collision(entity, records, count)) command = -1;
    }
    if (command < 0) return 0;
    FIELD(entity, 0x738, s32) = command;
    FIELD(entity, 0x754, u32) = 1;
    return 1;
}

s32 func_001778C8(GeorgeGoalEntity *entity, u32 object, s32 value)
{
    GeorgeGoalVirtualObject *vehicle = (GeorgeGoalVirtualObject *)object;
    const GeorgeActorVirtualConfigure *configure;
    if (entity->field0C == 14) return 0;
    func_00170538(entity);
    VEHICLE(entity) = vehicle;
    func_001F6138(vehicle);
    vehicle = VEHICLE(entity);
    configure = PAIR(vehicle, 0x1A8, GeorgeActorVirtualConfigure);
    configure->invoke(ADJUST(vehicle, configure->adjustment), entity, (signed char)value);
    FIELD(entity, 0x738, s32) = value;
    FIELD(entity, 0x73C, u32) = 1;
    entity->field0C = 14;
    FIELD(entity, 0x734, s32) = value;
    actor_member(entity, 14, 0);
    return 1;
}

s32 func_001779F8(GeorgeGoalEntity *entity, void *reference, s32 word)
{
    GeorgeMathVec3 position, direction;
    float angle;
    if (entity->field2D8 == 4 || entity->field2D8 == 6) return 0;
    if (entity->field0C != 0 && entity->field0C != 2 && entity->field0C != 12) return 0;
    if (reference == 0 || func_00120EA0(reference, word, entity) == 0) return 0;
    if (FIELD(reference, 0x30, u32) == 0) return 0;
    func_00170538(entity);
    FIELD(entity, 0x56C, u32) = FIELD(reference, 0x2C, u32);
    FIELD(entity, 0x570, u32) = FIELD(reference, 0x30, u32);
    FIELD(entity, 0x574, u32) = FIELD(reference, 0x34, u32);
    FIELD(entity, 0x578, u32) = FIELD(reference, 0x38, u32);
    func_00121A80(reference, &position, &direction);
    angle = func_0029B940(direction.z, direction.x);
    FIELD(entity, 0x57C, float) = angle;
    FIELD(entity, 0x580, void *) = reference;
    entity->field0C = 6;
    actor_member(entity, 6, 0);
    return 1;
}

void func_00177B80(GeorgeGoalEntity *entity)
{
    if (entity->field0C == 0) return;
    func_00170538(entity);
    entity->field0C = 0;
    actor_member(entity, 0, 0);
}

s32 func_00177C40(GeorgeGoalEntity *entity, u32 word0, u32 word1,
                 u32 word2, u32 word3, u32 word4, u32 override_word, float value)
{
    u32 state = entity->field0C;
    void *array;
    if (state == 1 || state == 3 || state == 6 || state == 11 || state == 13 || state == 14)
        return 0;
    if (state == 12 && override_word == 0) return 0;
    if (FIELD(entity, 0x23C, void *) != 0)
        func_00235CD8(FIELD(entity, 0x23C, void *), 0xB95616B6U, 1);
    array = FIELD(entity, 0x294, void *);
    if (array != 0) {
        s32 count = (s32)func_002AAF88(array), index;
        for (index = 0; index < count; ++index) {
            u32 word = *func_002AAF50(FIELD(entity, 0x294, void *), index);
            if (word != 0) func_002393F8(word);
        }
        func_002AAFD8(FIELD(entity, 0x294, void *));
    }
    if (entity->field2D8 == 1) {
        func_00272A58(FIELD(entity, 0x1B0, void *));
        if (FIELD(entity, 0x300, u32) != 0) func_00196980(entity);
        entity->field2D8 = 0;
    }
    func_00170538(entity);
    FIELD(entity, 0x704, u32) = word1;
    entity->field700 = word0;
    FIELD(entity, 0x708, u32) = word2;
    FIELD(entity, 0x70C, u32) = word3;
    FIELD(entity, 0x71C, u32) = word4;
    FIELD(entity, 0x720, float) = value;
    entity->field0C = 12;
    actor_member(entity, 12, 0);
    return 1;
}

static __inline__ void actor_project(GeorgeGoalEntity *entity,
                               const GeorgeMathVec3 *input, GeorgeMathVec3 *output)
{
    void *object = FIELD(entity, 0x20, void *);
    const u8 *table = FIELD(object, 0, const u8 *);
    const GeorgeGoalVirtualVector *pair = (const GeorgeGoalVirtualVector *)ADDRESS(table, 0x58);
    const GeorgeMathVec3 *basis = pair->invoke(ADJUST(object, pair->adjustment));
    float dot = (input->x * basis->x + input->y * basis->y) + input->z * basis->z;
    float scale;
    if (0.300000011920928955078125f < dot) dot = 1.0f;
    else if (dot < -0.300000011920928955078125f) dot = -1.0f;
    scale = dot * george_ee_square_root((input->x * input->x + input->y * input->y) + input->z * input->z);
    output->x = basis->x * scale;
    output->y = basis->y * scale;
    output->z = basis->z * scale;
}

s32 func_00177E48(GeorgeGoalEntity *entity, const GeorgeMathVec3 *input)
{
    u32 state;
    GeorgeMathVec3 projected, normalized, output;
    GeorgeGoalEntityData *data;
    GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
    float forward, vertical, side, weight, strength, length, threshold, turn;
    if (0.0f < FIELD(entity, 0x438, float)) return 0;
    state = entity->field0C;
    if (state == 3 || state == 4 || state == 27 || state == 29 || state == 30) {
        float scale;
        FIELD(entity, 0x74, u32) = 0;
        scale = FIELD(entity->field18, 0x10, float);
        motion->x = input->x * scale;
        motion->y = input->y * scale;
        motion->z = input->z * scale;
        FIELD(entity, 0x190, GeorgeActorBits64) &= 0xFFFFFFFFFFFFEFFFULL;
        return 1;
    }
    if (state == 10 || state == 14 || state == 26 || state == 41) {
        motion->x = input->x;
        motion->y = input->y;
        motion->z = input->z;
        FIELD(entity, 0x74, u32) = 0;
        FIELD(entity, 0x190, GeorgeActorBits64) &= 0xFFFFFFFFFFFFEFFFULL;
        return 1;
    }
    if (state == 31) {
        if (FIELD(entity->field18, 0x1D8, u32) == 0x45A78000U) {
            actor_project(entity, input, &projected);
            input = &projected;
        }
        length = george_ee_square_root((input->x * input->x + input->y * input->y) + input->z * input->z);
        func_002A35C0(&output, input, length * FIELD(entity->field18, 0x10, float));
        func_0018D8A0(entity, &output);
        return 1;
    }
    if (state < 42U && state != 0 && state != 2 && state != 5 && state != 12 && state != 23 && state != 32) {
        FIELD(entity, 0x68, u32) = 0;
        FIELD(entity, 0x70, u32) = 0;
        FIELD(entity, 0x6C, u32) = 0;
        FIELD(entity, 0x74, u32) = 0;
        FIELD(entity, 0x190, GeorgeActorBits64) &= 0xFFFFFFFFFFFFEFFFULL;
        return 0;
    }
    data = entity->field18;
    if (FIELD(data, 0x1D8, u32) == 0x45A78000U && FIELD(data, 0x1E0, float) <= 0.0f) {
        actor_project(entity, input, &projected);
        input = &projected;
    }
    forward = (input->x * entity->fieldD0.x + input->y * entity->fieldD0.y) + input->z * entity->fieldD0.z;
    vertical = (input->x * FIELD(entity, 0xC0, float) + input->y * FIELD(entity, 0xC4, float)) + input->z * FIELD(entity, 0xC8, float);
    side = (input->x * FIELD(entity, 0xB0, float) + input->y * FIELD(entity, 0xB4, float)) + input->z * FIELD(entity, 0xB8, float);
    data = entity->field18;
    weight = forward < 0.0f ? FIELD(data, 0x20, float) : 1.0f;
    strength = george_ee_square_root(((weight * forward) * forward +
                      (FIELD(data, 0x24, float) * side) * side) + vertical * vertical);
    func_002A35C0(motion, input, strength * FIELD(data, 0x10, float));
    data = entity->field18;
    if (FIELD(data, 0x1D4, u32) == 0) {
        length = george_ee_square_root((motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z);
        if (length < 0.300000011920928955078125f) {
            FIELD(entity, 0x68, u32) = 0;
            FIELD(entity, 0x6C, u32) = 0;
            FIELD(entity, 0x70, u32) = 0;
        } else {
            float scale = FIELD(data, 0x10, float);
            if (length < FIELD(data, 0x2C, float) * scale) {
                func_002A35C0(motion, motion, FIELD(data, 0x14, float));
                strength = FIELD(entity->field18, 0x2C, float);
            } else {
                strength = 1.0f;
                func_002A35C0(motion, motion, scale);
            }
        }
    }
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x400000ULL) != 0)
        FIELD(entity, 0xA0, s32) = func_00179070(entity, FIELD(entity->field18, 0x28, float));
    else FIELD(entity, 0xA0, u32) = 0;
    data = entity->field18;
    if (FIELD(entity, 0xA0, u32) - 2U < 2U) threshold = FIELD(data, 0x34, float);
    else if (forward < 0.0f) threshold = FIELD(data, 0x30, float);
    else threshold = FIELD(data, 0x2C, float);
    if (threshold < strength) func_0018FD30(entity, 0x1000ULL);
    else func_0018FD40(entity, 0x1000ULL);
    if (FIELD(entity, 0xA0, u32) - 2U < 2U || entity->field0C == 23) {
        FIELD(entity, 0x74, u32) = 0;
    } else {
        func_002A35C0(&normalized, motion, 1.0f);
        turn = (normalized.x * FIELD(entity, 0xB0, float) + normalized.y * FIELD(entity, 0xB4, float)) + normalized.z * FIELD(entity, 0xB8, float);
        if (0.0f <= forward) turn = -turn;
        FIELD(entity, 0x74, float) = turn;
    }
    return 1;
}

s32 func_00179070(GeorgeGoalEntity *entity, float threshold)
{
    GeorgeMathVec3 axis, normalized;
    float first = func_0029C168(FIELD(entity, 0x58, float));
    float second = func_0029C090(FIELD(entity, 0x58, float));
    float dot;
    axis.x = first;
    axis.y = 0.0f;
    axis.z = second;
    func_002A35C0(&normalized, VECTOR(entity, 0x68), 1.0f);
    dot = (axis.x * normalized.x + axis.y * normalized.y) + axis.z * normalized.z;
    if (threshold <= dot) return 0;
    if (dot <= -threshold) return 1;
    dot = (-second * normalized.x) + first * normalized.z;
    if (dot <= 0.0f) return 2;
    return 3;
}

#undef VEHICLE
#undef PAIR
#undef ADJUST
#undef VECTOR
#undef FIELD
#undef ADDRESS
