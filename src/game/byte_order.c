#include "george/byte_order.h"

typedef union GeorgeByteOrderFloat {
    u32 bits;
    float scalar;
} GeorgeByteOrderFloat;

/* Reuse this ordinary C expression for both identical scalar bodies and for
 * array elements. No original implementation/library identity is asserted. */
static __inline__ u32 byte_order32(u32 value)
{
    return (value >> 24) | ((value >> 8) & 0xFF00u)
         | ((value & 0xFF00u) << 8) | (value << 24);
}

static __inline__ u16 byte_order16(u16 value)
{
    return (u16)((value >> 8) | ((value & 0xFFu) << 8));
}

u32 func_002B2430(u16 value)
{
    return byte_order16(value);
}

u32 func_002B2448(u32 value)
{
    return byte_order32(value);
}

float func_002B2470(float value)
{
    GeorgeByteOrderFloat converted;
    converted.scalar = value;
    converted.bits = byte_order32(converted.bits);
    return converted.scalar;
}

u32 func_002B24A8(u32 value)
{
    return byte_order32(value);
}

void func_002B24D0(float *components)
{
    GeorgeByteOrderFloat x, y, z;
    const u32 *bits = (const u32 *)components;
    /* The original reads all three raw words before any output store. */
    x.bits = bits[0];
    y.bits = bits[1];
    z.bits = bits[2];
    x.bits = byte_order32(x.bits);
    y.bits = byte_order32(y.bits);
    z.bits = byte_order32(z.bits);
    components[0] = x.scalar;
    components[2] = z.scalar;
    components[1] = y.scalar;
}

void func_002B2568(void *destination, const void *source, s32 triples)
{
    u32 *output = (u32 *)destination;
    const u32 *input = (const u32 *)source;
    s32 remaining = (s32)((u32)triples * 3u);
    if (remaining > 0) {
        do {
            u32 value = *input++;
            *output++ = byte_order32(value);
            --remaining;
        } while (remaining != 0);
    }
}

void func_002B25E0(float *components)
{
    func_002B2640(components, 16);
}

void func_002B2600(float *components)
{
    func_002B2640(components, 6);
}

void func_002B2620(float *components)
{
    func_002B2640(components, 15);
}

void func_002B2640(float *components, s32 count)
{
    const u32 *input = (const u32 *)components;
    if (count > 0) {
        do {
            GeorgeByteOrderFloat value;
            value.bits = byte_order32(*input++);
            *components++ = value.scalar;
            --count;
        } while (count != 0);
    }
}

void func_002B26A8(u16 *values, s32 count)
{
    if (count > 0) {
        do {
            *values = byte_order16(*values);
            ++values;
            --count;
        } while (count != 0);
    }
}

void func_002B26E0(u16 *destination, const u16 *source, s32 rows, s32 columns)
{
    s32 remaining = (s32)((u32)rows * (u32)columns);
    if (remaining > 0) {
        do {
            u16 value = *source++;
            *destination++ = byte_order16(value);
            --remaining;
        } while (remaining != 0);
    }
}

void func_002B2730(u32 *values, s32 count)
{
    if (count > 0) {
        do {
            *values = byte_order32(*values);
            ++values;
            --count;
        } while (count != 0);
    }
}

void func_002B2790(u32 *destination, const u32 *source, s32 rows, s32 columns)
{
    s32 remaining = (s32)((u32)rows * (u32)columns);
    if (remaining > 0) {
        do {
            u32 value = *source++;
            *destination++ = byte_order32(value);
            --remaining;
        } while (remaining != 0);
    }
}
