#include "george/actor_states8.h"

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern u8 D_003F83F0[];
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042D650[];
extern void *func_002AEE60(u32 size);
extern void *func_002481F0(void *object);
extern s32 func_00100AA8(const void *, const void *);
extern void func_001007E0(GeorgeActorPointerRange *, void **, void *const *);
extern void func_002BD340(void);
extern s32 func_00396260(void (*)(void));
extern u32 *func_002BEBA0(u32 *, const u8 *);
extern void *func_00251A88(void *, u32);
extern s32 func_00270510(void *, u32, u32, u32, u32,
                       GeorgeActorRequestCallback, GeorgeGoalEntity *, u32, u32, float);
extern void func_00272390(void *, u32, u32, u32, GeorgeActorRequestCallback,
                        GeorgeGoalEntity *, u32, u32, u32, float, float);
extern void func_002727D8(void *);
extern float func_002A6E60(void *);
extern float func_0029C168(float);
extern float func_0029C090(float);
extern void func_00185EA0(GeorgeGoalEntity *, void *, u32, u32, const GeorgeMathVec3 *);
extern void func_00190E00(GeorgeGoalEntity *, u32);
extern void func_00190F70(GeorgeGoalEntity *);
extern void *func_00210078(u32, u32, u32);
extern void *func_00236CB8(const void *, void *, u32, u32, u32);
extern void func_002455C0(void *);
extern void func_002457D8(void *);
extern void func_00245B08(void *);
extern void func_002458E8(void *);
extern void func_002393F8(u32);
extern void func_0030C440(void *, const GeorgeMathVec4 *);
extern s32 func_0018B710(GeorgeGoalEntity *, const GeorgeMathVec3 *, const GeorgeMathVec3 *);
extern u32 func_00272C10(void *);
extern u32 func_00297640(u32);
extern u32 func_002A7418(u32);
extern void *func_002A6468(const void *, s32);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *,
                        const GeorgeRotationMatrix *);
extern void func_002A1C08(void *, const void *);
extern void *func_00238BA0(void *, u32);
extern void func_0022C2C8(void *, u32, void *);
extern float func_0030A6A0(void *);
extern void func_00307850(void *);

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

