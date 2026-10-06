#include "george/heap.h"
#include "george/function_templates.h"

extern u32 D_003FD200;
extern GeorgeHeap *D_003FD204;
extern GeorgeHeap *D_003FD208;
extern GeorgeHeap *D_003FD20C;
extern GeorgeHeap *D_003FD210;
extern GeorgeHeap *D_003FD218;
extern GeorgeHeap *D_003FD21C;
extern GeorgeHeap *D_003FD220;
extern GeorgeHeap *D_003FD228;
extern GeorgeHeap *D_003FD230;
extern GeorgeHeap *D_00469B80[];
extern GeorgeHeap D_004D84B0;
extern const char D_004471F8[];
extern const char D_00447208[];
extern const char D_00447238[];
extern s32 func_00394F68(const char *format, ...);
extern char *func_00394010(char *destination, const char *source, u32 size);

void *func_002ADF60(GeorgeHeap *heap, u32 size, u32 alignment)
{
    GeorgeHeapBlock *head, *previous, *block, *best = 0;
    GeorgeHeapBlock *allocated = 0;
    u32 consumed = 0;
    u32 mask = 0xFFFFFFFFU << (alignment & 31);
    void *result;

    if (D_003FD200 == 0) {
        if (size < 5) {
            size = 4;
            heap = D_003FD20C;
        } else if (size < 9) {
            size = 8;
            heap = D_003FD210;
        } else if (size < 17) {
            size = 16;
            if (alignment < 4) heap = D_003FD218;
            else if (alignment < 5) heap = D_003FD21C;
            else heap = D_003FD220;
        } else if (size < 25 && (u32)(alignment - 2) < 2) {
            size = 24;
            heap = D_003FD228;
        } else if (size < 33) {
            size = 32;
            heap = D_003FD230;
        }
    }

    head = heap->cursor;
    previous = head;
    do {
        u32 end, address, span;
        block = previous->field00.next;
        end = (u32)block + block->size + 8;
        address = ((end - size) & mask) - 8;
        span = end - address;
        if (span <= block->size && (best == 0 || block->size < best->size)) {
            allocated = (GeorgeHeapBlock *)address;
            consumed = span;
            best = block;
        }
        if (block == head) break;
        previous = block;
    } while (1);

    if (best == 0) {
        func_00394F68(D_00447238, heap, size, alignment);
        return 0;
    }
    result = (void *)((u32)allocated + 8);
    best->size -= consumed;
    allocated->size = consumed - 8;
    allocated->field00.owner = heap;
    heap->cursor = previous;
    heap->used += consumed;
    if (heap->total < heap->used) {
        func_00394F68(D_00447208, heap, heap->total - heap->used);
    }
    {
        u32 remaining = heap->total - 0x40 - heap->used;
        if (heap->minimum_free >= remaining) heap->minimum_free = remaining;
    }
    return result;
}

void func_002AE158(void *memory)
{
    GeorgeHeapBlock *block = (GeorgeHeapBlock *)((u32)memory - 8);
    GeorgeHeap *heap = block->field00.owner;
    GeorgeHeapBlock *previous = heap->cursor;
    GeorgeHeapBlock *next;
    u32 span;

    for (;;) {
        next = previous->field00.next;
        if ((u32)previous < (u32)block && (u32)block < (u32)next) break;
        if ((u32)previous >= (u32)next
                && ((u32)previous < (u32)block || (u32)block < (u32)next)) break;
        previous = previous->field00.next;
    }
    heap->used = heap->used - 8 - block->size;
    span = block->size + 8;
    next = previous->field00.next;
    if ((u32)block + span == (u32)next) {
        block->size = span + next->size;
        block->field00.next = previous->field00.next->field00.next;
    } else {
        block->field00.next = next;
    }
    span = previous->size + 8;
    if ((u32)previous + span == (u32)block) {
        previous->size = span + block->size;
        previous->field00.next = block->field00.next;
    } else {
        previous->field00.next = block;
    }
    heap->cursor = previous;
}

GeorgeHeap *func_002AE6C0(void) { return &D_004D84B0; }
void func_002AE6E8(GeorgeHeap *heap) { heap->flags |= 1; }
void func_002AE6F8(GeorgeHeap *heap) { heap->flags &= 0xFFFFFFFE; }
GEORGE_DEFINE_FIELD1C_BIT0_GETTER(func_002AE710)

