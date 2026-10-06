#include <stdio.h>
#include <string.h>
#include "george/pool_slots.h"

static unsigned checks, failures, allocations, releases, optional_releases;
static u32 requested_size;
static u8 arena[4096] __attribute__((aligned(16)));
static u16 snapshots[16][32];
static void *released[4];
static GeorgeSlotPool *callback_pool;
static int allocation_mode, release_mode;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)

void *func_002AEC28(u32 size)
{
    ++allocations;
    requested_size = size;
    if (allocation_mode == 1) {
        callback_pool->capacity = 3;
        callback_pool->stride = 8;
        return arena;
    }
    if (allocation_mode == 2) {
        callback_pool->free_count = 2;
        callback_pool->free_head = 1;
        return snapshots[allocations % 16];
    }
    if (size < 32)
        return snapshots[allocations % 16];
    return arena;
}

void func_002AEE40(void *memory)
{
    CHECK(releases < 4);
    if (releases < 4)
        released[releases] = memory;
    ++releases;
    if (release_mode == 1) {
        if (releases == 1) {
            CHECK(callback_pool->data == memory);
            callback_pool->snapshot = snapshots[9];
        } else {
            CHECK(callback_pool->data == 0);
            CHECK(memory == snapshots[9]);
            callback_pool->snapshot = snapshots[10];
        }
    }
}

void func_002AF100(void *memory)
{
    ++optional_releases;
    CHECK(memory == callback_pool);
    CHECK(callback_pool->data == 0);
    CHECK(callback_pool->snapshot == 0);
}

static u16 *header(GeorgeSlotPool *pool, unsigned index)
{
    return (u16 *)(pool->data + index * pool->stride);
}

static void reset_callbacks(void)
{
    allocations = releases = optional_releases = 0;
    allocation_mode = release_mode = 0;
    memset(released, 0, sizeof released);
}