void func_001A7708(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
    void *data = entity->field18;
    FIELD(entity, 0x190, GeorgeActorBits64) = flags | 0x40000ULL;
    if (FIELD(data, 0x1D8, u32) == 0x45A78000u)
        FIELD(CONTROL(entity), 0x34, u32) = 0;
    control_word(entity, 0x30, 0);
    FIELD(entity, 0x9E4, u8) = 0;
}
void func_001A7810(GeorgeGoalEntity *entity)
{
    if (FIELD(entity->field18, 0x1D8, u32) == 0x45A78000u)
        FIELD(CONTROL(entity), 0x34, u32) = 1;
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x40000ULL;
    control_word(entity, 0x30, 1);
    FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_001A7778(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0x9E4, signed char);
    if (phase == 2) {
        float blend = FIELD(entity, 0x9CC, float);
        void *primary = FIELD(entity, 0x1B0, void *);
        if (0.0f <= blend) blend = george_ee_minimum(blend, 0.99000000953674316f);
        else blend = 0.0f;
        FIELD(primary, 0x428, float) = blend;
    } else if (phase == 4) control_word(entity, 0xB8, 0);
}
s32 func_001A7560(void *object)
{
    return FIELD(object, 0x0C, u32) == 39u;
}
s32 func_001A7570(void *object)
{
    return FIELD(object, 0x0C, u32) == 39u && FIELD(object, 0x9E4, signed char) == 4;
}
s32 func_001A7598(GeorgeGoalEntity *entity, u32 word)
{
    if (entity->field0C != 32 || FIELD(entity, 0x180, signed char) != 1) return 0;
    FIELD(entity, 0x9E8, u32) = word;
    func_00170538(entity);
    FIELD(entity, 0x9CC, u32) = 0;
    func_00190E00(entity, 39);
    return 1;
}
void func_001A7600(GeorgeGoalEntity *entity, float angle)
{
    float cosine = func_0029C168(angle);
    float sine = func_0029C090(angle);
    float vertical = 0.0f;
    float scale = FIELD(entity->field18, 0x3D4, float) * FIELD(entity, 0x9CC, float);
    GeorgeMathVec3 *motion = VECTOR(entity, 0x9D8);
    motion->x = cosine * scale;
    motion->y = vertical;
    motion->z = sine * scale;
    motion->y = FIELD(entity->field18, 0x3D0, float) * FIELD(entity, 0x9CC, float);
    FIELD(entity, 0x9D4, float) = angle;
}
void func_001A7698(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    s32 phase;
    (void)unused;
    duration = func_002A6E60(source);
    phase = FIELD(entity, 0x9E5, signed char);
    FIELD(entity, 0x9D0, float) = duration * 0.000208333338377997279f;
    if (phase == 2)
        func_00185EA0(entity, FIELD(entity, 0x9E8, void *), 1, 0,
                     VECTOR(entity->field18, 0x3E4));
    FIELD(entity, 0x9E4, u8) = FIELD(entity, 0x9E5, u8);
}
void func_001A7378(GeorgeGoalEntity *entity, s32 mode)
{
    if (mode == 0 || mode == 1) {
        void *data = entity->field18;
        u32 word = FIELD(data, mode == 0 ? 0x3C4 : 0x3C8, u32);
        void *primary = FIELD(entity, 0x1B0, void *);
        func_00272390(primary, word, 0, 0, func_001A7698, entity, 0, 0, 0, 0.0f, 5.0f);
        if (mode == 1) FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 1;
        FIELD(entity, 0x9E5, u8) = mode == 0 ? 1 : 2;
    } else if (mode == 2) {
        u32 word = FIELD(entity->field18, 0x3CC, u32);
        GeorgeMathVec3 *motion = VECTOR(entity, 0x9D8);
        float angle;
        func_00272390(FIELD(entity, 0x1B0, void *), word, 0, 0,
                     func_001A7698, entity, 0, 0, 0, 0.0f, 5.0f);
        FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
        FIELD(entity, 0x9E4, u8) = 4;
        FIELD(entity, 0x9E5, u8) = 4;
        func_00194FD8(entity);
        control_word(entity, 0x30, 1);
        angle = FIELD(entity, 0x9D4, float);
        FIELD(entity, 0x64, float) = angle;
        FIELD(entity, 0x58, float) = angle;
        control_vector(entity, 0x98, motion);
        if (motion->x != 0.0f || motion->z != 0.0f) {
            motion->y = 0.0f;
            func_002A35C0(motion, motion, 5.0f);
        }
    }
}
void func_00180458(GeorgeGoalEntity *entity)
{
    void *effect;
    u32 key;
    FIELD(entity, 0xA0C, u32) = 0;
    FIELD(entity, 0xA08, u16) = 0;
    ensure_effect();
    effect = D_003F2D40;
    func_002BEBA0(&key, D_0042D650);
    FIELD(entity, 0x2EC, void *) = func_00251A88(effect, key);
}
void func_00194A00(GeorgeGoalEntity *entity)
{
    void *primary;
    func_002457D8(FIELD(entity, 0x2EC, void *));
    func_00245B08(FIELD(entity, 0x2EC, void *));
    primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x2EC, u32) = 0;
    func_002727D8(primary);
}
void func_001949B8(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    FIELD(entity, 0xA08, u16) = (u16)(FIELD(entity, 0xA08, u16) + 1u);
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0xA0C, float) = duration;
}
void func_00196920(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0xA3C, u32) = 0;
    FIELD(entity, 0xA30, u32) = 0;
    FIELD(entity, 0xA38, u32) = 0;
}
void func_00196930(GeorgeGoalEntity *entity)
{
    void *primary = FIELD(entity, 0x1B0, void *);
    u32 word;
    if (primary != 0) func_002727D8(primary);
    word = FIELD(entity, 0xA30, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0xA30, u32) = 0;}
}
void func_001968D8(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0xA38, u32) = FIELD(entity, 0xA38, u32) + 1u;
    FIELD(entity, 0xA3C, float) = duration;
}

