/* Separate-TU caller observations, not an allocator or OS implementation. */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <reent.h>

#include "allocator_wrappers_golden.h"

typedef char NativePointerMustBe32[sizeof(void *) == 4 ? 1 : -1];
typedef char NativeSizeMustBe32[sizeof(size_t) == 4 ? 1 : -1];

extern void *george_memalign(size_t, size_t);
extern void *george_realloc(void *, size_t);
struct _reent *_impure_ptr;
static unsigned int observed[15], event_count, stage, checks, case_index;
static const struct AllocatorWrapperGolden *current;

static void check(unsigned int actual, unsigned int expected, const char *what)
{
    ++checks;
    if (actual != expected) {
        printf("allocator fixture%u %s: %08X != %08X\n", case_index, what, actual, expected);
        /* No production allocator is called by this failure path. */
        fflush(stdout);
    }
}

static unsigned int failures;
static void equal(unsigned int actual, unsigned int expected, const char *what)
{
    if (actual != expected) ++failures;
    check(actual, expected, what);
}

static void observe(unsigned int expected_stage, struct _reent *reent,
                    unsigned int a1, unsigned int a2)
{
    equal(stage, expected_stage, "call order");
    equal((unsigned int)(uintptr_t)reent, (unsigned int)(uintptr_t)_impure_ptr, "fresh global");
    if (event_count + 5 > 15) {
        ++failures;
        return;
    }
    observed[event_count++] = stage;
    observed[event_count++] = (unsigned int)(uintptr_t)reent;
    observed[event_count++] = a1;
    observed[event_count++] = a2;
    observed[event_count++] = (unsigned int)(uintptr_t)_impure_ptr;
    if (current->mutation & (1u << stage))
        _impure_ptr = (struct _reent *)(uintptr_t)(0x20000u + (stage + 1u) * 4u);
    ++stage;
}

void __malloc_lock(struct _reent *reent)
{
    observe(0, reent, 0, 0);
}

void __malloc_unlock(struct _reent *reent)
{
    observe(2, reent, 0, 0);
}

void *_memalign_r(struct _reent *reent, size_t align, size_t nbytes)
{
    equal(current->routine, 0, "memalign core");
    observe(1, reent, (unsigned int)align, (unsigned int)nbytes);
    return (void *)(uintptr_t)current->result;
}

void *_realloc_r(struct _reent *reent, void *pointer, size_t nbytes)
{
    equal(current->routine, 1, "realloc core");
    observe(1, reent, (unsigned int)(uintptr_t)pointer, (unsigned int)nbytes);
    return (void *)(uintptr_t)current->result;
}

int main(void)
{
    unsigned int i;
    for (case_index = 0; case_index < sizeof(allocator_wrapper_golden) / sizeof(allocator_wrapper_golden[0]); ++case_index) {
        void *result;
        current = &allocator_wrapper_golden[case_index];
        stage = event_count = 0;
        _impure_ptr = (struct _reent *)(uintptr_t)0x20000u;
        if (current->routine == 0)
            result = george_memalign((size_t)current->arg1, (size_t)current->nbytes);
        else
            result = george_realloc((void *)(uintptr_t)current->arg1, (size_t)current->nbytes);
        equal((unsigned int)(uintptr_t)result, current->expected_return, "retained result");
        equal((unsigned int)(uintptr_t)_impure_ptr, current->expected_reent, "final global");
        equal(stage, 3, "three calls");
        equal(event_count, 15, "complete events");
        for (i = 0; i < 15; ++i) equal(observed[i], current->events[i], "event");
    }
    if (failures) return 1;
    printf("allocator wrappers: %u checks passed across %u original-traced fixtures\n", checks, case_index);
    return 0;
}
