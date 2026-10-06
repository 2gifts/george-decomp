#ifndef GEORGE_POOL_SLOTS_H
#define GEORGE_POOL_SLOTS_H

#include "george/types.h"
#include "george/compiler.h"

/* Only the observed 28-byte fixed-slot pool prefix is modeled. */
typedef struct GeorgeSlotPool {
    s32 capacity;
    u32 stride;
    u8 *data;
    u16 *snapshot;
    u16 free_head;
    u8 unknown12[2];
    s32 free_count;
    s16 owns_data;
    s16 frozen;
} GeorgeSlotPool;

typedef char george_slot_pool_size[(sizeof(GeorgeSlotPool) == 28) ? 1 : -1];
typedef char george_slot_pool_free_count[(offsetof(GeorgeSlotPool, free_count) == 0x14) ? 1 : -1];
typedef char george_slot_pool_frozen[(offsetof(GeorgeSlotPool, frozen) == 0x1A) ? 1 : -1];

GeorgeSlotPool *func_002AD460(GeorgeSlotPool *pool);
GEORGE_SAVE128 GeorgeSlotPool *func_002AD488(GeorgeSlotPool *pool, s32 count, u32 stride);
GEORGE_SAVE128 GeorgeSlotPool *func_002AD4D0(GeorgeSlotPool *pool, void *data, s32 count, u32 stride);
GEORGE_SAVE128 void func_002AD518(GeorgeSlotPool *pool, s32 count, u32 stride);
GEORGE_SAVE128 void func_002AD5A0(GeorgeSlotPool *pool, void *data, s32 count, u32 stride);
GEORGE_SAVE128 void func_002AD630(GeorgeSlotPool *pool);
GEORGE_SAVE128 void func_002AD688(GeorgeSlotPool *pool, u32 flags);
void *func_002AD700(GeorgeSlotPool *pool);
void func_002AD748(GeorgeSlotPool *pool, void *slot);
GEORGE_SAVE128 void func_002AD798(GeorgeSlotPool *pool);
GEORGE_SAVE128 void func_002AD848(GeorgeSlotPool *pool);
void func_002AD8E0(GeorgeSlotPool *pool);
s32 func_002AD930(const GeorgeSlotPool *pool, u32 start, u32 limit);

#endif
