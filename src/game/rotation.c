#include "george/rotation.h"
#include "george/ee_math.h"

extern float func_0029B940(float y, float x);
extern float func_0029C090(float angle);
extern float func_0029C168(float angle);
extern float func_0029C230(float cosine);

#define LENGTH3(x, y, z) george_ee_square_root(((x) * (x) + (y) * (y)) + (z) * (z))

void func_002A2658(GeorgeRotationMatrix *output, const GeorgeMathVec3 *input)
{
    float value;
    /* Capture before each intervening zero store, then reload the next input. */
    output->element[0] = 0.0f;
    output->element[1] = -input->z;
    value = input->y;
    output->element[3] = 0.0f;
    output->element[2] = value;
    value = input->z;
    output->element[5] = 0.0f;
    output->element[4] = value;
    value = -input->x;
    output->element[7] = 0.0f;
    output->element[6] = value;
    output->element[8] = -input->y;
    value = input->x;
    output->element[15] = 1.0f;
    output->element[9] = value;
    output->element[10] = 0.0f;
    output->element[11] = 0.0f;
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
}

void func_002A26C8(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right)
{
    float ly = left->y, ry = right->y, lx = left->x, rx = right->x;
    float lw = left->w, rw = right->w, rz = right->z, lz = left->z;
    float x = (ly * rz - lz * ry) + (lw * rx + rw * lx);
    float y = (lz * rx - lx * rz) + (lw * ry + rw * ly);
    float z = (lx * ry - ly * rx) + (lw * rz + rw * lz);
    float w = lw * rw - ((lx * rx + ly * ry) + lz * rz);
    output->x = x;
    output->w = w;
    output->y = y;
    output->z = z;
}

void func_002A2780(GeorgeMathVec4 *output, const GeorgeMathVec4 *left, const GeorgeMathVec4 *right, float blend)
{
    float dot = ((left->x * right->x + left->y * right->y) + left->z * right->z) + left->w * right->w;
    float first_weight, second_weight;
    if (0.0000009999999974752427078783512115478515625f < dot + 1.0f) {
        if (0.0000009999999974752427078783512115478515625f < 1.0f - dot) {
            float cosine = -1.0f;
            float angle, reciprocal;
            if (-1.0f <= dot) cosine = george_ee_minimum(dot, 1.0f);
            angle = func_0029C230(cosine);
            reciprocal = 1.0f / func_0029C090(angle);
            first_weight = func_0029C090((1.0f - blend) * angle) * reciprocal;
            second_weight = func_0029C090(blend * angle) * reciprocal;
        } else {
            first_weight = 1.0f - blend;
            second_weight = blend;
        }
        /* Reload one pair at a time, after all trig calls and prior stores. */
        output->x = first_weight * left->x + second_weight * right->x;
        output->y = first_weight * left->y + second_weight * right->y;
        output->z = first_weight * left->z + second_weight * right->z;
        output->w = first_weight * left->w + second_weight * right->w;
    } else {
        first_weight = func_0029C090((1.0f - blend) * 1.57079637050628662109375f);
        second_weight = func_0029C090(blend * 1.57079637050628662109375f);
        output->x = first_weight * left->x + (-second_weight) * left->y;
        output->y = first_weight * left->y + second_weight * left->x;
        output->z = first_weight * left->z + (-second_weight) * left->w;
        /* Retail copies this last z directly; it does not complete a rotation. */
        output->w = left->z;
    }
}

/* Reuse the reviewed component-log arithmetic of func_002A3138 as an inline
 * source helper. Retail inlines both blocks here; no new engine call is added. */
static __inline__ void rotation_relative_log(GeorgeMathVec4 *output,
                                             const GeorgeMathVec4 *base,
                                             const GeorgeMathVec4 *neighbor)
{
    float x = -base->x * neighbor->x;
    float y = -base->y * neighbor->y;
    float z = -base->z * neighbor->z;
    float w = base->w * neighbor->w;
    float length = LENGTH3(x, y, z);
    float angle = func_0029B940(length, w);
    float factor = length;
    if (0.0f < length) factor = angle / length;
    output->w = 0.0f;
    output->x = factor * x;
    output->y = factor * y;
    output->z = factor * z;
}

