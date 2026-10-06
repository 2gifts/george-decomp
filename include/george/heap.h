#ifndef GEORGE_HEAP_H
#define GEORGE_HEAP_H

#include "george/types.h"
#include "george/compiler.h"

typedef struct GeorgeHeap GeorgeHeap;
typedef struct GeorgeHeapBlock GeorgeHeapBlock;
struct GeorgeHeapBlock {
    union {
        GeorgeHeapBlock *next;
        GeorgeHeap *owner;
    } field00;
    u32 size;
};

/* Observed 64-byte heap header. Allocated blocks use owner in word zero;
 * free blocks use next, in a circular list ordered by guest address. */
struct GeorgeHeap {
    char name[0x18];
    GeorgeHeap *next;
    u32 flags;
    GeorgeHeap *field20;
    u32 total;
    u32 used;
    u32 minimum_free;
    u32 alignment;
    GeorgeHeapBlock *cursor;
    GeorgeHeapBlock initial;
};
typedef char george_heap_size[(sizeof(GeorgeHeap) == 0x40) ? 1 : -1];
typedef char george_heap_cursor_offset[(offsetof(GeorgeHeap, cursor) == 0x34) ? 1 : -1];
typedef char george_heap_block_size[(sizeof(GeorgeHeapBlock) == 8) ? 1 : -1];

void *func_002ADF60(GeorgeHeap *heap, u32 size, u32 alignment) GEORGE_SAVE128;
void func_002AE158(void *memory);
GeorgeHeap *func_002AE6C0(void);
void func_002AE6E8(GeorgeHeap *heap);
void func_002AE6F8(GeorgeHeap *heap);
u32 func_002AE710(const GeorgeHeap *heap);
u32 func_002AE730(const GeorgeHeap *heap, s32 mode);
GeorgeHeap *func_002AE7B0(GeorgeHeap *heap, u32 size, const char *name) GEORGE_SAVE128;
void func_002AE990(void *memory);
void *func_002AEA30(GeorgeHeap *heap, u32 size);
void func_002AEAF0(GeorgeHeap *heap);
void *func_002AEB60(u32 size, u32 alignment) GEORGE_SAVE128;
void *func_002AEC28(u32 size) GEORGE_SAVE128;
void *func_002AECD0(u32 size, u32 alignment) GEORGE_SAVE128;
void *func_002AED98(u32 size) GEORGE_SAVE128;
void func_002AEE40(void *memory);
void *func_002AEE60(u32 size) GEORGE_SAVE128;
void *func_002AEF08(u32 size) GEORGE_SAVE128;
void *func_002AEFB0(u32 size) GEORGE_SAVE128;
void *func_002AF058(u32 size) GEORGE_SAVE128;
void func_002AF100(void *memory);
void func_002AF120(void *memory);
void *func_002AF140(u32 size) GEORGE_SAVE128;
void func_002AF1E8(void *memory);

#endif
