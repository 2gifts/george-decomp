#ifndef GEORGE_TREE_UPDATES_H
#define GEORGE_TREE_UPDATES_H

#include "george/tree.h"

/* Only the physical Count-register read needs target-specific code. The pinned
 * EE compiler backend supplies no mfc0 builtin. Volatile and the memory
 * clobber keep the two reads on their observed sides of the callback/stores.
 * Native substitution tests the algorithm, not real hardware timing. */
static __inline__ u32 george_tree_read_count(void)
{
#ifdef GEORGE_TREE_NATIVE_COUNTER
    extern u32 george_tree_test_counter(void);
    return george_tree_test_counter();
#else
#if !defined(__mips__) && !defined(__mips) && !defined(R5900) && !defined(_R5900) && !defined(__R5900__)
#error tree_updates requires an EE target or explicit native counter substitution
#endif
    u32 value;
    __asm__ __volatile__("mfc0 %0, $9" : "=r" (value) : : "memory");
    return value;
#endif
}

void func_002AC820(GeorgeTreeNode *node, float elapsed) GEORGE_SAVE128;
GeorgeTreeNode *func_002AC9B8(GeorgeTreeCallback callback, u32 data_size) GEORGE_SAVE128;
GeorgeList *func_002ACB10(void);
void func_002ACB38(void) GEORGE_SAVE128;
void func_002ACBA8(void) GEORGE_SAVE128;
void func_002ACBF0(float elapsed) GEORGE_SAVE128;
void func_002ACC50(float elapsed) GEORGE_SAVE128;
void func_002ACD00(float elapsed) GEORGE_SAVE128;

#endif
