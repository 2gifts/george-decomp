#include "george/actor_states6.h"

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
extern const u8 D_0042DA60[], D_0042E780[];
extern void *D_004961F4;
extern void func_00235CD8(void *object, u32 key, u32 word);
extern void func_002455C0(void *object);
extern void func_002457D8(void *object);
extern void func_00245B08(void *object);
extern void func_002A1C08(void *output, const void *input);
extern void func_00272390(void *object, u32 word, u32 mode0, u32 mode1,
                        GeorgeActorRequestCallback callback, GeorgeGoalEntity *context,
                        u32 invoke_word, u32 callback_word, u32 mode2, float time, float scale);
extern void *func_002E2BB0(void *manager, u32 kind, u32 group);
extern void *func_0030C368(void *record, const GeorgeMathVec4 *bounds, u32 count);
extern void func_0030E018(void *record, GeorgeActorInteractionCallback *callback);
extern void *func_0022C1E0(void);
extern void func_00312688(void *manager, void *record);
extern void func_00312C00(void *manager, void *record);
extern void func_00317910(void *record);
extern void func_001413A8(void *object);
extern const u8 D_0042DA48[];
extern void *func_00251A88(void *object, u32 key);
extern void func_002458E8(void *object);
extern float func_0029C168(float angle);
extern float func_0029C090(float angle);
extern void func_001784D0(GeorgeGoalEntity *entity, u32 mode, float angle);
extern void func_0030C440(void *record, const GeorgeMathVec4 *bounds);
extern u32 func_00236A10(const GeorgeRotationMatrix *matrix, u32 word,
                      u32 mode0, u32 mode1, u32 mode2, u32 mode3);
extern void func_002393F8(u32 word);
extern void func_002A2200(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *first,
                        const GeorgeRotationMatrix *second);
extern void func_00141638(void *object, GeorgeMathVec3 *output);
extern void func_00141678(void *object, GeorgeMathVec3 *output);
extern void func_00141348(void *object);

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

void func_00196280(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration, reciprocal;
    u32 phase;
    (void)unused;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    phase = FIELD(entity, 0x908, u32) + 1u;
    reciprocal = 1.0f / duration;
    FIELD(entity, 0x908, u32) = phase;
    FIELD(entity, 0x918, float) = duration;
    FIELD(entity, 0x91C, float) = reciprocal;
}
void func_00196420(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0x924, u32) = FIELD(entity, 0x924, u32) + 1u;
    FIELD(entity, 0x928, float) = duration;
}
void func_00196680(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0x930, float) = duration;
}
void func_00196700(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    FIELD(entity, 0x9F0, u16) = (u16)(FIELD(entity, 0x9F0, u16) + 1u);
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0x9F8, float) = duration;
}

