#include "george/actor_states5.h"

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern u8 D_003F83F0[];
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042DA38[];
extern void *func_002AEE60(u32 size);
extern void *func_002481F0(void *object);
extern s32 func_00100AA8(const void *first, const void *second);
extern void func_001007E0(GeorgeActorPointerRange *range, void **position, void *const *value);
extern void func_002BD340(void);
extern s32 func_00396260(void (*callback)(void));
extern u32 *func_002BEBA0(u32 *output, const u8 *text);
extern void func_00251CC8(void *object, u32 key);
extern s32 func_00270510(void *object, u32 word, u32 mode0, u32 mode1,
                       u32 owner_word, GeorgeActorRequestCallback callback,
                       GeorgeGoalEntity *context, u32 invoke_word,
                       u32 callback_word, float time);
extern void func_002727D8(void *object);
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_0018FD40(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern float func_002A6E60(void *source);
extern float func_0029B940(float first, float second);
extern GeorgeActorBits64 func_00374848(float value);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372C68(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern s32 func_00373250(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern void func_00119888(void *object, GeorgeMathVec3 *output, float first, float second);
extern u32 func_00119A58(void *object, const GeorgeMathVec3 *vector);
extern s32 func_00119BC8(void *object, const GeorgeMathVec3 *point, s32 *direction);
extern s32 func_00119D58(void *object, const GeorgeMathVec3 *point, s32 *direction);
extern float func_0011A6E8(void *object, s32 direction, const GeorgeMathVec3 *point);
extern u32 func_0011A7A8(void *object, s32 direction);
extern void func_0013D118(void *object, const GeorgeMathVec3 *input,
                        GeorgeMathVec3 *point, GeorgeMathVec3 *normal, float unused);
extern const GeorgeActorUnalignedPosition D_0042D8D8;
extern void *func_003936A0(void *memory, s32 value, u32 size);
extern void func_001A5E90(void *object, void *value);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define PAIR(object, offset, type) ((const type *)ADDRESS((object)->field00, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))

ACTOR_INLINE void control_void(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualVoid *pair = PAIR(object, offset, GeorgeGoalVirtualVoid);
    pair->invoke(ADJUST(object, pair->adjustment));
}
ACTOR_INLINE void control_word(GeorgeGoalEntity *entity, u32 offset, u32 word)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualWord *pair = PAIR(object, offset, GeorgeGoalVirtualWord);
    pair->invoke(ADJUST(object, pair->adjustment), word);
}
ACTOR_INLINE s32 control_word_result(GeorgeGoalEntity *entity, u32 offset, u32 word)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualWordResult *pair = PAIR(object, offset, GeorgeActorVirtualWordResult);
    return pair->invoke(ADJUST(object, pair->adjustment), word);
}
ACTOR_INLINE void control_vector(GeorgeGoalEntity *entity, u32 offset, const GeorgeMathVec3 *vector)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualVectorInput *pair = PAIR(object, offset, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}
ACTOR_INLINE s32 control_int(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualInt *pair = PAIR(object, offset, GeorgeGoalVirtualInt);
    return pair->invoke(ADJUST(object, pair->adjustment));
}
ACTOR_INLINE s32 guarded_predicate(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((flags & 0x80ULL) == 0 && (flags & 0x80000ULL) == 0) {
        GeorgeActorControlObject *object = CONTROL(entity);
        const GeorgeActorVirtualPredicate *pair = PAIR(object, 0xD0, GeorgeActorVirtualPredicate);
        return pair->invoke(ADJUST(object, pair->adjustment), 0.20000000298023224f, 0.25f);
    }
    return 0;
}
ACTOR_INLINE s32 request_pair(GeorgeGoalEntity *entity, void *captured_main,
                             u32 word, GeorgeActorRequestCallback callback)
{
    if (captured_main != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                         FIELD(entity, 0x368, u32), 0, 0, 0, 0, 0.0f);
        return func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                            FIELD(entity, 0x368, u32), callback,
                            callback != 0 ? entity : 0, 0, 0, 0.0f);
    }
    return 0;
}
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
ACTOR_INLINE void ensure_effect(void)
{
    if (D_003F2D40 == 0) {
        GeorgeActorEffectRecord *record;
        void *registered, *value;
        void **begin, **end, **position;
        D_003F2D40 = func_002481F0(func_002AEE60(0x1B4));
        record = (GeorgeActorEffectRecord *)func_002AEE60(12);
        registered = D_003F2D40; begin = D_0046A0F0.field00; end = D_0046A0F0.field04;
        record->field00 = 9; record->field04 = D_00421160; record->field08 = registered;
        value = record;
        position = func_00100C30(begin, end, &value, func_00100AA8);
        if (D_0046A0F0.field04 != D_0046A0F0.field08 && position == D_0046A0F0.field04) {
            if (position != 0) *position = value;
            D_0046A0F0.field04 = (void **)ADDRESS(D_0046A0F0.field04, 4);
        } else func_001007E0(&D_0046A0F0, position, &value);
        func_00396260(func_002BD340);
    }
}

