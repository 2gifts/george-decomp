#include "george/actor_substates.h"

extern const u8 D_0042E7A8[], D_00421160[], D_0042C360[];
extern u8 D_003F83F0[];
extern void *D_003F2D40, *D_004961F4;
extern GeorgeActorPointerRange D_0046A0F0;
extern void *func_002AEE60(u32);
extern void func_002AF120(void *);
extern void func_002393F8(u32);
extern void *func_0022C1E0(void);
extern void func_00311680(void *, void *);
extern void func_00317910(void *);
extern u32 func_00236A10(const GeorgeRotationMatrix *, u32, u32, u32, u32, u32);
extern float func_00192DA8(GeorgeGoalEntity *);
extern void *func_002E2BB0(void *, u32, u32);
extern void *func_002F0520(void *, float);
extern void *func_0022B950(void *, const GeorgeMathVec3 *, const GeorgeMathVec4 *,
                         u32, u32, u32, u32, float, float, float, float, float);
extern void func_0022C360(void *, u32);
extern void *func_0014F1D0(void *, void *, GeorgeGoalEntity *);
extern void func_00310CC0(void *, void *, u32);
extern void *func_00238BA0(void *, u32);
extern void func_0022D838(void *);
extern void func_0022D788(void *);
extern void func_002727D8(void *);
extern void func_00235CD8(void *, u32, u32);
extern u32 func_00272C30(void *);
extern u32 func_00272C10(void *);
extern void *func_0023C230(void *, u32);
extern s32 func_00192D18(GeorgeGoalEntity *, u32, void *, GeorgeActorRequestCallback,
                        GeorgeGoalEntity *);
extern void func_002A1C08(void *, const void *);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *,
                        const GeorgeRotationMatrix *);
extern void *func_001EDF40(void *, const GeorgeRotationMatrix *, void *, void *,
                         GeorgeGoalEntity *);
extern void func_00191150(GeorgeGoalEntity *);
extern void func_0030C440(void *, const GeorgeMathVec4 *);
extern s32 func_00238D50(void *);
extern u32 func_00297640(u32);
extern u32 func_002A7418(u32);
extern void *func_002AAF50(void *, s32);
extern void func_002AAFD8(void *);
extern void func_00191B40(GeorgeGoalEntity *, u32);
extern void func_00272A58(void *);
extern void func_00181B70(GeorgeGoalEntity *);
extern void *func_002A6468(const void *, s32);
extern void func_00272970(void *, u32, u32, u32, u32, void *,
                        GeorgeActorRequestCallback, GeorgeGoalEntity *);
extern void func_00196A00(GeorgeGoalEntity *);
extern void *func_002481F0(void *);
extern s32 func_00100AA8(const void *, const void *);
extern void func_001007E0(GeorgeActorPointerRange *, void **, void *const *);
extern void func_002BD340(void);
extern s32 func_00396260(void (*)(void));
extern u32 *func_002BEBA0(u32 *, const u8 *);
extern void func_00251CC8(void *, u32);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

/* Same complete adjusted-member representation as actor_states6. The binding
 * selector is captured; the final this adjustment comes from fresh actor state. */
ACTOR_INLINE void actor_member(GeorgeGoalEntity *entity, u32 binding_state)
{
    GeorgeGoalMember *member = (GeorgeGoalMember *)ADDRESS(D_003F83F0, binding_state * 0x1CU);
    s32 selector = member->selector, adjustment;
    void (*invoke)(void *);
    GeorgeGoalVirtualVoid pair;
    if (selector == 0) return;
    if (selector < 0) invoke = member->target.direct;
    else {
        const GeorgeGoalVirtualVoid *table = FIELD(entity, member->target.vtable_offset,
                                                   const GeorgeGoalVirtualVoid *);
        pair = *(const GeorgeGoalVirtualVoid *)ADDRESS(table, (u32)selector * 8U - 8U);
        invoke = pair.invoke;
    }
    adjustment = ((GeorgeGoalMember *)ADDRESS(D_003F83F0, entity->field0C * 0x1CU))->adjustment;
    if (selector > 0) adjustment += pair.adjustment;
    invoke(ADJUST(entity, adjustment));
}

#define ENSURE_EFFECT() do { \
    if (D_003F2D40 == 0) { \
        GeorgeActorEffectRecord *record; \
        void *registered, *value; \
        void **begin, **end, **position; \
        D_003F2D40 = func_002481F0(func_002AEE60(0x1B4)); \
        record = (GeorgeActorEffectRecord *)func_002AEE60(12); \
        registered = D_003F2D40; begin = D_0046A0F0.field00; end = D_0046A0F0.field04; \
        record->field00 = 9; record->field04 = D_00421160; record->field08 = registered; \
        value = record; \
        position = func_00100C30(begin, end, &value, func_00100AA8); \
        if (D_0046A0F0.field04 != D_0046A0F0.field08 && position == D_0046A0F0.field04) { \
            if (position != 0) *position = value; \
            D_0046A0F0.field04 = (void **)ADDRESS(D_0046A0F0.field04, 4); \
        } else func_001007E0(&D_0046A0F0, position, &value); \
        func_00396260(func_002BD340); \
    } \
} while (0)

