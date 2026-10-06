#include "george/actor_actions.h"

extern GeorgeDeimosValue *D_00474F48;
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[];
extern void *func_002AEE60(u32);
extern void *func_002481F0(void *);
extern s32 func_00100AA8(const void *, const void *);
extern void func_001007E0(GeorgeActorPointerRange *, void **, void *const *);
extern void func_002BD340(void);
extern s32 func_00396260(void (*)(void));
extern void func_00251DB0(void *, u32, const GeorgeMathVec3 *);
extern float func_0029B940(float, float);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64, GeorgeActorBits64);
extern void func_00238E38(void *, u32);
extern void func_00307850(void *);
extern void func_00272390(void *, u32, u32, u32, GeorgeActorRequestCallback,
                        GeorgeGoalEntity *, u32, u32, u32, float, float);
extern void func_0021BF28(void *, const GeorgeMathVec3 *, float);
extern void func_0026FE30(void *, const GeorgeRotationMatrix *, u32, float, float);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *,
                        const GeorgeRotationMatrix *);
extern void func_002B85D0(void *, const GeorgeMathVec3 *, const GeorgeMathVec4 *, float);
extern void func_002393F8(u32);
extern float func_002A6E60(void *);
extern void *func_0023C230(void *, u32);
extern void func_00272970(void *, u32, u32, u32, u32, void *,
                        GeorgeActorRequestCallback, GeorgeGoalEntity *);
extern void func_001B67C0(void *, GeorgeGoalEntity *, u32, u32, u32, u32);
extern void *func_0013D328(void *, const GeorgeMathVec3 *, GeorgeMathVec3 *,
                         GeorgeMathVec3 *, u32, float);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))

/* The complete effect-registry block is already reviewed in actor_controls.
 * Expand it at the actual occurrences, without new private helper bindings. */
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

#define SOFT_ABSOLUTE(value) do { \
    if (func_00373250(value, 0ULL) < 0) value = func_00372CC0(0ULL, value); \
} while (0)

void func_001784D0(GeorgeGoalEntity *entity, u32 use_direction, float angle)
{
    u32 state;
    if (FIELD(entity, 0x2D8, u32) == 6) return;
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x40000ULL) != 0) return;
    FIELD(entity, 0x64, float) = angle;
    if (FIELD(entity->field18, 0x1D8, u32) == 0x45A78000U && use_direction != 0) {
        GeorgeActorControlObject *object = CONTROL(entity);
        const GeorgeGoalVirtualVector *pair = (const GeorgeGoalVirtualVector *)ADDRESS(object->field00, 0x58);
        const GeorgeMathVec3 *direction = pair->invoke(ADJUST(object, pair->adjustment));
        float desired = func_0029B940(direction->z, direction->x);
        float change = desired - angle;
        GeorgeActorBits64 difference = func_00374848(change);
        SOFT_ABSOLUTE(difference);
        if (func_00373250(difference, 0x400921FB60000000ULL) <= 0) {
            difference = func_00374848(change);
        } else {
            difference = func_00374848(change);
            SOFT_ABSOLUTE(difference);
            difference = func_00372CC0(difference, 0x401921FB60000000ULL);
            if (change < 0.0f) difference = func_00372D28(difference, 0xBFF0000000000000ULL);
        }
        SOFT_ABSOLUTE(difference);
        angle = desired;
        if (func_00373250(difference, 0x3FF921FB60000000ULL) >= 0) {
            angle = desired - 3.14159274101257324f;
            if (3.14159274101257324f < angle) angle = angle - 6.28318548202514648f;
            else if (!(-3.14159274101257324f <= angle)) angle = angle + 6.28318548202514648f;
        }
    }
    state = entity->field0C;
    if (state == 0) {
        float previous = FIELD(entity, 0x58, float);
        FIELD(entity, 0x58, float) = angle;
        FIELD(entity, 0x504, float) = previous;
    } else if (state >= 36 || state == 2 || state == 3 || state == 4 || state == 5 ||
               state == 7 || state == 8 || state == 11 || state == 13 || state == 15 ||
               state == 16 || state == 17 || state == 19 || state == 24 || state == 26 ||
               state == 27 || state == 29 || state == 30 || state == 31 || state == 33 || state == 34) {
        FIELD(entity, 0x58, float) = angle;
    }
}

