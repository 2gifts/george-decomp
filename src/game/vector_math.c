#include "george/vector_math.h"
#include "george/ee_math.h"

/* These are the original engine's trigonometric entry points. Their lookup
 * tables and approximations must not be silently replaced with host libm. */
extern float func_0029B940(float y, float x);
extern float func_0029C090(float angle);
extern float func_0029C168(float angle);
extern float func_0029C230(float cosine);

#define LENGTH3(x, y, z) george_ee_square_root(((x) * (x) + (y) * (y)) + (z) * (z))

void func_002A3080(GeorgeMathVec4 *output, const GeorgeMathVec4 *input)
{
    float length = LENGTH3(input->x, input->y, input->z);
    float angle = func_0029B940(length, input->w);
    float factor = length;
    if (0.0f < length) factor = angle / length;
    output->x = factor * input->x;
    output->y = factor * input->y;
    /* The last component is read before the output w store. */
    angle = factor * input->z;
    output->w = 0.0f;
    output->z = angle;
}

void func_002A3138(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right)
{
    /* Retail computes componentwise products here, not a quaternion product. */
    float x = -left->x * right->x;
    float y = -left->y * right->y;
    float z = -left->z * right->z;
    float w = left->w * right->w;
    float length = LENGTH3(x, y, z);
    float angle = func_0029B940(length, w);
    float factor = length;
    if (0.0f < length) factor = angle / length;
    x = factor * x;
    y = factor * y;
    z = factor * z;
    output->w = 0.0f;
    output->x = x;
    output->y = y;
    output->z = z;
}

void func_002A3220(GeorgeMathVec4 *output, const GeorgeMathVec3 *input)
{
    float length = LENGTH3(input->x, input->y, input->z);
    float factor = 1.0f;
    if (0.0000999999974737875163555145263671875f < length)
        factor = func_0029C090(length) / length;
    output->x = factor * input->x;
    output->y = factor * input->y;
    output->z = factor * input->z;
    output->w = func_0029C168(length);
}

void func_002A32E0(GeorgeMathVec4 *value, const GeorgeMathVec4 *reference)
{
    float x = value->x, y = value->y, z = value->z, w = value->w;
    float dot = ((x * reference->x + y * reference->y) + z * reference->z) + w * reference->w;
    if (dot < 0.0f) {
        value->x = -x;
        value->y = -y;
        value->z = -z;
        value->w = -w;
    }
}

void func_002A3390(GeorgeMathVec3 *output, const GeorgeMathVec3 *current, const GeorgeMathVec3 *target, float blend)
{
    float dot = (current->x * target->x + current->y * target->y) + current->z * target->z;
    float angle, reciprocal, left_weight, right_weight;
    float x, y, z;
    if (1.0f < dot) dot = 1.0f;
    else if (dot < -1.0f) dot = -1.0f;
    angle = func_0029C230(dot);
    if (angle == 0.0f || angle == 3.1415927410125732421875f) {
        /* Interleaved copy retains shifted source/output overlap behavior. */
        output->x = current->x;
        output->y = current->y;
        output->z = current->z;
        return;
    }
    reciprocal = 1.0f / func_0029C090(angle);
    left_weight = func_0029C090((1.0f - blend) * angle);
    right_weight = func_0029C090(blend * angle);
    right_weight *= reciprocal;
    left_weight *= reciprocal;
    /* All six components are reloaded after the calls and before any store. */
    z = current->z * left_weight + right_weight * target->z;
    x = current->x * left_weight + right_weight * target->x;
    y = current->y * left_weight + right_weight * target->y;
    output->z = z;
    output->x = x;
    output->y = y;
}

float func_002A3538(GeorgeMathVec3 *value)
{
    float x = value->x, y = value->y, z = value->z;
    float length = LENGTH3(x, y, z);
    if (length == 0.0f) {
        value->z = 0.0f;
        value->x = 1.0f;
        value->y = 0.0f;
    } else {
        float inverse = 1.0f / length;
        z *= inverse;
        x *= inverse;
        y *= inverse;
        value->z = z;
        value->x = x;
        value->y = y;
    }
    return length;
}

float func_002A35C0(GeorgeMathVec3 *output, const GeorgeMathVec3 *input, float scale)
{
    float x = input->x;
    float length = LENGTH3(x, input->y, input->z);
    if (length == 0.0f) {
        output->z = 0.0f;
        output->x = scale;
        output->y = 0.0f;
    } else {
        float factor = scale / length;
        output->x = x * factor;
        /* The y/z loads occur after preceding output stores. */
        output->y = input->y * factor;
        output->z = input->z * factor;
    }
    return length;
}

#undef LENGTH3
