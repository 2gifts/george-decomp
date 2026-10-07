#include <stdio.h>
#include <string.h>
#include "george/matrix_copy_identity.h"
#include "matrix_copy_identity_golden.h"

int main(void)
{
    unsigned c, i, checks = 0;
    float typed[64] __attribute__((__aligned__(16)));
    unsigned char *arena = (unsigned char *)typed;
    if (sizeof(float) != 4 || sizeof(MATRIX) != 64 || sizeof(unsigned) != 4)
        return 2;
    typed[0] = 0.0f;
    for (i = 0; i < 4; ++i) if (arena[i] != 0) return 3;
    for (c = 0; c < sizeof(matrix_copy_identity_golden) / sizeof(matrix_copy_identity_golden[0]); ++c) {
        const struct MatrixCopyIdentityGolden *q = &matrix_copy_identity_golden[c];
        memcpy(arena, q->initial, 256);
        if (q->routine == 0)
            func_002A1C08(arena + q->output, arena + q->source);
        else
            matrix_unit(typed + q->output / 4);
        for (i = 0; i < 256; ++i) {
            ++checks;
            if (arena[i] != q->expected[i]) {
                printf("FAIL case%u byte%u got%u expected%u\n", c, i, arena[i], q->expected[i]);
                return 1;
            }
        }
    }
    printf("PASS %u checks\n", checks);
    return 0;
}