void func_001726C8(GeorgeGoalEntity *entity, float delta)
{
    GeorgeRotationMatrix frame;
    GeorgeMathVec3 point;
    void *record = FIELD(entity, 0x228, void *);
    float timer = FIELD(entity, 0x2E8, float) - delta;
    float progress = FIELD(entity, 0x324, float) + delta;
    float elapsed = FIELD(entity, 0x334, float) + delta;
    u32 phase, desired;
    FIELD(entity, 0x2E8, float) = timer;
    FIELD(entity, 0x324, float) = progress;
    FIELD(entity, 0x334, float) = elapsed;
    point.x = FIELD(record, 0x40, float) + FIELD(record, 0x20, float) * 0.34999999403953552f;
    point.y = FIELD(record, 0x44, float) + FIELD(record, 0x24, float) * 0.34999999403953552f;
    point.z = FIELD(record, 0x48, float) + FIELD(record, 0x28, float) * 0.34999999403953552f;
    func_002A1C08(&frame, FRAME(entity, 0xB0));
    frame.element[12] = point.x;
    frame.element[13] = point.y;
    frame.element[15] = 1.0f;
    frame.element[14] = point.z;
    phase = FIELD(entity, 0x330, u32);
    desired = FIELD(entity, 0x32C, u32);
    if (phase >= 7) goto finish;
    if (phase == 0) {
        void *reference = func_0023C230((void *)0xA5CFFBD5U, 0xBCF06440U);
        desired = 0x58;
        FIELD(entity, 0x324, u32) = 0;
        FIELD(entity, 0x2DC, u32) = 1;
        func_00192D18(entity, 0x58, reference, func_00190FC0, entity);
        FIELD(entity, 0x330, u32) = 4;
        goto phase_four;
    }
    if (phase == 1 || phase == 2) {
        u32 offset = phase == 1 ? 0x360 : 0x364;
        if (FIELD(entity, 0x354, u32) != 0) goto cleanup;
        if (FIELD(entity->field18, offset, float) < FIELD(entity, 0x324, float)) {
            func_00235CD8(FIELD(entity, 0x264, void *),
                         phase == 1 ? 0x03557B00U : 0x74524B96U, 1);
            FIELD(entity, 0x330, u32) = phase == 1 ? 2 : 3;
        }
        goto follow_frame;
    }
    if (phase == 3) {
        if (FIELD(entity, 0x354, u32) != 0) {
            GeorgeActorControlObject *control = CONTROL(entity);
            const GeorgeActorVirtualPredicate *predicate = (const GeorgeActorVirtualPredicate *)ADDRESS(control->field00, 0xD0);
            if (predicate->invoke(ADJUST(control, predicate->adjustment), 0.0f, 0.25f) == 0) {
                func_00191DC8(entity, FIELD(entity->field18, 0x358, u32));
                func_001910C8(entity);
                func_00170538(entity);
                entity->field0C = 0x12;
                actor_member(entity, 0x12);
                return;
            }
        }
follow_frame:
        record = FIELD(entity, 0x264, void *);
        func_002A1C08(ADDRESS(record, 0x10), &frame);
        FIELD(record, 0xA0, u32) |= 0x100U;
        goto finish;
    }
    if (phase == 4) {
phase_four:
        record = FIELD(entity, 0x1B0, void *);
        if (record != 0 && (func_00272C30(record) & 0x2000U) != 0) {
            void *reference = func_0023C230((void *)FIELD(entity->field18, 0x354, u32), 0xFFFFFFFFU);
            void *memory;
            FIELD(entity, 0x338, void *) = reference;
            memory = func_002AEE60(0xC0);
            func_001EDF40(memory, &frame, ADDRESS(entity, 0xD0), FIELD(entity, 0x338, void *), entity);
        }
        if (FIELD(entity, 0x354, u32) != 0) goto timeout;
        if (!(FIELD(entity->field18, 0x35C, float) < FIELD(entity, 0x324, float))) goto finish;
        FIELD(entity, 0x330, u32) = 1;
        record = FIELD(entity, 0x264, void *);
        desired = 0x5F;
        func_002A1C08(ADDRESS(record, 0x10), &frame);
        FIELD(record, 0xA0, u32) |= 0x100U;
        func_00235CD8(FIELD(entity, 0x264, void *), 0x9F79558FU, 1);
        goto finish;
    }
    /* States 5 and 6 differ only in the proven configuration key. */
    record = FIELD(entity, 0x1B0, void *);
    if (record != 0 && (func_00272C30(record) & 0x2000U) != 0) {
        void *reference = func_0023C230((void *)FIELD(entity->field18, phase == 5 ? 0x354 : 0x350, u32), 0xFFFFFFFFU);
        void *memory, *effect;
        FIELD(entity, 0x338, void *) = reference;
        memory = func_002AEE60(0xC0);
        effect = func_001EDF40(memory, &frame, ADDRESS(entity, 0xD0), FIELD(entity, 0x338, void *), entity);
        progress = FIELD(entity, 0x324, float);
        if (0.0f <= progress) progress = george_ee_minimum(progress, FIELD(entity->field18, 0x368, float));
        else progress = 0.0f;
        FIELD(entity, 0x324, float) = progress;
        FIELD(effect, 0x18, float) = progress * FIELD(entity->field18, 0x36C, float);
    }
timeout:
    if (!(FIELD(entity, 0x2E8, float) < 0.0f)) goto finish;
cleanup:
    func_001910C8(entity);
    return;
finish:
    if (desired == FIELD(entity, 0x32C, u32) && !(FIELD(entity, 0x2E8, float) <= 0.0f)) return;
    record = func_0023C230((void *)0xA5CFFBD5U, 0xBCF06440U);
    FIELD(entity, 0x2DC, u32) = 1;
    func_00192D18(entity, desired, record, func_00190FC0, entity);
    FIELD(entity, 0x32C, u32) = desired;
}

