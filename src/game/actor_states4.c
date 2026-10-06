#include "george/actor_states4.h"

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042D8A8[], D_0042D8C0[];
extern void *func_002AEE60(u32 size);
extern void *func_002481F0(void *object);
extern s32 func_00100AA8(const void *first, const void *second);
extern void func_001007E0(GeorgeActorPointerRange *range, void **position, void *const *value);
extern void func_002BD340(void);
extern s32 func_00396260(void (*callback)(void));
extern u32 *func_002BEBA0(u32 *output, const u8 *text);
extern void *func_00251B58(void *object, u32 key, const GeorgeMathVec3 *position);
extern void func_002455C0(void *object);
extern void func_002457D8(void *object);
extern void func_00245B08(void *object);
extern void func_001B67C0(void *context, GeorgeGoalEntity *entity, u32 word0,
                        u32 word1, u32 word2, u32 word3);
extern s32 func_00270510(void *object, u32 word, u32 mode0, u32 mode1,
                       u32 owner_word, GeorgeActorRequestCallback callback,
                       GeorgeGoalEntity *context, u32 invoke_word,
                       u32 callback_word, float time);
extern void func_002727D8(void *object);
extern void func_002393F8(u32 word);
extern float func_002A6E60(void *source);
extern float func_0029B940(float first, float second);
extern void func_0018FD40(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_002A1C08(void *output, const void *input);
extern void *func_00210078(u32 word, u32 mode0, u32 mode1);
extern void *func_00236CB8(const void *matrix, void *reference, u32 mode,
                         u32 word0, u32 word1);
extern GeorgeActorBits64 func_00374848(float value);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern s32 func_00373250(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern float func_003734F8(GeorgeActorBits64 value);
extern GeorgeActorBits64 func_00372C68(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern u8 D_003F83F0[];
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void *func_00131578(void *object, const GeorgeMathVec3 *position);
extern void func_00131798(void *object, const GeorgeMathVec3 *position, const GeorgeMathVec3 *motion);
extern void func_00132BB0(void *object, GeorgeGoalEntity *entity);
extern void func_00132C18(void *object);
extern void func_00132C60(void *object, u32 word);
extern void func_00132CC8(void *object);
extern void func_00132CF8(void *object);
extern s32 func_00132950(void *object, u32 word);
extern void func_0014B940(void *object, GeorgeMathVec3 *output, u32 mode, float elapsed);
extern float func_0014B8D8(void *object);
extern void func_0014B9C8(void *object, GeorgeGoalEntity *entity);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))
#define PAIR(object, offset, type) ((const type *)ADDRESS((object)->field00, offset))

static __inline__ void control_void(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualVoid *pair = PAIR(object, offset, GeorgeGoalVirtualVoid);
    pair->invoke(ADJUST(object, pair->adjustment));
}
static __inline__ void control_word(GeorgeGoalEntity *entity, u32 offset, u32 word)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualWord *pair = PAIR(object, offset, GeorgeGoalVirtualWord);
    pair->invoke(ADJUST(object, pair->adjustment), word);
}
static __inline__ s32 control_word_result(GeorgeGoalEntity *entity, u32 offset, u32 word)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualWordResult *pair = PAIR(object, offset, GeorgeActorVirtualWordResult);
    return pair->invoke(ADJUST(object, pair->adjustment), word);
}
static __inline__ void control_vector(GeorgeGoalEntity *entity, u32 offset, const GeorgeMathVec3 *vector)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualVectorInput *pair = PAIR(object, offset, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}
static __inline__ s32 control_predicate(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualPredicate *pair = PAIR(object, 0xD0, GeorgeActorVirtualPredicate);
    return pair->invoke(ADJUST(object, pair->adjustment), 0.20000000298023224f, 0.25f);
}
static __inline__ s32 control_int(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualInt *pair = PAIR(object, offset, GeorgeGoalVirtualInt);
    return pair->invoke(ADJUST(object, pair->adjustment));
}
ACTOR_INLINE s32 request_pair(GeorgeGoalEntity *entity, void *captured_main,
                             u32 word, GeorgeActorRequestCallback callback, float first_time)
{
    if (captured_main != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                         FIELD(entity, 0x368, u32), 0, 0, 0, 0, first_time);
        return func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                            FIELD(entity, 0x368, u32), callback,
                            callback != 0 ? entity : 0, 0, 0, 0.0f);
    }
    return 0;
}
ACTOR_INLINE void ensure_effect(void)
{
    if (D_003F2D40 == 0) {
        GeorgeActorEffectRecord *record;
        void *registered, *value;
        void **begin, **end, **position;
        D_003F2D40 = func_002481F0(func_002AEE60(0x1B4));
        record = (GeorgeActorEffectRecord *)func_002AEE60(12);
        registered = D_003F2D40;
        begin = D_0046A0F0.field00;
        end = D_0046A0F0.field04;
        record->field00 = 9;
        record->field04 = D_00421160;
        record->field08 = registered;
        value = record;
        position = func_00100C30(begin, end, &value, func_00100AA8);
        if (D_0046A0F0.field04 != D_0046A0F0.field08 && position == D_0046A0F0.field04) {
            if (position != 0) *position = value;
            D_0046A0F0.field04 = (void **)ADDRESS(D_0046A0F0.field04, 4);
        } else func_001007E0(&D_0046A0F0, position, &value);
        func_00396260(func_002BD340);
    }
}
#define DURATION_CALLBACK(name, phase_offset, timer_offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration; (void)unused; \
    FIELD(entity, phase_offset, u32) = FIELD(entity, phase_offset, u32) + 1u; \
    duration = func_002A6E60(source); \
    FIELD(entity, timer_offset, float) = duration * 0.000208333338377997279f; \
}
DURATION_CALLBACK(func_00195978, 0x7F4, 0x7F0)
DURATION_CALLBACK(func_00195B30, 0x818, 0x81C)
DURATION_CALLBACK(func_00195BE8, 0x828, 0x82C)

