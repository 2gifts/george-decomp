#include "george/deimos_tables.h"

extern GeorgeGenericMap *D_00481760;
extern void *D_00481768;
extern const char D_00448A90[];
extern void *func_002AD700(void *allocator, u32 size, s32 flag);
extern void *func_002AEE60(u32 size);
extern void *func_002AEC28(u32 size);
extern void func_002CC938(const char *format, ...);

/* Search the secondary table first through the existing Deimos lookup. */
void func_002CCB10(GeorgeDeimosPoolNode *table, u32 key, const GeorgeDeimosValue *input)
{
    GeorgeDeimosHashTable *hash_table = (GeorgeDeimosHashTable *)table;
    GeorgeDeimosValue *value = func_002CD990(hash_table, key);
    if (value != 0) {
        s16 tag = (s16)value->tag;
        if (tag >= 3 && tag < 6) {
            func_002CD130((GeorgeDeimosPoolNode *)value->payload.pointer);
        }
        *value = *input;
        tag = (s16)value->tag;
        if (tag >= 3 && tag < 6) {
            func_002CD0B8((GeorgeDeimosPoolNode *)value->payload.pointer);
        }
    } else {
        u32 bucket = key % 17u;
        GeorgeDeimosHashNode *node = func_002AD700(D_00481768, 16, 1);
        if (node != 0) {
            GeorgeDeimosHashNode *previous = hash_table->buckets[bucket];
            s16 tag;
            node->key = key;
            node->next = previous;
            node->value = *input;
            /* The bucket base and input tag are reloaded after these stores. */
            hash_table->buckets[bucket] = node;
            tag = (s16)input->tag;
            if (tag >= 3 && tag < 6) {
                func_002CD0B8((GeorgeDeimosPoolNode *)input->payload.pointer);
            }
        } else {
            func_002CC938(D_00448A90);
        }
    }
}

/* Global values are separately allocated records held by the generic map. */
void func_002CDCA0(u32 key, const GeorgeDeimosValue *input)
{
    GeorgeDeimosValue *value = func_002CDD60(key);
    s16 tag;
    if (value != 0) {
        tag = (s16)value->tag;
        if (tag >= 3 && tag < 6) {
            func_002CD130((GeorgeDeimosPoolNode *)value->payload.pointer);
        }
    } else {
        value = func_002AEE60(8);
        /* Allocation may replace the global dictionary; load it afterwards. */
        func_002A7CD0(D_00481760, key, value);
    }
    *value = *input;
    tag = (s16)value->tag;
    if (tag >= 3 && tag < 6) {
        func_002CD0B8((GeorgeDeimosPoolNode *)value->payload.pointer);
    }
}

GeorgeDeimosValue *func_002CDD60(u32 key)
{
    return func_002A7C08(D_00481760, key);
}

/* The map iterator ignores callback return registers; no return value is used. */
void func_002CDD88(GeorgeGenericMap *unused, u32 key, void *value, void *context)
{
    GeorgeDeimosGlobalVisitor *visitor = context;
    (void)unused;
    visitor->callback(key, (GeorgeDeimosValue *)value, visitor->context);
}

void func_002CDDB0(GeorgeDeimosVisitor callback, void *context)
{
    GeorgeDeimosGlobalVisitor visitor;
    visitor.callback = callback;
    visitor.context = context;
    func_002A8130(D_00481760, func_002CDD88, &visitor);
}

void *func_002A7C08(GeorgeGenericMap *map, u32 key)
{
    GeorgeGenericMapNode *node = map->buckets[key % map->bucket_count];
    while (node != 0) {
        if (node->key == key) {
            return node->value;
        }
        node = node->next;
    }
    return 0;
}

/* Bit zero permits duplicate keys; otherwise replace the first matching value. */
void func_002A7CD0(GeorgeGenericMap *map, u32 key, void *value)
{
    u32 bucket = key % map->bucket_count;
    GeorgeGenericMapNode *node = map->buckets[bucket];
    if ((map->flags & 1) == 0) {
        while (node != 0) {
            if (node->key == key) {
                node->value = value;
                return;
            }
            node = node->next;
        }
    }
    node = func_002AEC28(12);
    if (node != 0) {
        node->key = key;
        node->value = value;
        node->next = 0;
        node->next = map->buckets[bucket];
        /* Do not cache the bucket base across the node stores. */
        map->buckets[bucket] = node;
        map->count = map->count + 1u;
    }
}

/* Save next before invoking a callback that may remove the current node. */
void func_002A8130(GeorgeGenericMap *map, GeorgeGenericMapVisitor callback, void *context)
{
    u32 bucket = 0;
    if (map->bucket_count != 0) {
        do {
            GeorgeGenericMapNode *node = map->buckets[bucket];
            while (node != 0) {
                void *value = node->value;
                u32 key = node->key;
                node = node->next;
                callback(map, key, value, context);
            }
            ++bucket;
            /* Count and bucket base are observed again after each callback chain. */
        } while (bucket < map->bucket_count);
    }
}
