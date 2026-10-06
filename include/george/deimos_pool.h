#ifndef GEORGE_DEIMOS_POOL_H
#define GEORGE_DEIMOS_POOL_H

#include "george/deimos.h"

/* Only these three fields of the table iterator payload are established. */
typedef struct GeorgeDeimosTableIterator {
    GeorgeDeimosPoolNode *table;
    GeorgeDeimosHashNode *current;
    s32 bucket;
} GeorgeDeimosTableIterator;

typedef char deimos_iterator_size[(sizeof(GeorgeDeimosTableIterator) == 12) ? 1 : -1];
typedef char deimos_iterator_current_offset[(offsetof(GeorgeDeimosTableIterator, current) == 4) ? 1 : -1];
typedef char deimos_iterator_bucket_offset[(offsetof(GeorgeDeimosTableIterator, bucket) == 8) ? 1 : -1];

void func_002CCF88(void) GEORGE_SAVE128;
void func_002CD048(const GeorgeDeimosValue *value) GEORGE_SAVE128;
void func_002CD080(const GeorgeDeimosValue *value) GEORGE_SAVE128;
void func_002CD1A0(GeorgeDeimosPoolNode *node) GEORGE_SAVE128;
void func_002CD410(u32 key, GeorgeDeimosValue *value, void *table) GEORGE_SAVE128;
GeorgeDeimosPoolNode *func_002CD440(GeorgeDeimosHashTable *source,
                                 GeorgeDeimosHashTable *secondary) GEORGE_SAVE128;
u32 func_002CD8F8(const GeorgeDeimosPoolNode *node);
void func_002CDAB0(s32 iterator_index, s32 output_index);
const char *func_002CDFD8(s32 type);
u32 func_002CD908(const GeorgeDeimosValue *value) GEORGE_SAVE128;

#endif
