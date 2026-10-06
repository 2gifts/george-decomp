#include "george/string_algorithms.h"

/* Fresh reconstructions from the executable. A standard algorithm identity is
 * not evidence that a particular upstream implementation supplied this code. */

/* 0x00295050, 0x30 bytes: excludes the terminator; text must be non-null. */
u32 func_00295050(const signed char *text)
{
    u32 length = 0;
    if (*text != 0) {
        do {
            ++text;
            ++length;
        } while (*text != 0);
    }
    return length;
}

/* 0x0029C588, 0x68 bytes: bespoke binary search, not a standard lower_bound.
 * When the bounds meet/cross it returns low without testing that final value.
 * Unordered comparisons also take the match branch, as in the original. */
s32 func_0029C588(const float *values, s32 count, float value)
{
    s32 low = 0;
    s32 high = (s32)((u32)count - 1u);
    if (high > 0) {
        do {
            s32 middle = (s32)((u32)low + (u32)high) / 2;
            float current = values[middle];
            if (current < value) {
                low = middle + 1;
            } else if (value < current) {
                high = middle - 1;
            } else {
                return middle;
            }
        } while (low < high);
    }
    return low;
}

/* 0x0029C5F0, 0x54 bytes: update a finalized CRC with exactly length bytes.
 * Null bytes returns zero even when a previous CRC or nonzero length is given.
 * A non-null buffer of length zero returns previous_crc unchanged. */
u32 func_0029C5F0(u32 previous_crc, const u8 *bytes, u32 length)
{
    u32 crc;
    if (bytes == 0) {
        return 0;
    }
    crc = ~previous_crc;
    if (length != 0) {
        do {
            crc = D_00445650[(crc ^ *bytes) & 0xFF] ^ (crc >> 8);
            ++bytes;
            --length;
        } while (length != 0);
    }
    return ~crc;
}

/* 0x0029C648, 0x78 bytes: CRC of a terminated string, excluding its zero byte.
 * The original calls the unguarded length routine BEFORE checking text. The
 * subsequent null branch must not be mistaken for a safe CRC(null) contract. */
u32 func_0029C648(const signed char *text)
{
    u32 length = func_00295050(text);
    u32 crc = 0xFFFFFFFFu;
    const u8 *bytes = (const u8 *)text;
    if (text == 0) {
        return 0;
    }
    if (length != 0) {
        do {
            crc = D_00445650[(crc ^ *bytes) & 0xFF] ^ (crc >> 8);
            ++bytes;
            --length;
        } while (length != 0);
    }
    return ~crc;
}