void func_001959C0(GeorgeGoalEntity *entity)
{
    void *primary;
    FIELD(entity, 0x4E4, u32) = 0x50;
    primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x7F0, u32) = 0;
    FIELD(entity, 0x7F4, u32) = 0;
    if (request_pair(entity, primary, 0x50, func_00195978, FIELD(entity, 0x7F0, float)) == 0)
        FIELD(entity, 0x7F4, u32) = 1;
}
void func_00187648(GeorgeGoalEntity *entity)
{
    float timer;
    s32 phase;
    control_void(entity, 0xA0);
    timer = FIELD(entity, 0x7F0, float) - FIELD(entity, 0x35C, float);
    phase = FIELD(entity, 0x7F4, s32);
    FIELD(entity, 0x7F0, float) = timer;
    if (phase == 1 && timer <= 0.0f) {
        GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
        s32 result = 0;
        if ((flags & 0x80u) == 0 && (flags & 0x80000u) == 0) result = control_predicate(entity);
        FIELD(entity, 0x14, u32) = result != 0 ? 3 : 0;
    }
}
/* This repeated flag/virtual gate clears the flag only after a successful
 * result, and callers reload actor fields after that callback. */
ACTOR_INLINE s32 clear_gate(GeorgeGoalEntity *entity)
{
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x20000u) == 0) return 1;
    if (control_word_result(entity, 0x50, 0) == 0) return 0;
    func_0018FD40(entity, 0x20000);
    return 1;
}
void func_00187990(GeorgeGoalEntity *entity)
{
    s32 ready, result;
    u32 word, effect_word;
    void *primary;
    control_word(entity, 0x30, 1);
    ready = (FIELD(entity, 0x190, GeorgeActorBits64) & 0x20000u) == 0;
    FIELD(entity, 0x810, u32) = 0;
    if (!ready) {
        ready = control_word_result(entity, 0x50, 0) != 0;
        if (ready) func_0018FD40(entity, 0x20000);
    }
    word = ready ? 0x51 : 0x53;
    primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x4E4, u32) = word;
    result = request_pair(entity, primary, word, func_00195A88, 0.0f);
    if (result == 0) {
        u32 phase = FIELD(entity, 0x810, u32) + 1u;
        FIELD(entity, 0x814, float) = 1.0f;
        FIELD(entity, 0x810, u32) = phase;
    }
    FIELD(entity, 0x804, float) = FIELD(entity, 0x7F8, float);
    FIELD(entity, 0x808, float) = FIELD(entity, 0x7FC, float);
    FIELD(entity, 0x80C, float) = FIELD(entity, 0x800, float);
    /* Retail clears this even after the request-failure path wrote 1. */
    FIELD(entity, 0x814, u32) = 0;
    effect_word = FIELD(entity->field18, 0x1B4, u32);
    if (effect_word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000u) == 0)
        func_001B67C0(ADDRESS(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                     FIELD(entity, 0x3B4, u32), effect_word, 0, 0);
}
void func_00187BA0(GeorgeGoalEntity *entity)
{
    float target, current, difference, timer;
    GeorgeActorBits64 delta;
    s32 negative;
    if (FIELD(entity, 0x810, s32) != 1) { control_void(entity, 0xA0); return; }
    target = func_0029B940(-FIELD(entity, 0x800, float), -FIELD(entity, 0x7F8, float));
    if (3.14159274101257324f < target)
        target = func_0029B940(-FIELD(entity, 0x800, float), -FIELD(entity, 0x7F8, float)) - 6.28318548202514648f;
    else {
        target = func_0029B940(-FIELD(entity, 0x800, float), -FIELD(entity, 0x7F8, float));
        if (target < -3.14159274101257324f)
            target = func_0029B940(-FIELD(entity, 0x800, float), -FIELD(entity, 0x7F8, float)) + 6.28318548202514648f;
        else target = func_0029B940(-FIELD(entity, 0x800, float), -FIELD(entity, 0x7F8, float));
    }
    delta = func_00374848(target - FIELD(entity, 0x58, float));
    negative = func_00373250(delta, 0ULL) < 0;
    current = FIELD(entity, 0x58, float);
    if (negative) delta = func_00372CC0(0ULL, delta);
    if (func_00373250(delta, 0x400921FB60000000ULL) > 0) {
        GeorgeActorBits64 wrapped;
        target = target - current;
        delta = func_00374848(target);
        if (func_00373250(delta, 0ULL) < 0) delta = func_00372CC0(0ULL, delta);
        wrapped = func_00372CC0(delta, 0x401921FB60000000ULL);
        if (target < 0.0f) wrapped = func_00372D28(wrapped, 0xBFF0000000000000ULL);
        difference = func_003734F8(wrapped);
    } else difference = target - current;
    if (0.100000001490116119f < difference)
        FIELD(entity, 0x58, float) = current + difference * (FIELD(entity, 0x35C, float) * 5.0f);
    control_vector(entity, 0xB0, VECTOR(entity, 0x804));
    timer = FIELD(entity, 0x814, float) - FIELD(entity, 0x35C, float);
    FIELD(entity, 0x814, float) = timer;
    if (timer <= 0.0f) FIELD(entity, 0x14, u32) = clear_gate(entity) ? 0 : 0x1D;
}
void func_00195A88(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration, x, scale, y, z;
    GeorgeGoalEntityData *data;
    (void)unused;
    FIELD(entity, 0x810, u32) = FIELD(entity, 0x810, u32) + 1u;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    data = entity->field18;
    x = FIELD(entity, 0x804, float);
    FIELD(entity, 0x814, float) = duration;
    scale = FIELD(data, 0x1B0, float) / duration;
    FIELD(entity, 0x804, float) = x * scale;
    y = FIELD(entity, 0x808, float);
    z = FIELD(entity, 0x80C, float);
    FIELD(entity, 0x808, float) = y * scale;
    FIELD(entity, 0x80C, float) = z * scale;
}
void func_00195B08(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x1B0, void *);
    if (object != 0) func_002727D8(object);
}
void func_00187EA8(GeorgeGoalEntity *entity)
{
    const u8 *text;
    void *effect, *handle;
    u32 key, word;
    GeorgeMathVec3 position;
    FIELD(entity, 0x81C, u32) = 0;
    text = (FIELD(entity, 0x190, GeorgeActorBits64) & 0x200000u) != 0 ? D_0042D8A8 : D_0042D8C0;
    FIELD(entity, 0x55C, u32) = 0;
    FIELD(entity, 0x560, u32) = 0;
    ensure_effect();
    effect = D_003F2D40;
    func_002BEBA0(&key, text);
    position.x = FIELD(entity, 0x40, float);
    position.y = FIELD(entity, 0x44, float);
    position.z = FIELD(entity, 0x48, float);
    handle = func_00251B58(effect, key, &position);
    FIELD(entity, 0x820, void *) = handle;
    func_002455C0(handle);
    word = FIELD(entity->field18, 0x138, u32);
    if (word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000u) == 0)
        func_001B67C0(ADDRESS(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                     FIELD(entity, 0x3B4, u32), word, 0, 0);
    if (FIELD(entity, 0x10, u32) - 3u < 2u) FIELD(entity, 0x818, u32) = 0;
    else {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x818, u32) = 100;
        FIELD(entity, 0x4E4, u32) = 0x66;
        if (request_pair(entity, primary, 0x66, func_00195B30, 0.0f) == 0)
            FIELD(entity, 0x818, u32) = FIELD(entity, 0x818, u32) + 1u;
    }
    word = FIELD(entity->field18, 0xEC, u32);
    if (word != 0) {
        void *reference = func_00210078(word, 0, 0);
        if (reference != 0) {
            const GeorgeRotationMatrix *matrix = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
            void *object = func_00236CB8(matrix, reference, 1, 0, 0);
            const GeorgeGoalVirtualWord *pair;
            FIELD(entity, 0x234, void *) = object;
            func_002A1C08(ADDRESS(object, 0x10), matrix);
            FIELD(object, 0xA0, u32) |= 0x100u;
            pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(reference, 0x20, const u8 *), 0x10);
            pair->invoke(ADJUST(reference, pair->adjustment), 0);
        }
    }
}
void func_00188290(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x81C, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0x818, s32);
    void *object;
    FIELD(entity, 0x81C, float) = timer;
    if (phase == 100) control_void(entity, 0xA0);
    else if (phase == 101) {
        control_void(entity, 0xA0);
        if (FIELD(entity, 0x81C, float) <= 0.0f) FIELD(entity, 0x14, u32) = 0;
    } else {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x67;
        request_pair(entity, primary, 0x67, 0, 0.0f);
        control_word(entity, 0xB8, 0);
        func_0017D908(entity);
        if (FIELD(entity, 0x50, float) < FIELD(entity, 0x3C4, float))
            FIELD(entity, 0x3C4, float) = FIELD(entity, 0x50, float);
        if (control_int(entity, 0xD8) != 0) {
            GeorgeGoalEntityData *data = entity->field18;
            u32 word;
            FIELD(entity, 0x818, u32) = 100;
            if (FIELD(entity, 0x3C4, float) <= -FIELD(data, 0x98, float)) {
                if (FIELD(entity, 0x820, void *) != 0) {
                    func_002457D8(FIELD(entity, 0x820, void *));
                    func_00245B08(FIELD(entity, 0x820, void *));
                    FIELD(entity, 0x820, void *) = 0;
                }
                func_0017EBF0(entity);
                func_0017ED18(entity, 2);
                func_0017EE30(entity, 2);
                word = 0x69;
            } else word = 0x68;
            primary = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x4E4, u32) = word;
            if (request_pair(entity, primary, word, func_00195B30, 0.0f) == 0)
                FIELD(entity, 0x818, u32) = FIELD(entity, 0x818, u32) + 1u;
        }
    }
    object = FIELD(entity, 0x234, void *);
    if (object != 0) {
        float z;
        FIELD(object, 0x80, float) = FIELD(entity, 0x40, float);
        FIELD(object, 0x84, float) = FIELD(entity, 0x44, float);
        z = FIELD(entity, 0x48, float);
        FIELD(object, 0x8C, float) = 1.0f;
        FIELD(object, 0x88, float) = z;
    }
    FIELD(entity, 0x74, u32) = 0;
}
void func_00195B78(GeorgeGoalEntity *entity)
{
    void *object;
    func_002727D8(FIELD(entity, 0x1B0, void *));
    if (FIELD(entity, 0x820, void *) != 0) {
        func_002457D8(FIELD(entity, 0x820, void *));
        func_00245B08(FIELD(entity, 0x820, void *));
        FIELD(entity, 0x820, void *) = 0;
    }
    object = FIELD(entity, 0x234, void *);
    if (object != 0) {
        func_002393F8((u32)object);
        FIELD(entity, 0x234, void *) = 0;
    }
}

