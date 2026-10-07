#include "george/packet_construction.h"
#include "george/heap.h"
#include "george/string_algorithms.h"

/* Numeric original helper interfaces remain supporting-only. */
extern void *D_003F21B8[];
extern void *func_00252168(u32 bytes);
extern void *func_002521F8(u32 rounded_bytes);
extern void *func_003934F8(void *destination, const void *source, u32 bytes);

/* Fixed byte-element allocation expression adapted from the unchanged SGI
 * default allocator and the already-reviewed numeric constructor graph.
 * It is local to this batch; published source/header inputs stay unchanged.
 *
 * Copyright (c) 1996-1997 Silicon Graphics Computer Systems, Inc.
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.
 * Silicon Graphics makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or implied
 * warranty.
 *
 * Changes: numeric C interfaces, unsigned low32 arithmetic, and the observed
 * non-threaded free-list path; no helper entry or general allocator award.
 */
#define PACKET_ALLOCATE_BYTES(result, bytes) do { \
    if ((bytes) == 0) { \
        (result) = 0; \
    } else if ((bytes) >= 129U) { \
        (result) = (u8 *)func_002AF140(bytes); \
        if ((result) == 0) \
            (result) = (u8 *)func_00252168(bytes); \
    } else { \
        u32 packet_rounded = (bytes) + 7U; \
        u32 packet_index = (packet_rounded >> 3) - 1U; \
        (result) = (u8 *)D_003F21B8[packet_index]; \
        if ((result) == 0) \
            (result) = (u8 *)func_002521F8(packet_rounded & 0xFFFFFFF8U); \
        else \
            D_003F21B8[packet_index] = *(void **)(result); \
    } \
} while (0)

GeorgePacketRange **func_002BCB50(GeorgePacketRange **holder, u32 first_tag,
    u32 second_tag, const void *payload, u32 length)
{
    u32 bytes = length + 8U;
    GeorgePacketRange *range = (GeorgePacketRange *)func_002AEE60(12);
    u8 *allocated;
    u8 zero = 0;
    u8 *end;

    range->begin = 0;
    range->end = 0;
    range->capacity = 0;
    PACKET_ALLOCATE_BYTES(allocated, bytes);
    range->begin = allocated;
    range->capacity = (u8 *)((u32)allocated + bytes);
    range->end = allocated;
    end = func_002BD1F0(allocated, bytes, &zero);
    range->end = end;
    if (range != 0) {
        u8 *begin = range->begin;
        func_002BCFA8(begin, (u32)end - (u32)begin,
                      first_tag, second_tag, payload, length);
    }
    *holder = range;
    return holder;
}

GeorgePacketRange **func_002BCCA0(GeorgePacketRange **holder,
    const signed char *first, const signed char *second, u32 header)
{
    u32 second_bytes = func_00295050(second) + 1U;
    u32 first_bytes = func_00295050(first) + 1U;
    GeorgePacketRange *range = (GeorgePacketRange *)func_002AEE60(12);
    u32 bytes = first_bytes + second_bytes + 4U;
    u8 *allocated;
    u8 zero = 0;
    u8 *end;

    range->begin = 0;
    range->end = 0;
    range->capacity = 0;
    PACKET_ALLOCATE_BYTES(allocated, bytes);
    range->begin = allocated;
    range->capacity = (u8 *)((u32)allocated + bytes);
    range->end = allocated;
    end = func_002BD1F0(allocated, bytes, &zero);
    range->end = end;
    if (range != 0) {
        u8 *begin = range->begin;
        /* These actual nested calls recompute second/first lengths after the
         * allocator and fill, independently of the constructor's captures. */
        func_002BD000(begin, (u32)end - (u32)begin, first, second, header);
    }
    *holder = range;
    return holder;
}

u32 func_002BCFA8(void *destination, u32 capacity, u32 first_tag,
    u32 second_tag, const void *payload, u32 length)
{
    u32 bytes = length + 8U;
    u8 *copy;
    if (capacity < bytes)
        return 0;
    copy = (u8 *)((u32)destination + 8U);
    ((u8 *)destination)[0] = (u8)first_tag;
    if (length == 0)
        copy = 0;
    ((u8 *)destination)[1] = (u8)second_tag;
    *(u32 *)((u32)destination + 4U) = length;
    if (copy != 0)
        func_003934F8(copy, payload, length);
    return bytes;
}

u32 func_002BD000(void *destination, u32 capacity, const signed char *first,
    const signed char *second, u32 header)
{
    u32 second_bytes = func_00295050(second) + 1U;
    u32 first_bytes = func_00295050(first) + 1U;
    u32 bytes = first_bytes + second_bytes + 4U;
    u8 *copy;
    if (capacity < bytes)
        return 0;
    copy = (u8 *)((u32)destination + 4U);
    *(u32 *)destination = header;
    func_003934F8(copy, second, second_bytes);
    func_003934F8((u8 *)((u32)copy + second_bytes), first, first_bytes);
    return bytes;
}

#undef PACKET_ALLOCATE_BYTES