void func_001962E0(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags;
    FIELD(entity, 0x908, u32) = 100;
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x90C, u32) = 20;
    FIELD(entity, 0x918, u32) = 0;
    FIELD(entity, 0x920, u32) = 0;
    FIELD(entity, 0x910, u32) = 0;
    if ((flags & 0x20000ULL) == 0 && control_word_result(entity, 0x50, 1) != 0)
        func_0018FD30(entity, 0x20000ULL);
}
void func_00196368(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x2EC, void *);
    GeorgeActorBits64 flags;
    float value;
    if (object != 0) {
        func_002457D8(object);
        func_00245B08(FIELD(entity, 0x2EC, void *));
        FIELD(entity, 0x2EC, void *) = 0;
    }
    if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
    value = FIELD(entity->field18, 0x400, float);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x4FC, float) = value;
    if ((flags & 0x20000ULL) != 0 && control_word_result(entity, 0x50, 0) != 0)
        func_0018FD40(entity, 0x20000ULL);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_00196748(GeorgeGoalEntity *entity)
{
    control_void(entity, 0x88);
    FIELD(entity, 0x9F0, u16) = 0;
}
void func_00196468(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags;
    void *object;
    FIELD(entity, 0x924, u32) = 100;
    flags = FIELD(entity, 0x190, GeorgeActorBits64) | 0x40000ULL;
    object = FIELD(entity, 0x240, void *);
    FIELD(entity, 0x190, GeorgeActorBits64) = flags;
    if (object != 0) {
        GeorgeGeometryFrame frame;
        func_002A1C08(&frame, ADDRESS(entity, 0xF0));
        frame.position.x = FIELD(entity, 0x40, float);
        frame.position.y = FIELD(entity, 0x44, float) + FIELD(entity->field18, 0x118, float);
        frame.position.z = FIELD(entity, 0x48, float);
        frame.position.w = 1.0f;
        object = FIELD(entity, 0x240, void *);
        func_002A1C08(ADDRESS(object, 0x10), &frame);
        FIELD(object, 0xA0, u32) |= 0x100u;
    }
}
void func_00196500(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0x924, s32);
    if (phase == 100) {
        void *object;
        func_00272390(FIELD(entity, 0x1B0, void *), FIELD(entity, 0x184, u32), 0, 0,
                     func_00196420, entity, 0, 0, 0, 0.0f, FIELD(entity, 0x188, float));
        object = FIELD(entity, 0x240, void *);
        if (object != 0) func_00235CD8(object, 0x9F79558Fu, 1);
    } else if (phase == 101) FIELD(entity, 0x924, u32) = 102;
    else if (phase == 102) {
        float timer = FIELD(entity, 0x17C, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x17C, float) = timer;
        if (0.0f < timer) control_void(entity, 0xA0);
        else FIELD(entity, 0x924, u32) = 103;
    } else if (phase == 103) FIELD(entity, 0x14, u32) = 0;
}
void func_00196608(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64) & ~0x40000ULL;
    void *object = FIELD(entity, 0x240, void *);
    FIELD(entity, 0x190, GeorgeActorBits64) = flags;
    if (object != 0) func_00235CD8(object, 0xB95616B6u, 1);
    if (FIELD(entity->field18, 0x194, u32) != 0)
        func_00235CD8(FIELD(entity, 0x1CC, void *), 0xB95616B6u, 0);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_001966C0(GeorgeGoalEntity *entity)
{
    control_void(entity, 0x88);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_00196980(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x300, void *) != 0 && FIELD(entity, 0x2F8, void *) != 0) {
        void *manager = func_0022C1E0();
        void *callback;
        func_00312C00(manager, FIELD(entity, 0x300, void *));
        func_00317910(FIELD(entity, 0x300, void *));
        callback = FIELD(entity, 0x2F8, void *);
        FIELD(entity, 0x300, void *) = 0;
        if (callback != 0) {
            const GeorgeGoalVirtualWord *pair = (const GeorgeGoalVirtualWord *)
                ADDRESS(FIELD(callback, 0, const u8 *), 0x18);
            pair->invoke(ADJUST(callback, pair->adjustment), 3);
        }
        FIELD(entity, 0x2F8, void *) = 0;
    }
}
void func_0018E5A8(GeorgeGoalEntity *entity, const GeorgeMathVec3 *lower,
                     const GeorgeMathVec3 *upper, u32 word, u32 count)
{
    GeorgeMathVec4 bounds[2];
    GeorgeActorInteractionCallback *callback;
    void *record;
    void *manager = D_004961F4;
    bounds[0].x = lower->x; bounds[0].y = lower->y; bounds[0].z = lower->z; bounds[0].w = 0.0f;
    bounds[1].x = upper->x; bounds[1].y = upper->y; bounds[1].z = upper->z; bounds[1].w = 0.0f;
    record = func_002E2BB0(manager, 0xA0, 0x2F);
    FIELD(record, 4, u16) = 0xA0;
    record = func_0030C368(record, bounds, count);
    FIELD(entity, 0x300, void *) = record;
    callback = (GeorgeActorInteractionCallback *)func_002AEE60(0x38);
    record = FIELD(entity, 0x300, void *);
    callback->field08 = word;
    callback->field00 = D_0042E780;
    callback->field04 = entity;
    callback->field0C = 0;
    FIELD(entity, 0x2F8, void *) = callback;
    func_0030E018(record, callback);
    manager = func_0022C1E0();
    func_00312688(manager, FIELD(entity, 0x300, void *));
}
void func_0018D648(GeorgeGoalEntity *entity)
{
    GeorgeGeometryFrame frame;
    void *object;
    GeorgeActorControlObject *control;
    const GeorgeActorVirtualFrameParts *pair;
    frame.position.x = FIELD(entity, 0xE0, float);
    frame.position.y = FIELD(entity, 0xE4, float);
    frame.position.z = FIELD(entity, 0xE8, float);
    frame.position.w = 0.0f;
    frame.axis[0].x = FIELD(entity, 0xB0, float);
    frame.axis[0].z = FIELD(entity, 0xB8, float);
    frame.axis[0].y = FIELD(entity, 0xB4, float);
    frame.axis[0].w = 0.0f;
    frame.axis[1].x = FIELD(entity, 0xC0, float);
    frame.axis[1].z = FIELD(entity, 0xC8, float);
    frame.axis[1].y = FIELD(entity, 0xC4, float);
    frame.axis[1].w = 0.0f;
    frame.axis[2].x = FIELD(entity, 0xD0, float);
    frame.axis[2].y = FIELD(entity, 0xD4, float);
    frame.axis[2].z = FIELD(entity, 0xD8, float);
    frame.axis[2].w = 0.0f;
    control = CONTROL(entity);
    pair = PAIR(control, 0x100, GeorgeActorVirtualFrameParts);
    pair->invoke(ADJUST(control, pair->adjustment), frame.axis, &frame.position);
    object = FIELD(entity, 0x2F4, void *);
    if (object != 0) {
        func_002457D8(object);
        func_00245B08(FIELD(entity, 0x2F4, void *));
        FIELD(entity, 0x2F4, void *) = 0;
    }
    object = FIELD(entity, 0x2F0, void *);
    if (object != 0) {
        func_002457D8(object);
        func_00245B08(FIELD(entity, 0x2F0, void *));
        FIELD(entity, 0x2F0, void *) = 0;
    }
    object = FIELD(entity, 0x424, void *);
    FIELD(entity, 0x424, void *) = 0;
    FIELD(entity, 0x9EC, void *) = object;
    func_001413A8(object);
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_0018C708(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *control;
    const GeorgeGoalVirtualVector *pair;
    const GeorgeMathVec3 *returned;
    float x, z, dot, angle;
    u32 key;
    void *effect;
    FIELD(entity, 0x92C, u16) = 100;
    FIELD(entity, 0x9EC, void *) = 0;
    FIELD(entity, 0x930, u32) = 0;
    control = CONTROL(entity);
    pair = PAIR(control, 0x58, GeorgeGoalVirtualVector);
    returned = pair->invoke(ADJUST(control, pair->adjustment));
    x = returned->x;
    z = returned->z;
    dot = (x * FIELD(entity, 0x42C, float) + returned->y * FIELD(entity, 0x430, float))
        + z * FIELD(entity, 0x434, float);
    if (0.0f < dot) {z = -z; x = -x;}
    angle = func_0029B940(z, x);
    FIELD(entity, 0x58, float) = angle;
    FIELD(entity, 0x92E, u16) = 0;
    ensure_effect();
    effect = D_003F2D40;
    func_002BEBA0(&key, D_0042DA60);
    func_00251CC8(effect, key);
}

ACTOR_INLINE void reverse_control_angle(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *control = CONTROL(entity);
    const GeorgeGoalVirtualVector *pair = PAIR(control, 0x58, GeorgeGoalVirtualVector);
    const GeorgeMathVec3 *returned = pair->invoke(ADJUST(control, pair->adjustment));
    float x = returned->x, z = returned->z;
    float dot = (x * FIELD(entity, 0x42C, float) + returned->y * FIELD(entity, 0x430, float))
              + z * FIELD(entity, 0x434, float);
    if (!(0.0f < dot)) {z = -z; x = -x;}
    FIELD(entity, 0x58, float) = func_0029B940(z, x);
    FIELD(entity, 0x92C, u16) = 400;
}
void func_0018C8A0(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x930, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0x92C, s16);
    FIELD(entity, 0x930, float) = timer;
    if (phase == 100) {
        void *primary;
        control_void(entity, 0x88);
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0xB6;
        request_pair(entity, primary, 0xB6, func_00196680);
        FIELD(entity, 0x92C, u16) = 200;
    } else if (phase == 200) {
        control_void(entity, 0x88);
        if (FIELD(entity, 0x930, float) < 0.0f) FIELD(entity, 0x92C, u16) = 210;
    } else if (phase == 210) {
        control_void(entity, 0x88);
        if (FIELD(entity, 0x92E, s16) == 0) {
            void *primary = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x4E4, u32) = 0xB7;
            request_pair(entity, primary, 0xB7, func_00196680);
            FIELD(entity, 0x934, u32) = 0;
            FIELD(entity, 0x92C, u16) = 300;
        } else reverse_control_angle(entity);
    } else if (phase == 300) {
        FIELD(entity, 0x934, float) += FIELD(entity, 0x35C, float);
        control_void(entity, 0x88);
        if (FIELD(entity, 0x92E, s16) != 0) reverse_control_angle(entity);
        else if (FIELD(entity->field18, 0x54, float) <= FIELD(entity, 0x934, float)) {
            GeorgeActorControlObject *control;
            const GeorgeActorVirtualVectorInput *pair;
            void *primary = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x4E4, u32) = 0xB9;
            request_pair(entity, primary, 0xB9, func_00196680);
            FIELD(entity, 0x190, GeorgeActorBits64) |= 0x40000ULL;
            control = CONTROL(entity);
            pair = PAIR(control, 0x98, GeorgeActorVirtualVectorInput);
            pair->invoke(ADJUST(control, pair->adjustment), VECTOR(entity, 0x42C));
            FIELD(entity, 0x92C, u16) = 500;
        }
    } else if (phase == 400) {
        void *data;
        float x, first, second;
        FIELD(entity, 0x4C, u32) = 0;
        FIELD(entity, 0x54, u32) = 0;
        FIELD(entity, 0x50, u32) = 0;
        FIELD(entity, 0x190, GeorgeActorBits64) |= 0x40000ULL;
        func_00170538(entity);
        data = entity->field18;
        FIELD(entity, 0x53C, u32) = 0xB8;
        FIELD(entity, 0x538, u32) = 0xB8;
        x = FIELD(entity, 0x42C, float);
        first = FIELD(data, 0x48, float);
        FIELD(entity, 0x530, float) = first;
        second = FIELD(data, 0x44, float);
        FIELD(entity, 0x540, float) = x;
        FIELD(entity, 0x534, float) = second;
        FIELD(entity, 0x544, float) = FIELD(entity, 0x430, float);
        FIELD(entity, 0x548, float) = FIELD(entity, 0x434, float);
        first = FIELD(entity->field18, 0x50, float);
        entity->field0C = 4;
        FIELD(entity, 0x438, float) = first;
        FIELD(entity, 0x558, u32) = 0;
        actor_member(entity, 4);
    } else if (phase == 500) {
        control_vector(entity, 0xB8, VECTOR(entity, 0x4C));
        if (FIELD(entity, 0x930, float) < 0.0f) {
            FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x40000ULL;
            func_00170538(entity);
            entity->field0C = 3;
            actor_member(entity, 3);
        }
    }
}