void func_00172C08(GeorgeGoalEntity *entity, float delta)
{
    s32 phase = FIELD(entity, 0x33C, s16);
    FIELD(entity, 0x2E8, float) = FIELD(entity, 0x2E8, float) - delta;
    if (phase == 0) {
        void *reference = func_0023C230((void *)0xCE867063U, 0xBCF06440U);
        FIELD(entity, 0x2DC, u32) = 1;
        func_00192D18(entity, 0x5E, reference, func_00190FC0, entity);
        FIELD(entity, 0x33C, s16) = 0x64;
        return;
    }
    if (phase != 0x64) return;
    if (FIELD(entity, 0x228, void *) != 0) {
        void *primary = FIELD(entity, 0x1B0, void *);
        if (primary != 0 && (func_00272C10(primary) & 0x2000U) != 0) {
            s32 enabled = FIELD(entity, 0x33E, signed char) == 0;
            FIELD(entity, 0x33E, signed char) = (signed char)enabled;
            if (enabled != 0) {
                void *record;
                if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
                record = FIELD(entity, 0x228, void *);
                if (record != 0) {
                    GeorgeMathVec3 lower, upper;
                    float x = FIELD(record, 0x40, float), y = FIELD(record, 0x44, float), z = FIELD(record, 0x48, float);
                    lower.z = z + -0.00999999977648258f;
                    lower.x = x + -0.00999999977648258f;
                    lower.y = y + -0.00999999977648258f;
                    upper.x = FIELD(record, 0x40, float) + 0.00999999977648258f;
                    upper.z = FIELD(record, 0x48, float) + 0.00999999977648258f;
                    upper.y = FIELD(record, 0x44, float) + 0.00999999977648258f;
                    func_0018E5A8(entity, &lower, &upper, 1, 5);
                }
            } else if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
        }
    } else {
        void *primary = FIELD(entity, 0x1B0, void *);
        if (primary != 0 && (func_00272C30(primary) & 0x4000U) != 0) {
            s32 enabled = FIELD(entity, 0x33E, signed char) == 0;
            FIELD(entity, 0x33E, signed char) = (signed char)enabled;
            if (enabled != 0) {
                GeorgeRotationMatrix *source;
                if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
                source = func_00192748(entity, 0x28);
                if (source != 0) {
                    GeorgeRotationMatrix frame;
                    GeorgeMathVec3 lower, upper;
                    u32 key;
                    void *effect;
                    float negative, radius, x, y, z;
                    ENSURE_EFFECT();
                    effect = D_003F2D40;
                    func_002BEBA0(&key, D_0042C360);
                    func_00251CC8(effect, key);
                    FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x400000000000ULL;
                    func_002A2200(&frame, source, FRAME(entity, 0xF0));
                    negative = -FIELD(entity->field18, 0x15C, float);
                    x = frame.element[12]; y = frame.element[13]; z = frame.element[14];
                    lower.z = z + negative;
                    lower.x = x + negative;
                    lower.y = y + negative;
                    radius = FIELD(entity->field18, 0x15C, float);
                    upper.z = z + radius;
                    upper.x = x + radius;
                    upper.y = y + radius;
                    func_0018E5A8(entity, &lower, &upper, 1, 5);
                }
            } else if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
        }
    }
    if (0.0f < FIELD(entity, 0x2E8, float)) {
        void *record = FIELD(entity, 0x228, void *);
        if (record != 0) {
            GeorgeMathVec4 bounds[2];
            float factor = FIELD(entity, 0x5C8, float);
            float fx = factor * FIELD(record, 0x20, float);
            float fz = factor * FIELD(record, 0x28, float);
            float fy = factor * FIELD(record, 0x24, float);
            float x = FIELD(record, 0x40, float) + fx;
            float y = FIELD(record, 0x44, float) + fy;
            float z = FIELD(record, 0x48, float) + fz;
            GeorgeGoalEntityData *data = entity->field18;
            void *output = FIELD(entity, 0x300, void *);
            float negative = -FIELD(data, 0x15C, float), radius;
            bounds[0].z = z + negative;
            bounds[0].x = x + negative;
            bounds[0].y = y + negative;
            bounds[0].w = 0.0f;
            radius = FIELD(data, 0x15C, float);
            bounds[1].x = x + radius;
            bounds[1].y = y + radius;
            bounds[1].z = z + radius;
            bounds[1].w = 0.0f;
            func_0030C440(output, bounds);
        } else {
            GeorgeRotationMatrix *source = func_00192748(entity, 0x28);
            void *output = FIELD(entity, 0x300, void *);
            if (output != 0 && source != 0) {
                GeorgeRotationMatrix frame;
                GeorgeMathVec4 bounds[2];
                float x, y, z, negative, radius;
                func_002A2200(&frame, source, FRAME(entity, 0xF0));
                x = frame.element[12]; y = frame.element[13]; z = frame.element[14];
                output = FIELD(entity, 0x300, void *);
                negative = -FIELD(entity->field18, 0x15C, float);
                bounds[0].z = z + negative;
                bounds[0].x = x + negative;
                bounds[0].y = y + negative;
                bounds[0].w = 0.0f;
                radius = FIELD(entity->field18, 0x15C, float);
                bounds[1].x = x + radius;
                bounds[1].y = y + radius;
                bounds[1].z = z + radius;
                bounds[1].w = 0.0f;
                func_0030C440(output, bounds);
            }
        }
    } else func_00191150(entity);
}

