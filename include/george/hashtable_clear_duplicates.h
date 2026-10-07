#ifndef GEORGE_HASHTABLE_CLEAR_DUPLICATES_H
#define GEORGE_HASHTABLE_CLEAR_DUPLICATES_H

#include "george/types.h"

/* Consumed prefixes only: sizeof these views does not establish any complete
 * original node/table allocation, payload, class or ownership contract. */
typedef struct GeorgeClearNodePrefix {
    struct GeorgeClearNodePrefix *field00;
} GeorgeClearNodePrefix;

typedef struct GeorgeClearRangePrefix {
    void **field00;
    void **field04;
    u32 unknown08;
} GeorgeClearRangePrefix;

typedef struct GeorgeClearTablePrefix {
    u32 unknown00;
    GeorgeClearRangePrefix field04;
    u32 field10;
} GeorgeClearTablePrefix;

typedef char george_clear_node_pointer[(sizeof(void *) == 4) ? 1 : -1];
typedef char george_clear_node_first[(offsetof(GeorgeClearNodePrefix, field00) == 0) ? 1 : -1];
typedef char george_clear_begin[(offsetof(GeorgeClearTablePrefix, field04.field00) == 4) ? 1 : -1];
typedef char george_clear_end[(offsetof(GeorgeClearTablePrefix, field04.field04) == 8) ? 1 : -1];
typedef char george_clear_count[(offsetof(GeorgeClearTablePrefix, field10) == 16) ? 1 : -1];

/* Effects-only numeric conventions from observed callers. Original declarations
 * and incidental GPR2 result promises remain unresolved. */
void func_0024B7D0(GeorgeClearTablePrefix *table);
void func_0024BF58(GeorgeClearTablePrefix *table);
void func_0024C320(GeorgeClearTablePrefix *table);
void func_0024C6E8(GeorgeClearTablePrefix *table);
void func_0024CE78(GeorgeClearTablePrefix *table);

#endif