void func_0018C0C8(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0x908, s32);
    if (phase == 100) {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x99;
        if (request_pair(entity, primary, 0x99, func_00196280) != 0) {
            GeorgeMathVec3 lower, upper;
            void *data = entity->field18;
            float x = FIELD(entity, 0x40, float), z = FIELD(entity, 0x48, float);
            float radius = FIELD(data, 0x0C, float), y = FIELD(entity, 0x44, float);
            u32 key;
            void *effect, *handle;
            lower.x = x + -radius; lower.y = y; lower.z = z + -radius;
            upper.z = z + FIELD(data, 0x0C, float);
            upper.y = y + FIELD(data, 0x08, float);
            upper.x = x + FIELD(data, 0x0C, float);
            func_0018E5A8(entity, &lower, &upper, 0x20, 5);
            ensure_effect();
            effect = D_003F2D40;
            func_002BEBA0(&key, D_0042DA48);
            handle = func_00251A88(effect, key);
            FIELD(entity, 0x2EC, void *) = handle;
            func_002455C0(handle);
            FIELD(entity, 0x908, u32) = FIELD(entity, 0x908, u32) + 1u;
        } else FIELD(entity, 0x908, u32) = 103;
    } else if (phase == 102) {
        float step = FIELD(entity, 0x35C, float);
        float timer = FIELD(entity, 0x918, float) - step;
        float elapsed = FIELD(entity, 0x920, float) + step;
        FIELD(entity, 0x918, float) = timer;
        FIELD(entity, 0x920, float) = elapsed;
        if (0.0f < timer) {
            GeorgeMathVec3 vector;
            const GeorgeMathVec3 *first, *second;
            GeorgeActorControlObject *control;
            const GeorgeGoalVirtualVector *pair;
            GeorgeMathVec4 bounds[2];
            float dot, scalar, x, y, z, radius, height;
            void *data, *record;
            u32 word;
            float cosine = func_0029C168(FIELD(entity, 0x58, float));
            float sine = func_0029C090(FIELD(entity, 0x58, float));
            vector.x = cosine; vector.z = sine; vector.y = 0.0f;
            control = CONTROL(entity);
            pair = PAIR(control, 0x58, GeorgeGoalVirtualVector);
            first = pair->invoke(ADJUST(control, pair->adjustment));
            control = CONTROL(entity);
            pair = PAIR(control, 0x58, GeorgeGoalVirtualVector);
            second = pair->invoke(ADJUST(control, pair->adjustment));
            dot = (vector.x * second->x + vector.y * second->y) + vector.z * second->z;
            scalar = FIELD(entity->field18, 0x18, float);
            func_002A35C0(&vector, first, dot * scalar);
            func_00177E48(entity, &vector);
            func_001784D0(entity, 1, FIELD(entity, 0x58, float));
            data = entity->field18;
            z = FIELD(entity, 0x48, float); radius = FIELD(data, 0x0C, float);
            x = FIELD(entity, 0x40, float); y = FIELD(entity, 0x44, float);
            record = FIELD(entity, 0x300, void *);
            bounds[0].y = y;
            bounds[0].z = z + -radius;
            bounds[0].x = x + -radius;
            radius = FIELD(data, 0x0C, float); height = FIELD(data, 0x08, float);
            bounds[0].w = 0.0f;
            bounds[1].y = y + height; bounds[1].x = x + radius;
            bounds[1].z = z + radius; bounds[1].w = 0.0f;
            func_0030C440(record, bounds);
            if (control_int(entity, 0xD8) == 0) control_vector(entity, 0xB8, &vector);
            else {
                s32 counter, limit;
                control_vector(entity, 0xB0, &vector);
                counter = FIELD(entity, 0x910, s32);
                limit = FIELD(entity, 0x90C, s32);
                if (counter < limit) FIELD(entity, 0x910, u32) = (u32)counter + 1u;
                else {
                    word = FIELD(entity, 0x3DC, u32);
                    if (word != 0) {
                        func_002393F8(func_00236A10((const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0),
                                                  word, 0, 0, 0, 0));
                        FIELD(entity, 0x910, u32) = 0;
                    }
                }
            }
        } else {
            func_002458E8(FIELD(entity, 0x2EC, void *));
            control_void(entity, 0x88);
            FIELD(entity, 0x908, u32) = FIELD(entity, 0x908, u32) + 1u;
        }
    } else if (phase == 103) {
        u32 state;
        func_00170538(entity);
        state = guarded_predicate(entity) != 0 ? 3 : 29;
        entity->field0C = state;
        actor_member(entity, state);
    }
}

