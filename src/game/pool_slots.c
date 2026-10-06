#include "george/pool_slots.h"
#include "george/heap.h"

/* R5900 multiplies and pointer arithmetic retain the low 32 bits. */
#define SLOT_HEADER(pool, index) \
    ((u16 *)((u32)(pool)->data + (u32)(index) * (pool)->stride))

/* The slot store can alias the pool header: reload capacity/stride/data. */
#define POPULATE_LINKS(pool, final_count) do { \
    s32 index = 0; \
    while (index < (pool)->capacity) { \
        u16 *slot = (u16 *)((u32)(pool)->data + (u32)index * (pool)->stride); \
        index = (s32)((u32)index + 1U); \
        *slot = (u16)index; \
    } \
    (final_count) = (pool)->capacity; \
} while (0)

/* These initialization stores deliberately leave other fields untouched. */
GeorgeSlotPool *func_002AD460(GeorgeSlotPool *pool)
{
    pool->free_head = 0xFFFF;
    pool->owns_data = 1;
    pool->data = 0;
    pool->snapshot = 0;
    pool->frozen = 0;
    return pool;
}

GeorgeSlotPool *func_002AD488(GeorgeSlotPool *pool, s32 count, u32 stride)
{
    pool->data = 0;
    pool->snapshot = 0;
    pool->frozen = 0;
    pool->free_head = 0xFFFF;
    pool->owns_data = 1;
    func_002AD518(pool, count, stride);
    return pool;
}

GeorgeSlotPool *func_002AD4D0(GeorgeSlotPool *pool, void *data, s32 count, u32 stride)
{
    pool->data = 0;
    pool->snapshot = 0;
    pool->frozen = 0;
    pool->free_head = 0xFFFF;
    pool->owns_data = 1;
    func_002AD5A0(pool, data, count, stride);
    return pool;
}

void func_002AD518(GeorgeSlotPool *pool, s32 count, u32 stride)
{
    s32 free_count;
    pool->capacity = count;
    pool->stride = stride;
    if (pool->owns_data != 0)
        pool->data = func_002AEC28((u32)count * stride);
    POPULATE_LINKS(pool, free_count);
    pool->free_head = 0;
    pool->free_count = free_count;
}

void func_002AD5A0(GeorgeSlotPool *pool, void *data, s32 count, u32 stride)
{
    s32 free_count;
    if (data != 0) {
        pool->data = data;
        pool->owns_data = 0;
    }
    pool->capacity = count;
    pool->stride = stride;
    if (pool->owns_data != 0)
        pool->data = func_002AEC28((u32)count * stride);
    POPULATE_LINKS(pool, free_count);
    pool->free_head = 0;
    pool->free_count = free_count;
}

#define RELEASE_STORAGE(pool) do { \
    u16 *snapshot; \
    if ((pool)->owns_data != 0 && (pool)->data != 0) \
        func_002AEE40((pool)->data); \
    snapshot = (pool)->snapshot; \
    (pool)->data = 0; \
    if (snapshot != 0) { \
        func_002AEE40(snapshot); \
        (pool)->snapshot = 0; \
    } \
} while (0)

void func_002AD630(GeorgeSlotPool *pool)
{
    RELEASE_STORAGE(pool);
}

void func_002AD688(GeorgeSlotPool *pool, u32 flags)
{
    RELEASE_STORAGE(pool);
    if (flags & 1U)
        func_002AF100(pool);
}

void *func_002AD700(GeorgeSlotPool *pool)
{
    s32 count = pool->free_count;
    u16 *slot;
    u16 next;
    if (count == 0 || pool->frozen != 0)
        return 0;
    slot = SLOT_HEADER(pool, pool->free_head);
    next = *slot;
    pool->free_count = (s32)((u32)count - 1U);
    pool->free_head = next;
    return slot;
}

void func_002AD748(GeorgeSlotPool *pool, void *slot)
{
    u32 index;
    if (slot == 0 || pool->frozen != 0)
        return;
    *(u16 *)slot = pool->free_head;
    index = ((u32)slot - (u32)pool->data) / pool->stride;
    pool->free_count = (s32)((u32)pool->free_count + 1U);
    pool->free_head = (u16)index;
}

void func_002AD798(GeorgeSlotPool *pool)
{
    s32 index;
    u16 current;
    if (pool->owns_data != 0)
        return;
    if (pool->free_count >= 2) {
        pool->snapshot = func_002AEC28(((u32)pool->free_count - 1U) << 1);
        current = pool->free_head;
        index = 0;
        while (index < (s32)((u32)pool->free_count - 1U)) {
            u16 next = *SLOT_HEADER(pool, current);
            pool->snapshot[index] = next;
            current = pool->snapshot[index];
            index = (s32)((u32)index + 1U);
        }
    }
    pool->frozen = 1;
}

void func_002AD848(GeorgeSlotPool *pool)
{
    s32 index;
    u16 current;
    if (pool->frozen == 0)
        return;
    if (pool->snapshot != 0) {
        current = pool->free_head;
        index = 0;
        while (index < (s32)((u32)pool->free_count - 1U)) {
            u16 *slot = SLOT_HEADER(pool, current);
            u16 next = pool->snapshot[index];
            *slot = next;
            current = next;
            index = (s32)((u32)index + 1U);
        }
        func_002AEE40(pool->snapshot);
        pool->snapshot = 0;
    }
    pool->frozen = 0;
}

void func_002AD8E0(GeorgeSlotPool *pool)
{
    s32 free_count;
    POPULATE_LINKS(pool, free_count);
    pool->free_count = free_count;
}

s32 func_002AD930(const GeorgeSlotPool *pool, u32 start, u32 limit)
{
    u32 first, end;
    if (pool->owns_data != 0)
        return 0;
    first = (u32)pool->data;
    end = first + (u32)pool->capacity * pool->stride;
    if (start >= first && start < end)
        return 1;
    if (first < limit && limit <= end)
        return 1;
    if (start < first && end < limit)
        return 1;
    return 0;
}