void func_001732F0(GeorgeGoalEntity *entity)
{
    void *record = FIELD(entity, 0x238, void *);
    float position_z;
    s32 phase;
    FIELD(entity, 0x2E8, float) = FIELD(entity, 0x2E8, float) - FIELD(entity, 0x35C, float);
    FIELD(record, 0x40, float) = FIELD(entity, 0x120, float);
    FIELD(record, 0x44, float) = FIELD(entity, 0x124, float);
    position_z = FIELD(entity, 0x128, float);
    FIELD(record, 0x4C, float) = 1.0f;
    FIELD(record, 0x48, float) = position_z;
    FIELD(record, 0xA0, u32) |= 0x100U;
    phase = FIELD(entity, 0x340, s32);
    if (phase == 0) {
        void *reference = func_0023C230((void *)0xF114E180U, 0xBCF06440U);
        FIELD(entity, 0x2E8, u32) = 0;
        FIELD(entity, 0x2DC, u32) = 1;
        func_00192D18(entity, 0x6A, reference, func_00190FC0, entity);
        FIELD(entity, 0x340, u32) = 2;
        FIELD(entity, 0x344, u32) = 0;
    } else if (phase == 2) {
        void *primary = FIELD(entity, 0x1B0, void *);
        if (primary == 0) return;
        if ((func_00272C30(primary) & 0x2000U) != 0) {
            GeorgeGoalEntityData *data;
            GeorgeMathVec3 lower, upper;
            float x, y, z, negative, radius;
            if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
            data = entity->field18;
            z = FIELD(entity, 0x48, float);
            negative = -FIELD(data, 0x14C, float);
            x = FIELD(entity, 0x40, float);
            y = FIELD(entity, 0x44, float);
            lower.y = y;
            lower.z = z + negative;
            lower.x = x + negative;
            radius = FIELD(data, 0x14C, float);
            upper.z = z + radius;
            upper.x = x + radius;
            upper.y = y + radius;
            func_0018E5A8(entity, &lower, &upper, 4, 12);
            FIELD(entity, 0x340, u32) = FIELD(entity, 0x340, u32) + 1U;
        }
        primary = FIELD(entity, 0x1B0, void *);
        if (primary == 0) return;
        if ((func_00272C30(primary) & 0x80000U) != 0) {
            u32 notify = FIELD(entity, 0x34C, u32);
            FIELD(entity, 0x344, u32) = 1;
            if (notify != 0) {
                func_00235CD8(FIELD(entity, 0x238, void *), 0x9F79558FU, 1);
                FIELD(entity, 0x34C, u32) = 0;
            }
        }
    } else if (phase == 3) {
        void *primary = FIELD(entity, 0x1B0, void *);
        GeorgeGoalEntityData *data;
        GeorgeMathVec4 bounds[2];
        float x, y, z, negative, radius;
        void *output;
        if (primary != 0 && (func_00272C30(primary) & 0x80000U) != 0) {
            u32 repeat = FIELD(entity, 0x348, u32);
            FIELD(entity, 0x340, u32) = 4;
            FIELD(entity, 0x344, u32) = 0;
            if (repeat == 0) {
                func_00235CD8(FIELD(entity, 0x238, void *), 0xB95616B6U, 1);
                FIELD(entity, 0x34C, u32) = 1;
            }
        }
        data = entity->field18;
        z = FIELD(entity, 0x48, float);
        negative = -FIELD(data, 0x14C, float);
        x = FIELD(entity, 0x40, float);
        y = FIELD(entity, 0x44, float);
        output = FIELD(entity, 0x300, void *);
        bounds[0].y = y;
        bounds[0].z = z + negative;
        bounds[0].x = x + negative;
        bounds[0].w = 0.0f;
        radius = FIELD(data, 0x14C, float);
        bounds[1].x = x + radius;
        bounds[1].y = y + radius;
        bounds[1].z = z + radius;
        bounds[1].w = 0.0f;
        func_0030C440(output, bounds);
    } else if (phase == 4) {
        if (FIELD(entity, 0x2E8, float) <= 0.0f) {
            if (FIELD(entity, 0x348, u32) != 0) {
                FIELD(entity, 0x340, u32) = 0;
                FIELD(entity, 0x348, u32) = 0;
            } else {
                s32 ready = func_00238D50(FIELD(entity, 0x238, void *));
                void *next = FIELD(FIELD(entity, 0x238, void *), 0xAC, void *);
                while (next != 0 && ready != 0) {
                    ready = func_00238D50(next);
                    next = FIELD(next, 0xAC, void *);
                }
                if (ready != 0) func_00191180(entity);
            }
        }
    }
}