#define EMIT_EFFECT(member) do { \
    if (FIELD(entity->field18, member, u32) != 0) { \
        GeorgeMathVec3 position; void *effect; u32 key; \
        ENSURE_EFFECT(); \
        effect = D_003F2D40; \
        key = FIELD(entity->field18, member, u32); \
        position.x = FIELD(entity, 0x40, float); \
        position.y = FIELD(entity, 0x44, float); \
        position.z = FIELD(entity, 0x48, float); \
        func_00251DB0(effect, key, &position); \
    } \
} while (0)
void func_001787B0(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
    u32 key = FIELD(entity, 0x3BC, u32);
    if ((flags & 0x200000ULL) != 0) {
        if (key == 0x728F0147U) EMIT_EFFECT(0x1C4);
        else if (key == 0x260105ECU) EMIT_EFFECT(0x1C8);
    } else {
        if (key == 0x728F0147U) EMIT_EFFECT(0x1B8);
        else if (key == 0x260105ECU) EMIT_EFFECT(0x1BC);
        else if (key == 0xEC789588U) EMIT_EFFECT(0x1C0);
    }
}

void func_00178D10(GeorgeGoalEntity *entity, u32 hidden)
{
    void *object;
    u32 visible = hidden == 0;
    s32 index;
    u8 *record;
    if (hidden != 0) FIELD(entity, 0x190, GeorgeActorBits64) |= 1ULL << 33;
    else FIELD(entity, 0x190, GeorgeActorBits64) &= ~(1ULL << 33);
    object = FIELD(entity, 0x228, void *); if (object != 0) func_00238E38(object, visible);
    object = FIELD(entity, 0x29C, void *); if (object != 0) func_00238E38(object, visible);
    object = FIELD(entity, 0x2A0, void *); if (object != 0) func_00238E38(object, visible);
    object = FIELD(entity, 0x2A4, void *); if (object != 0) func_00238E38(object, visible);
    record = ADDRESS(entity, 0x2B4);
    for (index = 0; index < FIELD(entity, 0x2C0, s16); ++index) {
        object = FIELD(record, 0, void *);
        record = ADDRESS(record, 12);
        func_00238E38(object, visible);
    }
}

/* Identical complete reset block in 178E80 and 192B28. Object+0xA0 is an
 * inline component, with its own table at that offset. */
#define RESET_POSE(entity) do { \
    GeorgeActorControlObject *control; const GeorgeGoalVirtualVoid *pair; void *object; \
    FIELD(entity, 0x190, GeorgeActorBits64) |= 1ULL << 32; \
    control = CONTROL(entity); \
    pair = (const GeorgeGoalVirtualVoid *)ADDRESS(control->field00, 0xF0); \
    pair->invoke(ADJUST(control, pair->adjustment)); \
    object = FIELD(entity, 0x1A0, void *); \
    if (object != 0) { \
        GeorgeMathVec4 zero; \
        const GeorgeActorVirtualVec4Input *vector_pair; \
        FIELD(&zero, 0, u32) = 0; FIELD(&zero, 4, u32) = 0; \
        FIELD(&zero, 8, u32) = 0; FIELD(&zero, 12, u32) = 0; \
        func_00307850(object); \
        vector_pair = (const GeorgeActorVirtualVec4Input *)ADDRESS(FIELD(object, 0xA0, const void *), 0x80); \
        vector_pair->invoke(ADJUST(ADDRESS(object, 0xA0), vector_pair->adjustment), &zero); \
    } \
} while (0)
void func_00192B28(GeorgeGoalEntity *entity) { RESET_POSE(entity); }
void func_00178E80(GeorgeGoalEntity *entity, u32 word, float blend)
{
    const GeorgeRotationMatrix *matrix;
    void *primary;
    FIELD(entity, 0x370, float) = blend;
    RESET_POSE(entity);
    func_00272390(FIELD(entity, 0x1B0, void *), word, 0, 0, func_00192BC0, entity, 0, 0, 0, 0.0f, 5.0f);
    func_00177C40(entity, word, 0, 0, 1, 0, 0, 5.0f);
    matrix = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xF0);
    primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x190, GeorgeActorBits64) |= 0x4000000ULL;
    FIELD(primary, 0x428, float) = FIELD(entity, 0x370, float);
    FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 1;
    func_0021BF28(FIELD(entity, 0x384, void *), VECTOR(entity, 0x40), 1.0f);
    func_0026FE30(FIELD(entity, 0x1B0, void *), matrix, 0, FIELD(entity, 0x35C, float), 0.0f);
    if (FIELD(entity, 0x1B4, void *) != 0) {
        u32 companion = func_00195D88(entity, word);
        func_00272390(FIELD(entity, 0x1B4, void *), companion, 0, 0, 0, 0, 0, 0, 0, 0.0f, 5.0f);
        FIELD(FIELD(entity, 0x1B4, void *), 0x428, float) = FIELD(entity, 0x370, float);
        FIELD(FIELD(entity, 0x1B4, void *), 0x434, u32) = 1;
        func_0026FE30(FIELD(entity, 0x1B4, void *), matrix, 0, FIELD(entity, 0x35C, float), 0.0f);
    }
}