void func_0018CE18(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x9F8, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0x9F0, s16);
    FIELD(entity, 0x9F8, float) = timer;
    if (phase == 1) {
        if (timer <= 0.0f) FIELD(entity, 0x9F0, u16) = 300;
    } else if (phase == 100 || phase == 200 || phase == 300) {
        void *primary = FIELD(entity, 0x1B0, void *);
        u32 word = phase == 100 ? 0xAB : phase == 200 ? 0xA7 : 0xA8;
        FIELD(primary, 0x434, u32) = 0;
        FIELD(entity, 0x4E4, u32) = word;
        request_pair(entity, FIELD(entity, 0x1B0, void *), word, 0);
        FIELD(entity, 0x9F0, u16) = 500;
    } else if (phase == 500) {
        GeorgeGeometryFrame local, transformed;
        GeorgeMathVec4 basis[3], position;
        GeorgeMathVec3 orientation, cross, up;
        GeorgeActorControlObject *control;
        const GeorgeActorVirtualFrameParts *pair;
        float blend, x, y, z, w;
        void *primary = FIELD(entity, 0x1B0, void *);
        void *matrix_array;
        u32 matrix_index, matrix_offset;
        FIELD(entity, 0x4E4, u32) = 0xA5;
        request_pair(entity, primary, 0xA5, 0);
        FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 1;
        blend = func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *));
        if (0.0f <= blend) {
            blend = func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *));
            if (blend <= 0.99000000953674316f)
                blend = func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *));
            else blend = 0.99000000953674316f;
        } else blend = 0.0f;
        primary = FIELD(entity, 0x1B0, void *);
        FIELD(primary, 0x428, float) = blend;
        primary = FIELD(entity, 0x1B0, void *);
        matrix_offset = FIELD(entity, 0x3FC, u32) << 6;
        matrix_index = FIELD(primary, 0x0C, u32) << 2;
        matrix_array = FIELD(primary, 0x3D8u + matrix_index, void *);
        func_002A1C08(&local, ADDRESS(matrix_array, matrix_offset));
        func_002A2200((GeorgeRotationMatrix *)&transformed,
                     (const GeorgeRotationMatrix *)&local,
                     (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0));
        x = (transformed.position.x + transformed.axis[2].x * -0.51499998569488525f)
          + transformed.axis[1].x * 0.30000001192092896f;
        y = (transformed.position.y + transformed.axis[2].y * -0.51499998569488525f)
          + transformed.axis[1].y * 0.30000001192092896f;
        z = (transformed.position.z + transformed.axis[2].z * -0.51499998569488525f)
          + transformed.axis[1].z * 0.30000001192092896f;
        orientation.x = transformed.axis[1].x;
        orientation.y = transformed.axis[1].y;
        orientation.z = transformed.axis[1].z;
        position.x = x; position.y = y; position.z = z; position.w = 0.0f;
        func_002A3538(&orientation);
        basis[2].x = orientation.x;
        basis[2].y = orientation.y;
        basis[2].z = orientation.z;
        basis[2].w = 0.0f;
        up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;
        cross.x = up.y * orientation.z;
        cross.y = -(up.x * orientation.z);
        cross.z = up.x * orientation.y - up.y * orientation.x;
        w = up.z;
        func_002A3538(&cross);
        x = orientation.y * cross.z - orientation.z * cross.y;
        y = orientation.z * cross.x - orientation.x * cross.z;
        z = orientation.x * cross.y - orientation.y * cross.x;
        basis[0].x = cross.x; basis[0].y = cross.y;
        basis[0].z = cross.z; basis[0].w = w;
        up.y = y; up.z = z; up.x = x;
        func_002A3538(&up);
        basis[1].x = up.x; basis[1].y = up.y;
        basis[1].z = up.z; basis[1].w = w;
        control = CONTROL(entity);
        pair = PAIR(control, 0x100, GeorgeActorVirtualFrameParts);
        pair->invoke(ADJUST(control, pair->adjustment), basis, &position);
    } else if (phase == 700) {
        GeorgeMathVec3 point, direction, projection;
        float blend, x, y, z, zero, height, speed;
        void *data;
        FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
        blend = 0.5f - func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *));
        if (0.0f <= blend)
            blend = 0.5f - func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *));
        else blend = func_00141590(FIELD(entity, 0x424, const GeorgeMathAngularAC *)) - 0.5f;
        blend = blend + blend;
        func_00141638(FIELD(entity, 0x424, void *), &point);
        func_00141678(FIELD(entity, 0x424, void *), &direction);
        func_00141348(FIELD(entity, 0x424, void *));
        func_00170538(entity);
        x = direction.x; y = direction.y;
        FIELD(entity, 0x540, float) = x;
        FIELD(entity, 0x544, float) = y;
        z = direction.z;
        FIELD(entity, 0x544, u32) = 0;
        FIELD(entity, 0x548, float) = z;
        zero = FIELD(entity, 0x544, float);
        if (FIELD(entity, 0x540, float) == zero && FIELD(entity, 0x548, float) == zero)
            projection.x = projection.y = projection.z = zero;
        else {
            GeorgeActorControlObject *control;
            const GeorgeGoalVirtualVector *pair;
            const GeorgeMathVec3 *returned;
            float dot, magnitude;
            func_002A3538(VECTOR(entity, 0x540));
            control = CONTROL(entity);
            pair = PAIR(control, 0x58, GeorgeGoalVirtualVector);
            returned = pair->invoke(ADJUST(control, pair->adjustment));
            x = returned->x; z = returned->z;
            dot = (x * direction.x + returned->y * direction.y) + z * direction.z;
            if (!(zero < dot)) { z = -z; x = -x; }
            FIELD(entity, 0x58, float) = func_0029B940(z, x);
            magnitude = __builtin_fabsf(dot);
            projection.z = FIELD(entity, 0x548, float) * magnitude;
            projection.x = FIELD(entity, 0x540, float) * magnitude;
            projection.y = FIELD(entity, 0x544, float) * magnitude;
        }
        y = direction.y;
        speed = george_ee_square_root((direction.x * direction.x + y * y)
                                     + direction.z * direction.z);
        FIELD(entity, 0x538, u32) = 13;
        FIELD(entity, 0x53C, u32) = 14;
        data = entity->field18;
        speed = speed + blend * FIELD(data, 0x250, float);
        FIELD(entity, 0x530, float) = speed;
        height = FIELD(data, 0x24C, float);
        if (0.0f <= y) height = y + height;
        {
            GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64) | 0x40000ULL;
            FIELD(entity, 0x534, float) = height;
            FIELD(entity, 0x438, float) = 0.10000000149011612f;
            FIELD(entity, 0x558, u32) = 1;
            FIELD(entity, 0x190, GeorgeActorBits64) = flags;
            entity->field0C = 4;
            actor_member(entity, 4);
        }
        (void)projection;
    } else {
        FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
        FIELD(entity, 0x4E4, u32) = 0xA6;
        request_pair(entity, FIELD(entity, 0x1B0, void *), 0xA6, func_00196700);
    }
}