void func_00174868(GeorgeGoalEntity *entity, float delta)
{
    void *primary = FIELD(entity, 0x1B0, void *), *record;
    FIELD(entity, 0x2E8, float) = FIELD(entity, 0x2E8, float) - delta;
    if (primary != 0) {
        u32 status = func_00272C30(primary);
        u32 mask = func_00297640(FIELD(entity->field18, 0x13C, u32));
        if ((status & mask) != 0) {
            s32 enabled = FIELD(entity, 0x320, signed char) == 0;
            FIELD(entity, 0x320, signed char) = (signed char)enabled;
            if (enabled != 0) {
                GeorgeGoalEntityData *data;
                GeorgeMathVec3 lower, upper;
                float x, y, z, negative, radius, height;
                if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
                data = entity->field18;
                z = FIELD(entity, 0x48, float);
                negative = -FIELD(data, 0x130, float);
                x = FIELD(entity, 0x40, float);
                y = FIELD(entity, 0x44, float);
                lower.y = y;
                lower.z = z + negative;
                lower.x = x + negative;
                radius = FIELD(data, 0x130, float);
                height = FIELD(data, 0x134, float);
                upper.z = z + radius;
                upper.y = y + height;
                upper.x = x + radius;
                func_0018E5A8(entity, &lower, &upper, 2, 5);
            } else if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
        }
    }
    /* Retail recomputes these six scratch floats whenever 0x320 is enabled.
     * No later instruction consumes that scratch; these reads are retained as
     * ordinary C expressions and may be removed by a candidate optimizer. */
    if (FIELD(entity, 0x320, signed char) != 0) {
        GeorgeGoalEntityData *data = entity->field18;
        GeorgeMathVec3 lower, upper;
        float z = FIELD(entity, 0x48, float), negative = -FIELD(data, 0x130, float);
        float x = FIELD(entity, 0x40, float), y = FIELD(entity, 0x44, float);
        lower.y = y;
        lower.z = z + negative;
        lower.x = x + negative;
        upper.z = z + FIELD(data, 0x130, float);
        upper.y = y + FIELD(data, 0x134, float);
        upper.x = x + FIELD(data, 0x130, float);
        (void)lower; (void)upper;
    }
    if (FIELD(entity, 0x2E8, float) <= 0.0f) {
        GeorgeGoalEntityData *data = entity->field18;
        s32 count = FIELD(data, 0x148, s32);
        if (count > 0 && FIELD(entity, 0x318, s32) < (s32)((u32)count - 1U) && FIELD(entity, 0x31C, u32) != 0) {
            void *reference = func_0023C230((void *)0xF114E180U, 0xBCF06440U);
            u32 index = FIELD(entity, 0x318, u32) + 1U, key;
            data = entity->field18;
            FIELD(entity, 0x2DC, u32) = 1;
            FIELD(entity, 0x31C, u32) = 0;
            FIELD(entity, 0x318, u32) = index;
            key = FIELD(data, 0x204U + (index << 4), u32);
            if (key != 0) func_00191B40(entity, key);
            index = FIELD(entity, 0x318, u32);
            func_00192D18(entity, FIELD(entity->field18, 0x200U + (index << 4), u32) + 0x63U,
                         reference, func_00190FC0, entity);
        } else {
            record = FIELD(entity, 0x23C, void *);
            if (record != 0) func_00235CD8(record, 0xB95616B6U, 1);
            record = FIELD(entity, 0x294, void *);
            if (record != 0) {
                s32 i = 0, count = (s32)func_002AAF88(record);
                while (i < count) {
                    void *entry = func_002AAF50(FIELD(entity, 0x294, void *), i);
                    u32 object = FIELD(entry, 0, u32);
                    i = (s32)((u32)i + 1U);
                    if (object != 0) func_002393F8(object);
                }
                func_002AAFD8(FIELD(entity, 0x294, void *));
            }
            if (FIELD(entity, 0x2D8, u32) == 1) {
                func_00272A58(FIELD(entity, 0x1B0, void *));
                if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
                FIELD(entity, 0x2D8, u32) = 0;
            }
            data = entity->field18;
            if (FIELD(data, 0x12C, u32) != 0 && !(FIELD(entity, 0x318, s32) < FIELD(data, 0x148, s32))) {
                func_00170538(entity);
                entity->field0C = 0x16;
                actor_member(entity, 0x16);
            }
        }
    }
    record = FIELD(entity, 0x23C, void *);
    if (record != 0) {
        u32 identifier = func_002A7418(FIELD(entity->field18, 0x144, u32));
        GeorgeRotationMatrix *source = func_00192748(entity, identifier);
        GeorgeRotationMatrix frame;
        func_002A2200(&frame, source, FRAME(entity, 0xF0));
        record = FIELD(entity, 0x23C, void *);
        func_002A1C08(ADDRESS(record, 0x10), &frame);
        FIELD(record, 0xA0, u32) |= 0x100U;
    }
    record = FIELD(entity, 0x294, void *);
    if (record != 0) {
        s32 i = 0, count = (s32)func_002AAF88(record);
        while (i < count) {
            void *entry = func_002AAF50(FIELD(entity, 0x294, void *), i);
            void *object = FIELD(entry, 0, void *);
            if (object != 0) {
                FIELD(object, 0x80, float) = FIELD(entity, 0x40, float);
                FIELD(object, 0x84, float) = FIELD(entity, 0x44, float);
                {
                    float z = FIELD(entity, 0x48, float);
                    FIELD(object, 0x8C, float) = 1.0f;
                    FIELD(object, 0x88, float) = z;
                }
            }
            i = (s32)((u32)i + 1U);
        }
    }
}

