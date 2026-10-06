#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/matrix_rigid.h"
#include "matrix_rigid_golden.h"

static unsigned checks;
static void check(int value, unsigned fixture, unsigned index)
{
    ++checks;
    if (!value) {
        fprintf(stderr, "matrix_rigid fixture %u word %u failed\n", fixture, index);
        exit(1);
    }
}

int main(void)
{
    union { float values[64]; u32 words[64]; } buffer;
    unsigned fixture, index;
    check(sizeof(void *) == 4 && sizeof(GeorgeRotationMatrix) == 64, 0, 64);
    for (fixture = 0; fixture < sizeof matrix_rigid_golden / sizeof matrix_rigid_golden[0]; ++fixture) {
        const struct MatrixRigidGolden *golden = matrix_rigid_golden + fixture;
        memcpy(buffer.words, golden->initial, sizeof buffer.words);
        func_002A1098((GeorgeRotationMatrix *)((u8 *)buffer.values + golden->output),
                      (const GeorgeRotationMatrix *)(buffer.values + 32));
        for (index = 0; index < 64; ++index)
            check(buffer.words[index] == golden->expected[index], fixture, index);
    }
    printf("matrix_rigid: %u checks passed\n", checks);
    return 0;
}
