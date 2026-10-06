#include "george/deimos.h"

extern GeorgeDeimosValue D_00474748[];
extern GeorgeDeimosPoolNode D_00474F50[];
extern s16 D_0047EF50[];
extern s32 D_00480350;
extern u16 D_00480358[];
extern s32 D_00481758;
extern s32 D_0048176C;
extern GeorgeDeimosFrame D_00481770[];
extern u32 D_0048180C;
extern void func_002CC9B8(GeorgeDeimosPoolNode *node);

/* First reference removes the node from the unreferenced active list. */
void func_002CD0B8(GeorgeDeimosPoolNode *node)
{
    if (node->references == 0) {
        s32 count = D_00481758 - 1;
        s16 slot = node->active_slot;
        u16 moved = D_00480358[count];

        D_00481758 = count;
        D_00480358[slot] = moved;
        if (count > 0) {
            D_00474F50[(s16)moved].active_slot = slot;
        }
        node->active_slot = -1;
    }
    node->references = (u16)(node->references + 1);
}

/* Last reference releases the payload before returning the index to the stack. */
void func_002CD130(GeorgeDeimosPoolNode *node)
{
    node->references = (u16)(node->references - 1);
    if (node->references == 0) {
        s32 free_count;
        u32 index;
        func_002CC9B8(node);
        free_count = D_00480350;
        index = ((u32)node - (u32)D_00474F50) >> 4;
        D_0047EF50[free_count] = (s16)index;
        D_00480350 = free_count + 1;
    }
}

/* Allocation takes a signed 16-bit index from the free-list stack. */
GeorgeDeimosPoolNode *func_002CD248(void)
{
    s32 free_count = D_00480350 - 1;
    s32 active_count = D_00481758;
    u16 slot = (u16)D_00481758;
    s16 index = D_0047EF50[free_count];
    GeorgeDeimosPoolNode *node;

    D_00480358[active_count] = (u16)index;
    node = &D_00474F50[index];
    D_00481758 = active_count + 1;
    D_00480350 = free_count;
    node->active_slot = (s16)slot;
    return node;
}

/* This entry point includes allocation, then writes type 1 and a word payload. */
GeorgeDeimosPoolNode *func_002CD528(u32 payload)
{
    s32 free_count = D_00480350 - 1;
    s32 active_count = D_00481758;
    u16 slot = (u16)D_00481758;
    s16 index = D_0047EF50[free_count];
    GeorgeDeimosPoolNode *node;

    D_00480358[active_count] = (u16)index;
    node = &D_00474F50[index];
    D_00481758 = active_count + 1;
    D_00480350 = free_count;
    node->active_slot = (s16)slot;
    node->type = 1;
    node->payload = payload;
    return node;
}

/* Search the secondary table before the local table, using 17 buckets. */
GeorgeDeimosValue *func_002CD990(GeorgeDeimosHashTable *table, u32 key)
{
    u32 bucket = key % 17;
    GeorgeDeimosHashNode *node;

    if (table->field08 != 0) {
        node = table->field08->buckets[bucket];
        while (node != 0) {
            if (node->key == key) {
                return &node->value;
            }
            node = node->next;
        }
    }
    node = table->buckets[bucket];
    while (node != 0) {
        if (node->key == key) {
            return &node->value;
        }
        node = node->next;
    }
    return 0;
}

/* Read next after the callback: it may change the chain or bucket array. */
void func_002CDA20(GeorgeDeimosHashTable *table, GeorgeDeimosVisitor callback, void *context)
{
    s32 bucket;
    for (bucket = 0; bucket < 17; ++bucket) {
        GeorgeDeimosHashNode *node = table->buckets[bucket];
        while (node != 0) {
            callback(node->key, &node->value, context);
            node = node->next;
        }
    }
}

/* Preserve listed keys; otherwise release references for signed tags 3..5. */
void func_002CDB88(u32 key, GeorgeDeimosValue *value, const u32 *preserved_keys)
{
    if (preserved_keys != 0) {
        while (*preserved_keys != 0) {
            if (*preserved_keys++ == key) {
                return;
            }
        }
    }
    if ((s16)value->tag >= 3 && (s16)value->tag < 6) {
        func_002CD130((GeorgeDeimosPoolNode *)value->payload.pointer);
    }
    value->tag = 0;
}

/* The retail code inlines traversal rather than calling func_002CDA20. */
void func_002CDC00(GeorgeDeimosHashTable *table, const u32 *preserved_keys)
{
    s32 bucket;
    for (bucket = 0; bucket < 17; ++bucket) {
        GeorgeDeimosHashNode *node = table->buckets[bucket];
        while (node != 0) {
            func_002CDB88(node->key, &node->value, preserved_keys);
            node = node->next;
        }
    }
}

/* Running script frames contain an object whose +0x14 member is the context. */
void *func_002CDF90(void)
{
    s32 depth = D_0048176C;
    if (depth < 0) {
        return 0;
    }
    return D_00481770[depth].context->field14;
}

/* Tag 2 denotes a float in the observed script callbacks. */
void func_002D0560(s32 unused, s32 destination)
{
    float seconds = (float)D_0048180C * 0.001f;
    (void)unused;
    if (destination != -1) {
        D_00474748[destination].payload.scalar = seconds;
        D_00474748[destination].tag = 2;
        D_00474748[destination].subtype = 0;
    }
}
