#include "george/deimos.h"

extern GeorgeDeimosPoolNode D_00474F50[];
extern s16 D_0047EF50[];
extern s32 D_00480350;
extern u16 D_00480358[];
extern s32 D_00481758;
extern void *D_00481764;
extern void *D_00481768;

extern void *func_002AEE60(u32 size);
extern void *func_002AEF08(u32 size);
extern void func_002AF100(void *pointer);
extern void func_002AF120(void *pointer);
extern void *func_002AD700(void *allocator, u32 size, s32 flag);
extern void func_002AD748(void *allocator, void *pointer);
extern u32 func_00295050(const char *text);
extern char *func_00393B74(char *destination, const char *source);
extern void *func_003934F8(void *destination, const void *source, u32 size);

/* This allocation sequence is inlined in each retail constructor. */
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

/* Type-specific payload destruction, followed by invalidating type and slot. */
void func_002CC9B8(GeorgeDeimosPoolNode *node)
{
    if (node->type == 0) {
        if (node->payload != 0) {
            func_002AF120((void *)node->payload);
        }
    } else if (node->type == 2) {
        s32 bucket;
        for (bucket = 0; bucket < 17; ++bucket) {
            GeorgeDeimosHashNode *entry = ((GeorgeDeimosHashNode **)node->payload)[bucket];
            while (entry != 0) {
                GeorgeDeimosHashNode *next = entry->next;
                s16 tag = (s16)entry->value.tag;
                if (tag >= 3 && tag < 6) {
                    func_002CD130((GeorgeDeimosPoolNode *)entry->value.payload.pointer);
                }
                func_002AD748(D_00481768, entry);
                entry = next;
            }
        }
        func_002AD748(D_00481764, (void *)node->payload);
    } else if (node->type == 3) {
        func_002AF100((void *)node->payload);
    } else if (node->type == 4) {
        GeorgeDeimosCallable *callable = (GeorgeDeimosCallable *)node->payload;
        if (callable->data0C != 0) {
            func_002AF120(callable->data0C);
        }
        func_002AF100((void *)node->payload);
    }
    node->active_slot = -1;
    node->type = (u8)-1;
}

GeorgeDeimosPoolNode *func_002CD2B0(void)
{
    GeorgeDeimosPoolNode *node = pool_take();
    node->type = 3;
    node->payload = (u32)func_002AEE60(12);
    return node;
}

GeorgeDeimosPoolNode *func_002CD348(GeorgeDeimosHashTable *secondary)
{
    GeorgeDeimosPoolNode *node = pool_take();
    s32 bucket;
    node->field08 = (u32)secondary;
    node->type = 2;
    node->payload = (u32)func_002AD700(D_00481764, 0x44, 1);
    for (bucket = 0; bucket < 17; ++bucket) {
        ((GeorgeDeimosHashNode **)node->payload)[bucket] = 0;
    }
    return node;
}

GeorgeDeimosPoolNode *func_002CD5A0(u32 size)
{
    GeorgeDeimosPoolNode *node = pool_take();
    node->type = 0;
    node->payload = (u32)func_002AEF08(size);
    return node;
}

GeorgeDeimosPoolNode *func_002CD630(const char *text)
{
    GeorgeDeimosPoolNode *node = pool_take();
    char *buffer;
    node->type = 0;
    buffer = func_002AEF08(func_00295050(text) + 1);
    node->payload = (u32)buffer;
    func_00393B74(buffer, text);
    return node;
}

GeorgeDeimosPoolNode *func_002CD6E0(u32 field00, u32 field04, const u32 *data, u32 count, u32 field18)
{
    GeorgeDeimosPoolNode *node = pool_take();
    GeorgeDeimosCallable *callable;
    node->type = 4;
    callable = func_002AEE60(0x1C);
    callable->field00 = field00;
    callable->field04 = field04;
    node->payload = (u32)callable;
    callable->data0C = func_002AEF08(count << 2);
    func_003934F8(((GeorgeDeimosCallable *)node->payload)->data0C, data, count << 2);
    callable = (GeorgeDeimosCallable *)node->payload;
    callable->count08 = count;
    ((GeorgeDeimosCallable *)node->payload)->field10 = 0;
    ((GeorgeDeimosCallable *)node->payload)->field14 = 0;
    ((GeorgeDeimosCallable *)node->payload)->field18 = field18;
    return node;
}

GeorgeDeimosPoolNode *func_002CD810(u32 field00, u32 field04, u32 field10, u32 field14)
{
    GeorgeDeimosPoolNode *node = pool_take();
    GeorgeDeimosCallable *callable;
    node->type = 4;
    callable = func_002AEE60(0x1C);
    callable->field00 = field00;
    callable->field04 = field04;
    callable->field10 = field10;
    callable->field14 = field14;
    callable->field18 = 0;
    node->payload = (u32)callable;
    callable->data0C = 0;
    callable->count08 = 0;
    return node;
}
