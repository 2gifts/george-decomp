#include <math.h>
#include <stdio.h>
#include <string.h>
#include "george/engine_angles.h"
#include "engine_angles_golden.h"

static unsigned checks, failures;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)

static float scalar(u32 word)
{
    float result;
    memcpy(&result, &word, sizeof result);
    return result;
}

static u32 bits(float value)
{
    u32 result;
    memcpy(&result, &value, sizeof result);
    return result;
}

int main(void)
{
    unsigned index;
    float (*routines[])(float) = {func_0029C230, func_0029C2A0, func_0029C300};
    for (index = 0; index < sizeof angle_golden / sizeof angle_golden[0]; ++index) {
        const struct AngleGolden *fixture = &angle_golden[index];
        float input = scalar(fixture->input);
        float result = routines[fixture->routine](input);
        CHECK(bits(result) == fixture->expected);
        /* Independent host libm is only a broad numerical oracle; the retail
         * convergence and expression graph are checked by exact fixture bits. */
        if (fixture->routine == 0)
            CHECK(fabs((double)result - acos((double)input)) < 0.000002);
        else if (fixture->routine == 1)
            CHECK(fabs((double)result - asin((double)input)) < 0.000002);
        else
            CHECK(fabs((double)result - atan((double)input)) < 0.000002);
    }
    CHECK(bits(func_0029C300(-0.0f)) == 0x80000000u);
    CHECK(bits(func_0029C2A0(-0.0f)) == 0x80000000u);
    printf("engine_angles: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