/* Same complete adjusted member representation as the reviewed actor-state
 * dispatcher. Selector/target are captured, while adjustment uses fresh state. */
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

s32 func_00188590(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 motion, up, direction, cross;
    float x = FIELD(entity, 0x68, float), y = FIELD(entity, 0x6C, float);
    float z = FIELD(entity, 0x70, float), forward, side;
    GeorgeActorBits64 forward_abs, side_abs;
    void *map, *parent, *point;
    if ((x * x + y * y) + z * z < 0.00999999977648258209f) {
        motion.x = motion.y = motion.z = 0.0f;
        func_00131798(FIELD(entity, 0x824, void *), VECTOR(entity, 0x40), &motion);
        return 4;
    }
    motion.x = x; motion.z = z; motion.y = 0.0f;
    func_00131798(FIELD(entity, 0x824, void *), VECTOR(entity, 0x40), &motion);
    up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;
    map = FIELD(entity, 0x824, void *);
    parent = FIELD(map, 0x0C, void *);
    point = func_00131578(map, VECTOR(entity, 0x40));
    direction.x = FIELD(parent, 0x80, float) - FIELD(point, 0x30, float);
    direction.z = FIELD(parent, 0x88, float) - FIELD(point, 0x38, float);
    direction.y = FIELD(parent, 0x84, float) - FIELD(point, 0x34, float);
    func_002A3538(&direction);
    cross.x = up.y * direction.z - up.z * direction.y;
    cross.z = up.x * direction.y - up.y * direction.x;
    cross.y = up.z * direction.x - up.x * direction.z;
    func_002A3538(&cross);
    map = FIELD(entity, 0x824, void *);
    x = FIELD(map, 0x114, float); y = FIELD(map, 0x118, float); z = FIELD(map, 0x11C, float);
    forward = (direction.x * x + direction.y * y) + direction.z * z;
    side = (cross.x * x + cross.y * y) + cross.z * z;
    forward_abs = func_00374848(forward);
    if (func_00373250(forward_abs, 0ULL) < 0) forward_abs = func_00372CC0(0ULL, forward_abs);
    side_abs = func_00374848(side);
    if (func_00373250(side_abs, 0ULL) < 0) side_abs = func_00372CC0(0ULL, side_abs);
    if (func_00373250(forward_abs, side_abs) > 0) return 0.0f < forward ? 0 : 2;
    return side < 0.0f ? 1 : 3;
}

