#ifndef GEORGE_POOLED_DESTRUCTOR_DUPLICATES_H
#define GEORGE_POOLED_DESTRUCTOR_DUPLICATES_H

#include <stddef.h>
#include "george/types.h"
#include "george/compiler.h"

/* Authored consumed prefix only: no original class, ownership, allocation
 * extent, or meaning for the unknown first four bytes is established. */
typedef struct GeorgePooledDestructorConsumedPrefix {
    u8 unknown04[4];
    u16 release_size;
} GeorgePooledDestructorConsumedPrefix;

/* Effects-only convention; incidental v0 is not a promised result. */
void func_0019A3D0(void *, u32) GEORGE_SAVE128;
void func_0019D3F0(void *, u32) GEORGE_SAVE128;
void func_001F0120(void *, u32) GEORGE_SAVE128;
void func_0022CB40(void *, u32) GEORGE_SAVE128;
void func_00276C60(void *, u32) GEORGE_SAVE128;

#endif
