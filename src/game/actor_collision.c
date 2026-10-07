#include "george/actor_collision.h"
#include "george/engine_angles.h"
#include "george/segment_distance.h"

#define AT(type, object, offset) ((type *)((u32)(object) + (u32)(offset)))
#define FIELD(type, object, offset) (*AT(type, object, offset))
#if __GNUC__ >= 3
#define COLLISION_INLINE static __inline__ __attribute__((always_inline))
#else
#define COLLISION_INLINE static __inline__
#endif

extern GeorgeSlotPool D_0045C6A0;
extern void *D_003F8B58;
extern const GeorgeMathVec3 D_FLT_00437288;
extern u32 func_001C1540(void *manager, const GeorgeMathVec3 *start,
                       const GeorgeMathVec3 *end, void **output,
                       u32 capacity, float radius);
extern u16 *func_001CEA80(const GeorgeMathVec3 *point, GeorgeGoalRoad **road,
                        u8 *vertex, s32 *mode, float radius);
extern unsigned long long func_00374848(float value);
extern s32 func_00373250(unsigned long long left, unsigned long long right);
extern unsigned long long func_00372CC0(unsigned long long left, unsigned long long right);
extern float func_003734F8(unsigned long long value);

typedef struct CollisionTurn {
    signed char slot, vertex, flag, factor;
    const GeorgeMathVec3 *position, *basis;
} CollisionTurn;
typedef const GeorgeMathVec3 *(*CollisionVectorCall)(void *);
typedef float (*CollisionFloatCall)(void *);

COLLISION_INLINE float dot3(const GeorgeMathVec3 *a, const GeorgeMathVec3 *b)
{
    return (a->x * b->x + a->y * b->y) + a->z * b->z;
}

COLLISION_INLINE float square3(float x, float y, float z)
{
    return (x * x + y * y) + z * z;
}

COLLISION_INLINE void inverse_squared(GeorgeMathVec3 *value)
{
    float x = value->x, y = value->y, z = value->z;
    float square = square3(x, y, z);
    if (0.00000999999974737875163555145263671875f < square) {
        float reciprocal = 1.0f / square;
        value->z = z * reciprocal;
        value->x = x * reciprocal;
        value->y = y * reciprocal;
    }
}

COLLISION_INLINE float signed_angle(const GeorgeMathVec3 *direction,
                                   const GeorgeMathVec3 *vector,
                                   const GeorgeMathVec3 *basis)
{
    float angle = func_0029C230(dot3(direction, vector));
    if (dot3(basis, vector) <= 0.0f) angle = -angle;
    return angle;
}

COLLISION_INLINE unsigned long long soft_absolute_bits(float value)
{
    unsigned long long bits = func_00374848(value);
    if (func_00373250(bits, 0) < 0) bits = func_00372CC0(0, bits);
    return bits;
}

COLLISION_INLINE float absolute_angle(const GeorgeMathVec3 *direction,
                                     const GeorgeMathVec3 *vector)
{
    return func_003734F8(soft_absolute_bits(func_0029C230(dot3(direction, vector))));
}

COLLISION_INLINE const GeorgeMathVec3 *road_vertex(GeorgeGoalRoad *road,
                                                 u32 record, u32 halfword,
                                                 u32 vertex)
{
    void *entry = AT(u8, FIELD(void *, road, 0x38), record * 0x34U);
    u32 index = FIELD(u16, entry, halfword) + vertex;
    return AT(GeorgeMathVec3, FIELD(void *, road, 0x48), index * 12U);
}

COLLISION_INLINE const GeorgeMathVec3 *virtual_position(void *object)
{
    u8 *table = FIELD(u8 *, object, 4);
    CollisionVectorCall call = FIELD(CollisionVectorCall, table, 0x2C);
    return call(AT(u8, object, (s32)FIELD(s16, table, 0x28)));
}

COLLISION_INLINE float virtual_radius(void *object)
{
    u8 *table = FIELD(u8 *, object, 4);
    CollisionFloatCall call = FIELD(CollisionFloatCall, table, 0x44);
    return call(AT(u8, object, (s32)FIELD(s16, table, 0x40)));
}