ACTOR_INLINE void release_actor40_handles(GeorgeGoalEntity *entity)
{
    u32 word = FIELD(entity, 0x28C, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x28C, u32) = 0;}
    word = FIELD(entity, 0x290, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x290, u32) = 0;}
}
ACTOR_INLINE void request_actor40(GeorgeGoalEntity *entity, u32 word,
                                  GeorgeActorRequestCallback callback)
{
    void *primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x4E4, u32) = word;
    request_pair(entity, primary, word, callback);
}
/* Complete repeated update/create geometry. Existing records use -.05/1.0
 * vertical bounds in phase100; creation uses0/1.5. Phase200 also adds .02
 * times the just-commanded motion to both endpoints. */
#define ACTOR40_BOUNDS(entity, forward, movement) \
 do { \
    void *record = FIELD(entity, 0x300, void *); \
    const GeorgeMathVec3 *position = VECTOR(entity, 0x40); \
    void *data = entity->field18; \
    GeorgeMathVec4 bounds[2]; \
    float radius = FIELD(data, 0x178, float); \
    float x = position->x, z = position->z, y = position->y; \
    bounds[0].x = x + -radius; bounds[0].z = z + -radius; \
    bounds[0].y = record != 0 && !(forward) ? y + -0.05000000074505806f : y; \
    radius = FIELD(data, 0x178, float); \
    bounds[1].x = x + radius; bounds[1].z = position->z + radius; \
    bounds[1].y = position->y + (record != 0 ? 1.0f : 1.5f); \
    if (forward) { \
        float dx = (movement).x * 0.019999999552965164f; \
        float dy = (movement).y * 0.019999999552965164f; \
        float dz = (movement).z * 0.019999999552965164f; \
        bounds[0].x += dx; bounds[1].x += dx; \
        bounds[0].y += dy; bounds[0].z += dz; \
        bounds[1].z += (movement).z * 0.019999999552965164f; \
        bounds[1].y += (movement).y * 0.019999999552965164f; \
    } \
    bounds[0].w = 0.0f; bounds[1].w = 0.0f; \
    if (record != 0) func_0030C440(record, bounds); \
    else func_0018E5A8(entity, (const GeorgeMathVec3 *)&bounds[0], \
                     (const GeorgeMathVec3 *)&bounds[1], 8, 5); \
 } while (0)
