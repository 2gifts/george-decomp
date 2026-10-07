/*
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* Modified 2026-10-07: reuse the complete reviewed licensed clear traversal
 * from src/game/hashtable_clear.c. Only the numeric entry name and consumed
 * prefix type names change. This carrier leaves each unknown node tail and
 * C++ class/allocation size unmodeled. Free-list slot1 alone identifies a
 * rounded8-byte class for requested9..16, not an original12-byte node.
 */
#define GEORGE_CLEAR_PREFIX_FUNCTION(function_name) \
void function_name(GeorgeClearTablePrefix *table) \
{ \
    u32 begin = (u32)table->field04.field00; \
    u32 end = (u32)table->field04.field04; \
    u32 index = 0; \
    u32 count = (u32)((s32)(end - begin) >> 2); \
 \
    if (count != 0) { \
        do { \
            u32 next_index = index + 1U; \
            u32 byte_offset = index << 2; \
            GeorgeClearNodePrefix *node = (GeorgeClearNodePrefix *) \
                *(void **)((u32)table->field04.field00 + byte_offset); \
 \
            if (node != 0) { \
                GeorgeClearNodePrefix *free_head = \
                    (GeorgeClearNodePrefix *)D_003F21B8[1]; \
                do { \
                    GeorgeClearNodePrefix *next = node->field00; \
                    node->field00 = free_head; \
                    D_003F21B8[1] = node; \
                    node = next; \
                    if (node != 0) \
                        free_head = (GeorgeClearNodePrefix *)D_003F21B8[1]; \
                } while (node != 0); \
            } \
            *(void **)((u32)table->field04.field00 + byte_offset) = 0; \
            index = next_index; \
            end = (u32)table->field04.field04; \
            begin = (u32)table->field04.field00; \
            count = (u32)((s32)(end - begin) >> 2); \
        } while (index < count); \
    } \
    table->field10 = 0; \
}