COLLISION_INLINE void turn_position(GeorgeMathVec3 *output, const CollisionTurn *turn)
{
    float factor = (float)turn->factor;
    float z = turn->position->z + factor * turn->basis->z;
    float x = turn->position->x + factor * turn->basis->x;
    float y = turn->position->y + factor * turn->basis->y;
    output->z = z; output->x = x; output->y = y;
}

void func_001E52E8(void *state, GeorgeActorCollisionNode *node)
{
    u32 count = FIELD(u8, state, 0x42);
    FIELD(GeorgeActorCollisionNode *, state, 0x6CU + count * 4U) = node;
    count = FIELD(u8, state, 0x42);
    FIELD(u8, state, 0x42) = (u8)(count + 1U);
}

GeorgeActorCollisionNode *func_001E2EC8(
    void *state, const GeorgeMathVec3 *point, const GeorgeMathVec3 *direction,
    u32 depth, s32 input_slot, u32 input_vertex, s32 flag, s32 control,
    float scalar0, float scalar1)
{
    GeorgeActorCollisionNode *node, *child;
    GeorgeGoalRoad *road, *queried_road;
    const GeorgeMathVec3 *previous0 = NULL, *previous1 = NULL;
    const GeorgeMathVec3 *first, *second, *basis_point;
    GeorgeMathVec3 target, basis, vector, displacement, lateral, edge, object_vector;
    CollisionTurn turn0, turn1;
    void *record, *objects[32];
    u32 key, record_index, count, object_index;
    u8 vertex = (u8)input_vertex;
    signed char slot = (signed char)input_slot;
    s32 final_control = (signed char)control, orientation, step, state_code = 0;
    /* Original SP+104 is an output slot, never initialized here. */
    s32 queried_mode;
    float angle0, angle1, bound0, bound1, length0, length1;
    float length, radius, angle, fraction0, fraction1;
    float point_x, point_y, point_z;
    s32 made_child;

    if (5 - (s32)FIELD(u8, state, 0x42) <= 0) return NULL;
    road = func_001CCA28(FIELD(u32, state, 0x14U + (u32)(s32)slot * 8U));
    if (road == NULL) goto no_road;
    node = (GeorgeActorCollisionNode *)func_002AD700(&D_0045C6A0);
    node->field00.x = point->x;
    node->field00.y = point->y;
    {
        float z = point->z;
        node->field10 = scalar1;
        node->field00.z = z;
    }
    node->field0C = scalar0;
    node->field14 = NULL; node->field18 = NULL; node->field1C = NULL;
    /* This gate uses the full input, before its separate byte narrowing. */
    if (control != 0 || 100.0f <= scalar0 || !(depth < 4U)) return node;

    basis.z = D_FLT_00437288.x * direction->y - D_FLT_00437288.y * direction->x;
    basis.y = D_FLT_00437288.z * direction->x - D_FLT_00437288.x * direction->z;
    basis.x = D_FLT_00437288.y * direction->z - D_FLT_00437288.z * direction->y;
    key = FIELD(u32, state, 0x14U + (u32)(s32)slot * 8U);
    record_index = key & 0xFFFFU;
    orientation = FIELD(signed char, state, 0x18U + (u32)(s32)slot * 8U);
    record = AT(u8, FIELD(void *, road, 0x38), record_index * 0x34U);
    turn0.vertex = -1; turn1.vertex = -1;
    turn0.flag = 0; turn1.flag = 0;

    for (;;) {
        u32 offset0, offset1;
        if (orientation == 0) {
            step = 1;
            offset0 = flag != 0 ? 8U : 10U;
            offset1 = flag != 0 ? 12U : 14U;
        } else {
            step = -1;
            offset0 = flag != 0 ? 12U : 14U;
            offset1 = flag != 0 ? 8U : 10U;
        }
        first = road_vertex(road, record_index, offset0, vertex);
        second = road_vertex(road, record_index, offset1, vertex);
        basis_point = road_vertex(road, record_index, 0x12, vertex);
        vector.z = first->z - point->z;
        vector.x = first->x - point->x;
        vector.y = first->y - point->y;
        length0 = func_002A3538(&vector);
        angle0 = signed_angle(direction, &vector, &basis);
        vector.x = second->x - point->x;
        vector.y = second->y - point->y;
        vector.z = second->z - point->z;
        length1 = func_002A3538(&vector);
        angle1 = signed_angle(direction, &vector, &basis);
        bound0 = angle0; bound1 = angle1;
        {
            float factor;
            const GeorgeMathVec3 *selected;
            float x, y, z;
            if (length0 < length1) {
                factor = orientation == 0 ? -2.0f : 2.0f;
                selected = first;
            } else {
                factor = orientation == 0 ? 2.0f : -2.0f;
                selected = second;
            }
            z = selected->z + factor * basis_point->z;
            x = selected->x + factor * basis_point->x;
            y = selected->y + factor * basis_point->y;
            target.z = z; target.x = x; target.y = y;
        }
        if (state_code == 0 && vertex < FIELD(u8, record, 5)) {
            do {
                if (turn0.vertex == -1) {
                    const GeorgeMathVec3 *current;
                    turn0.flag = (signed char)flag;
                    turn0.factor = orientation == 0 ? -2 : 2;
                    current = road_vertex(road, record_index, offset0, vertex);
                    vector.x = current->x - point->x;
                    vector.y = current->y - point->y;
                    vector.z = current->z - point->z;
                    inverse_squared(&vector);
                    angle0 = signed_angle(direction, &vector, &basis);
                    if (bound0 + 0.00999999977648258209228515625f < angle0) {
                        turn0.slot = slot;
                        turn0.vertex = (signed char)(vertex - step);
                        turn0.position = previous0;
                        turn0.basis = road_vertex(road, record_index, 0x12, (u8)turn0.vertex);
                    } else bound0 = angle0;
                    previous0 = current;
                }
                if (turn1.vertex == -1) {
                    const GeorgeMathVec3 *current;
                    turn1.flag = (signed char)flag;
                    turn1.factor = orientation == 0 ? 2 : -2;
                    current = road_vertex(road, record_index, offset1, vertex);
                    vector.x = current->x - point->x;
                    vector.y = current->y - point->y;
                    vector.z = current->z - point->z;
                    inverse_squared(&vector);
                    angle1 = signed_angle(direction, &vector, &basis);
                    if (angle1 < bound1 - 0.00999999977648258209228515625f) {
                        turn1.slot = slot;
                        turn1.vertex = (signed char)(vertex - step);
                        turn1.position = previous1;
                        turn1.basis = road_vertex(road, record_index, 0x12, (u8)turn1.vertex);
                    } else bound1 = angle1;
                    previous1 = current;
                }
                if (angle0 <= bound1 && bound0 <= angle1) state_code = 1;
                if (turn0.vertex >= 0 && turn1.vertex >= 0) state_code = 1;
                {
                    u32 stop = FIELD(u8, state, 0x41);
                    if ((turn0.vertex == -1 && turn1.vertex == -1 &&
                         (s32)vertex - step == (s32)stop) ||
                        (s32)turn0.vertex == (s32)stop || (s32)turn1.vertex == (s32)stop) {
                        if (key == FIELD(u32, state, 0x4C)) state_code = 2;
                    }
                }
                if (state_code != 0) break;
                vertex = (u8)(vertex + step);
            } while (vertex < FIELD(u8, record, 5));
        }
        if (state_code == 0) {
            if (slot > 0) {
                signed char side;
                slot = (signed char)(slot - 1);
                key = FIELD(u32, state, 0x14U + (u32)(s32)slot * 8U);
                side = FIELD(signed char, state, 0x19U + (u32)(s32)slot * 8U);
                orientation = FIELD(signed char, state, 0x18U + (u32)(s32)slot * 8U);
                road = func_001CCA28(key);
                record_index = key & 0xFFFFU;
                if (road == NULL) goto no_road;
                record = AT(u8, FIELD(void *, road, 0x38), record_index * 0x34U);
                if (orientation == 0) vertex = side == 0;
                else vertex = (u8)(FIELD(u8, record, 5) - (side != 0 ? 1 : 2));
                continue;
            }
            state_code = 2;
            final_control = 1;
            vertex = (u8)(vertex - step);
            target.x = FIELD(float, state, 0x114);
            target.y = FIELD(float, state, 0x118);
            target.z = FIELD(float, state, 0x11C);
        } else if (state_code == 1) {
            const CollisionTurn *selected = NULL;
            if (turn0.vertex >= 0) {
                if (turn1.vertex >= 0) {
                    unsigned long long absolute1 = soft_absolute_bits(bound1);
                    unsigned long long absolute0 = soft_absolute_bits(bound0);
                    selected = func_00373250(absolute1, absolute0) < 0 ? &turn1 : &turn0;
                } else selected = &turn0;
            } else if (turn1.vertex >= 0) selected = &turn1;
            if (selected != NULL) {
                vertex = (u8)selected->vertex;
                turn_position(&target, selected);
                slot = selected->slot;
                flag = selected->flag;
            }
            final_control = 0;
        } else if (state_code == 2) {
            final_control = 1;
            target.x = FIELD(float, state, 0x114);
            target.y = FIELD(float, state, 0x118);
            target.z = FIELD(float, state, 0x11C);
        }
        if (state_code != 0) break;
    }

    made_child = 0;
    count = func_001C1540(D_003F8B58, point, &target, objects, 32, 1.0f);
    point_x = point->x; point_y = point->y; point_z = point->z;
    vector.x = target.x - point_x;
    vector.y = target.y - point_y;
    vector.z = target.z - point_z;
    displacement.x = vector.x; displacement.y = vector.y; displacement.z = vector.z;
    length = func_002A3538(&vector);
    object_index = 0;
    while (object_index < count) {
        void *object = objects[object_index];
        const GeorgeMathVec3 *position;
        float x, y, z, parameter, candidate_radius;
        if (object == FIELD(void *, state, 0x64) ||
            object == FIELD(void *, FIELD(void *, state, 0), 4)) goto next_object;
        if (FIELD(void *, state, 0x110) != NULL) {
            if ((FIELD(u16, state, 0x46) & 0x10U) == 0) {
                const GeorgeMathVec3 *other;
                position = virtual_position(object);
                other = func_001CAFE0(FIELD(GeorgeGoalReferencedObject *, state, 0x110));
                x = position->x - other->x;
                y = position->y - other->y;
                z = position->z - other->z;
                if (square3(x, y, z) < 25.0f) goto next_object;
            }
        }
        if ((FIELD(u16, state, 0x46) & 8U) == 0 &&
            FIELD(u8, objects[object_index], 0xC) == 0) goto next_object;
        object = objects[object_index];
        if (FIELD(u8, object, 0xC) == 1 && FIELD(u8, object, 0x141) == 1) goto next_object;
        candidate_radius = virtual_radius(object);
        position = virtual_position(objects[object_index]); x = position->x;
        position = virtual_position(objects[object_index]); y = position->y;
        position = virtual_position(objects[object_index]); z = position->z;
        parameter = ((displacement.x * (x - point_x) + displacement.y * (y - point_y)) +
                     displacement.z * (z - point_z)) /
                    square3(displacement.x, displacement.y, displacement.z);
        if (!(0.0f <= parameter && parameter <= 1.2000000476837158203125f)) goto next_object;
        {
            void *owner = FIELD(void *, state, 0x64);
            u8 *table = FIELD(u8 *, owner, 4);
            CollisionFloatCall radius_call = FIELD(CollisionFloatCall, table, 0x44);
            float residual_x = x - (point_x + displacement.x * parameter);
            float residual_y = y - (point_y + displacement.y * parameter);
            float residual_z = z - (point_z + displacement.z * parameter);
            radius = (candidate_radius + radius_call(AT(u8, owner, (s32)FIELD(s16, table, 0x40)))) + 2.0f;
            if (!(square3(residual_x, residual_y, residual_z) < radius * radius)) goto next_object;
        }
        position = virtual_position(objects[object_index]);
        object_vector.x = position->x - point->x;
        object_vector.y = position->y - point->y;
        object_vector.z = position->z - point->z;
        inverse_squared(&object_vector);
        if (!(0.7070000171661376953125f < dot3(&object_vector, direction))) goto next_object;
        lateral.z = D_FLT_00437288.x * object_vector.y - D_FLT_00437288.y * object_vector.x;
        lateral.y = D_FLT_00437288.z * object_vector.x - D_FLT_00437288.x * object_vector.z;
        lateral.x = D_FLT_00437288.y * object_vector.z - D_FLT_00437288.z * object_vector.y;

        /* Two independently captured side projections; the second is always
         * attempted after the first child succeeds or the first gate fails. */
        {
            u32 side;
            for (side = 0; side < 2; ++side) {
                float factor = radius + 1.0f;
                s32 accepted = 0;
                u16 *query;
                if (side != 0) factor = -factor;
                position = virtual_position(objects[object_index]);
                x = position->x + factor * lateral.x;
                z = position->z + factor * lateral.z;
                y = position->y + factor * lateral.y;
                vector.x = x - point->x;
                vector.z = z - point->z;
                vector.y = y - point->y;
                target.x = x; target.y = y; target.z = z;
                displacement.x = vector.x; displacement.y = vector.y; displacement.z = vector.z;
                length = func_002A3538(&vector);
                angle = absolute_angle(direction, &vector);
                if (side == 0) queried_road = NULL;
                if (angle <= 1.57079637050628662109375f) {
                    query = func_001CEA80(&target, &queried_road, &vertex, &queried_mode, 0.0f);
                    if (query != NULL) {
                        u32 which;
                        for (which = 0; which < 2; ++which) {
                            u32 base = FIELD(u16, AT(u8, FIELD(void *, queried_road, 0x38),
                                                     (u32)*query * 0x34U), which == 0 ? 8U : 12U);
                            const GeorgeMathVec3 *from = AT(GeorgeMathVec3,
                                FIELD(void *, queried_road, 0x48), (base + vertex) * 12U);
                            const GeorgeMathVec3 *to = AT(GeorgeMathVec3,
                                FIELD(void *, queried_road, 0x48), (base + (u8)(vertex + 1U)) * 12U);
                            float distance;
                            edge.x = to->x - from->x;
                            edge.y = to->y - from->y;
                            edge.z = to->z - from->z;
                            distance = func_0029D5D0(point, &displacement, from, &edge,
                                                     &fraction0, &fraction1);
                            /* Retail performs both calls. The first gate has
                             * no lasting effect; the second decides acceptance. */
                            if (which != 0) {
                                if (0.100000001490116119384765625f < distance) {
                                    accepted = 1;
                                    if (!(fraction0 <= 0.0f) && !(1.0f <= fraction0) &&
                                        !(fraction1 <= 0.0f) && !(1.0f <= fraction1)) accepted = 0;
                                } else accepted = 0;
                            }
                        }
                        if (FIELD(signed char, state, 0x3C) >= 0) {
                            u32 query_key = FIELD(u32, query, 0);
                            slot = FIELD(signed char, state, 0x3C);
                            while (slot >= 0 && FIELD(u32, state, 0x14U + (u32)(s32)slot * 8U) != query_key)
                                slot = (signed char)(slot - 1);
                            if (slot < 0) {
                                slot = 0;
                                FIELD(signed char, state, 0x19) = 0;
                                FIELD(signed char, state, 0x18) = FIELD(u8, state, 0x41) < FIELD(u8, state, 0x3F);
                                FIELD(u16, state, 0x46) &= 0xFFFDU;
                                FIELD(u32, state, 0x14) = FIELD(u32, query, 0);
                                FIELD(signed char, state, 0x3C) = 0;
                            }
                            if (FIELD(signed char, state, 0x18U + (u32)(s32)slot * 8U) == 0)
                                vertex = (u8)(vertex + 1U);
                        }
                    } else accepted = func_001CF340(&target) != NULL;
                }
                if (accepted) {
                    made_child = 1;
                    child = func_001E2EC8(state, &target, &vector, depth + 1U,
                                        slot, vertex, queried_mode == 0, 0,
                                        node->field0C + length, node->field10 + angle);
                    if (child == NULL) return node;
                    if (child->field18 == NULL && child->field1C == NULL) func_001E52E8(state, child);
                    child->field14 = node;
                    if (side == 0) node->field18 = child;
                    else node->field1C = child;
                }
            }
        }
        break;
next_object:
        ++object_index;
    }
    if (!made_child) {
        angle = absolute_angle(direction, &vector);
        child = func_001E2EC8(state, &target, &vector, depth + 1U, slot, vertex,
                            flag, final_control, node->field0C + length, node->field10 + angle);
        if (child != NULL) {
            if (child->field18 == NULL && child->field1C == NULL) func_001E52E8(state, child);
            child->field14 = node;
            node->field18 = child;
        }
    }
    return node;
no_road:
    FIELD(signed char, state, 0x3E) = 2;
    return NULL;
}

#undef COLLISION_INLINE
#undef FIELD
#undef AT
