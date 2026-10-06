#ifndef GEORGE_RESOURCE_BASE_H
#define GEORGE_RESOURCE_BASE_H

#include "george/resource_geometry.h"

struct GeorgeSlotPool;
typedef void (*GeorgeResourceCallback)(u32 resource, void *argument);
typedef struct GeorgeResourceCallbackNode {
    GeorgeResourceCallback callback;
    void *argument;
    void *key;
    struct GeorgeResourceCallbackNode *next;
} GeorgeResourceCallbackNode;

/* Observed base prefix; derived records retain their existing declarations. */
typedef struct GeorgeResourceBasePrefix {
    u16 count;
    u8 flags, kind;
    void *key;
    u32 field08, size;
    void *payload;
    u32 field14;
    GeorgeResourceCallbackNode *callbacks;
    u32 field1C;
    const u8 *table;
} GeorgeResourceBasePrefix;

/* Only the allocator fields read here are named. The 1,000 slot entries are
 * independently bounded by the original allocation helper's 0x3E8 test. */
typedef struct GeorgeResourceAllocator {
    struct GeorgeSlotPool *slots[1000];
    u32 first, last;
    u8 reservedFA8[0x14];
    void *cursor;
    u8 reservedFC0[4];
    struct GeorgeSlotPool *override_pool;
    u32 mode;
} GeorgeResourceAllocator;
typedef struct GeorgeResourceManagerPrefix {
    u8 reserved00[0x168];
    GeorgeResourceAllocator *allocator;
} GeorgeResourceManagerPrefix;

typedef char resource_base_size[(sizeof(GeorgeResourceBasePrefix)==0x24)?1:-1];
typedef char resource_base_callbacks[(offsetof(GeorgeResourceBasePrefix,callbacks)==0x18)?1:-1];
typedef char resource_callback_size[(sizeof(GeorgeResourceCallbackNode)==0x10)?1:-1];
typedef char resource_allocator_first[(offsetof(GeorgeResourceAllocator,first)==0xFA0)?1:-1];
typedef char resource_allocator_cursor[(offsetof(GeorgeResourceAllocator,cursor)==0xFBC)?1:-1];
typedef char resource_allocator_override[(offsetof(GeorgeResourceAllocator,override_pool)==0xFC4)?1:-1];
typedef char resource_allocator_mode[(offsetof(GeorgeResourceAllocator,mode)==0xFC8)?1:-1];
typedef char resource_manager_allocator[(offsetof(GeorgeResourceManagerPrefix,allocator)==0x168)?1:-1];

GeorgeResourceRecord *func_00225E80(const void *key,u32 kind) GEORGE_SAVE128;
void func_002267D0(GeorgeResourceRecord *,u32 mode) GEORGE_SAVE128;
GeorgeResourceRecord *func_00226BC0(GeorgeResourceRecord *,const void *key,
                                  u32 kind,u32 mode,u32 word) GEORGE_SAVE128;
void func_00226D78(GeorgeResourceRecord *) GEORGE_SAVE128;
u32 func_00226E38(GeorgeResourceRecord *,GeorgeResourceCallback,void *argument,
                 void *key) GEORGE_SAVE128;
u32 func_00226ED8(GeorgeResourceRecord *,void *key) GEORGE_SAVE128;
void func_00226F40(GeorgeResourceRecord *) GEORGE_SAVE128;
void func_00226FB0(GeorgeResourceRecord *) GEORGE_SAVE128;

#endif
