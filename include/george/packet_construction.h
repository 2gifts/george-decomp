#ifndef GEORGE_PACKET_CONSTRUCTION_H
#define GEORGE_PACKET_CONSTRUCTION_H

#include "george/types.h"
#include "george/compiler.h"

/* Only the observed three-pointer prefix is named. No class, protocol or
 * network implementation identity follows from this storage view. */
typedef struct GeorgePacketRange {
    u8 *begin;
    u8 *end;
    u8 *capacity;
} GeorgePacketRange;

typedef char george_packet_range_size[(sizeof(GeorgePacketRange) == 12) ? 1 : -1];
typedef char george_packet_range_end[(offsetof(GeorgePacketRange, end) == 4) ? 1 : -1];
typedef char george_packet_range_capacity[(offsetof(GeorgePacketRange, capacity) == 8) ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif

GeorgePacketRange **func_002BCB50(GeorgePacketRange **holder, u32 first_tag,
    u32 second_tag, const void *payload, u32 length) GEORGE_SAVE128;
GeorgePacketRange **func_002BCCA0(GeorgePacketRange **holder,
    const signed char *first, const signed char *second, u32 header) GEORGE_SAVE128;
u32 func_002BCFA8(void *destination, u32 capacity, u32 first_tag,
    u32 second_tag, const void *payload, u32 length) GEORGE_SAVE128;
u32 func_002BD000(void *destination, u32 capacity, const signed char *first,
    const signed char *second, u32 header) GEORGE_SAVE128;

/* Numeric C caller ABI for the actual three-lane byte-reference fill.
 * Its selected definition is the genuine unsigned-count SGI specialization;
 * this declaration supplies no forwarding body or invented mangled binding. */
u8 *func_002BD1F0(u8 *destination, u32 count, const u8 *value);

#ifdef __cplusplus
}
#endif

#endif