void func_00181120(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *control = CONTROL(entity);
    const GeorgeGoalVirtualOutput *output;
    s32 phase;
    FIELD(entity, 0x2E8, float) = FIELD(entity, 0x2E8, float) - FIELD(entity, 0x35C, float);
    output = (const GeorgeGoalVirtualOutput *)ADDRESS(control->field00, 0xB0);
    output->invoke(ADJUST(control, output->adjustment), (GeorgeGoalOutput *)ADDRESS(entity, 0x5AC));
    phase = FIELD(entity, 0x5C4, s32);
    if (phase != 0x65 && phase != 0xC8 && phase != 0xC9) {
        void *reference = func_0023C230((void *)0xA9D253A6U, 0xBCF06440U);
        u32 index = FIELD(entity, 0x5B8, u32);
        void *primary = FIELD(entity, 0x1B0, void *);
        u32 word = (index << 1) + 0x58U;
        FIELD(entity, 0x2DC, u32) = 1;
        if (primary != 0) {
            func_00272970(primary, word, 0, 0, FIELD(entity, 0x368, u32), reference, func_00190FC0, entity);
            if (FIELD(entity, 0x1B4, void *) != 0)
                func_00272970(FIELD(entity, 0x1B4, void *), word, 0, 0, FIELD(entity, 0x368, u32), reference, 0, 0);
        }
        FIELD(entity, 0x5C4, u32) = 0x65;
    } else if (phase == 0x65) {
        s32 index = 0;
        func_00181B70(entity);
        while (index < FIELD(entity->field18, 0x288, s32)) {
            u32 slot_offset = (u32)index << 2, entry_offset = (u32)index << 4;
            s32 next_index = (s32)((u32)index + 1U);
            if (FIELD(entity, 0x244U + slot_offset, void *) != 0) {
                void *primary = FIELD(entity, 0x1B0, void *);
                u32 identifier = FIELD(entity, 0x5DCU + entry_offset, u32);
                const GeorgeRotationMatrix *input = 0;
                GeorgeRotationMatrix frame;
                void *record;
                if (primary != 0) {
                    u32 state = FIELD(primary, 0xC, u32);
                    u32 offset = (state == 3 ? 2 : state) << 2;
                    s32 count = (s32)func_002A6460(FIELD(primary, 0x378U + offset, const void *));
                    if (count > 0) {
                        const u8 *records = (const u8 *)func_002A6468(
                            FIELD(FIELD(entity, 0x1B0, void *), 0x378U + offset, const void *), 0);
                        const GeorgeRotationMatrix *matrices = FIELD(FIELD(entity, 0x1B0, void *), 0x3E8U + offset, const GeorgeRotationMatrix *);
                        s32 i;
                        for (i = 0; i < count; i = (s32)((u32)i + 1U)) {
                            if (records[((u32)i << 5) + 0x1D] == identifier) {
                                input = (const GeorgeRotationMatrix *)ADDRESS(matrices, (u32)i << 6);
                                break;
                            }
                        }
                    }
                }
                func_002A2200(&frame, input, FRAME(entity, 0xB0));
                record = FIELD(entity, 0x244U + slot_offset, void *);
                func_002A1C08(ADDRESS(record, 0x10), &frame);
                FIELD(record, 0xA0, u32) |= 0x100U;
                primary = FIELD(entity, 0x1B0, void *);
                if (primary != 0) {
                    u32 status = func_00272C30(primary);
                    if ((status & FIELD(entity, 0x5D8U + entry_offset, u32)) != 0) {
                        u32 bits = FIELD(entity, 0x5CC, u16) ^ (1U << ((u32)index & 31U));
                        s32 enabled = ((s32)(s16)bits >> ((u32)index & 31U)) & 1;
                        FIELD(entity, 0x5CC, u16) = (u16)bits;
                        func_00235CD8(FIELD(entity, 0x244U + slot_offset, void *),
                                     enabled != 0 ? 0x9F79558FU : 0xB95616B6U, 1);
                    }
                }
            }
            index = next_index;
        }
        if (FIELD(entity, 0x2E8, float) <= 0.0f) {
            if (FIELD(entity, 0x5BC, u32) != 0) {
                u32 index = FIELD(entity, 0x5B8, u32);
                FIELD(entity, 0x5C4, u32) = 0x64;
                FIELD(entity, 0x5BC, u32) = 0;
                FIELD(entity, 0x5B8, u32) = index + 1U;
            } else FIELD(entity, 0x5C4, u32) = 0xC8;
        }
    } else if (phase == 0xC8) {
        void *reference = func_0023C230((void *)0xA9D253A6U, 0xBCF06440U);
        u32 index = FIELD(entity, 0x5B8, u32);
        void *primary = FIELD(entity, 0x1B0, void *);
        u32 word = (index << 1) + 0x59U;
        FIELD(entity, 0x2DC, u32) = 1;
        if (primary != 0) {
            func_00272970(primary, word, 0, 0, FIELD(entity, 0x368, u32), reference, func_00190FC0, entity);
            if (FIELD(entity, 0x1B4, void *) != 0)
                func_00272970(FIELD(entity, 0x1B4, void *), word, 0, 0, FIELD(entity, 0x368, u32), reference, 0, 0);
        }
        FIELD(entity, 0x5C4, u32) = 0xC9;
    } else {
        func_00181B70(entity);
        if (FIELD(entity, 0x2E8, float) <= 0.0f) {
            s32 index = 0;
            void *record;
            FIELD(entity, 0x2D8, u32) = 0;
            func_002727D8(FIELD(entity, 0x1B0, void *));
            while (index < FIELD(entity->field18, 0x288, s32)) {
                record = FIELD(entity, 0x244U + ((u32)index << 2), void *);
                if (record != 0) func_00235CD8(record, 0xB95616B6U, 1);
                index = (s32)((u32)index + 1U);
            }
            func_00196A00(entity);
            record = FIELD(entity, 0x230, void *);
            if (record != 0) FIELD(record, 0x30, float) = FIELD(entity->field18, 0x378, float);
        }
    }
}

