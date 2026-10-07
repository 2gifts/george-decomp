#ifndef GEORGE_ALLOCATOR_LOCKS_H
#define GEORGE_ALLOCATOR_LOCKS_H

#include "george/types.h"

struct _reent;
extern s32 D_00400BC0;
extern s32 D_004059F8;
extern u32 D_004059FC;

/* Observed conventional syscall lanes; kernel implementations are unrecovered. */
s32 func_00363750(void);
s32 func_003638A0(s32 semaphore);
s32 func_00363880(s32 semaphore);

void __malloc_lock(struct _reent *context);
void __malloc_unlock(struct _reent *context);

#endif