void func_00180580(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0xA0C, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0xA08, s16);
    FIELD(entity, 0xA0C, float) = timer;
    if (phase == 1) {
        GeorgeMathVec3 zero;
        zero.x = 0.0f; zero.y = 0.0f; zero.z = 0.0f;
        control_vector(entity, 0x78, &zero);
        if (FIELD(entity, 0xA0C, float) <= 0.0f) {
            u32 word = FIELD(entity->field18, 0x110, u32);
            void *handle;
            s32 next;
            if (word != 0) {
                void *object;
                const GeorgeRotationMatrix *matrix = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
                if (FIELD(entity, 0x28C, u32) != 0 || FIELD(entity, 0x290, u32) != 0)
                    func_00190F70(entity);
                object = func_00210078(word, 0, 0);
                if (object != 0) {
                    const GeorgeGoalVirtualWord *pair;
                    FIELD(entity, 0x28C, void *) = func_00236CB8(matrix, object, 0, 0, 0);
                    FIELD(entity, 0x290, void *) = func_00236CB8(matrix, object, 0, 0, 0);
                    pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0x20, void *), 0x10);
                    pair->invoke(ADJUST(object, pair->adjustment), 0);
                }
            }
            next = FIELD(entity, 0xA10, s32) == 1 ? 100 : 200;
            handle = FIELD(entity, 0x2EC, void *);
            FIELD(entity, 0xA08, u16) = (u16)next;
            func_002455C0(handle);
        }
    } else if (phase == 100) {
        GeorgeMathVec3 movement;
        request_actor40(entity, 0xAD, 0);
        movement.x = 0.0f;
        movement.y = -FIELD(entity->field18, 0x180, float);
        movement.z = 0.0f;
        control_vector(entity, 0x78, &movement);
        ACTOR40_BOUNDS(entity, 0, movement);
    } else if (phase == 200) {
        GeorgeMathVec3 movement;
        float scale;
        request_actor40(entity, 0xB1, 0);
        movement.x = FIELD(entity, 0xD0, float);
        movement.y = FIELD(entity, 0xD4, float);
        movement.z = FIELD(entity, 0xD8, float);
        func_002A3538(&movement);
        scale = FIELD(entity->field18, 0x180, float);
        movement.x *= scale;
        movement.z *= scale;
        movement.y = -scale;
        control_vector(entity, 0x78, &movement);
        ACTOR40_BOUNDS(entity, 1, movement);
    } else if (phase == 400) {
        GeorgeMathVec3 direction, position;
        void *data;
        float x, y, z;
        func_002458E8(FIELD(entity, 0x2EC, void *));
        release_actor40_handles(entity);
        control_void(entity, 0x88);
        x = 0.0f + FIELD(entity, 0xD0, float);
        y = 1.0f + FIELD(entity, 0xD4, float);
        z = FIELD(entity, 0xD8, float);
        direction.z = -z; direction.x = -x; direction.y = -y;
        data = entity->field18;
        func_002A35C0(&position, VECTOR(entity, 0x4C), FIELD(data, 0x0C, float));
        x = FIELD(entity, 0x40, float); z = FIELD(entity, 0x48, float); y = FIELD(entity, 0x44, float);
        position.x += x; position.y += y; position.z += z;
        func_0018B710(entity, &direction, &position);
        func_00196980(entity);
    } else if (phase == 500) {
        func_002458E8(FIELD(entity, 0x2EC, void *));
        release_actor40_handles(entity);
        request_actor40(entity, FIELD(entity, 0xA10, s32) == 1 ? 0xAE : 0xB2, func_001949B8);
        control_void(entity, 0x88);
        func_00196980(entity);
    } else if (phase == 501) {
        if (guarded_predicate(entity) != 0) control_word(entity, 0xB8, 0);
        if (FIELD(entity, 0xA0C, float) <= 0.0f) {
            func_00170538(entity);
            if (guarded_predicate(entity) != 0) {entity->field0C = 3; actor_member(entity, 3);}
            else {entity->field0C = 0; actor_member(entity, 0);}
        }
    } else {
        request_actor40(entity, FIELD(entity, 0xA10, s32) == 1 ? 0xAC : 0xB0, func_001949B8);
    }
}

