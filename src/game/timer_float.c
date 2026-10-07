#include "george/timer_float.h"
#include "george/heap.h"

extern GeorgeTimerRate *D_003FD4D8;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00447AA0[];

/* Already reviewed numeric contracts. The pop callback and descriptor table
 * supply no additional helper or data recovery credit in this batch. */
extern s32 func_00100AA8(const void *left, const void *right);
extern void func_001007E0(GeorgeActorPointerRange *range, void **position,
                        void *const *value);
extern void func_002BD340(void);
extern s32 func_00396260(void (*callback)(void));

/* Complete repeated registration expression, shared as ordinary C. Arguments
 * and field loads retain the actual capture/publication order; this macro is
 * an authoring choice, not a claim about original source syntax. */
#define GEORGE_TIMER_LAZY_REGISTER() do { \
    if (D_003FD4D8 == 0) { \
        GeorgeTimerRate *rate = (GeorgeTimerRate *)func_002AEE60(12); \
        GeorgeActorEffectRecord *record; \
        GeorgeTimerRate *registered; \
        void *value; \
        void **begin, **end, **position; \
        rate->field08 = 1; \
        rate->field04 = 0x1193FF10U; \
        D_003FD4D8 = rate; \
        rate->field08 = 0x1193FF10U / 1000U; \
        record = (GeorgeActorEffectRecord *)func_002AEE60(12); \
        registered = D_003FD4D8; \
        begin = D_0046A0F0.field00; \
        end = D_0046A0F0.field04; \
        record->field04 = D_00447AA0; \
        record->field00 = 0xFFFFFFFFU; \
        record->field08 = registered; \
        value = record; \
        position = func_00100C30(begin, end, &value, func_00100AA8); \
        if (D_0046A0F0.field04 != D_0046A0F0.field08 && \
            position == D_0046A0F0.field04) { \
            if (position != 0) \
                *position = value; \
            D_0046A0F0.field04 = (void **)((u32)D_0046A0F0.field04 + 4U); \
        } else { \
            func_001007E0(&D_0046A0F0, position, &value); \
        } \
        func_00396260(func_002BD340); \
    } \
} while (0)

float func_002BD770(u32 *timer, s32 reset)
{
    GeorgeTimerRate *rate;
    u32 elapsed;
    GEORGE_TIMER_LAZY_REGISTER();
    rate = D_003FD4D8;
    elapsed = func_002BD618(timer, reset);
    return (float)elapsed / (float)rate->field04;
}

/* Incoming GPR4 is not consumed. The second numeric slot models the
 * sign-extended low32 u32 argument under the actual target caller ABI.
 * Original prototype and invocation remain unresolved. */
float func_002BDA78(u32 unused, u32 value)
{
    (void)unused;
    GEORGE_TIMER_LAZY_REGISTER();
    return (float)value / (float)D_003FD4D8->field04;
}

#undef GEORGE_TIMER_LAZY_REGISTER
