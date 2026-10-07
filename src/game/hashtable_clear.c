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

/* Modified 2026-10-07: translate the complete licensed SGI clear traversal
 * and specialize default deallocation to the observed node12/free-class1.
 * The unchanged primary headers are pinned at b595ded606227e93b8c4a447446c1d2ac093827d.
 * Scalar pair destruction has no runtime operation; this C adaptation does
 * not assert C++ object lifetime semantics or original source identity.
 * Explicit captures preserve retail's free-head-before-next load and fresh
 * bucket range order. Integer subtraction models the measured wrapping32
 * pointer ABI without claiming unrelated/null pointer subtraction in ISO C.
 */
#include "george/completion_resolver.h"

extern void *D_003F21B8[];

void func_002BF418(GeorgeCompletionTable *table)
{
    u32 begin = (u32)table->field04.field00;
    u32 end = (u32)table->field04.field04;
    u32 index = 0;
    u32 count = (u32)((s32)(end - begin) >> 2);

    if (count != 0) {
        do {
            u32 next_index = index + 1U;
            u32 byte_offset = index << 2;
            GeorgeCompletionNode *node = (GeorgeCompletionNode *)
                *(void **)((u32)table->field04.field00 + byte_offset);

            if (node != 0) {
                GeorgeCompletionNode *free_head =
                    (GeorgeCompletionNode *)D_003F21B8[1];
                do {
                    GeorgeCompletionNode *next = node->field00;
                    node->field00 = free_head;
                    D_003F21B8[1] = node;
                    node = next;
                    if (node != 0)
                        free_head = (GeorgeCompletionNode *)D_003F21B8[1];
                } while (node != 0);
            }
            *(void **)((u32)table->field04.field00 + byte_offset) = 0;
            index = next_index;
            end = (u32)table->field04.field04;
            begin = (u32)table->field04.field00;
            count = (u32)((s32)(end - begin) >> 2);
        } while (index < count);
    }
    table->field10 = 0;
}
