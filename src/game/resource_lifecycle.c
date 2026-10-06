#include "george/resource_lifecycle.h"

extern const u8 D_0043A408[];
extern void func_00226D78(GeorgeResourceRecord *);
extern void func_00226ED8(GeorgeResourceRecord *, GeorgeResourceRecord *);
extern void func_0021C508(void *);
extern void func_0021C5D8(void *);
extern void func_002267D0(GeorgeResourceRecord *, u32);

#if defined(__GNUC__) && __GNUC__ >= 3
#define RESOURCE_INLINE static __inline__ __attribute__((always_inline))
#else
#define RESOURCE_INLINE static __inline__
#endif

/* Both complete entries contain this same original release sequence. Keep
 * the captured flags/child and the post-call child reload distinct. */
RESOURCE_INLINE void resource_release_child(GeorgeResourceRecord *record,
                                            GeorgeResourceRecord *child,
                                            u8 flags)
{
    const GeorgeGoalVirtualWord *pair;
    if ((flags & 4) != 0 && (flags & 0x20) == 0)
        func_00226ED8(child, record);
    child = record->field24;
    pair = (const GeorgeGoalVirtualWord *)(child->field20 + 0x10);
    pair->invoke((void *)((u32)child + (s32)pair->adjustment), 0);
}

void func_0020EF60(GeorgeResourceRecord *record)
{
    GeorgeResourceRecord *child = record->field24;
    void *holder;
    if ((child->flags & 0x80) != 0)
        func_00226D78(child);
    else
        child->count = (u16)(child->count + 1);
    holder = record->field10;
    if (((const u8 *)holder)[2] != 0)
        func_0021C508(holder);
}

void func_0020EFC8(GeorgeResourceRecord *record)
{
    void *holder;
    func_0020F008(record);
    holder = record->field10;
    if (((const u8 *)holder)[2] != 0)
        func_0021C5D8(holder);
}

void func_0020F008(GeorgeResourceRecord *record)
{
    GeorgeResourceRecord *child = record->field24;
    if (child != 0)
        resource_release_child(record, child, record->flags);
}

void func_0020F070(GeorgeResourceRecord *record, u32 mode)
{
    u8 flags;
    GeorgeResourceRecord *child;
    void *holder;
    record->field20 = D_0043A408;
    flags = record->flags;
    if ((flags & 0x80) == 0) {
        child = record->field24;
        if (child != 0)
            resource_release_child(record, child, flags);
        if ((record->flags & 0x20) != 0) {
            holder = record->field10;
            if (((const u8 *)holder)[2] != 0)
                func_0021C5D8(holder);
        }
    }
    func_002267D0(record, mode);
}