void func_00190C18(GeorgeGoalEntity *entity, u32 flags)
{
    FIELD(entity, 4, const u8 *) = D_0042E7A8;
    if (FIELD(entity, 0x4F8, void *) != 0)
        func_002AF120(FIELD(entity, 0x4F8, void *));
    func_002D0700((GeorgeScriptObject *)entity, flags);
}

void func_00190C70(GeorgeGoalEntity *entity, GeorgeGoalEntityData *data)
{
    void *record;
    u32 word;
    entity->field18 = data;
    func_00175AF0(entity, (FIELD(entity, 0x190, GeorgeActorBits64) & 0x2000ULL) != 0);
    record = func_00238BA0(FIELD(entity, 0x1C8, void *), 0x1A6B0F5DU);
    word = FIELD(entity->field18, 0xA8, u32) | 1U;
    FIELD(record, 0x28, u32) = word;
    if (FIELD(record, 0x3C, void *) != 0) {
        func_0022D838(FIELD(record, 0x3C, void *));
        word = FIELD(record, 0x28, u32);
        FIELD(record, 0x2C, u32) = word;
        func_0022D788(FIELD(record, 0x3C, void *));
    } else FIELD(record, 0x2C, u32) = word;
}

void func_001910C8(GeorgeGoalEntity *entity)
{
    GeorgeGoalEntityData *data;
    void *record;
    if (FIELD(entity, 0x2D8, u32) != 4) return;
    if (FIELD(entity, 0x1B0, void *) != 0)
        func_002727D8(FIELD(entity, 0x1B0, void *));
    func_00235CD8(FIELD(entity, 0x264, void *), 0xB95616B6U, 1);
    data = entity->field18;
    FIELD(entity, 0x350, u32) = 0;
    FIELD(entity, 0x354, u32) = 0;
    FIELD(entity, 0x2D8, u32) = 0;
    FIELD(entity, 0x2DC, u32) = 0;
    record = FIELD(entity, 0x230, void *);
    FIELD(entity, 0x334, float) = FIELD(data, 0x370, float);
    if (record != 0) FIELD(record, 0x30, float) = FIELD(data, 0x378, float);
}