void func_00179168(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x1F4, u32) == 2) {
        GeorgeRotationMatrix matrix;
        GeorgeMathVec4 start;
        GeorgeMathVec3 end;
        void *primary = FIELD(entity, 0x1B0, void *);
        u32 index = FIELD(primary, 0x0C, u32);
        const void *array = FIELD(primary, 0x3D8U + (index << 2), const void *);
        const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(array, FIELD(entity, 0x3F8, u32) << 6);
        func_002A2200(&matrix, source, (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0));
        start.x = matrix.element[12]; start.y = matrix.element[13] + 1.0f;
        start.z = matrix.element[14]; start.w = 2.0f;
        end.x = start.x; end.y = start.y; end.y = end.y + 3000.0f; end.z = start.z;
        func_002B85D0(FIELD(entity, 0x1F8, void *), &end, &start, 2.0f);
        FIELD(entity, 0x1FC, u32) = 1;
    }
}
void func_00190FC0(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration = func_002A6E60(source);
    (void)unused;
    FIELD(entity, 0x2DC, u32) = 0;
    FIELD(entity, 0x2E8, float) = duration * 0.000208333338377997279f;
}
void func_00192BC0(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration = func_002A6E60(source);
    float scaled = duration * 0.000208333338377997279f;
    float blend = FIELD(entity, 0x370, float);
    (void)unused;
    FIELD(entity, 0x710, u32) = 102;
    FIELD(entity, 0x714, float) = scaled - scaled * blend;
}