void func_002A2990(GeorgeMathVec4 *output, const GeorgeMathVec4 *previous,
                  const GeorgeMathVec4 *current, const GeorgeMathVec4 *next)
{
    GeorgeMathVec4 first, second, adjustment;
    float length, factor = 1.0f;
    rotation_relative_log(&first, current, previous);
    /* The current and next inputs reload after the first angle callback. */
    rotation_relative_log(&second, current, next);
    adjustment.x = (first.x + second.x) * -0.25f;
    adjustment.y = (first.y + second.y) * -0.25f;
    adjustment.z = (first.z + second.z) * -0.25f;
    adjustment.w = first.w * -0.25f;
    length = LENGTH3(adjustment.x, adjustment.y, adjustment.z);
    if (0.0000999999974737875163555145263671875f < length)
        factor = func_0029C090(length) / length;
    adjustment.x *= factor;
    adjustment.y *= factor;
    adjustment.z *= factor;
    adjustment.w = func_0029C168(length);
    /* The current quaternion reloads after all trigonometric callbacks. */
    func_002A26C8(output, current, &adjustment);
}

void func_002A2C48(const GeorgeMathVec4 *rotation, const GeorgeMathVec3 *input,
                  GeorgeMathVec3 *output)
{
    GeorgeMathVec4 point, inverse, rotated;
    point.x = input->x;
    point.y = input->y;
    point.z = input->z;
    point.w = 0.0f;
    inverse.x = -rotation->x;
    inverse.w = rotation->w;
    inverse.y = -rotation->y;
    inverse.z = -rotation->z;
    func_002A26C8(&point, rotation, &point);
    func_002A26C8(&rotated, &point, &inverse);
    output->x = rotated.x;
    output->z = rotated.z;
    output->y = rotated.y;
}

void func_002A2CF0(GeorgeMathVec4 *output, const GeorgeMathVec4 *axis_angle)
{
    float half = axis_angle->w * 0.5f;
    float sine = func_0029C090(half);
    output->x = axis_angle->x * sine;
    output->y = axis_angle->y * sine;
    output->z = axis_angle->z * sine;
    output->w = func_0029C168(half);
}

void func_002A2D70(GeorgeMathVec4 *output, float angle)
{
    float half = angle * 0.5f;
    output->x = 0.0f;
    output->y = 0.0f;
    output->z = func_0029C090(half);
    output->w = func_0029C168(half);
}

void func_002A2DC8(GeorgeMathVec4 *output, float angle)
{
    float half = angle * 0.5f;
    float sine;
    output->x = 0.0f;
    sine = func_0029C090(half);
    output->z = 0.0f;
    output->y = sine;
    output->w = func_0029C168(half);
}

void func_002A2E20(GeorgeMathVec4 *output, float angle)
{
    float half = angle * 0.5f;
    float sine = func_0029C090(half);
    output->y = 0.0f;
    output->z = 0.0f;
    output->x = sine;
    output->w = func_0029C168(half);
}

void func_002A2E78(GeorgeMathVec4 *output, float first, float second, float third)
{
    float a = first * 0.5f, b = second * 0.5f, c = third * 0.5f;
    float ca = func_0029C168(a), sa = func_0029C090(a);
    float cb = func_0029C168(b), sb = func_0029C090(b);
    float cc = func_0029C168(c), sc = func_0029C090(c);
    float cbca = cb * ca, sbsa = sb * sa, cbsa = cb * sa;
    float common = sc * cbca, other = cc * cbca;
    float z = cc * cbsa - common;
    float w = other + sc * sbsa;
    float x = common - cc * sbsa;
    float y = other + sc * cbsa;
    output->z = z;
    output->w = w;
    output->x = x;
    output->y = y;
}

void func_002A2F68(GeorgeMathVec4 *output, const GeorgeMathVec4 *left,
                  const GeorgeMathVec4 *right, float blend)
{
    float first_weight = 1.0f - blend;
    output->x = first_weight * left->x + blend * right->x;
    output->y = first_weight * left->y + blend * right->y;
    output->z = first_weight * left->z + blend * right->z;
    output->w = first_weight * left->w + blend * right->w;
}

void func_002A2FE0(GeorgeMathVec4 *output, const GeorgeMathVec4 *first_left,
                  const GeorgeMathVec4 *second_left, const GeorgeMathVec4 *second_right,
                  const GeorgeMathVec4 *first_right, float blend)
{
    GeorgeMathVec4 first, second;
    float complement = 1.0f - blend;
    float weight = (complement + complement) * blend;
    func_002A2780(&first, first_left, first_right, blend);
    func_002A2780(&second, second_left, second_right, blend);
    func_002A2780(output, &first, &second, weight);
}

#undef LENGTH3
