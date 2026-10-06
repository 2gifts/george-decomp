#include "george/math_helpers.h"

/*
 * Scalar C reconstruction of observed float/vector operations. All arithmetic
 * order and comparisons matter: do not enable fast-math or fused contraction.
 * Address names, object prefixes and guest pointer widths remain explicit.
 * See config/functions/game_math.json for exact-byte verification status.
 */

#define DOT3(left, right) (((left).x * (right).x + (left).y * (right).y) + (left).z * (right).z)

/* Select one of two vectors and return its signed dot product. */
float func_00119EE8(const GeorgeMathDirectionsFC *object, s32 direction, const GeorgeMathVec3 *vector)
{
    switch (direction) {
    case 0:
        return -DOT3(*vector, object->fieldF0);
    case 1:
        return DOT3(*vector, object->fieldF0);
    case 2:
        return -DOT3(*vector, object->fieldE4);
    case 3:
        return DOT3(*vector, object->fieldE4);
    default:
        return 0.0f;
    }
}

/* Strict componentwise tolerance: boundaries and unordered values fail. */
u32 func_0011A568(const GeorgeMathVec3 *left, const GeorgeMathVec3 *right, float tolerance)
{
    GeorgeMathVec3 delta;
    delta.x = left->x - right->x;
    delta.z = left->z - right->z;
    delta.y = left->y - right->y;
    if (delta.x < tolerance && delta.y < tolerance && delta.z < tolerance) {
        tolerance = -tolerance;
        if (tolerance < delta.x && tolerance < delta.y && tolerance < delta.z) {
            return 1;
        }
    }
    return 0;
}

/* Copy a vector, then scale the destination by the observed scalar. */
void func_00132BC8(GeorgeMathScaled16C *object)
{
    float scale;
    object->field160.x = object->field114.x;
    object->field160.y = object->field114.y;
    object->field160.z = object->field114.z;
    scale = object->field1C;
    object->field160.x *= scale;
    object->field160.y *= scale;
    object->field160.z *= scale;
}

/* Left-associated component sum, then ordered comparison against zero. */
u32 func_001348F8(void *unused, const GeorgeMathVec3 *vector)
{
    float sum = vector->x + vector->y;
    (void)unused;
    sum += vector->z;
    return sum < 0.0f;
}

/* Side-dependent angular interpolation around single-precision pi/2. */
float func_00141590(const GeorgeMathAngularAC *object)
{
    u32 side;
    float distance;
    float width;

    if (object->fieldA8 != 0) {
        side = 0.0f <= object->field90;
    } else {
        side = object->field90 < 0.0f;
    }
    distance = object->field98 - 1.57079637050628662109375f;
    distance = __builtin_fabsf(distance);
    width = object->field48 + object->field48;
    distance /= width;
    if (side == 0) {
        return distance + 0.5f;
    }
    return 0.5f - distance;
}

/* These source bodies reuse the same observed normalization operation. */
void func_00162650(GeorgeMathColors40 *object)
{
    const float scale = 1.0f / 255.0f;
    object->field28.x *= scale;
    object->field28.y *= scale;
    object->field28.z *= scale;
    object->field34.x *= scale;
    object->field34.y *= scale;
    object->field34.z *= scale;
}

void func_00166250(GeorgeMathVec3 *object)
{
    const float scale = 1.0f / 255.0f;
    float x = object->x;
    float y = object->y;
    float z = object->z;
    x *= scale;
    y *= scale;
    z *= scale;
    object->x = x;
    object->y = y;
    object->z = z;
}

void func_00166C00(GeorgeMathColors18 *object)
{
    const float scale = 1.0f / 255.0f;
    float x = object->field00.x;
    float y = object->field00.y;
    float z = object->field00.z;
    float next_x = object->field0C.x;
    x *= scale;
    y *= scale;
    z *= scale;
    next_x *= scale;
    object->field00.x = x;
    object->field00.y = y;
    object->field00.z = z;
    object->field0C.x = next_x;
    object->field0C.y *= scale;
    object->field0C.z *= scale;
}

/* Dot-product sign with an independent force flag. */
u32 func_0016ED28(const GeorgeMathSign258 *object)
{
    float dot = DOT3(object->field0C, object->field40);
    if (object->field254 != 0) {
        return 1;
    }
    return object->field24C * dot < 0.0f;
}

/* Load all vector operands before output writes, allowing source overlap. */
void func_0018F7A8(const GeorgeMathDifferenceA0 *object, GeorgeMathVec3 *output)
{
    float z = object->field4C.z - object->field94.z;
    float x = object->field4C.x - object->field94.x;
    float y = object->field4C.y - object->field94.y;
    output->z = z;
    output->x = x;
    output->y = y;
}

#define SIGN268_BODY \
    float dot = DOT3(object->field0C, object->field40); \
    if (object->field264 != 0) { return 1; } \
    return object->field254 * dot < 0.0f

/* Same instruction behavior occurs in two distinct game entry points. */
u32 func_0019CBD0(const GeorgeMathSign268 *object)
{
    SIGN268_BODY;
}

u32 func_0019D740(const GeorgeMathSign268 *object)
{
    SIGN268_BODY;
}

#define ADD_VECTOR_BODY(member) \
    float x = object->member.x + vector->x; \
    float y = object->member.y + vector->y; \
    float z = object->member.z + vector->z; \
    object->member.x = x; \
    object->member.y = y; \
    object->member.z = z

void func_001A1450(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector)
{
    ADD_VECTOR_BODY(field0C);
}

void func_001A1488(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector)
{
    ADD_VECTOR_BODY(field18);
}

#define SUM_VECTOR_BODY \
    float z = object->field0C.z + object->field18.z; \
    float x = object->field0C.x + object->field18.x; \
    float y = object->field0C.y + object->field18.y; \
    output->z = z; \
    output->x = x; \
    output->y = y

void func_001A14F0(const GeorgeMathSum24 *object, GeorgeMathVec3 *output)
{
    SUM_VECTOR_BODY;
}

/* Integrate one axis, consume the additive term, then update linked position. */
void func_001A21C0(GeorgeMathGravity78 *object, float step)
{
    float velocity = object->field70 + object->field74;
    float gravity = step * 19.6000003814697265625f;
    GeorgeMathPosition48 *position = (GeorgeMathPosition48 *)(size_t)object->field18;
    object->field74 = 0.0f;
    velocity -= gravity;
    object->field70 = velocity;
    step *= velocity;
    step += position->field44;
    position->field44 = step;
}

void func_001A5C40(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector)
{
    ADD_VECTOR_BODY(field0C);
}

/* The second class also synchronizes a word in a linked object. */
void func_001A5C78(GeorgeMathSumSync64 *object, const GeorgeMathVec3 *vector)
{
    GeorgeMathSync10 *linked;
    ADD_VECTOR_BODY(field18);
    linked = (GeorgeMathSync10 *)(size_t)object->field60;
    if (linked != 0) {
        linked->field08 = linked->field0C;
    }
}

void func_001A5CF8(const GeorgeMathSum24 *object, GeorgeMathVec3 *output)
{
    SUM_VECTOR_BODY;
}

/* Solve the plane equation for Y and add the observed +0.1f offset. */
void func_001CDEB0(GeorgeMathVec3 *position, const GeorgeMathVec4 *plane)
{
    float product = plane->x * position->x;
    float value = plane->w - product;
    product = plane->z * position->z;
    value -= product;
    value /= plane->y;
    value += 0.1f;
    position->y = value;
}

#undef SUM_VECTOR_BODY
#undef ADD_VECTOR_BODY
#undef SIGN268_BODY
#undef DOT3
