#include "george/matrix_scalar.h"

extern float func_0029C090(float angle);
extern float func_0029C168(float angle);

void func_002A1CE0(const GeorgeRotationMatrix *matrix, const GeorgeMathVec3 *input,
                  GeorgeMathVec3 *output)
{
    output->x = (input->x * matrix->element[0] + input->y * matrix->element[1])
        + input->z * matrix->element[2];
    output->y = (input->x * matrix->element[4] + input->y * matrix->element[5])
        + input->z * matrix->element[6];
    output->z = (input->x * matrix->element[8] + input->y * matrix->element[9])
        + input->z * matrix->element[10];
}

/* Both retail builders inline this same captured-axis arithmetic. */
static __inline__ void matrix_axis_rows(GeorgeRotationMatrix *output,
                                       const GeorgeMathVec4 *axis_angle)
{
    float cosine = func_0029C168(axis_angle->w);
    /* The angle reloads after cosine; components reload after both calls. */
    float sine = func_0029C090(axis_angle->w);
    float z = axis_angle->z, x = axis_angle->x, y = axis_angle->y;
    float complement = 1.0f - cosine;
    float sz = sine * z, sx = sine * x;
    float tz = complement * z, ty = complement * y;
    float sy = sine * y, tx = complement * x;
    output->element[3] = 0.0f;
    output->element[0] = tx * x + cosine;
    output->element[2] = tz * x - sy;
    output->element[1] = ty * x + sz;
    output->element[7] = 0.0f;
    output->element[4] = tx * y - sz;
    output->element[6] = tz * y + sx;
    output->element[5] = ty * y + cosine;
    output->element[11] = 0.0f;
    output->element[8] = tx * z + sy;
    output->element[10] = tz * z + cosine;
    output->element[9] = ty * z - sx;
}

void func_002A1FD8(GeorgeRotationMatrix *output, const GeorgeMathVec4 *axis_angle)
{
    matrix_axis_rows(output, axis_angle);
    output->element[12] = 0.0f;
    output->element[15] = 1.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
}

void func_002A20D8(GeorgeRotationMatrix *output, const GeorgeMathVec4 *axis_angle,
                  const GeorgeMathVec3 *position)
{
    matrix_axis_rows(output, axis_angle);
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
    output->element[15] = 1.0f;
    /* These position reads occur after all preceding matrix stores. */
    output->element[12] = position->x;
    output->element[13] = position->y;
    output->element[14] = position->z;
}

void func_002A2278(GeorgeRotationMatrix *matrix, float x, float y, float z)
{
    float first = ((x * matrix->element[0] + y * matrix->element[4])
                   + z * matrix->element[8]) + matrix->element[12];
    float second = ((x * matrix->element[1] + y * matrix->element[5])
                    + z * matrix->element[9]) + matrix->element[13];
    float third = ((x * matrix->element[2] + y * matrix->element[6])
                   + z * matrix->element[10]) + matrix->element[14];
    float fourth = ((x * matrix->element[3] + y * matrix->element[7])
                    + z * matrix->element[11]) + matrix->element[15];
    matrix->element[12] = first;
    matrix->element[13] = second;
    matrix->element[14] = third;
    matrix->element[15] = fourth;
}

void func_002A2330(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input)
{
    int row;
    /* Each load follows the previous store; in-place aliases propagate. */
    for (row = 0; row < 4; ++row) {
        output->element[row * 4] = input->element[row];
        output->element[row * 4 + 1] = input->element[row + 4];
        output->element[row * 4 + 2] = input->element[row + 8];
        output->element[row * 4 + 3] = input->element[row + 12];
    }
}

void func_002A2370(GeorgeRotationMatrix *output, float angle)
{
    float cosine = func_0029C168(angle);
    float sine = func_0029C090(angle);
    output->element[5] = cosine;
    output->element[0] = cosine;
    output->element[15] = 1.0f;
    output->element[4] = -sine;
    output->element[1] = sine;
    output->element[2] = 0.0f;
    output->element[3] = 0.0f;
    output->element[6] = 0.0f;
    output->element[7] = 0.0f;
    output->element[8] = 0.0f;
    output->element[9] = 0.0f;
    output->element[10] = 1.0f;
    output->element[11] = 0.0f;
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
}

void func_002A2400(GeorgeRotationMatrix *output, float angle)
{
    float cosine = func_0029C168(angle);
    float sine = func_0029C090(angle);
    output->element[10] = cosine;
    output->element[0] = cosine;
    output->element[8] = sine;
    output->element[15] = 1.0f;
    output->element[2] = -sine;
    output->element[1] = 0.0f;
    output->element[3] = 0.0f;
    output->element[4] = 0.0f;
    output->element[5] = 1.0f;
    output->element[6] = 0.0f;
    output->element[7] = 0.0f;
    output->element[9] = 0.0f;
    output->element[11] = 0.0f;
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
}

void func_002A2490(GeorgeRotationMatrix *output, float angle)
{
    float cosine = func_0029C168(angle);
    float sine = func_0029C090(angle);
    output->element[10] = cosine;
    output->element[5] = cosine;
    output->element[15] = 1.0f;
    output->element[0] = 1.0f;
    output->element[9] = -sine;
    output->element[1] = 0.0f;
    output->element[2] = 0.0f;
    output->element[3] = 0.0f;
    output->element[4] = 0.0f;
    output->element[6] = sine;
    output->element[7] = 0.0f;
    output->element[8] = 0.0f;
    output->element[11] = 0.0f;
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
}

void func_002A2520(GeorgeRotationMatrix *output, float first, float second, float third)
{
    float ca = func_0029C168(first), sa = func_0029C090(first);
    float cb = func_0029C168(second), sb = func_0029C090(second);
    float cc = func_0029C168(third), sc = func_0029C090(third);
    float common = ca * cc;
    float first_product = ca * sb;
    float second_product = sa * sb;
    float third_product = cc * sa;
    float neg_cosine = -cc;
    float fourth_product = ca * sc;
    float fifth_product = sa * sc;
    output->element[3] = 0.0f;
    output->element[15] = 1.0f;
    output->element[7] = 0.0f;
    output->element[11] = 0.0f;
    output->element[12] = 0.0f;
    output->element[13] = 0.0f;
    output->element[14] = 0.0f;
    output->element[10] = ca * cb;
    output->element[0] = cb * cc;
    output->element[1] = cb * sc;
    output->element[6] = cb * sa;
    output->element[9] = neg_cosine * sa + first_product * sc;
    output->element[4] = third_product * sb - fourth_product;
    output->element[8] = common * sb + fifth_product;
    output->element[5] = common + second_product * sc;
    output->element[2] = -sb;
}
