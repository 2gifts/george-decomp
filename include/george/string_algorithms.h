#ifndef GEORGE_STRING_ALGORITHMS_H
#define GEORGE_STRING_ALGORITHMS_H

#include "george/types.h"

/* The table at this address was independently identified as reflected CRC-32.
 * Its 256 words are not copied from the disc into tracked source. */
extern const u32 D_00445650[256];

u32 func_00295050(const signed char *text);
s32 func_0029C588(const float *values, s32 count, float value);
u32 func_0029C5F0(u32 previous_crc, const u8 *bytes, u32 length);
u32 func_0029C648(const signed char *text);

#endif
