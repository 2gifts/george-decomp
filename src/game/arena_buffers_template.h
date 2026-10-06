#ifndef GEORGE_ARENA_BUFFERS_TEMPLATE_H
#define GEORGE_ARENA_BUFFERS_TEMPLATE_H

/* Common observed inline reservation algorithm. Capture scalar arguments,
 * retain fresh descriptor loads and publish cursor before caller outputs.
 * Address/size operations deliberately use original low32 wrap semantics. */
#define GEORGE_ARENA_ALIGN(arena_,exponent_) do { \
    GeorgeArenaBuffer *arena_descriptor=(arena_); \
    u32 arena_alignment=1U<<((exponent_)&31U); \
    u32 arena_cursor=(u32)arena_descriptor->cursor; \
    u32 arena_remainder=arena_cursor&(arena_alignment-1U); \
    if (arena_remainder!=0) \
        arena_descriptor->cursor=(u8 *)(arena_cursor+arena_alignment-arena_remainder); \
} while (0)

#define GEORGE_ARENA_RESERVE(arena_,bytes_,exponent_,result_) do { \
    GeorgeArenaBuffer *reserve_descriptor=(arena_); \
    u32 reserve_bytes=(bytes_); \
    u32 reserve_exponent=(exponent_); \
    u32 reserve_alignment=1U<<(reserve_exponent&31U); \
    u32 reserve_mask=reserve_alignment-1U; \
    u32 reserve_cursor,reserve_base,reserve_remainder; \
    GEORGE_ARENA_ALIGN(reserve_descriptor,reserve_exponent); \
    reserve_cursor=(u32)reserve_descriptor->cursor; \
    if (reserve_cursor+reserve_bytes>=(u32)reserve_descriptor->base+reserve_descriptor->capacity) { \
        if ((u32)reserve_descriptor->high_water<reserve_cursor) \
            reserve_descriptor->high_water=(u8 *)reserve_cursor; \
        reserve_base=(u32)reserve_descriptor->base; \
        reserve_descriptor->cursor=(u8 *)reserve_base; \
        reserve_remainder=reserve_base&reserve_mask; \
        if (reserve_remainder!=0) \
            reserve_descriptor->cursor=(u8 *)(reserve_base+reserve_alignment-reserve_remainder); \
    } \
    reserve_cursor=(u32)reserve_descriptor->cursor; \
    reserve_descriptor->cursor=(u8 *)(reserve_cursor+reserve_bytes); \
    (result_)=reserve_cursor; \
} while (0)

#endif
