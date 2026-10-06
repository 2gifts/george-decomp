#ifndef GEORGE_ALGORITHM_TEMPLATES_H
#define GEORGE_ALGORITHM_TEMPLATES_H

/* Ordinary C definitions shared by complete byte-identical original bodies.
 * Types are supplied by the consuming headers. These are full definitions,
 * preserving each original entry rather than introducing forwarding calls. */
#define GEORGE_DEFINE_UPPER_BOUND(name) \
void **name(void **begin, void **end, void *const *key, GeorgeStartupCompare compare) \
{ \
    s32 count = (s32)(end - begin); \
    while (count > 0) { \
        s32 half = count >> 1; \
        void **middle = begin + half; \
        if (compare(*key, *middle) != 0) { \
            count = half; \
        } else { \
            begin = middle + 1; \
            count = count - half - 1; \
        } \
    } \
    return begin; \
}

#define GEORGE_DEFINE_MAP_LOOKUP(name) \
void *name(GeorgeGenericMap *map, u32 key) \
{ \
    GeorgeGenericMapNode *node = map->buckets[key % map->bucket_count]; \
    while (node != 0) { \
        if (node->key == key) { \
            return node->value; \
        } \
        node = node->next; \
    } \
    return 0; \
}

#define GEORGE_DEFINE_MAP_VISITOR(name) \
void name(GeorgeGenericMap *map, GeorgeGenericMapVisitor callback, void *context) \
{ \
    u32 bucket = 0; \
    if (map->bucket_count != 0) { \
        do { \
            GeorgeGenericMapNode *node = map->buckets[bucket]; \
            while (node != 0) { \
                void *value = node->value; \
                u32 key = node->key; \
                node = node->next; \
                callback(map, key, value, context); \
            } \
            ++bucket; \
        } while (bucket < map->bucket_count); \
    } \
}

#define GEORGE_DEFINE_ACTION_RESTART(name) \
void name(GeorgeGoalTimedAction *goal) \
{ \
    if (func_00174500(((GeorgeGoalOwner *)goal->base.links.unknown00)->field08) != 0) { \
        float value = goal->field14; \
        GeorgeGoalOwner *owner = (GeorgeGoalOwner *)goal->base.links.unknown00; \
        goal->field10 = value; \
        owner->field24 |= 8; \
        goal->field18 = 0; \
    } \
}

#endif