void func_00195ED8(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x9EC, u32) = 0;
    control_word(entity, 0x30, 0);
}
void func_00195F10(GeorgeGoalEntity *entity)
{
    void *primary = FIELD(entity, 0x1B0, void *);
    if (primary != 0) func_002727D8(primary);
    control_word(entity, 0x30, 1);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_00195E88(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration; (void)unused;
    FIELD(entity, 0x880, u32) = FIELD(entity, 0x880, u32) + 1u;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0x870, float) = duration;
    FIELD(entity, 0x86C, float) = duration;
}
#define AFTER_DURATION(name, phase_offset, timer_offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration; (void)unused; \
    duration = func_002A6E60(source) * 0.000208333338377997279f; \
    FIELD(entity, phase_offset, u32) = FIELD(entity, phase_offset, u32) + 1u; \
    FIELD(entity, timer_offset, float) = duration; \
}
AFTER_DURATION(func_00195F68, 0x8B8, 0x8BC)
AFTER_DURATION(func_00195FE8, 0x8E0, 0x8F0)

void func_0018ACC8(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x24, void *);
    GeorgeMathVec3 vector;
    float target;
    vector.x = FIELD(object, 0xFC, float);
    vector.y = FIELD(object, 0x100, float);
    vector.z = FIELD(object, 0x104, float);
    vector.y = 0.0f;
    func_002A3538(&vector);
    target = func_0029B940(-vector.z, -vector.x);
    if (3.14159274101257324f < target) target = func_0029B940(-vector.z, -vector.x) - 6.28318548202514648f;
    else {
        target = func_0029B940(-vector.z, -vector.x);
        if (target < -3.14159274101257324f) target = func_0029B940(-vector.z, -vector.x) + 6.28318548202514648f;
        else target = func_0029B940(-vector.z, -vector.x);
    }
    FIELD(entity, 0x58, float) = target;
    FIELD(entity, 0x8B8, u32) = 100;
    FIELD(entity, 0x8C0, float) = target;
    FIELD(entity, 0x8BC, u32) = 0;
    FIELD(entity, 0x8C4, u32) = 0; FIELD(entity, 0x8CC, u32) = 0; FIELD(entity, 0x8C8, u32) = 0;
    FIELD(entity, 0x8D0, u32) = 0; FIELD(entity, 0x8D8, u32) = 0; FIELD(entity, 0x8D4, u32) = 0;
}
void func_00195FB0(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x14, u32) = 3;
    func_002727D8(FIELD(entity, 0x1B0, void *));
    FIELD(entity, 0x24, void *) = 0;
}
void func_00196030(GeorgeGoalEntity *entity)
{
    float angle = FIELD(entity, 0x8F4, float);
    FIELD(entity, 0x8E0, u32) = 100;
    FIELD(entity, 0x58, float) = angle;
    FIELD(entity, 0x9EC, u32) = 0;
    FIELD(entity, 0x8F0, u32) = 0;
    FIELD(entity, 0x8EC, u32) = 0;
}
void func_00196050(GeorgeGoalEntity *entity)
{
    func_002727D8(FIELD(entity, 0x1B0, void *));
    FIELD(entity, 0x8F8, u32) = 0; FIELD(entity, 0x900, u32) = 0; FIELD(entity, 0x8FC, u32) = 0;
}
void func_0018B528(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0x8E0, s32);
    if (phase == 100) {
        u32 word = FIELD(entity, 0x8E4, u32);
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = word;
        if (request_pair(entity, primary, word, func_00195FE8) == 0) FIELD(entity, 0x8E0, u32) = 102;
        else control_vector(entity, 0x98, VECTOR(entity, 0x8F8));
    } else if (phase == 101) {
        float step = FIELD(entity, 0x35C, float);
        float timer = FIELD(entity, 0x8F0, float) - step;
        float remaining = FIELD(entity, 0x8E8, float) - step;
        float elapsed = FIELD(entity, 0x8EC, float) + step;
        float angle = FIELD(entity, 0x8F4, float);
        FIELD(entity, 0x8F0, float) = timer;
        FIELD(entity, 0x58, float) = angle;
        FIELD(entity, 0x8EC, float) = elapsed;
        FIELD(entity, 0x8E8, float) = remaining;
        if (!(0.0f <= timer)) FIELD(entity, 0x8E0, u32) = 102;
        else {
            if (FIELD(entity, 0x50, float) < FIELD(entity, 0x3C4, float))
                FIELD(entity, 0x3C4, float) = FIELD(entity, 0x50, float);
            control_word(entity, 0xB8, 0);
            if (control_int(entity, 0xD8) != 0 && 0.100000001490116119f < FIELD(entity, 0x8EC, float)) {
                u32 next = FIELD(entity, 0x8E0, u32) + 1u;
                FIELD(entity, 0x14, u32) = 5;
                FIELD(entity, 0x8E0, u32) = next;
            }
        }
    } else if (phase == 102) entity->field0C = 3;
}