/* Each occurrence performs two queries. The first result's Z is read before
 * the second query; its X comes from the second result afterward. */
ACTOR_INLINE float query_angle(GeorgeGoalEntity *entity, GeorgeMathVec3 *position)
{
    void *first = func_00131578(FIELD(entity, 0x824, void *), position);
    float z = FIELD(first, 0x28, float);
    void *second = func_00131578(FIELD(entity, 0x824, void *), position);
    return func_0029B940(z, FIELD(second, 0x20, float));
}
void func_00188968(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 *position = VECTOR(entity, 0x40);
    float target = query_angle(entity, position);
    void *map;
    if (3.14159274101257324f < target) target = query_angle(entity, position) - 6.28318548202514648f;
    else {
        target = query_angle(entity, position);
        if (target < -3.14159274101257324f) target = query_angle(entity, position) + 6.28318548202514648f;
        else target = query_angle(entity, position);
    }
    map = FIELD(entity, 0x824, void *);
    FIELD(entity, 0x848, float) = target;
    FIELD(entity, 0x828, u32) = 100;
    FIELD(entity, 0x834, u32) = 0x84;
    FIELD(entity, 0x838, u32) = 0x85;
    FIELD(entity, 0x83C, u32) = 0x86;
    FIELD(entity, 0x840, u32) = 0x87;
    FIELD(entity, 0x844, u32) = 0x83;
    FIELD(entity, 0x830, u32) = 0x83;
    func_00132BB0(map, entity);
    FIELD(entity, 0x190, GeorgeActorBits64) |= 0x40000ULL;
}

