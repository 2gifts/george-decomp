#include "george/completion_resolver.h"
#include "george/heap.h"

extern void *D_003F2C44;
extern u32 D_003F2C9C;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_004208A0[];
extern const u8 D_004200F8[];
extern const char D_00447F48[];
extern const GeorgeCompletionPrime D_00447F78[];
extern void *D_003F21B8[];

/* Existing numeric helper contracts; no additional recovery is awarded. */
extern const GeorgeCompletionPrime *func_00252110(
    const GeorgeCompletionPrime *begin, const GeorgeCompletionPrime *end,
    const GeorgeCompletionPrime *key);
extern void *func_00252168(u32 bytes);
extern void *func_002521F8(u32 rounded_bytes);
extern void *func_003935A4(void *destination, const void *source, u32 bytes);
extern s32 func_003952C8(char *destination, const char *format, ...);

/* Transparent declaration-only inlining for the two locally shared C
 * expressions. GCC 3 otherwise emits invented out-of-line helper symbols;
 * this does not establish the original source's syntax or compiler profile. */
#if defined(__GNUC__) && __GNUC__ >= 3
#define GEORGE_COMPLETION_INLINE static __inline__ __attribute__((always_inline))
#else
#define GEORGE_COMPLETION_INLINE static __inline__
#endif

/* Observed pointer subtraction is a wrapping32 subtraction followed by SRA.
 * This retains the target arithmetic without claiming host null subtraction
 * or unrelated pointer subtraction is defined ordinary C pointer arithmetic. */
GEORGE_COMPLETION_INLINE s32 range_count(const void *end, const void *begin)
{
    return (s32)((u32)end - (u32)begin) >> 2;
}

/* Shared numeric deallocation expression. Each argument is captured once
 * before any store or call. The macro avoids an invented out-of-line helper
 * on GCC 2.9; this is not a claim about original source syntax. Genuine SGI
 * allocator definitions used by the C++ methods remain unchanged. */
#define GEORGE_COMPLETION_RELEASE_RANGE(begin_arg, capacity_arg) do { \
    void **completion_begin = (begin_arg); \
    void **completion_capacity = (capacity_arg); \
    u32 completion_count = (u32)range_count(completion_capacity, completion_begin); \
    if (completion_count != 0) { \
        u32 completion_bytes = completion_count << 2; \
        if (completion_bytes >= 129U) { \
            func_002AF1E8(completion_begin); \
        } else { \
            u32 completion_index = ((completion_bytes + 7U) >> 3) - 1U; \
            *completion_begin = D_003F21B8[completion_index]; \
            D_003F21B8[completion_index] = completion_begin; \
        } \
    } \
} while (0)

void *func_002BF550(void *object)
{
    GeorgeCompletionResolver *resolver = (GeorgeCompletionResolver *)object;
    func_002BF748(&resolver->field04);
    resolver->field18 = 0;
    return object;
}

signed char *func_002BF580(void *object, u32 key)
{
    GeorgeCompletionResolver *resolver = (GeorgeCompletionResolver *)object;
    signed char *destination = (signed char *)((u32)object + 0x1CU +
                                               (resolver->field18 << 6));
    func_003952C8((char *)destination, D_00447F48, key);
    resolver->field18 = (resolver->field18 + 1U) & 15U;
    return destination;
}

/* Numeric constructor reconstruction. Unknown empty-functor byte identity
 * prevents claiming the entire original constructor is an unchanged SGI
 * instantiation. The actual prior-byte read stays explicit; native tests must
 * supply initialized input bytes. No zero is invented for that read. */