void func_001960E8(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0xA14, u32) = 100;
    FIELD(entity, 0xA18, u32) = 0; FIELD(entity, 0xA1C, u32) = 0;
    func_0018FD30(entity, 0x40000ULL);
}
void func_00196088(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration, value;
    GeorgeGoalEntityData *data;
    (void)unused;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    data = entity->field18;
    FIELD(entity, 0xA18, float) = duration;
    value = FIELD(data, 0x1B0, float);
    FIELD(entity, 0xA14, u32) = 200;
    FIELD(entity, 0xA20, float) = value / duration;
}
void func_00196120(GeorgeGoalEntity *entity)
{
    control_void(entity, 0x88);
    func_0018FD40(entity, 0x40000ULL);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_0018B7E8(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0xA14, s32);
    if (phase == 100) {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0xC1;
        if (request_pair(entity, primary, 0xC1, func_00196088) == 0) FIELD(entity, 0xA14, u32) = 201;
    } else if (phase == 200) {
        GeorgeMathVec3 *vector = VECTOR(entity, 0x478);
        control_void(entity, 0x88);
        func_002A35C0(vector, vector, FIELD(entity->field18, 0x1B0, float) / FIELD(entity, 0xA18, float));
        control_vector(entity, 0xB0, vector);
        FIELD(entity, 0xA14, u32) = FIELD(entity, 0xA14, u32) + 1u;
    } else if (phase == 201) {
        float step = FIELD(entity, 0x35C, float);
        float elapsed = FIELD(entity, 0xA1C, float) + step;
        float duration = FIELD(entity, 0xA18, float);
        FIELD(entity, 0xA1C, float) = elapsed;
        if (elapsed < duration) {
            GeorgeActorControlObject *object = CONTROL(entity);
            const GeorgeGoalVirtualFloat *pair = PAIR(object, 0xC0, GeorgeGoalVirtualFloat);
            pair->invoke(ADJUST(object, pair->adjustment), step);
        } else FIELD(entity, 0xA14, u32) = 202;
    } else if (phase == 202) {
        u32 state;
        func_00170538(entity);
        state = guarded_predicate(entity) != 0 ? 3 : 0;
        entity->field0C = state;
        actor_member(entity, state);
    }
}

