/* Separate production TU; callbacks observe contracts rather than emulate OS. */
#include <stdio.h>
#include <stdint.h>
#include "george/allocator_locks.h"
#include "allocator_locks_golden.h"

typedef char NativePointer32[sizeof(void *) == 4 ? 1 : -1];
typedef char NativeSigned32[sizeof(s32) == 4 ? 1 : -1];
typedef char NativeUnsigned32[sizeof(u32) == 4 ? 1 : -1];
s32 D_00400BC0, D_004059F8;
u32 D_004059FC;
static const struct AllocatorLockGolden *current;
static u32 events[10], event_words, checks, failures, case_index;

static void equal(u32 actual, u32 expected, const char *what)
{
    ++checks;
    if (actual != expected) {
        ++failures;
        if (failures == 1)
            printf("allocator lock fixture%u %s: %08X != %08X\n", case_index, what, actual, expected);
    }
}

static void observe(u32 kind, u32 argument)
{
    if (event_words + 5 > 10) {
        ++failures;
        return;
    }
    events[event_words++] = kind;
    events[event_words++] = argument;
    events[event_words++] = D_004059FC;
    events[event_words++] = (u32)D_004059F8;
    events[event_words++] = (u32)D_00400BC0;
}

s32 func_00363750(void)
{
    observe(0, 0);
    if (current->mutation & 1) {
        D_004059F8 = (s32)(current->thread ^ 1U);
        D_004059FC = 0x7FFFFFFFU;
        D_00400BC0 = (s32)0x80000003U;
    }
    if (current->mutation & 2) {
        D_004059F8 = (s32)current->thread;
        D_004059FC = 0xFFFFFFFFU;
        D_00400BC0 = (s32)0xFFFF0001U;
    }
    return (s32)current->thread;
}

s32 func_003638A0(s32 semaphore)
{
    observe(1, (u32)semaphore);
    if (current->mutation & 4) {
        D_004059FC = 0xFFFFFFFFU;
        D_004059F8 = (s32)0xDEADBEEFU;
        D_00400BC0 = 0x12345678;
    }
    return (s32)0xA5A5A5A5U;
}

s32 func_00363880(s32 semaphore)
{
    observe(2, (u32)semaphore);
    if (current->mutation & 8) {
        D_004059FC = 0x80000000U;
        D_004059F8 = 0x55667788;
        D_00400BC0 = (s32)0x87654321U;
    }
    return (s32)0xA5A5A5A5U;
}

int main(void)
{
    u32 i;
    for (case_index = 0; case_index < sizeof(allocator_lock_golden) / sizeof(allocator_lock_golden[0]); ++case_index) {
        current = &allocator_lock_golden[case_index];
        D_00400BC0 = (s32)current->semaphore;
        D_004059F8 = (s32)current->owner;
        D_004059FC = current->count;
        event_words = 0;
        for (i = 0; i < 10; ++i) events[i] = 0;
        if (current->routine == 0)
            __malloc_lock((struct _reent *)(uintptr_t)0x81234567U);
        else
            __malloc_unlock((struct _reent *)(uintptr_t)0x81234567U);
        equal((u32)D_00400BC0, current->expected_semaphore, "final semaphore");
        equal((u32)D_004059F8, current->expected_owner, "final owner");
        equal(D_004059FC, current->expected_count, "final nesting");
        equal(event_words, current->event_words, "callback count");
        for (i = 0; i < 10; ++i) equal(events[i], current->events[i], "ordered event");
    }
    if (failures) {
        printf("%u failures\n", failures);
        return 1;
    }
    printf("%u allocator lock checks passed\n", checks);
    return 0;
}
