#include "george/ring.h"

extern void *func_003934F8(void *destination, const void *source, u32 size);

void *func_002AF208(const GeorgeDeimosRing *ring, u32 index)
{
    u8 *element;
    if (ring->count == 0) {
        return 0;
    }
    element = ring->read;
    while (index != 0) {
        element = (u8 *)((u32)element + ring->element_size);
        if ((u32)element >= (u32)ring->end) {
            element = ring->begin;
        }
        --index;
    }
    return element;
}

/* Shift following entries toward the selected slot. The stop pointer is
 * captured before copies; size/end/begin and final state reload after copies. */
void func_002AF258(GeorgeDeimosRing *ring, u32 index)
{
    u8 *element;
    u8 *begin;
    u8 *last;
    u32 size;
    if (ring->count == 0) {
        return;
    }
    element = ring->read;
    size = ring->element_size;
    begin = ring->begin;
    last = ring->write;
    if (index != 0) {
        u8 *end = ring->end;
        while (index != 0) {
            u8 *next = (u8 *)((u32)element + size);
            element = (u32)next < (u32)end ? next : begin;
            --index;
        }
        size = ring->element_size;
    }
    last = (u8 *)((u32)last - size);
    if ((u32)last < (u32)begin) {
        last = (u8 *)((u32)ring->end - size);
    }
    while (element != last) {
        u8 *next;
        size = ring->element_size;
        next = (u8 *)((u32)element + size);
        if ((u32)next >= (u32)ring->end) {
            next = ring->begin;
        }
        func_003934F8(element, next, size);
        element = next;
    }
    {
        u32 count;
        u8 *write;
        size = ring->element_size;
        write = (u8 *)((u32)ring->write - size);
        if ((u32)write < (u32)ring->begin) {
            write = (u8 *)((u32)ring->end - size);
        }
        count = ring->count;
        ring->write = write;
        ring->count = count - 1;
    }
}

u32 func_002AF358(const GeorgeDeimosRing *ring, void *output)
{
    if (ring->count == 0) {
        return 0;
    }
    func_003934F8(output, ring->read, ring->element_size);
    return 1;
}

u32 func_002AF398(const GeorgeDeimosRing *ring, void *output)
{
    u32 size;
    u8 *last;
    if (ring->count == 0) {
        return 0;
    }
    size = ring->element_size;
    last = (u8 *)((u32)ring->write - size);
    if ((u32)last < (u32)ring->begin) {
        last = (u8 *)((u32)ring->end - size);
    }
    func_003934F8(output, last, size);
    return 1;
}