void func_0018BC28(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x904, u32) != 101) {
        u32 key;
        void *effect;
        ensure_effect();
        effect = D_003F2D40;
        func_002BEBA0(&key, D_0042DA38);
        func_00251CC8(effect, key);
    }
    {
        GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
        FIELD(entity, 0x904, u32) = 100;
        if ((flags & 0x20000ULL) == 0 && control_word_result(entity, 0x50, 1) != 0)
            func_0018FD30(entity, 0x20000ULL);
    }
}
void func_0018BDA0(GeorgeGoalEntity *entity)
{
    s32 phase;
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0x904, s32);
    if (phase == 100) {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0xC0;
        if (request_pair(entity, primary, 0xC0, 0) != 0)
            FIELD(entity, 0x904, u32) = FIELD(entity, 0x904, u32) + 1u;
        else FIELD(entity, 0x904, u32) = 200;
    } else if (phase == 101) {
        if (guarded_predicate(entity) != 0) FIELD(entity, 0x904, u32) = 200;
    } else if (phase == 200) FIELD(entity, 0x14, u32) = guarded_predicate(entity) != 0 ? 3 : 0;
}
void func_00196210(GeorgeGoalEntity *entity)
{
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x20000ULL) != 0 && control_word_result(entity, 0x50, 0) != 0)
        func_0018FD40(entity, 0x20000ULL);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}

ACTOR_INLINE GeorgeActorBits64 soft_absolute(float value)
{
    GeorgeActorBits64 bits = func_00374848(value);
    if (func_00373250(bits, 0) < 0) bits = func_00372CC0(0, bits);
    return bits;
}

void func_0018AE08(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x8BC, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0x8B8, s32);
    void *object;
    GeorgeMathVec3 point;
    float weight;
    GeorgeMathVec3 *vector = VECTOR(entity, 0x8C4);
    GeorgeMathVec3 *position = VECTOR(entity, 0x40);
    FIELD(entity, 0x8BC, float) = timer;
    if (phase == 100) {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x8F;
        request_pair(entity, primary, 0x8F, func_00195F68);
        return;
    }
    if (phase != 101) return;
    object = FIELD(entity, 0x24, void *);
    if (object == 0) {
        GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 38);
        FIELD(entity, 0x14, u32) = 3;
        FIELD(entity, 0x190, GeorgeActorBits64) = flags;
        return;
    }
    {
        const GeorgeActorVirtualPointVectorScalar *pair = (const GeorgeActorVirtualPointVectorScalar *)
            ADDRESS(FIELD(object, 4, const u8 *), 0x98);
        if (pair->invoke(ADJUST(object, pair->adjustment), position, &point, &weight) == 0) {
            GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 38);
            FIELD(entity, 0x24, void *) = 0;
            FIELD(entity, 0x14, u32) = 3;
            FIELD(entity, 0x190, GeorgeActorBits64) = flags;
            FIELD(entity, 0x34, u32) = 0;
            FIELD(entity, 0x38, u32) = 0;
            return;
        }
    }
    {
        GeorgeActorBits64 first = soft_absolute(FIELD(entity, 0x38, float));
        GeorgeActorBits64 second = soft_absolute(FIELD(entity, 0x34, float));
        GeorgeActorBits64 total = func_00372C68(first, second);
        if (func_00373250(total, 0x3FB99999A0000000ULL) > 0) {
            float scalar, x, y, z, captured_weight;
            s32 direction;
            func_00119888(FIELD(entity, 0x24, void *), vector,
                          FIELD(entity, 0x38, float), FIELD(entity, 0x34, float));
            scalar = FIELD(entity->field18, 0x1C, float);
            x = vector->x;
            captured_weight = weight;
            vector->x = x * scalar;
            y = vector->y * scalar;
            z = vector->z * scalar;
            vector->y = y;
            vector->z = z;
            if (captured_weight < 0.49000000953674316f) {
                void *fresh_object = FIELD(entity, 0x24, void *);
                float blend = 0.5f - captured_weight;
                float ox = FIELD(fresh_object, 0xFC, float);
                float oz = FIELD(fresh_object, 0x104, float);
                float oy = FIELD(fresh_object, 0x100, float);
                float nx = vector->x + blend * ox;
                float nz = z + blend * oz;
                float ny = y + blend * oy;
                vector->x = nx;
                vector->z = nz;
                vector->y = ny;
            }
            if (func_00119BC8(FIELD(entity, 0x24, void *), &point, &direction) != 0 &&
                0.0f < func_00119EE8(FIELD(entity, 0x24, void *), direction, vector)) {
                s32 captured_direction = direction;
                float factor = FIELD(entity, 0x35C, float) * FIELD(entity->field18, 0x1C, float);
                GeorgeMathVec3 predicted;
                float vx = vector->x, vy = vector->y, vz = vector->z;
                float pz = position->z, px = position->x, py = position->y;
                predicted.z = pz + factor * vz;
                predicted.x = px + factor * vx;
                predicted.y = py + factor * vy;
                if ((u32)captured_direction - 2u < 2u) {
                    if (func_0011A6E8(FIELD(entity, 0x24, void *), captured_direction, &predicted)
                        < 0.009999999776482582f) {
                        func_002A35C0(VECTOR(entity, 0x8D0), vector, -1.0f);
                        vector->x = 0.0f;
                        vector->z = 0.0f;
                        vector->y = 0.0f;
                        FIELD(entity, 0x190, GeorgeActorBits64) |= 1ULL << 38;
                    } else {
                        u32 word = func_0011A7A8(FIELD(entity, 0x24, void *), direction);
                        void *primary = FIELD(entity, 0x1B0, void *);
                        FIELD(entity, 0x4E4, u32) = word;
                        request_pair(entity, primary, word, 0);
                    }
                } else goto choose_vector_word;
            } else if (func_00119D58(FIELD(entity, 0x24, void *), &point, &direction) != 0 &&
                       0.0f < func_00119EE8(FIELD(entity, 0x24, void *), direction, vector)) {
                GeorgeActorBits64 flags;
                void *primary;
                vector->x = 0.0f;
                vector->y = 0.0f;
                vector->z = 0.0f;
                FIELD(entity, 0x4E4, u32) = 0x8C;
                flags = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 38);
                primary = FIELD(entity, 0x1B0, void *);
                FIELD(entity, 0x190, GeorgeActorBits64) = flags;
                request_pair(entity, primary, 0x8C, 0);
            } else {
choose_vector_word:
                {
                    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 38);
                    void *fresh_object = FIELD(entity, 0x24, void *);
                    u32 word;
                    void *primary;
                    FIELD(entity, 0x190, GeorgeActorBits64) = flags;
                    word = func_00119A58(fresh_object, vector);
                    primary = FIELD(entity, 0x1B0, void *);
                    FIELD(entity, 0x4E4, u32) = word;
                    request_pair(entity, primary, word, 0);
                }
            }
            control_vector(entity, 0x78, vector);
        } else {
            s32 direction;
            if (func_00119BC8(FIELD(entity, 0x24, void *), &point, &direction) != 0 &&
                func_0011A6E8(FIELD(entity, 0x24, void *), direction, position)
                    < 0.1899999976158142f) {
                control_vector(entity, 0x78, VECTOR(entity, 0x8D0));
            } else {
                vector->x = 0.0f;
                vector->y = 0.0f;
                vector->z = 0.0f;
                control_vector(entity, 0x78, vector);
            }
            {
                void *primary = FIELD(entity, 0x1B0, void *);
                FIELD(entity, 0x4E4, u32) = 0x8C;
                request_pair(entity, primary, 0x8C, 0);
            }
            FIELD(entity, 0x190, GeorgeActorBits64) &= ~(1ULL << 38);
        }
    }
    FIELD(entity, 0x34, u32) = 0;
    FIELD(entity, 0x38, u32) = 0;
}

