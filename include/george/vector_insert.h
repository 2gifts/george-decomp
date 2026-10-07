#ifndef GEORGE_VECTOR_INSERT_H
#define GEORGE_VECTOR_INSERT_H

#include "george/types.h"

/* Observed three-pointer prefix, without an original class/type-name claim.
 * The selected insertion entry is a genuine C++ member template instantiation.
 * Its equivalent numeric caller interface has this in GPR4, iterator in GPR5
 * and the address of the incoming four-byte element in GPR6.
 * Object/storage must be distinct valid initialized allocations in the native
 * ordinary-C++ domain; original wrapping/corrupt/overlapping domains are not
 * silently asserted equivalent to the upstream source's final store order.
 */
typedef struct GeorgeVectorInsertRange {
    void **start;
    void **finish;
    void **end_of_storage;
} GeorgeVectorInsertRange;

#ifdef __cplusplus
extern "C" {
#endif

void func_001007E0(GeorgeVectorInsertRange *range, void **position,
                   void *const *value);
s32 func_00100AA8(const void *left, const void *right);

#ifdef __cplusplus
}
#endif

#endif