ACTOR_INLINE void *component_pointer(void *component)
{
    const GeorgeGoalVirtualPointer *pair = (const GeorgeGoalVirtualPointer *)
        ADDRESS(FIELD(component, 0x04, const u8 *), 0x98);
    return pair->invoke(ADJUST(component, pair->adjustment));
}
ACTOR_INLINE const GeorgeRotationMatrix *actor41_matrix(GeorgeGoalEntity *entity,
                                                        void *primary, u32 identifier)
{
    if (primary != 0) {
        s32 command = FIELD(primary, 0x0C, s32), count, index;
        u32 offset;
        if (command == 3) command = 2;
        offset = (u32)command << 2;
        count = (s32)func_002A6460(FIELD(primary, 0x378u + offset, void *));
        if (count > 0) {
            const u8 *records = (const u8 *)func_002A6468(
                FIELD(FIELD(entity, 0x1B0, void *), 0x378u + offset, void *), 0);
            void *matrices = FIELD(FIELD(entity, 0x1B0, void *), 0x3E8u + offset, void *);
            for (index = 0; index < count; ++index)
                if (records[(u32)index * 0x20u + 0x1Du] == identifier)
                    return (const GeorgeRotationMatrix *)ADDRESS(matrices, (u32)index << 6);
        }
    }
    return 0;
}
void func_0018E0E8(GeorgeGoalEntity *entity)
{
    u32 phase;
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0xA38, u32);
    if (phase == 1) FIELD(entity, 0xA38, u32) = FIELD(entity, 0xA38, u32) + 1u;
    else if (phase == 2) {
        void *primary;
        float timer;
        if (FIELD(entity, 0xA34, u32) == 0) FIELD(entity, 0x14, u32) = 0;
        primary = FIELD(entity, 0x1B0, void *);
        if (primary != 0) {
            u32 bits = func_00272C10(primary);
            u32 mask = func_00297640(FIELD(entity->field18, 0x418, u32));
            if ((bits & mask) != 0) {
                const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xF0);
                u32 identifier = func_002A7418(FIELD(entity->field18, 0x414, u32));
                const GeorgeRotationMatrix *matrix = actor41_matrix(entity,
                    FIELD(entity, 0x1B0, void *), identifier);
                GeorgeRotationMatrix output __attribute__((aligned(16)));
                void *reference;
                func_002A2200(&output, matrix, source);
                reference = func_00210078(FIELD(entity, 0xA34, u32), 0, 0);
                if (reference != 0) {
                    void *object = func_00236CB8(&output, reference, 0, 0, 0);
                    const GeorgeGoalVirtualWord *pair;
                    FIELD(entity, 0xA30, void *) = object;
                    func_002A1C08(ADDRESS(object, 0x10), &output);
                    FIELD(object, 0xA0, u32) |= 0x100u;
                    pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(reference, 0x20, void *), 0x10);
                    pair->invoke(ADJUST(reference, pair->adjustment), 0);
                }
                FIELD(entity, 0xA38, u32) = FIELD(entity, 0xA38, u32) + 1u;
            }
        }
        timer = FIELD(entity, 0xA3C, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0xA3C, float) = timer;
        if (timer <= 0.0f) {
            FIELD(entity, 0x14, u32) = 0;
            FIELD(entity, 0xA30, u32) = 0;
        }
    } else if (phase == 3) {
        void *reference = FIELD(entity, 0xA30, void *);
        float timer;
        if (reference != 0) {
            void *component = func_00238BA0(reference, 0x0B6C8F2Bu);
            void *object = component_pointer(component);
            void *source;
            const GeorgeMathVec3 *target = VECTOR(entity, 0xA44), *position;
            GeorgeMathVec3 delta, motion;
            GeorgeMathVec4 start, displacement;
            float vertical, scale, time, reciprocal;
            const GeorgeActorVirtualFrameParts *pair;
            func_0022C2C8(object, 0x007269B7u, FIELD(entity, 0xA30, void *));
            source = FIELD(entity, 0xA30, void *);
            position = VECTOR(source, 0x40);
            delta.x = target->x - position->x;
            delta.z = target->z - position->z;
            vertical = target->y - position->y;
            delta.y = vertical;
            scale = FIELD(entity, 0xA40, float);
            time = george_ee_square_root(delta.x * delta.x + delta.z * delta.z) / scale;
            motion.x = delta.x; motion.y = 0.0f; motion.z = delta.z;
            vertical = vertical / time - time * -4.9000000953674316f;
            func_002A35C0(&motion, &motion, scale);
            motion.y = vertical;
            object = component_pointer(component);
            reciprocal = func_0030A6A0(ADDRESS(object, 0xA0));
            motion.x *= reciprocal; motion.y *= reciprocal; motion.z *= reciprocal;
            source = FIELD(entity, 0xA30, void *);
            start.x = FIELD(source, 0x40, float);
            start.y = FIELD(source, 0x44, float);
            start.z = FIELD(source, 0x48, float);
            start.w = 0.0f;
            displacement.x = motion.x; displacement.y = motion.y;
            displacement.z = motion.z; displacement.w = 0.0f;
            object = component_pointer(component);
            func_00307850(object);
            object = ADDRESS(object, 0xA0);
            pair = (const GeorgeActorVirtualFrameParts *)ADDRESS(FIELD(object, 0x00, void *), 0x98);
            pair->invoke(ADJUST(object, pair->adjustment), &start, &displacement);
        }
        timer = FIELD(entity, 0xA3C, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0xA38, u32) = FIELD(entity, 0xA38, u32) + 1u;
        FIELD(entity, 0xA3C, float) = timer;
    } else if (phase == 4) {
        float timer = FIELD(entity, 0xA3C, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0xA3C, float) = timer;
        if (timer <= 0.0f) {
            u32 word = FIELD(entity, 0xA30, u32);
            if (word != 0) {func_002393F8(word); FIELD(entity, 0xA30, u32) = 0;}
            FIELD(entity, 0x14, u32) = 0;
        }
    } else {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0xC7;
        request_pair(entity, primary, 0xC7, func_001968D8);
    }
}