/* This private source template retains the two call occurrences' shared local
 * output objects: callees may retain them and change their contents later. */
#define QUERY_AHEAD() do { \
    void *data = entity->field18; \
    GeorgeMathVec3 *direction = VECTOR(entity, 0xD0); \
    float scalar = FIELD(data, 0x234, float); \
    float dz = direction->z, dy = direction->y, dx = direction->x; \
    float px = position->x, py = position->y, pz = position->z; \
    input.y = py + scalar * dy; \
    input.x = px + scalar * dx; \
    input.z = pz + scalar * dz; \
    input.y += FIELD(data, 0x230, float); \
    func_0013D118(FIELD(entity, 0x868, void *), &input, &point, &normal, \
                  FIELD(data, 0x244, float)); \
} while (0)

void func_00189E88(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 input, point, normal, delta, zero;
    GeorgeMathVec3 *position = VECTOR(entity, 0x40);
    GeorgeMathVec3 *previous = VECTOR(entity, 0x890);
    GeorgeMathVec3 *offset = VECTOR(entity, 0x884);
    union { GeorgeActorUnalignedPosition bits; GeorgeMathVec3 vector; } up;
    s32 phase;
    FIELD(entity, 0x86C, float) -= FIELD(entity, 0x35C, float);
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0x880, s32);
    if (phase == 100) {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x9B;
        request_pair(entity, primary, 0x9B, func_00195E88);
        FIELD(entity, 0x58, float) = FIELD(entity, 0x87C, float);
    } else if (phase == 101 || phase == 200) {
        GeorgeMathVec3 *target = phase == 101 ? previous : position;
        float y, z, scalar, nx, ny, nz;
        QUERY_AHEAD();
        up.bits.field00 = D_0042D8D8.field00;
        up.bits.field08 = D_0042D8D8.field08;
        target->x = point.x;
        y = point.y;
        target->y = y;
        z = point.z;
        target->z = z;
        scalar = -FIELD(entity->field18, 0x230, float);
        nx = target->x + scalar * up.vector.x;
        nz = z + scalar * up.vector.z;
        ny = y + scalar * up.vector.y;
        target->x = nx;
        target->z = nz;
        target->y = ny;
        scalar = FIELD(entity->field18, 0x234, float);
        nx = target->x + scalar * normal.x;
        ny += scalar * normal.y;
        nz += scalar * normal.z;
        target->x = nx;
        if (phase == 101) {
            target->y = ny;
            target->z = nz;
            delta.z = (nz - position->z) * 10.0f;
            delta.x = (previous->x - position->x) * 10.0f;
            delta.y = (ny - position->y) * 10.0f;
            control_vector(entity, 0x78, &delta);
            if (FIELD(entity, 0x86C, float) <= 0.0f) FIELD(entity, 0x880, u32) = 200;
        } else {
            GeorgeActorBits64 flags;
            void *primary;
            target->z = nz;
            target->y = ny;
            FIELD(entity, 0x4E4, u32) = 0x9A;
            primary = FIELD(entity, 0x1B0, void *);
            request_pair(entity, primary, 0x9A, 0);
            delta.x = 0.0f; delta.y = 0.0f; delta.z = 0.0f;
            control_vector(entity, 0x78, &delta);
            flags = FIELD(entity, 0x190, GeorgeActorBits64);
            if ((flags & 0x10000ULL) != 0) FIELD(entity, 0x880, u32) = 300;
            else if ((flags & 0x8000ULL) != 0) {
                primary = FIELD(entity, 0x1B0, void *);
                FIELD(entity, 0x880, u32) = 250;
                FIELD(entity, 0x4E4, u32) = 0x10;
                request_pair(entity, primary, 0x10, 0);
            }
        }
    } else if (phase == 250) {
        float radius, height, ceiling;
        control_vector(entity, 0xB8, VECTOR(entity, 0x68));
        radius = FIELD(entity->field18, 0x244, float);
        ceiling = FIELD(entity, 0x8A0, float);
        height = FIELD(entity->field18, 0x230, float);
        if (position->y + height < ceiling - (radius + radius)) {
            func_00170538(entity);
            entity->field0C = 3;
            actor_member(entity, 3);
        }
        /* The original continues to this callback after switching state. */
        if (control_int(entity, 0xD8) != 0) {
            func_00170538(entity);
            entity->field0C = 5;
            actor_member(entity, 5);
        }
    } else if (phase == 300) {
        float scalar, x, y, z;
        void *primary;
        void *data = entity->field18;
        GeorgeMathVec3 *direction = VECTOR(entity, 0xD0);
        scalar = FIELD(data, 0x234, float);
        input.y = position->y + scalar * direction->y;
        input.x = position->x + scalar * direction->x;
        input.z = position->z + scalar * direction->z;
        input.y += FIELD(data, 0x230, float);
        func_0013D118(FIELD(entity, 0x868, void *), &input, previous, &normal,
                      FIELD(data, 0x244, float));
        func_003936A0(&zero, 0, 12);
        zero.y = FIELD(entity->field18, 0x230, float);
        up.bits.field00 = ((GeorgeActorUnalignedPosition *)&zero)->field00;
        up.bits.field08 = ((GeorgeActorUnalignedPosition *)&zero)->field08;
        offset->x = up.vector.x;
        y = up.vector.y; offset->y = y;
        z = up.vector.z; offset->z = z;
        scalar = -FIELD(entity->field18, 0x238, float);
        x = offset->x + scalar * normal.x;
        z += scalar * normal.z;
        y += scalar * normal.y;
        offset->x = x; offset->z = z; offset->y = y;
        FIELD(entity, 0x4E4, u32) = 0xA3;
        primary = FIELD(entity, 0x1B0, void *);
        request_pair(entity, primary, 0xA3, func_00195E88);
        delta.x = 0.0f; delta.y = 0.0f; delta.z = 0.0f;
        control_vector(entity, 0x78, &delta);
        input.x = position->x; input.y = position->y; input.z = position->z;
        func_0013D118(FIELD(entity, 0x868, void *), &input, &point, &normal,
                      FIELD(entity->field18, 0x244, float));
        delta.x = point.x - previous->x;
        y = previous->y; z = previous->z;
        delta.y = point.y - y; delta.z = point.z - z;
        x = position->x + delta.x;
        y = position->y + delta.y;
        z = position->z + delta.z;
        position->x = x; position->y = y; position->z = z;
        previous->x = point.x; previous->z = point.z; previous->y = point.y;
        scalar = FIELD(entity, 0x35C, float) / FIELD(entity, 0x870, float);
        z = offset->z; x = offset->x; y = offset->y;
        x = position->x + scalar * x;
        y = position->y + scalar * y;
        z = position->z + scalar * z;
        position->x = x; position->z = z; position->y = y;
    } else if (phase == 301 || phase == 401) {
        float scalar, x, y, z;
        delta.x = 0.0f; delta.y = 0.0f; delta.z = 0.0f;
        control_vector(entity, 0x78, &delta);
        input.x = position->x; input.y = position->y; input.z = position->z;
        func_0013D118(FIELD(entity, 0x868, void *), &input, &point, &normal,
                      FIELD(entity->field18, 0x244, float));
        delta.x = point.x - previous->x;
        y = previous->y; z = previous->z;
        delta.y = point.y - y; delta.z = point.z - z;
        x = position->x + delta.x;
        y = position->y + delta.y;
        z = position->z + delta.z;
        position->x = x; position->y = y; position->z = z;
        previous->x = point.x; previous->y = point.y; previous->z = point.z;
        scalar = FIELD(entity, 0x35C, float) / FIELD(entity, 0x870, float);
        if (phase == 401) z = offset->z;
        x = offset->x; y = offset->y;
        if (phase == 301) z = offset->z;
        x = position->x + scalar * x;
        y = position->y + scalar * y;
        z = position->z + scalar * z;
        position->x = x; position->z = z; position->y = y;
        if (phase == 301) {
            up.vector.x = offset->x; up.vector.y = offset->y;
            scalar = FIELD(entity, 0x86C, float);
            x = FIELD(entity, 0x35C, float); up.vector.z = offset->z;
            if (scalar < x) {
                func_001A5E90(CONTROL(entity), FIELD(entity, 0x868, void *));
                func_00170538(entity);
                entity->field0C = 0;
                actor_member(entity, 0);
            }
        } else if (FIELD(entity, 0x86C, float) <= 0.0f) FIELD(entity, 0x880, u32) = 200;
    } else if (phase == 400) {
        float scalar, x, y, z;
        void *primary;
        input.x = position->x; input.y = position->y; input.z = position->z;
        func_0013D118(FIELD(entity, 0x868, void *), &input, previous, &normal,
                      FIELD(entity->field18, 0x244, float));
        func_003936A0(&zero, 0, 12);
        zero.y = -FIELD(entity->field18, 0x230, float);
        up.bits.field00 = ((GeorgeActorUnalignedPosition *)&zero)->field00;
        up.bits.field08 = ((GeorgeActorUnalignedPosition *)&zero)->field08;
        offset->x = up.vector.x;
        y = up.vector.y; offset->y = y;
        z = up.vector.z; offset->z = z;
        scalar = FIELD(entity->field18, 0x234, float);
        x = offset->x + scalar * normal.x;
        y += scalar * normal.y; z += scalar * normal.z;
        offset->x = x; offset->y = y; offset->z = z;
        FIELD(entity, 0x4E4, u32) = 0xA4;
        x = FIELD(entity, 0x87C, float);
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x58, float) = x;
        request_pair(entity, primary, 0xA4, func_00195E88);
    }
}
