#include "george/resource_base.h"
#include "george/deimos_tables.h"

extern GeorgeResourceManagerPrefix *D_003F960C;
extern struct GeorgeSlotPool *D_003F9610;
extern const u8 D_0043B1C8[];
extern void *func_00219FF0(GeorgeGenericMap *container,u32 key);
extern void func_00224AB8(GeorgeResourceRecord *);
extern void func_00225C28(const void *key,u32 kind);
extern void func_00224750(u32 first,u32 second,GeorgeResourceRecord *);
/* Original caller supplies these three lanes. The independently recovered
 * pool body consumes only its first argument. */
extern void *func_002AD700(struct GeorgeSlotPool *,u32 size,u32 mode);
extern void func_002AD748(struct GeorgeSlotPool *,void *);
extern void func_0020E5C0(void *);
extern void func_002AF100(void *);

#define BASE(record) ((GeorgeResourceBasePrefix *)(record))

typedef struct ResourceReadyPair {
    s16 adjustment;
    u16 reserved;
    void (*invoke)(GeorgeResourceRecord *);
} ResourceReadyPair;

GeorgeResourceRecord *func_00225E80(const void *key,u32 kind)
{
    GeorgeGenericMap *container = *(GeorgeGenericMap **)((u32)D_003F960C + 0x78U + (kind << 2));
    return func_00219FF0(container,(u32)key);
}

void func_002267D0(GeorgeResourceRecord *record,u32 mode)
{
    GeorgeResourceCallbackNode *next;
    GeorgeResourceBasePrefix *base = BASE(record);
    base->table = D_0043B1C8;
    if (base->callbacks != 0) {
        do {
            GeorgeResourceCallbackNode *node = base->callbacks;
            struct GeorgeSlotPool *pool = D_003F9610;
            next = node->next;
            func_002AD748(pool,node);
            base->callbacks = next;
        } while (next != 0);
    }
    func_00225C28(base->key,base->kind);
    if (base->payload != 0 && (base->flags & 0x10) == 0) {
        GeorgeResourceAllocator *allocator;
        GeorgeResourceManagerPrefix *manager;
        void *payload;
        u32 size;
        func_00224750(0,1,record);
        payload = base->payload;
        manager = D_003F960C;
        size = base->size;
        allocator = manager->allocator;
        if (payload != 0) {
            if (allocator->cursor != 0 && allocator->mode == 1) {
                struct GeorgeSlotPool *pool = allocator->override_pool;
                if (pool != 0)
                    func_002AD748(pool,payload);
            } else {
                u32 address = (u32)payload & 0x0FFFFFFFU;
                if (address >= allocator->first && address <= allocator->last) {
                    u32 index = (size + 0x7FU) >> 7;
                    struct GeorgeSlotPool *pool =
                        *(struct GeorgeSlotPool **)((u32)allocator + (index << 2));
                    func_002AD748(pool,(void *)address);
                } else {
                    func_0020E5C0((void *)address);
                }
            }
        }
    }
    base->count = 0xDE;
    base->flags = 0xDE;
    base->payload = (void *)0xDEADBEEFU;
    base->key = (void *)0xDEADBEEFU;
    base->field08 = 0xDEADBEEFU;
    base->size = 0xDEADBEEFU;
    if ((mode & 1U) != 0)
        func_002AF100(record);
}

GeorgeResourceRecord *func_00226BC0(GeorgeResourceRecord *record,const void *key,
                                  u32 kind,u32 mode,u32 word)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    base->kind = (u8)kind;
    base->table = D_0043B1C8;
    base->key = (void *)key;
    base->field1C = word;
    base->flags = 0;
    base->field08 = 0;
    base->callbacks = 0;
    base->payload = 0;
    if (mode != 0) {
        base->count = 0;
        base->flags |= 0x40;
    } else {
        base->count = 1;
    }
    func_00224AB8(record);
    if (base->payload != 0 && (base->flags & 0x10) == 0)
        base->flags |= 1;
    return record;
}

void func_00226D78(GeorgeResourceRecord *record)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    u16 count;
    u8 flags;
    base->flags &= 0x7F;
    count = base->count;
    flags = base->flags;
    base->count = (u16)(count + 1U);
    if ((flags & 4) != 0) {
        const ResourceReadyPair *pair = (const ResourceReadyPair *)(base->table + 0x28);
        pair->invoke((GeorgeResourceRecord *)((u32)record + (s32)pair->adjustment));
    } else {
        func_00226FB0(record);
    }
}

u32 func_00226E38(GeorgeResourceRecord *record,GeorgeResourceCallback callback,
                 void *argument,void *key)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    GeorgeResourceCallbackNode *node;
    if ((base->flags & 4) != 0) {
        callback((u32)record,argument);
        return 0;
    }
    node = func_002AD700(D_003F9610,0x10,0);
    if (node == 0)
        return 0;
    node->callback = callback;
    node->argument = argument;
    node->key = key;
    node->next = base->callbacks;
    base->callbacks = node;
    return 1;
}

u32 func_00226ED8(GeorgeResourceRecord *record,void *key)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    GeorgeResourceCallbackNode *node = base->callbacks;
    GeorgeResourceCallbackNode *previous = 0;
    while (node != 0) {
        if (node->key == key) {
            GeorgeResourceCallbackNode *next = node->next;
            if (previous == 0)
                base->callbacks = next;
            else
                previous->next = next;
            func_002AD748(D_003F9610,node);
            return 1;
        }
        previous = node;
        node = node->next;
    }
    return 0;
}

void func_00226F40(GeorgeResourceRecord *record)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    GeorgeResourceCallbackNode *node = base->callbacks;
    while (node != 0) {
        GeorgeResourceCallbackNode *head = base->callbacks;
        base->callbacks = head->next;
        node->callback((u32)record,node->argument);
        func_002AD748(D_003F9610,node);
        node = base->callbacks;
    }
}

void func_00226FB0(GeorgeResourceRecord *record)
{
    GeorgeResourceBasePrefix *base = BASE(record);
    const GeorgeGoalVirtualWord *pair;
    if (base->count == 0 && (base->flags & 0x40) != 0) {
        base->flags |= 0x80;
    } else {
        pair = (const GeorgeGoalVirtualWord *)(base->table + 0x18);
        pair->invoke((void *)((u32)record + (s32)pair->adjustment),0);
    }
}

#undef BASE