void func_00191180(GeorgeGoalEntity *entity)
{
    void *record;
    FIELD(entity, 0x2D8, u32) = 0;
    func_002727D8(FIELD(entity, 0x1B0, void *));
    func_00235CD8(FIELD(entity, 0x238, void *), 0xB95616B6U, 1);
    if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
    record = FIELD(entity, 0x230, void *);
    if (record != 0) FIELD(record, 0x30, float) = FIELD(entity->field18, 0x378, float);
}

void func_00191000(GeorgeGoalEntity *entity, float delta)
{
    u32 state;
    if (FIELD(entity, 0x2DC, u32) != 0) return;
    state = FIELD(entity, 0x2D8, u32);
    if (state == 1) func_00174868(entity, delta);
    else if (state == 2) {
        float timer = FIELD(entity, 0x2E8, float) - delta;
        FIELD(entity, 0x2E8, float) = timer;
        if (timer <= 0.0f) FIELD(entity, 0x2D8, u32) = 0;
    } else if (state == 4) func_001726C8(entity, delta);
    else if (state == 5) func_00172C08(entity, delta);
    else if (state == 6) func_001732F0(entity);
    else if (state == 7) func_00181120(entity);
}

void func_00175AF0(GeorgeGoalEntity *entity, u32 mode)
{
    GeorgeActorControlObject *control;
    const GeorgeGoalVirtualWord *configure;
    void *record;
    if (FIELD(entity, 0x1A4, u32) != 0) {
        func_002393F8(FIELD(entity, 0x1A4, u32));
        FIELD(entity, 0x1A4, u32) = 0;
    }
    if (FIELD(entity, 0x1A0, void *) != 0) {
        void *manager = func_0022C1E0();
        func_00311680(manager, FIELD(entity, 0x1A0, void *));
        func_00317910(FIELD(entity, 0x1A0, void *));
        FIELD(entity, 0x1A0, void *) = 0;
    }
    FIELD(entity, 0x1A4, u32) = func_00236A10(FRAME(entity, 0xB0), 0x09DB5CDDU, 1, 0, 0, 0);
    control = CONTROL(entity);
    configure = (const GeorgeGoalVirtualWord *)ADDRESS(control->field00, 0x20);
    configure->invoke(ADJUST(control, configure->adjustment), mode);
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x400ULL) != 0) {
        GeorgeMathVec3 position;
        void *memory, *manager;
        const GeorgeGoalVirtualWord *release;
        u32 selector;
        position.x = FIELD(entity, 0x40, float);
        position.y = FIELD(entity, 0x44, float);
        position.z = FIELD(entity, 0x48, float);
        position.y = position.y + func_00192DA8(entity);
        memory = func_002E2BB0(D_004961F4, 0x14, 0x25);
        FIELD(memory, 4, u16) = 0x14;
        record = func_002F0520(memory, 0.40000000596046448f);
        selector = (FIELD(entity, 0x190, GeorgeActorBits64) & 0x10000000ULL) != 0 ? 0x19 : 0x1A;
        FIELD(entity, 0x1A0, void *) = func_0022B950(record, &position, 0, selector, 6, 0, 0,
                              1.0f, 0.5f, 0.40000000596046448f, 0.0f, 0.05000000074505806f);
        if (FIELD(record, 4, u16) != 0) {
            u16 count = (u16)(FIELD(record, 6, u16) - 1U);
            FIELD(record, 6, u16) = count;
            if (count == 0 && record != 0) {
                release = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(record, 0, const u8 *), 8);
                release->invoke(ADJUST(record, release->adjustment), 3);
            }
        }
        func_0022C360(FIELD(entity, 0x1A0, void *), FIELD(entity, 0x19C, u32));
        memory = func_002AEE60(0x28);
        FIELD(entity, 0x374, void *) = func_0014F1D0(memory, FIELD(entity, 0x1A0, void *), entity);
        manager = func_0022C1E0();
        func_00310CC0(manager, FIELD(entity, 0x1A0, void *), 1);
    }
    record = FIELD(entity, 0x454, void *);
    if (record != 0) {
        const GeorgeGoalVirtualWord *release = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(record, 0, const u8 *), 0x28);
        release->invoke(ADJUST(record, release->adjustment), 3);
        FIELD(entity, 0x454, void *) = 0;
    }
}
