#ifndef GEORGE_DEIMOS_TABLES_H
#define GEORGE_DEIMOS_TABLES_H

#include "george/deimos.h"

/* Separate from Deimos's fixed 17-bucket, inline-value table representation. */
typedef struct GeorgeGenericMapNode {
    struct GeorgeGenericMapNode *next;
    u32 key;
    void *value;
} GeorgeGenericMapNode;

typedef struct GeorgeGenericMap {
    u32 flags;
    u32 bucket_count;
    u32 count;
    GeorgeGenericMapNode **buckets;
} GeorgeGenericMap;

typedef void (*GeorgeGenericMapVisitor)(GeorgeGenericMap *map, u32 key,
                                       void *value, void *context);
typedef struct GeorgeDeimosGlobalVisitor {
    GeorgeDeimosVisitor callback;
    void *context;
} GeorgeDeimosGlobalVisitor;

#define TABLE_OFFSET(type, field, offset) \
    typedef char tables_offset_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
typedef char generic_map_node_size[(sizeof(GeorgeGenericMapNode) == 12) ? 1 : -1];
typedef char generic_map_size[(sizeof(GeorgeGenericMap) == 16) ? 1 : -1];
typedef char deimos_global_visitor_size[(sizeof(GeorgeDeimosGlobalVisitor) == 8) ? 1 : -1];
TABLE_OFFSET(GeorgeGenericMapNode, key, 4);
TABLE_OFFSET(GeorgeGenericMapNode, value, 8);
TABLE_OFFSET(GeorgeGenericMap, bucket_count, 4);
TABLE_OFFSET(GeorgeGenericMap, count, 8);
TABLE_OFFSET(GeorgeGenericMap, buckets, 12);
TABLE_OFFSET(GeorgeDeimosGlobalVisitor, context, 4);
#undef TABLE_OFFSET

void func_002CCB10(GeorgeDeimosPoolNode *table, u32 key, const GeorgeDeimosValue *input);
void func_002CDCA0(u32 key, const GeorgeDeimosValue *input);
GeorgeDeimosValue *func_002CDD60(u32 key);
void func_002CDD88(GeorgeGenericMap *unused, u32 key, void *value, void *context);
void func_002CDDB0(GeorgeDeimosVisitor callback, void *context);
void *func_002A7C08(GeorgeGenericMap *map, u32 key);
void func_002A7CD0(GeorgeGenericMap *map, u32 key, void *value);
void func_002A8130(GeorgeGenericMap *map, GeorgeGenericMapVisitor callback, void *context);

#endif
