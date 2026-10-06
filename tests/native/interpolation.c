#include <stdio.h>
#include <string.h>
#include <math.h>
#include "george/interpolation.h"

static unsigned checks, failures, copies;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)
void *func_003934F8(void *output, const void *input, u32 size)
{
    ++copies;
    return memcpy(output, input, size);
}
static u32 bits(float value) { union { float value; u32 bits; } x; x.value = value; return x.bits; }
int main(void)
{
    float records[] = {0, 8, 12, 4, 16, 20, 8, 32, 28};
    float output[3];
    float nan_value;
    union { u32 bits; float value; } nan_bits;
    int index;
    copies = 0;
    func_002ADC80(3, records, 2, output, -1);
    CHECK(output[0] == 8 && output[1] == 12 && copies == 1);
    func_002ADC80(3, records, 2, output, 0);
    CHECK(output[0] == 8 && output[1] == 12 && copies == 2);
    func_002ADC80(3, records, 2, output, 8);
    CHECK(output[0] == 32 && output[1] == 28 && copies == 3);
    func_002ADC80(3, records, 2, output, 99);
    CHECK(output[0] == 32 && output[1] == 28 && copies == 4);
    func_002ADC80(3, records, 2, output, 4);
    CHECK(output[0] == 16 && output[1] == 20 && copies == 4);
    func_002ADC80(3, records, 2, output, 2);
    CHECK(output[0] == 12 && output[1] == 16 && copies == 4);
    func_002ADC80(3, records, 2, output, 6);
    CHECK(output[0] == 24 && output[1] == 24 && copies == 4);
    nan_bits.bits = 0x7FC00000U; nan_value = nan_bits.value;
    output[0] = 101; output[1] = 102;
    func_002ADC80(3, records, 2, output, nan_value);
    CHECK(output[0] == 101 && output[1] == 102 && copies == 4);
    records[3] = nan_value;
    func_002ADC80(3, records, 2, output, 6);
    CHECK(isnan(output[0]) && isnan(output[1]));
    records[3] = 4;
    /* Every component's two loads occur before its store; a prior output store
       may replace a later input component. Values are not pre-captured. */
    {
        float alias[] = {0, 8, 12, 4, 16, 20, 8, 32, 28};
        func_002ADC80(3, alias, 2, alias + 2, 2);
        CHECK(alias[2] == 12 && alias[3] == 16);
    }
    {
        float alias[] = {0, 8, 12, 4, 16, 20, 8, 32, 28};
        func_002ADC80(3, alias, 2, alias + 5, 2);
        CHECK(alias[5] == 12 && alias[6] == 12);
    }
    {
        float zero_components[] = {0, 4, 8};
        output[0] = 77;
        func_002ADC80(3, zero_components, 0, output, 2);
        CHECK(output[0] == 77 && copies == 4);
    }
    {
        float single[] = {3, -0.0f, 5};
        func_002ADC80(1, single, 2, output, 4);
        CHECK(bits(output[0]) == 0x80000000U && output[1] == 5);
    }
    for (index = 0; index <= 16; ++index) {
        float key = (float)index * 0.5f;
        float x = key <= 4 ? 8 + 2 * key : 16 + 4 * (key - 4);
        float y = 12 + 2 * key;
        func_002ADC80(3, records, 2, output, key);
        CHECK(output[0] == x && output[1] == y);
    }
    printf("interpolation semantic checks: %u, failures: %u\n", checks, failures);
    return failures != 0;
}
