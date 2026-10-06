#include "george/matrix_rigid.h"

void func_002A1098(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *input)
{
    float component, x, y, z, tx, ty, tz, value;
    output->element[0] = input->element[0];
    output->element[1] = input->element[4];
    component = input->element[8];
    output->element[3] = 0.0f;
    output->element[2] = component;
    output->element[4] = input->element[1];
    output->element[5] = input->element[5];
    component = input->element[9];
    output->element[7] = 0.0f;
    output->element[6] = component;
    output->element[8] = input->element[2];
    output->element[9] = input->element[6];
    component = input->element[10];
    output->element[11] = 0.0f;
    output->element[10] = component;

    x = input->element[0]; y = input->element[1];
    tx = input->element[12]; ty = input->element[13];
    tz = input->element[14]; z = input->element[2];
    value = (tx * x + ty * y) + tz * z;
    output->element[12] = -value;

    x = input->element[4]; y = input->element[5];
    tx = input->element[12]; ty = input->element[13];
    z = input->element[6]; tz = input->element[14];
    value = (tx * x + ty * y) + tz * z;
    output->element[13] = -value;

    x = input->element[8]; tx = input->element[12];
    ty = input->element[13]; y = input->element[9];
    tz = input->element[14]; z = input->element[10];
    output->element[15] = 1.0f;
    value = (tx * x + ty * y) + tz * z;
    output->element[14] = -value;
}
