#include "george/interpolation.h"

extern void *func_003934F8(void *output, const void *input, u32 size);

void func_002ADC80(s32 key_count, const float *records, s32 component_count,
                   float *output, float key)
{
    s32 interval;
    s32 interval_count = (s32)((u32)key_count - 1U);
    u32 stride = ((u32)component_count + 1U) << 2;
    const float *right;
    if (key <= records[0]) {
        func_003934F8(output, records + 1, (u32)component_count << 2);
        return;
    }
    right = (const float *)((u32)records + (u32)interval_count * stride);
    if (right[0] <= key) {
        func_003934F8(output, right + 1, (u32)component_count << 2);
        return;
    }
    interval = 0;
    right = (const float *)((u32)records + stride);
    while (interval < interval_count) {
        float right_key = right[0];
        if (key <= right_key) {
            float left_key = records[0];
            float fraction = (key - left_key) / (right_key - left_key);
            const float *left = records + 1;
            right += 1;
            if (component_count > 0) {
                float complement = 1.0f - fraction;
                do {
                    float left_value = *left++;
                    float right_value = *right++;
                    *output++ = left_value * complement + right_value * fraction;
                    component_count -= 1;
                } while (component_count != 0);
            }
            return;
        }
        interval += 1;
        records = (const float *)((u32)records + stride);
        right = (const float *)((u32)right + stride);
    }
}