GeorgeCompletionTable *func_002BF748(GeorgeCompletionTable *table)
{
    GeorgeCompletionPrime request = 100;
    const GeorgeCompletionPrime *selected;
    GeorgeActorPointerRange *range = &table->field04;
    u8 prior;
    u32 count;
    void *zero = 0;

    table->unknown00[2] = 0;
    prior = ((const volatile u8 *)table)[1];
    (void)prior;
    table->unknown00[3] = 0;
    range->field00 = 0;
    range->field08 = 0;
    range->field04 = 0;
    table->field10 = 0;
    selected = func_00252110(D_00447F78, D_00447F78 + 28, &request);
    count = (u32)(selected == D_00447F78 + 28 ? D_00447F78[27] : *selected);
    {
        void **old_begin = range->field00;
        if ((u32)range_count(range->field08, old_begin) < count) {
            void **old_end = range->field04;
            u32 old_count = (u32)range_count(old_end, old_begin);
            u32 bytes = count << 2;
            void **allocated;
            if (count == 0) {
                allocated = 0;
                bytes = 0;
            } else if (bytes >= 129U) {
                allocated = (void **)func_002AF140(bytes);
                if (allocated == 0)
                    allocated = (void **)func_00252168(bytes);
            } else {
                u32 rounded = bytes + 7U;
                u32 index = (rounded >> 3) - 1U;
                allocated = (void **)D_003F21B8[index];
                if (allocated == 0)
                    allocated = (void **)func_002521F8(rounded & 0xFFFFFFF8U);
                else
                    D_003F21B8[index] = *allocated;
            }
            func_003935A4(allocated, old_begin,
                          (u32)range_count(old_end, old_begin) << 2);
            GEORGE_COMPLETION_RELEASE_RANGE(range->field00, range->field08);
            range->field08 = (void **)((u32)allocated + bytes);
            range->field04 = (void **)((u32)allocated + (old_count << 2));
            range->field00 = allocated;
        }
    }
    func_002BF0F0(range, range->field04, count, &zero);
    table->field10 = 0;
    return table;
}

void func_002BEDF0(void *object, s32 flags)
{
    GeorgeCompletionResolver *resolver = (GeorgeCompletionResolver *)object;
    GeorgeCompletionTable *table = &resolver->field04;
    GeorgeCompletionIterator iterator;
    u32 index = 0;
    u32 size = (u32)range_count(table->field04.field04,
                               table->field04.field00);
    u32 destroy_owner;

    iterator.field00 = 0;
    iterator.field04 = table;
    if (size != 0) {
        void **bucket = table->field04.field00;
        do {
            GeorgeCompletionNode *node = (GeorgeCompletionNode *)*bucket;
            ++index;
            if (node != 0) {
                iterator.field00 = node;
                break;
            }
            ++bucket;
        } while (index < size);
    }
    destroy_owner = (u32)flags & 1U;
    while (iterator.field00 != 0) {
        void *payload = iterator.field00->field08;
        if (payload != 0)
            func_002AF120(payload);
        func_002BF4B8(&iterator);
    }
    func_002BF418(table);
    GEORGE_COMPLETION_RELEASE_RANGE(table->field04.field00, table->field04.field08);
    if (destroy_owner != 0)
        func_002AF100(object);
}

void func_00104B10(void *descriptor, s32 flags)
{
    GeorgeActorEffectRecord *record = (GeorgeActorEffectRecord *)descriptor;
    void *resolver;
    record->field04 = D_004208A0;
    resolver = D_003F2C44;
    if (resolver != 0)
        func_002BEDF0(resolver, 3);
    D_003F2C9C = 1;
    D_003F2C44 = 0;
    record->field04 = D_004200F8;
    if (((u32)flags & 1U) != 0)
        func_002AF100(descriptor);
}

void func_002BD340(void)
{
    void **old_end = D_0046A0F0.field04;
    void *descriptor = *(void **)((u32)old_end - 4U);
    D_0046A0F0.field04 = (void **)((u32)old_end - 4U);
    if (descriptor != 0) {
        const u8 *table = ((GeorgeActorEffectRecord *)descriptor)->field04;
        const GeorgeCompletionVirtual *method =
            (const GeorgeCompletionVirtual *)(table + 8);
        s32 adjustment = method->adjustment;
        void (*invoke)(void *, s32) = method->invoke;
        invoke((void *)((u32)descriptor + (u32)adjustment), 3);
    }
}