int main(void)
{
    GeorgeSlotPool pool;
    u8 borrowed[256] __attribute__((aligned(16)));
    void *slots[8];
    int count, taken, index;
    reset_callbacks();
    memset(&pool, 0xA5, sizeof pool);
    CHECK(func_002AD460(&pool) == &pool);
    CHECK(pool.capacity == (s32)0xA5A5A5A5U && pool.stride == 0xA5A5A5A5U);
    CHECK(pool.free_count == (s32)0xA5A5A5A5U && pool.unknown12[0] == 0xA5);
    CHECK(pool.data == 0 && pool.snapshot == 0 && pool.free_head == 0xFFFF);
    CHECK(pool.owns_data == 1 && pool.frozen == 0);

    for (count = 1; count <= 8; ++count) {
        for (taken = 0; taken <= count; ++taken) {
            memset(borrowed, 0xCD, sizeof borrowed);
            CHECK(func_002AD4D0(&pool, borrowed, count, 8) == &pool);
            CHECK(pool.capacity == count && pool.stride == 8 && pool.owns_data == 0);
            CHECK(pool.free_count == count && pool.free_head == 0 && pool.frozen == 0);
            for (index = 0; index < count; ++index) {
                CHECK(*header(&pool, index) == (u16)(index + 1));
                CHECK(pool.data[index * 8 + 2] == 0xCD);
            }
            for (index = 0; index < taken; ++index) {
                slots[index] = func_002AD700(&pool);
                CHECK(slots[index] == header(&pool, index));
                CHECK(pool.free_count == count - index - 1);
            }
            for (index = 0; index < taken; ++index)
                func_002AD748(&pool, slots[index]);
            CHECK(pool.free_count == count);
            for (index = taken - 1; index >= 0; --index)
                CHECK(func_002AD700(&pool) == slots[index]);
            for (index = taken; index < count; ++index)
                CHECK(func_002AD700(&pool) == header(&pool, index));
            CHECK(pool.free_count == 0 && func_002AD700(&pool) == 0);
            func_002AD748(&pool, 0);
            CHECK(pool.free_count == 0);
        }
    }

    /* Borrowed backing storage may be overwritten while frozen. Only the
       count-1 live successors are saved; the last sentinel is not restored. */
    for (count = 0; count <= 8; ++count) {
        reset_callbacks();
        func_002AD4D0(&pool, borrowed, count, 8);
        func_002AD798(&pool);
        CHECK(pool.frozen == 1);
        CHECK(allocations == (unsigned)(count >= 2));
        if (count >= 2) {
            CHECK(requested_size == (u32)(count - 1) * 2U);
            for (index = 0; index < count - 1; ++index)
                CHECK(pool.snapshot[index] == (u16)(index + 1));
        }
        CHECK(func_002AD700(&pool) == 0);
        func_002AD748(&pool, borrowed);
        CHECK(pool.free_count == count);
        for (index = 0; index < count; ++index)
            *header(&pool, index) = 0xEEEE;
        func_002AD848(&pool);
        CHECK(pool.frozen == 0 && pool.snapshot == 0);
        CHECK(releases == (unsigned)(count >= 2));
        for (index = 0; index < count - 1; ++index)
            CHECK(*header(&pool, index) == (u16)(index + 1));
        if (count > 0)
            CHECK(*header(&pool, count - 1) == 0xEEEE);
        if (count >= 2) {
            for (index = 0; index < count; ++index)
                CHECK(func_002AD700(&pool) == header(&pool, index));
            CHECK(pool.free_count == 0 && func_002AD700(&pool) == 0);
        }
    }

    reset_callbacks();
    CHECK(func_002AD488(&pool, 4, 16) == &pool);
    CHECK(allocations == 1 && requested_size == 64 && pool.data == arena);
    pool.frozen = -1;
    func_002AD798(&pool);
    CHECK(allocations == 1 && pool.frozen == -1);
    pool.frozen = 0;
    func_002AD630(&pool);
    CHECK(releases == 1 && released[0] == arena && pool.data == 0);

    /* Allocation callbacks can replace counts and stride before initialization. */
    reset_callbacks();
    callback_pool = &pool;
    allocation_mode = 1;
    func_002AD488(&pool, 5, 16);
    CHECK(requested_size == 80 && pool.capacity == 3 && pool.stride == 8);
    CHECK(pool.free_count == 3 && pool.free_head == 0);
    for (index = 0; index < 3; ++index)
        CHECK(*header(&pool, index) == (u16)(index + 1));

    reset_callbacks();
    func_002AD4D0(&pool, borrowed, 4, 8);
    callback_pool = &pool;
    allocation_mode = 2;
    func_002AD798(&pool);
    CHECK(requested_size == 6 && pool.free_count == 2 && pool.free_head == 1);
    CHECK(pool.snapshot[0] == 2 && pool.frozen == 1);

    /* Release callbacks see the retail order and can replace the snapshot. */
    reset_callbacks();
    func_002AD488(&pool, 4, 16);
    pool.snapshot = snapshots[8];
    callback_pool = &pool;
    release_mode = 1;
    func_002AD688(&pool, 1);
    CHECK(releases == 2 && released[0] == arena && released[1] == snapshots[9]);
    CHECK(pool.snapshot == 0 && optional_releases == 1);
    reset_callbacks();
    func_002AD4D0(&pool, borrowed, 3, 8);
    func_002AD688(&pool, 2);
    CHECK(releases == 0 && optional_releases == 0 && pool.data == 0);

    /* Reset preserves the head, lock, and ownership. A negative nonzero count
       still permits pop: it is a zero test, not a positivity test. */
    func_002AD4D0(&pool, borrowed, 3, 8);
    pool.free_head = 2;
    pool.frozen = 7;
    func_002AD8E0(&pool);
    CHECK(pool.free_head == 2 && pool.frozen == 7 && pool.free_count == 3);
    CHECK(pool.owns_data == 0);
    pool.frozen = 0;
    pool.free_head = 0;
    pool.free_count = -1;
    CHECK(func_002AD700(&pool) == borrowed);
    CHECK(pool.free_count == -2 && pool.free_head == 1);

    /* Halfword index arithmetic is unsigned and truncates after division. */
    pool.data = borrowed;
    pool.stride = 3;
    pool.free_count = 0x7FFFFFFF;
    func_002AD748(&pool, borrowed + 9);
    CHECK(pool.free_head == 3 && pool.free_count == (s32)0x80000000U);

    pool.owns_data = 0; pool.capacity = 4; pool.stride = 8;
    pool.data = (u8 *)100;
    CHECK(func_002AD930(&pool, 100, 100) == 1);
    CHECK(func_002AD930(&pool, 132, 132) == 1);
    CHECK(func_002AD930(&pool, 90, 100) == 0);
    CHECK(func_002AD930(&pool, 132, 140) == 0);
    CHECK(func_002AD930(&pool, 90, 132) == 1);
    CHECK(func_002AD930(&pool, 90, 140) == 1);
    CHECK(func_002AD930(&pool, 133, 131) == 1);
    CHECK(func_002AD930(&pool, 99, 99) == 0);
    pool.owns_data = -1;
    CHECK(func_002AD930(&pool, 100, 101) == 0);
    pool.owns_data = 0; pool.data = (u8 *)0xFFFFFFF0U;
    CHECK(func_002AD930(&pool, 1, 17) == 1);
    CHECK(func_002AD930(&pool, 0xFFFFFFF0U, 17) == 0);
    printf("pool slot semantic checks: %u, failures: %u\n", checks, failures);
    return failures != 0;
}