ACTOR_INLINE void point_delta(GeorgeGoalEntity *entity, void *point, GeorgeMathVec3 *delta)
{
    delta->x = FIELD(point, 0x30, float) - FIELD(entity, 0x40, float);
    delta->z = FIELD(point, 0x38, float) - FIELD(entity, 0x48, float);
    delta->y = FIELD(point, 0x34, float) - FIELD(entity, 0x44, float);
}
void func_00188B10(GeorgeGoalEntity *entity)
{
    s32 selector = func_00188590(entity), phase;
    GeorgeActorBits64 step, current, delta, result, scale, adjustment;
    float target, difference, angle, timer;
    void *map, *primary, *point;
    GeorgeMathVec3 vector;
    control_void(entity, 0xA0);
    timer = FIELD(entity, 0x82C, float) - FIELD(entity, 0x35C, float);
    map = FIELD(entity, 0x824, void *);
    FIELD(entity, 0x82C, float) = timer;
    func_00131798(map, VECTOR(entity, 0x40), VECTOR(entity, 0x68));
    step = func_00374848(FIELD(entity, 0x35C, float));
    current = func_00374848(FIELD(entity, 0x58, float));
    delta = func_00374848(FIELD(entity, 0x848, float) - FIELD(entity, 0x58, float));
    if (func_00373250(delta, 0ULL) < 0) delta = func_00372CC0(0ULL, delta);
    phase = func_00373250(delta, 0x400921FB60000000ULL);
    target = FIELD(entity, 0x848, float);
    if (phase > 0) {
        s32 negative;
        delta = func_00374848(target - FIELD(entity, 0x58, float));
        negative = func_00373250(delta, 0ULL) < 0;
        target = FIELD(entity, 0x848, float);
        difference = FIELD(entity, 0x58, float);
        if (negative) delta = func_00372CC0(0ULL, delta);
        delta = func_00372CC0(delta, 0x401921FB60000000ULL);
        scale = target - difference < 0.0f ? 0xC014000000000000ULL : 0x4014000000000000ULL;
    } else {
        delta = func_00374848(target - FIELD(entity, 0x58, float));
        scale = 0x4014000000000000ULL;
    }
    adjustment = func_00372D28(step, scale);
    adjustment = func_00372D28(delta, adjustment);
    result = func_00372C68(current, adjustment);
    angle = func_003734F8(result);
    phase = FIELD(entity, 0x828, s32);
    FIELD(entity, 0x58, float) = angle;
    if (phase == 201) {
        u32 word = FIELD(entity, 0x830u + ((u32)selector << 2), u32);
        if (FIELD(entity, 0x844, u32) != word) {
            FIELD(entity, 0x844, u32) = word;
            FIELD(entity, 0x828, u32) = 200;
        }
    }
    phase = FIELD(entity, 0x828, s32);
    if (phase == 101) {
        GeorgeActorControlObject *control;
        const GeorgeActorVirtualVectorInput *pair;
        point = func_00131578(FIELD(entity, 0x824, void *), VECTOR(entity, 0x40));
        point_delta(entity, point, &vector);
        control = CONTROL(entity);
        vector.x *= 4.0f; vector.y *= 4.0f; vector.z *= 4.0f;
        pair = PAIR(control, 0x78, GeorgeActorVirtualVectorInput);
        pair->invoke(ADJUST(control, pair->adjustment), &vector);
        if (FIELD(entity, 0x82C, float) <= 0.0f) FIELD(entity, 0x828, u32) = 200;
        func_00132CC8(FIELD(entity, 0x824, void *));
        return;
    }
    if (phase == 200) {
        u32 word;
        func_00132C60(FIELD(entity, 0x824, void *), 1);
        func_0018FD40(entity, 0x4000ULL);
        control_void(entity, 0x38);
        word = FIELD(entity, 0x844, u32);
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = word;
        request_pair(entity, primary, word, func_00195BE8, 0.0f);
        /* Retail falls through to movement even if a request changed phase. */
    }
    if (phase == 200 || phase == 201) {
        float speed = selector == 4 ? 30.0f : 20.0f;
        point = func_00131578(FIELD(entity, 0x824, void *), VECTOR(entity, 0x40));
        point_delta(entity, point, &vector);
        vector.x *= speed; vector.y *= speed; vector.z *= speed;
        control_vector(entity, 0xB0, &vector);
        func_00132BC8(FIELD(entity, 0x824, GeorgeMathScaled16C *));
        if (func_00132950(FIELD(entity, 0x824, void *), 8) == 0) FIELD(entity, 0x828, u32) = 300;
    } else if (phase == 300) {
        func_0018FD30(entity, 0x4000ULL);
        control_void(entity, 0x38);
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x88;
        request_pair(entity, primary, 0x88, func_00195BE8, 0.0f);
    } else if (phase == 301) {
        if (FIELD(entity, 0x82C, float) <= 0.0f) {
            func_00132CF8(FIELD(entity, 0x824, void *));
            func_00170538(entity);
            entity->field0C = 0;
            actor_member(entity, 0);
        }
    } else {
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x82;
        request_pair(entity, primary, 0x82, func_00195BE8, 0.0f);
    }
}
void func_00195C30(GeorgeGoalEntity *entity)
{
    void *primary = FIELD(entity, 0x1B0, void *);
    if (primary != 0) func_002727D8(primary);
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x40000ULL;
    func_0018FD30(entity, 0x4000ULL);
    control_void(entity, 0x38);
    func_00132C18(FIELD(entity, 0x824, void *));
    func_00132C60(FIELD(entity, 0x824, void *), 0);
    func_00132BB0(FIELD(entity, 0x824, void *), 0);
    FIELD(entity, 0x824, void *) = 0;
}

