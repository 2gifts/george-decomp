#include "george/cache_transfer.h"

extern void *func_003934F8(void *destination, const void *source, u32 size);

void func_002B2F40(const void *source, u32 size)
{
    u32 previous = (D_003FD250 + 9u) % 10u;
    u32 slot, stored_offset, begin, end;

    /* Search only the backward live run. Retail has no full-cycle guard. */
    while (D_00469E00[previous].source != NULL) {
        if (D_00469E00[previous].source == source) return;
        previous = (previous + 9u) % 10u;
    }

    func_002B3150(D_003FD24C, source, size);
    /* The copy call may change globals. Reload both cursor words afterward. */
    slot = D_003FD250;
    stored_offset = D_003FD24C;
    previous = (slot + 9u) % 10u;
    D_00469E00[slot].source = source;
    D_00469E00[slot].offset = stored_offset;
    D_00469E00[slot].size = size;
    begin = D_00469E00[slot].offset;
    end = begin + size;

    while (D_00469E00[previous].source != NULL) {
        u32 old_begin = D_00469E00[previous].offset;
        u32 old_size = D_00469E00[previous].size;
        /* Unsigned wrapped endpoints; equality is not overlap. */
        if (old_begin < end && begin < old_begin + old_size)
            D_00469E00[previous].source = NULL;
        previous = (previous + 9u) % 10u;
    }

    slot = (D_003FD250 + 1u) % 10u;
    D_003FD24C = (D_003FD24C + ((size + 63u) & ~63u)) & 0xFFFu;
    D_003FD250 = slot;
}

u32 func_002B3150(u32 offset, const void *source, u32 size)
{
    /* Preserve the observed numeric address and original copy entry. */
    func_003934F8((void *)(offset + 0x11000000u), source, size);
    return offset + size;
}
