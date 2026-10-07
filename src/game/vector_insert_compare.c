#include "george/vector_insert.h"

/* Complete unsigned-word callback consumed by the published upper bound.
 * These arguments address comparison words; no full pointed-object type or
 * original declaration spelling is asserted. */
s32 func_00100AA8(const void *left, const void *right)
{
    return *(const u32 *)left < *(const u32 *)right;
}