ACTOR_INLINE float associated_angle(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x84C, void *);
    return func_0029B940(-FIELD(object, 0x68, float), -FIELD(object, 0x60, float));
}
ACTOR_INLINE float associated_target(GeorgeGoalEntity *entity)
{
    float target = associated_angle(entity);
    if (3.14159274101257324f < target) return associated_angle(entity) - 6.28318548202514648f;
    target = associated_angle(entity);
    if (target < -3.14159274101257324f) return associated_angle(entity) + 6.28318548202514648f;
    return associated_angle(entity);
}
/* All soft arithmetic remains explicit 64-bit ABI calls. The current actor
 * angle is captured after the sign comparison, before the possible abs call. */
ACTOR_INLINE float associated_difference(GeorgeGoalEntity *entity, float target, float *current)
{
    GeorgeActorBits64 delta = func_00374848(target - FIELD(entity, 0x58, float));
    s32 negative = func_00373250(delta, 0ULL) < 0;
    *current = FIELD(entity, 0x58, float);
    if (negative) delta = func_00372CC0(0ULL, delta);
    if (func_00373250(delta, 0x400921FB60000000ULL) > 0) {
        GeorgeActorBits64 wrapped;
        target = target - *current;
        delta = func_00374848(target);
        if (func_00373250(delta, 0ULL) < 0) delta = func_00372CC0(0ULL, delta);
        wrapped = func_00372CC0(delta, 0x401921FB60000000ULL);
        if (target < 0.0f) wrapped = func_00372D28(wrapped, 0xBFF0000000000000ULL);
        return func_003734F8(wrapped);
    }
    return target - *current;
}
ACTOR_INLINE void scaled_output(GeorgeGoalEntity *entity, const GeorgeMathVec3 *output,
                               const GeorgeMathVec3 *position, GeorgeMathVec3 *delta,
                               GeorgeMathVec3 *scaled)
{
    GeorgeActorControlObject *control;
    const GeorgeActorVirtualVectorInput *pair;
    delta->x = output->x - position->x;
    delta->y = output->y - position->y;
    control = CONTROL(entity);
    delta->z = output->z - position->z;
    scaled->x = delta->x * 10.0f;
    scaled->y = delta->y * 10.0f;
    scaled->z = delta->z * 10.0f;
    pair = PAIR(control, 0x78, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(control, pair->adjustment), scaled);
}
void func_00189310(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 position, output, delta, scaled;
    float timer, elapsed, current, difference;
    s32 phase;
    void *object, *primary;
    position.x = FIELD(entity, 0x40, float);
    position.y = FIELD(entity, 0x44, float) + FIELD(entity, 0x860, float);
    position.z = FIELD(entity, 0x48, float);
    timer = FIELD(entity, 0x858, float) - FIELD(entity, 0x35C, float);
    FIELD(entity, 0x858, float) = timer;
    control_void(entity, 0xA0);
    object = FIELD(entity, 0x84C, void *);
    elapsed = FIELD(entity, 0x85C, float) + FIELD(object, 0x48, float) * FIELD(entity, 0x35C, float);
    phase = FIELD(entity, 0x854, s16);
    FIELD(entity, 0x85C, float) = elapsed;
    if (phase == 0) { FIELD(entity, 0x854, u16) = 100; phase = 100; }
    if (phase == 100 || phase == 200) {
        u32 word = phase == 100 ? 0x89 : 0x8A;
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x854, u16) = (u16)(phase + 1);
        FIELD(entity, 0x4E4, u32) = word;
        if (request_pair(entity, primary, word, func_00195CC0, 0.0f) == 0)
            FIELD(entity, 0x854, u16) = (u16)(phase + 2);
    } else if (phase == 102 || phase == 202) {
        func_0014B940(object, &output, 0, elapsed);
        scaled_output(entity, &output, &position, &delta, &scaled);
        difference = associated_difference(entity, associated_target(entity), &current);
        if (phase == 102) {
            GeorgeActorControlObject *control;
            const GeorgeActorVirtualVectorInput *pair;
            /* Re-read local output after the callback/angle calls. */
            delta.x = output.x - position.x;
            delta.y = output.y - position.y;
            delta.z = output.z - position.z;
            control = CONTROL(entity);
            scaled.z = delta.z * 10.0f;
            FIELD(entity, 0x58, float) = current + difference * (FIELD(entity, 0x35C, float) * 5.0f);
            scaled.x = delta.x * 10.0f; scaled.y = delta.y * 10.0f;
            pair = PAIR(control, 0x78, GeorgeActorVirtualVectorInput);
            pair->invoke(ADJUST(control, pair->adjustment), &scaled);
            if (FIELD(entity, 0x858, float) <= 0.0f) FIELD(entity, 0x854, u16) = 200;
        } else {
            object = FIELD(entity, 0x84C, void *);
            FIELD(entity, 0x58, float) = current + difference * (FIELD(entity, 0x35C, float) * 5.0f);
            timer = func_0014B8D8(object);
            if (timer <= FIELD(entity, 0x85C, float)) {
                func_00170538(entity);
                entity->field0C = 3;
                actor_member(entity, 3);
            }
        }
    }
}
void func_00195CC0(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration; (void)unused;
    FIELD(entity, 0x854, u16) = (u16)(FIELD(entity, 0x854, u16) + 1u);
    duration = func_002A6E60(source);
    FIELD(entity, 0x858, float) = duration * 0.000208333338377997279f;
}
void func_00195D08(GeorgeGoalEntity *entity)
{
    control_word(entity, 0x30, 1);
    FIELD(entity, 0x856, u16) = 0;
    FIELD(entity, 0x854, u16) = 0;
}
void func_00195D50(GeorgeGoalEntity *entity)
{
    func_0014B9C8(FIELD(entity, 0x84C, void *), entity);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