#define NOTIFY_HEALTH(entity) do { \
    if (FIELD(entity, 0x38C, GeorgeDeimosPoolNode *) != 0) { \
        float notice_scalar = FIELD(entity, 0x36C, float); \
        GeorgeDeimosValue *arguments = D_00474F48; \
        arguments[0].payload.scalar = notice_scalar; \
        arguments[0].tag = 2; arguments[0].subtype = 0; \
        func_002D02A8(FIELD(entity, 0x38C, GeorgeDeimosPoolNode *), 1, 0); \
    } \
} while (0)
void func_00191F78(GeorgeGoalEntity *entity, float value)
{
    float previous = FIELD(entity, 0x36C, float);
    float offset = FIELD(entity->field18, 0x404, float);
    FIELD(entity, 0x36C, float) = previous < value ? value + offset : value - offset;
    NOTIFY_HEALTH(entity);
}
void func_00191FF0(GeorgeGoalEntity *entity, float change)
{
    float previous = FIELD(entity, 0x36C, float);
    float value = george_ee_minimum(previous + change, 1.0f);
    float offset = FIELD(entity->field18, 0x404, float);
    FIELD(entity, 0x36C, float) = previous < value ? value + offset : value - offset;
    NOTIFY_HEALTH(entity);
}
void func_00192078(GeorgeGoalEntity *entity, u32 key, float change)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((flags & 0x200ULL) == 0 && (flags & 0x100ULL) == 0) {
        void *object = FIELD(entity, 0x28C, void *);
        float previous, value, offset;
        GeorgeGoalEntityData *data;
        if (object != 0) { func_002393F8((u32)object); FIELD(entity, 0x28C, void *) = 0; }
        object = FIELD(entity, 0x290, void *);
        if (object != 0) { func_002393F8((u32)object); FIELD(entity, 0x290, void *) = 0; }
        data = entity->field18;
        previous = FIELD(entity, 0x36C, float);
        value = previous - change * FIELD(data, 0x198, float);
        offset = FIELD(data, 0x404, float);
        FIELD(entity, 0x36C, float) = previous < value ? value + offset : value - offset;
        NOTIFY_HEALTH(entity);
        FIELD(entity, 0x3C0, u32) = key;
    }
}
void func_00191E88(GeorgeGoalEntity *entity, u32 notify, float change)
{
    if (change <= 0.0f || entity->field0C == 13) return;
    func_00192078(entity, 0xEC789588U, change);
    if (FIELD(entity, 0x3A0, GeorgeDeimosPoolNode *) != 0 && notify != 0) {
        GeorgeDeimosPoolNode *result = func_002D0790((GeorgeScriptObject *)entity);
        GeorgeDeimosValue *arguments = D_00474F48;
        arguments[0].tag = 4; arguments[0].payload.pointer = result; arguments[0].subtype = 0;
        func_002D02A8(FIELD(entity, 0x3A0, GeorgeDeimosPoolNode *), 1, 0);
    }
    if (FIELD(entity->field18, 0x154, u32) != 0) func_00195850(entity, 0x51, 0xF114E180U, 0);
    else if (entity->field0C != 13 && entity->field0C != 37) {
        GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
        FIELD(entity, 0x590, u32) = 0x51;
        FIELD(entity, 0x190, GeorgeActorBits64) = flags | 0x8000000ULL;
    }
}
void func_00192180(GeorgeGoalEntity *entity, u32 key)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x3C0, u32) = key;
    FIELD(entity, 0x36C, u32) = 0;
    if ((flags & (0x200ULL | 0x100ULL | 0x20ULL)) != 0) return;
    if (key == 0) FIELD(entity, 0x3BC, u32) = 0x728F0147U;
    else if (key == 0x260105ECU || key == 0xEC789588U || key == 0xCAD99652U ||
             key == 0xCB426EF9U || key == 0x1111150CU || key == 0x75188880U ||
             key == 0x3BAC798CU || key == 0x728F0147U || key == 0x794F22DAU || key == 0x7F9000CFU)
        FIELD(entity, 0x3BC, u32) = key;
}
s32 func_00195850(GeorgeGoalEntity *entity, u32 word, u32 key, u32 emit)
{
    void *reference;
    if (FIELD(entity, 0x2D8, u32) != 0 || entity->field0C == 13) return 0;
    FIELD(entity, 0x2D8, u32) = 2; FIELD(entity, 0x2DC, u32) = 1;
    reference = func_0023C230((void *)key, 0xBCF06440U);
    if (reference != 0 && FIELD(entity, 0x1B0, void *) != 0) {
        func_00272970(FIELD(entity, 0x1B0, void *), word, 0, 0, FIELD(entity, 0x368, u32), reference, func_00190FC0, entity);
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00272970(FIELD(entity, 0x1B4, void *), word, 0, 0, FIELD(entity, 0x368, u32), reference, 0, 0);
    }
    if (FIELD(entity->field18, 0x1B4, u32) != 0 && emit != 0 &&
        (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000ULL) == 0)
        func_001B67C0(ADDRESS(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                     FIELD(entity, 0x3B4, u32), FIELD(entity->field18, 0x1B4, u32), 0, 0);
    return 1;
}
void *func_00195DC8(GeorgeGoalEntity *entity, GeorgeMathVec3 *point_output,
                   GeorgeMathVec3 *normal_output, u32 mode, float threshold)
{
    GeorgeGoalEntityData *data = entity->field18;
    if (FIELD(data, 0x1D8, u32) == 0x45A78000U) {
        float vertical = 0.0f, forward = 0.0f;
        GeorgeMathVec3 point;
        if (mode != 0) { vertical = FIELD(data, 0x230, float); forward = FIELD(data, 0x234, float); }
        point.y = (FIELD(entity, 0x44, float) + forward * FIELD(entity, 0xD4, float)) + vertical;
        point.z = FIELD(entity, 0x48, float) + forward * FIELD(entity, 0xD8, float);
        point.x = FIELD(entity, 0x40, float) + forward * FIELD(entity, 0xD0, float);
        return func_0013D328(FIELD(CONTROL(entity), 0x48, void *), &point,
                            point_output, normal_output, mode, threshold);
    }
    return 0;
}
