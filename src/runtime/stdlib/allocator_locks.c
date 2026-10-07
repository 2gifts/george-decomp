/* Independently reconstructed original behavior; no SDK implementation copied. */
#include "george/allocator_locks.h"

void __malloc_lock(struct _reent *context)
{
    s32 thread;
    (void)context;
    thread = func_00363750();
    if (D_004059F8 != thread) {
        (void)func_003638A0(D_00400BC0);
        D_004059F8 = thread;
    }
    ++D_004059FC;
}

void __malloc_unlock(struct _reent *context)
{
    u32 count;
    (void)context;
    count = D_004059FC - 1U;
    D_004059FC = count;
    if (count == 0) {
        s32 semaphore = D_00400BC0;
        D_004059F8 = -1;
        (void)func_00363880(semaphore);
    }
}
