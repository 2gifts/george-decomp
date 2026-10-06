#include "george/deimos_pool.h"
#include "george/string_algorithms.h"

extern GeorgeDeimosPoolNode D_00474F50[];
extern s16 D_0047EF50[];
extern s32 D_00480350;
extern u16 D_00480358[];
extern s32 D_00481758;
extern void *D_00481764;
extern GeorgeDeimosValue *D_00474F48;
extern const char *D_003FDB80[];
extern const char D_00448A58[];
extern void *func_002AD700(void *allocator, u32 size, s32 flag);
extern void func_002AF100(void *pointer);
extern void func_002AF120(void *pointer);
extern void func_002CC938(const char *format, ...);

/* Reuse the reviewed inlined allocation sequence from deimos_lifecycle.c. */
static __inline__ GeorgeDeimosPoolNode *pool_take(void)
{
    s32 free_count = D_00480350 - 1;
    s32 active_count = D_00481758;
    u16 slot = (u16)D_00481758;
    s16 index = D_0047EF50[free_count];
    GeorgeDeimosPoolNode *node = &D_00474F50[index];

    D_00480358[active_count] = (u16)index;
    D_00481758 = active_count + 1;
    D_00480350 = free_count;
    node->active_slot = (s16)slot;
    return node;
}

/* Destructors may change both active count and the free-stack position. */
void func_002CCF88(void)
{
    s32 slot = 0;
    if (D_00481758 > 0) {
        do {
            s16 index = (s16)D_00480358[slot];
            s32 free_count;
            s32 active_count;
            ++slot;
            func_002CC9B8(&D_00474F50[index]);
            free_count = D_00480350;
            active_count = D_00481758;
            D_0047EF50[free_count] = index;
            D_00480350 = free_count + 1;
            if (!(slot < active_count)) {
                break;
            }
        } while (1);
    }
    D_00481758 = 0;
}

void func_002CD048(const GeorgeDeimosValue *value)
{
    s16 tag = (s16)value->tag;
    if (tag >= 3 && tag < 6) {
        func_002CD0B8(value->payload.pointer);
    }
}

void func_002CD080(const GeorgeDeimosValue *value)
{
    s16 tag = (s16)value->tag;
    if (tag >= 3 && tag < 6) {
        func_002CD130(value->payload.pointer);
    }
}

/* This entry point intentionally leaves type-2 table payloads untouched. */
void func_002CD1A0(GeorgeDeimosPoolNode *node)
{
    if (node->type == 0) {
        if (node->payload != 0) {
            func_002AF120((void *)node->payload);
        }
    } else if (node->type == 3) {
        func_002AF100((void *)node->payload);
    } else if (node->type == 4) {
        GeorgeDeimosCallable *callable = (GeorgeDeimosCallable *)node->payload;
        if (callable->data0C != 0) {
            func_002AF120(callable->data0C);
        }
        /* The data release may replace the node's callable payload. */
        func_002AF100((void *)node->payload);
    }
    node->active_slot = -1;
    node->type = (u8)-1;
}

void func_002CD410(u32 key, GeorgeDeimosValue *value, void *table)
{
    func_002CCB10(table, key, value);
}

GeorgeDeimosPoolNode *func_002CD440(GeorgeDeimosHashTable *source,
                                 GeorgeDeimosHashTable *secondary)
{
    GeorgeDeimosPoolNode *node = pool_take();
    s32 bucket;
    node->field08 = (u32)secondary;
    node->type = 2;
    node->payload = (u32)func_002AD700(D_00481764, 0x44, 1);
    for (bucket = 0; bucket < 17; ++bucket) {
        /* Reload payload after each store, including aliased allocator results. */
        ((GeorgeDeimosHashNode **)node->payload)[bucket] = 0;
    }
    func_002CDA20(source, func_002CD410, node);
    return node;
}

u32 func_002CD8F8(const GeorgeDeimosPoolNode *node)
{
    return *(const u32 *)node->payload;
}

/* Advance the iterator's chain first, then find the next nonempty local bucket. */
void func_002CDAB0(s32 iterator_index, s32 output_index)
{
    GeorgeDeimosPoolNode *node = D_00474F48[iterator_index].payload.pointer;
    GeorgeDeimosTableIterator *iterator = (GeorgeDeimosTableIterator *)node->payload;
    if (iterator->current != 0) {
        iterator->current = iterator->current->next;
        if (iterator->current != 0) {
            D_00474F48[output_index] = iterator->current->value;
            return;
        }
    }
    {
        s32 bucket = iterator->bucket;
        while (bucket < 17) {
            GeorgeDeimosHashNode **buckets = (GeorgeDeimosHashNode **)iterator->table->payload;
            iterator->current = buckets[bucket];
            if (iterator->current != 0) {
                iterator->bucket = bucket + 1;
                break;
            }
            ++bucket;
        }
    }
    if (iterator->current != 0) {
        D_00474F48[output_index] = iterator->current->value;
    } else {
        GeorgeDeimosValue *value;
        iterator->bucket = 17;
        value = &D_00474F48[iterator_index];
        value->subtype = 0;
        value->tag = 0;
        /* Clearing the iterator slot can replace the global argument window. */
        value = &D_00474F48[output_index];
        value->subtype = 0;
        value->tag = 0;
    }
}

const char *func_002CDFD8(s32 type)
{
    return D_003FDB80[type];
}

/* Original jump-table cases: tags 2/6 use bits, tag 3 hashes string payload. */
u32 func_002CD908(const GeorgeDeimosValue *value)
{
    s16 tag = (s16)value->tag;
    if (tag == 2 || tag == 6) {
        return value->payload.bits;
    } else if (tag == 3) {
        GeorgeDeimosPoolNode *node = value->payload.pointer;
        return func_0029C648((const signed char *)node->payload);
    }
    func_002CC938(D_00448A58, func_002CDFD8(tag));
    return 0;
}