u32 func_002AE730(const GeorgeHeap *heap, s32 mode)
{
    if (mode > 0 && mode < 3) {
        GeorgeHeapBlock *head = heap->cursor;
        GeorgeHeapBlock *block = head->field00.next;
        u32 smallest = heap->total, largest = 0;
        for (;;) {
            u32 size = block->size;
            if (size > largest) largest = size;
            if (size < smallest) smallest = size;
            if (block == head) break;
            block = block->field00.next;
        }
        return mode == 2 ? smallest - 8 : largest - 8;
    }
    return heap->total - heap->used;
}

GeorgeHeap *func_002AE7B0(GeorgeHeap *heap, u32 size, const char *name)
{
    u32 available = size - 0x40;
    heap->alignment = 3;
    heap->initial.field00.next = &heap->initial;
    heap->minimum_free = available;
    heap->next = 0;
    heap->flags = 0;
    heap->field20 = heap;
    heap->total = size;
    heap->used = 0;
    heap->cursor = &heap->initial;
    heap->initial.size = available;
    func_00394010(heap->name, name ? name : D_004471F8, 0x18);
    heap->name[0x17] = 0;
    return heap;
}

void *func_002AEA30(GeorgeHeap *heap, u32 size)
{
    return func_002ADF60(heap, size, heap->alignment);
}

void func_002AEAF0(GeorgeHeap *heap)
{
    if (heap != 0) {
        u32 depth = D_003FD200 + 1;
        D_003FD204 = heap;
        D_003FD200 = depth;
        D_00469B80[depth] = heap;
    } else if (D_003FD200 != 0) {
        u32 depth = D_003FD200 - 1;
        GeorgeHeap *saved = D_00469B80[depth];
        D_003FD200 = depth;
        D_003FD204 = saved;
    }
}

/* Keep the original next-heap load after the allocation call even on success.
 * Diagnostics receive the final cursor, which can be the following heap. */
#define DEFINE_ALLOCATION(name, declaration, first_heap, overhead, alignment) \
void *name declaration \
{ \
    GeorgeHeap *heap = first_heap; \
    void *result = 0; \
    u32 needed = overhead; \
    while (heap != 0 && result == 0) { \
        if (size < heap->total - heap->used - needed) \
            result = func_002ADF60(heap, size, alignment); \
        heap = heap->next; \
    } \
    if (result == 0) func_00394F68(D_00447238, heap, size, alignment); \
    return result; \
}

DEFINE_ALLOCATION(func_002AEB60, (u32 size, u32 align), D_003FD204, (2U << (align & 31)) + 8, align)
DEFINE_ALLOCATION(func_002AEC28, (u32 size), D_003FD204, 0x18, 3)
DEFINE_ALLOCATION(func_002AECD0, (u32 size, u32 align), D_003FD208, (2U << (align & 31)) + 8, align)
DEFINE_ALLOCATION(func_002AED98, (u32 size), D_003FD208, 0x18, 3)
DEFINE_ALLOCATION(func_002AEE60, (u32 size), D_003FD204, 0x28, 4)
DEFINE_ALLOCATION(func_002AEF08, (u32 size), D_003FD204, 0x28, 4)
DEFINE_ALLOCATION(func_002AEFB0, (u32 size), D_003FD204, 0x28, 4)
DEFINE_ALLOCATION(func_002AF058, (u32 size), D_003FD204, 0x28, 4)
DEFINE_ALLOCATION(func_002AF140, (u32 size), D_003FD204, 0x18, 3)
#undef DEFINE_ALLOCATION

GEORGE_DEFINE_FREE_FORWARD(func_002AE990, func_002AEE40)
GEORGE_DEFINE_FREE_FORWARD(func_002AEE40, func_002AE158)
void func_002AF100(void *memory) { if (memory != 0) func_002AE158(memory); }
void func_002AF120(void *memory) { if (memory != 0) func_002AE158(memory); }
GEORGE_DEFINE_FREE_FORWARD(func_002AF1E8, func_002AE158)
