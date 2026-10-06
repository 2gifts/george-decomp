#include "george/actor_states7.h"

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
extern u32 func_00272C10(void *object);
extern void *func_002A6468(const void *array, s32 index);
extern s32 func_00192C58(GeorgeGoalEntity *entity, u32 word,
                       GeorgeActorRequestCallback callback, GeorgeGoalEntity *context, float time);
extern void func_00192078(GeorgeGoalEntity *entity, u32 key, float adjustment);
extern void func_0013E4C8(void *object, u32 mode);
extern s32 func_00192A18(void *object);
extern s32 func_001A7560(void *object);
extern s32 func_001A7570(void *object);
extern void func_001A7600(GeorgeGoalEntity *entity, float angle);
extern void func_001A7378(GeorgeGoalEntity *entity, s32 mode);
extern s32 func_00176E10(GeorgeGoalEntity *entity);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern float func_003734F8(GeorgeActorBits64 value);
extern const u8 D_0042D618[], D_0042D628[];
extern void func_00190F70(GeorgeGoalEntity *entity);
extern void *func_00210078(u32 word, u32 mode0, u32 mode1);
extern void *func_00236CB8(const void *matrix, void *object,
                         u32 mode, u32 word0, u32 word1);

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

#define TIMER_ONLY(name, offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration; (void)unused; \
    duration = func_002A6E60(source) * 0.000208333338377997279f; \
    FIELD(entity, offset, float) = duration; \
}
TIMER_ONLY(func_00194388, 0x93C)
TIMER_ONLY(func_0019DBA8, 0x988)
void func_001947A8(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration; (void)unused;
    FIELD(entity, 0x654, u32) = FIELD(entity, 0x654, u32) + 1u;
    duration = func_002A6E60(source) * 0.000208333338377997279f;
    FIELD(entity, 0x650, float) = duration;
}
#define TIMER_BYTE_PHASE(name, timer_offset, phase_offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration; (void)unused; \
    duration = func_002A6E60(source); \
    FIELD(entity, phase_offset, u8) = 1; \
    FIELD(entity, timer_offset, float) = duration * 0.000208333338377997279f; \
}
TIMER_BYTE_PHASE(func_0019E538, 0x950, 0x944)
TIMER_BYTE_PHASE(func_001A71A0, 0x9BC, 0x9C4)

void func_001943C8(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x938, u16) = 0;
    control_void(entity, 0x88);
}
void func_001943F8(GeorgeGoalEntity *entity)
{
    void *handle = FIELD(entity, 0x2EC, void *);
    u32 word;
    if (handle != 0) {
        func_002458E8(handle);
        func_002457D8(FIELD(entity, 0x2EC, void *));
        func_00245B08(FIELD(entity, 0x2EC, void *));
        FIELD(entity, 0x2EC, void *) = 0;
    }
    func_002727D8(FIELD(entity, 0x1B0, void *));
    word = FIELD(entity, 0x28C, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x28C, u32) = 0;}
    word = FIELD(entity, 0x290, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x290, u32) = 0;}
    if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
}
void func_001947F0(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x658, u32) = 0;
    FIELD(entity, 0x650, u32) = 0;
    FIELD(entity, 0x654, u32) = 0;
}
void func_00194800(GeorgeGoalEntity *entity)
{
    s32 index = 0;
    u32 *position = (u32 *)ADDRESS(entity, 0x268);
    func_002727D8(FIELD(entity, 0x1B0, void *));
    if (0 < FIELD(entity->field18, 0x2EC, s32)) {
        do {
            void *object = (void *)*position;
            if (object != 0) func_00235CD8(object, 0xB95616B6u, 1);
            index = (s32)((u32)index + 1u);
            position = (u32 *)ADDRESS(position, 4);
        } while (index < FIELD(entity->field18, 0x2EC, s32));
    }
}
void func_0019E580(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x97C, u32) = 0;
    FIELD(entity, 0x944, u8) = 0;
    FIELD(entity, 0x946, u8) = 0;
    FIELD(entity, 0x947, u8) = 0;
}
void func_0019E598(GeorgeGoalEntity *entity)
{
    FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
    func_002727D8(FIELD(entity, 0x1B0, void *));
    FIELD(entity, 0x978, u32) = 0;
    FIELD(entity, 0x948, u32) = 0;
    FIELD(entity, 0x94C, u32) = 0;
    FIELD(entity, 0x96C, u32) = 0;
    FIELD(entity, 0x970, u32) = 0;
    FIELD(entity, 0x974, u32) = 0;
}
void func_0019DBE8(GeorgeGoalEntity *entity)
{
    FIELD(entity, 0x985, u8) = 0;
    FIELD(entity, 0x98C, u32) = 0;
    FIELD(entity, 0x984, u8) = 0;
}
void func_0019DBF8(GeorgeGoalEntity *entity)
{
    func_002727D8(FIELD(entity, 0x1B0, void *));
}
void func_001A71E8(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x990, void *);
    float angle = func_0029B940(FIELD(object, 0x118, float), FIELD(object, 0x110, float));
    void *data = entity->field18;
    GeorgeActorBits64 flags;
    float angular_rate;
    FIELD(entity, 0x9A4, float) = -1.0f;
    FIELD(entity, 0x9A8, float) = angle;
    FIELD(entity, 0x9B0, u32) = 0;
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    angular_rate = FIELD(data, 0x3DC, float) * 6.2831854820251465f;
    FIELD(entity, 0x9C4, u8) = 0;
    FIELD(entity, 0x9C7, u8) = (flags & 0x80000000000ULL) != 0;
    FIELD(entity, 0x9B4, float) = angular_rate;
    FIELD(entity, 0x9C6, u8) = (flags & 0x2000000ULL) != 0;
    func_001784D0(entity, 1, angle);
}
void func_001A7288(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x990, void *);
    if (object != 0 && func_00192A18(object) == 0 &&
        func_001A7560(FIELD(entity, 0x990, void *)) != 0 &&
        func_001A7570(FIELD(entity, 0x990, void *)) == 0) {
        func_001A7600(FIELD(entity, 0x990, GeorgeGoalEntity *), FIELD(entity, 0x9A8, float));
        func_001A7378(FIELD(entity, 0x990, GeorgeGoalEntity *), 2);
    }
    if (FIELD(entity, 0x9C7, signed char) != 0) func_0018FD30(entity, 0x80000000000ULL);
    else func_0018FD40(entity, 0x80000000000ULL);
    if (FIELD(entity, 0x9C6, signed char) != 0) func_0018FD30(entity, 0x2000000ULL);
    else func_0018FD40(entity, 0x2000000ULL);
    FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
    func_002727D8(FIELD(entity, 0x1B0, void *));
    FIELD(entity, 0x99C, u32) = 0;
    FIELD(entity, 0x990, u32) = 0;
    FIELD(entity, 0x994, u32) = 0;
    FIELD(entity, 0x998, u32) = 0;
}

void func_0019D8E8(GeorgeGoalEntity *entity)
{
    s32 phase;
    FIELD(entity, 0x988, float) = FIELD(entity, 0x988, float) - FIELD(entity, 0x35C, float);
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0x984, signed char);
    if (phase == 1) {
        if (FIELD(entity, 0x988, float) < 0.0f) {
            func_00192C58(entity, 0xC4, func_0019DBA8, entity, 0.0f);
            FIELD(entity, 0x984, u8) = 2;
        }
    } else if (phase == 2) {
        s32 command = FIELD(entity, 0x985, signed char);
        if (command == 0) {
            float blend = FIELD(entity, 0x98C, float);
            void *primary = FIELD(entity, 0x1B0, void *);
            if (0.0f <= blend) blend = george_ee_minimum(blend, 0.99000000953674316f);
            else blend = 0.0f;
            FIELD(primary, 0x428, float) = blend;
        } else if (command == 1) FIELD(entity, 0x14, u32) = 0;
        else if (command == 2) {
            func_00192C58(entity, 0xC6, func_0019DBA8, entity, 0.0f);
            FIELD(entity, 0x984, u8) = 3;
        } else if (command == 3) {
            func_00192C58(entity, 0xC5, func_0019DBA8, entity, 0.0f);
            FIELD(entity, 0x984, signed char) = (signed char)command;
        }
    } else if (phase == 3) {
        if (FIELD(entity, 0x988, float) < 0.0f) FIELD(entity, 0x14, u32) = 0;
    } else {
        func_00192C58(entity, 0xC3, func_0019DBA8, entity, 0.0f);
        FIELD(entity, 0x984, u8) = 1;
    }
}

void func_0017FC10(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x650, float) - FIELD(entity, 0x35C, float);
    s32 phase = FIELD(entity, 0x654, s32);
    FIELD(entity, 0x650, float) = timer;
    if (phase == 1) return;
    if (phase == 2) {
        s32 index = 0;
        u32 *objects = (u32 *)ADDRESS(entity, 0x268);
        if (0 < FIELD(entity->field18, 0x2EC, s32)) {
            do {
                s32 next_index = (s32)((u32)index + 1u);
                u32 *slot = (u32 *)ADDRESS(objects, (u32)index << 2);
                if (*slot != 0) {
                    GeorgeRotationMatrix output __attribute__((aligned(16)));
                    const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
                    const GeorgeRotationMatrix *matrix = 0;
                    u32 identifier = FIELD(entity, 0x668u + ((u32)index << 4), u32);
                    void *primary = FIELD(entity, 0x1B0, void *), *object;
                    if (primary != 0) {
                        s32 command = FIELD(primary, 0x0C, s32), count, j;
                        u32 offset;
                        if (command == 3) command = 2;
                        offset = (u32)command << 2;
                        count = (s32)func_002A6460(FIELD(primary, 0x378u + offset, void *));
                        if (count > 0) {
                            const u8 *records = (const u8 *)func_002A6468(
                                FIELD(FIELD(entity, 0x1B0, void *), 0x378u + offset, void *), 0);
                            void *matrices = FIELD(FIELD(entity, 0x1B0, void *), 0x3E8u + offset, void *);
                            for (j = 0; j < count; ++j) {
                                if (records[(u32)j * 0x20u + 0x1Du] == identifier) {
                                    matrix = (const GeorgeRotationMatrix *)ADDRESS(matrices, (u32)j << 6);
                                    break;
                                }
                            }
                        }
                    }
                    func_002A2200(&output, matrix, source);
                    object = (void *)*slot;
                    func_002A1C08(ADDRESS(object, 0x10), &output);
                    FIELD(object, 0xA0, u32) |= 0x100u;
                    primary = FIELD(entity, 0x1B0, void *);
                    if (primary != 0 && (func_00272C10(primary) &
                        FIELD(entity, 0x664u + ((u32)index << 4), u32)) != 0) {
                        u32 mask = 1u << ((u32)index & 31u);
                        u32 bits = FIELD(entity, 0x658, u32) ^ mask;
                        u32 key = ((bits >> ((u32)index & 31u)) & 1u) != 0
                                ? 0x9F79558Fu : 0xB95616B6u;
                        FIELD(entity, 0x658, u32) = bits;
                        func_00235CD8((void *)*slot, key, 1);
                    }
                }
                index = next_index;
            } while (index < FIELD(entity->field18, 0x2EC, s32));
        }
        if (FIELD(entity, 0x650, float) <= 0.0f) FIELD(entity, 0x14, u32) = 0;
    } else {
        void *primary = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0xB4;
        if (request_pair(entity, primary, 0xB4, func_001947A8) == 0)
            FIELD(entity, 0x14, u32) = 0;
        else FIELD(entity, 0x654, u32) = FIELD(entity, 0x654, u32) + 1u;
    }
}

ACTOR_INLINE void request_scaled(GeorgeGoalEntity *entity, u32 word,
                                 GeorgeActorRequestCallback callback)
{
    func_00272390(FIELD(entity, 0x1B0, void *), word, 0, 0, callback, entity,
                 0, 0, 0, 0.0f, 5.0f);
}
void func_0019DDB8(GeorgeGoalEntity *entity)
{
    s32 phase;
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0x944, signed char);
    if (phase == 1) FIELD(entity, 0x944, u8) = FIELD(entity, 0x945, u8);
    else if (phase == 2) {
        const GeorgeMathVec3 *target = FIELD(entity, 0x980, const GeorgeMathVec3 *);
        GeorgeMathVec3 motion;
        float timer;
        motion.x = target->x - FIELD(entity, 0x40, float);
        motion.z = target->z - FIELD(entity, 0x48, float);
        motion.y = target->y - FIELD(entity, 0x44, float);
        motion.x *= 10.0f; motion.y *= 10.0f; motion.z *= 10.0f;
        control_vector(entity, 0x78, &motion);
        timer = FIELD(entity, 0x950, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x950, float) = timer;
        if (timer < 0.0f) {
            void *object;
            FIELD(entity, 0x97C, u32) = 1;
            request_scaled(entity, FIELD(entity, 0x970, u32), func_0019E538);
            FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 1;
            FIELD(entity, 0x945, u8) = 3;
            object = FIELD(entity, 0x948, void *);
            if (object != 0) func_0013E4C8(object, 2);
        }
    } else if (phase == 3) {
        s32 command = FIELD(entity, 0x946, signed char);
        if (command == 0) {
            float change = FIELD(entity, 0x95C, float), blend;
            if (change != 0.0f) func_00192078(entity, 0, change * FIELD(entity, 0x35C, float));
            blend = FIELD(entity, 0x954, float);
            if (0.0f <= blend) blend = george_ee_minimum(blend, 0.99000000953674316f);
            else blend = 0.0f;
            FIELD(FIELD(entity, 0x1B0, void *), 0x428, float) = blend;
        } else if (command == 1) {
            void *object = FIELD(entity, 0x948, void *);
            if (object != 0) func_0013E4C8(object, 0);
            FIELD(entity, 0x14, u32) = 0;
        } else if (command == 2 || command == 3) {
            void *object = FIELD(entity, 0x948, void *);
            u32 word;
            if (object != 0) func_0013E4C8(object, command == 2 ? 3 : 4);
            word = FIELD(entity, command == 2 ? 0x974 : 0x978, u32);
            if (word != 0) {
                FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
                request_scaled(entity, FIELD(entity, command == 2 ? 0x974 : 0x978, u32), func_0019E538);
                FIELD(entity, 0x945, u8) = command == 2 ? 4 : 5;
            } else FIELD(entity, 0x14, u32) = 0;
        }
    } else if (phase == 4) {
        float timer = FIELD(entity, 0x950, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x950, float) = timer;
        if (timer < 0.0f) FIELD(entity, 0x14, u32) = 0;
    } else if (phase == 5) {
        float timer = FIELD(entity, 0x950, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x950, float) = timer;
        if (timer < 0.0f) {
            float adjustment = FIELD(entity, 0x958, float);
            if (adjustment != 0.0f) func_00192078(entity, 0x3BAC798Cu, adjustment);
            if (func_00176E10(entity) == 0) FIELD(entity, 0x14, u32) = 0;
        }
    } else {
        u32 word = FIELD(entity, 0x96C, u32);
        if (word != 0) {
            void *object;
            request_scaled(entity, word, func_0019E538);
            FIELD(entity, 0x945, u8) = 2;
            object = FIELD(entity, 0x948, void *);
            if (object != 0) func_0013E4C8(object, 1);
        } else {
            FIELD(entity, 0x950, u32) = 0;
            FIELD(entity, 0x945, u8) = 2;
        }
    }
}

ACTOR_INLINE float wrap_actor_angle(float angle)
{
    if (3.1415927410125732f < angle) angle = angle - 6.2831854820251465f;
    else if (!(-3.1415927410125732f <= angle)) angle = angle + 6.2831854820251465f;
    return angle;
}
#define INTEGRATE_ACTOR_ANGLE(entity) \
 do { \
    float angle = FIELD(entity, 0x9A8, float) \
                + FIELD(entity, 0x9B0, float) * FIELD(entity, 0x35C, float); \
    angle = wrap_actor_angle(angle); \
    FIELD(entity, 0x9A8, float) = angle; \
    func_001784D0(entity, 1, angle); \
 } while (0)
ACTOR_INLINE void write_actor_blend(GeorgeGoalEntity *entity)
{
    float blend = FIELD(entity, 0x9A0, float);
    void *primary = FIELD(entity, 0x1B0, void *);
    if (0.0f <= blend) blend = george_ee_minimum(blend, 0.99000000953674316f);
    else blend = 0.0f;
    FIELD(primary, 0x428, float) = blend;
}
#define DECELERATE_ACTOR_ANGLE(entity) \
 do { \
    GeorgeActorBits64 speed = func_00374848(FIELD(entity, 0x9B0, float)); \
    float current = FIELD(entity, 0x9A8, float); \
    float offset = (FIELD(entity, 0x9AC, float) - current) * 0.63661974668502808f; \
    GeorgeActorBits64 scaled = func_00374848(offset); \
    s32 negative = func_00373250(scaled, 0); \
    float angle; \
    current = FIELD(entity, 0x9A8, float); \
    if (negative < 0) scaled = func_00372CC0(0, scaled); \
    scaled = func_00372D28(speed, scaled); \
    angle = func_003734F8(scaled); \
    angle = current + angle * FIELD(entity, 0x35C, float); \
    angle = wrap_actor_angle(angle); \
    FIELD(entity, 0x9A8, float) = angle; \
    func_001784D0(entity, 1, angle); \
 } while (0)
void func_001A6630(GeorgeGoalEntity *entity)
{
    s32 phase;
    control_void(entity, 0xA0);
    phase = FIELD(entity, 0x9C4, signed char);
    if (phase == 1) FIELD(entity, 0x9C4, u8) = FIELD(entity, 0x9C5, u8);
    else if (phase == 2) {
        GeorgeMathVec3 motion;
        const GeorgeMathVec3 *target = FIELD(entity, 0x9C8, const GeorgeMathVec3 *);
        float timer;
        motion.x = target->x - FIELD(entity, 0x40, float);
        motion.z = target->z - FIELD(entity, 0x48, float);
        motion.y = target->y - FIELD(entity, 0x44, float);
        motion.x *= 10.0f; motion.y *= 10.0f; motion.z *= 10.0f;
        control_vector(entity, 0x78, &motion);
        timer = FIELD(entity, 0x9BC, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x9BC, float) = timer;
        if (timer < 0.0f) {
            GeorgeActorBits64 flags = (FIELD(entity, 0x190, GeorgeActorBits64)
                                    | 0x80000000000ULL) & ~0x2000000ULL;
            void *primary = FIELD(entity, 0x1B0, void *);
            u32 word = FIELD(entity, 0x998, u32);
            FIELD(entity, 0x190, GeorgeActorBits64) = flags;
            func_00272390(primary, word, 0, 0, func_001A71A0, entity, 0, 0, 0, 0.0f, 5.0f);
            func_001A7378(FIELD(entity, 0x990, GeorgeGoalEntity *), 1);
            FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 1;
            FIELD(entity, 0x9C5, u8) = 3;
        }
    } else if (phase == 3) {
        float change = FIELD(entity, 0x9B8, float);
        float speed = FIELD(entity, 0x9B0, float);
        float sum = speed + change;
        if (change == 0.0f && __builtin_fabsf(speed) <= FIELD(entity->field18, 0x3E0, float))
            FIELD(entity, 0x9B0, u32) = 0;
        else {
            float limit = FIELD(entity, 0x9B4, float);
            if (0.0f < sum) sum = george_ee_minimum(sum, limit);
            else sum = george_ee_maximum(sum, -limit);
            FIELD(entity, 0x9B0, float) = sum;
        }
        INTEGRATE_ACTOR_ANGLE(entity);
        write_actor_blend(entity);
    } else if (phase == 4) {
        GeorgeActorBits64 delta;
        s32 negative;
        float current, target, first_delta, second_delta, speed;
        delta = func_00374848(FIELD(entity, 0x9C0, float) - FIELD(entity, 0x9A8, float));
        negative = func_00373250(delta, 0);
        current = FIELD(entity, 0x9A8, float);
        target = FIELD(entity, 0x9C0, float);
        if (negative < 0) delta = func_00372CC0(0, delta);
        if (func_00373250(delta, 0x400921FB60000000ULL) > 0) {
            delta = func_00374848(target - current);
            if (func_00373250(delta, 0) < 0) delta = func_00372CC0(0, delta);
            delta = func_00372CC0(delta, 0x401921FB60000000ULL);
            if (FIELD(entity, 0x9C0, float) - current < 0.0f)
                delta = func_00372D28(delta, 0xBFF0000000000000ULL);
            first_delta = func_003734F8(delta);
        } else first_delta = FIELD(entity, 0x9C0, float) - current;
        INTEGRATE_ACTOR_ANGLE(entity);
        write_actor_blend(entity);
        delta = func_00374848(FIELD(entity, 0x9C0, float) - FIELD(entity, 0x9A8, float));
        if (func_00373250(delta, 0) < 0) delta = func_00372CC0(0, delta);
        if (func_00373250(delta, 0x400921FB60000000ULL) > 0) {
            delta = func_00374848(FIELD(entity, 0x9C0, float) - FIELD(entity, 0x9A8, float));
            negative = func_00373250(delta, 0);
            current = FIELD(entity, 0x9A8, float);
            target = FIELD(entity, 0x9C0, float);
            if (negative < 0) delta = func_00372CC0(0, delta);
            delta = func_00372CC0(delta, 0x401921FB60000000ULL);
            if (target - current < 0.0f) delta = func_00372D28(delta, 0xBFF0000000000000ULL);
            second_delta = func_003734F8(delta);
        } else {
            current = FIELD(entity, 0x9A8, float);
            second_delta = FIELD(entity, 0x9C0, float) - current;
        }
        speed = FIELD(entity, 0x9B0, float);
        if (speed == 0.0f ||
            (0.0f < speed && 0.0f <= first_delta && second_delta <= 0.0f) ||
            (speed < 0.0f && first_delta <= 0.0f && 0.0f <= second_delta)) {
            float limit = FIELD(entity, 0x9B4, float);
            float fraction = speed / limit;
            FIELD(entity, 0x9C4, u8) = 5;
            FIELD(entity, 0x9AC, float) = current + fraction * 1.5707963705062866f;
        }
    } else if (phase == 5) {
        void *object;
        float angle;
        DECELERATE_ACTOR_ANGLE(entity);
        FIELD(FIELD(entity, 0x1B0, void *), 0x434, u32) = 0;
        request_scaled(entity, FIELD(entity, 0x99C, u32), func_001A71A0);
        object = FIELD(entity, 0x990, void *);
        angle = FIELD(entity, 0x9C0, float);
        FIELD(entity, 0x9C5, u8) = 6;
        func_001A7600((GeorgeGoalEntity *)object, angle);
        func_001A7378(FIELD(entity, 0x990, GeorgeGoalEntity *), 2);
    } else if (phase == 6) {
        float timer;
        DECELERATE_ACTOR_ANGLE(entity);
        timer = FIELD(entity, 0x9BC, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x9BC, float) = timer;
        if (timer < 0.0f) FIELD(entity, 0x14, u32) = 0;
    } else {
        request_scaled(entity, FIELD(entity, 0x994, u32), func_001A71A0);
        func_001A7378(FIELD(entity, 0x990, GeorgeGoalEntity *), 0);
        FIELD(entity, 0x9C5, u8) = 2;
    }
}

ACTOR_INLINE void request_actor34(GeorgeGoalEntity *entity, u32 word)
{
    void *primary = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x4E4, u32) = word;
    request_pair(entity, primary, word, func_00194388);
}
ACTOR_INLINE void release_actor34_handles(GeorgeGoalEntity *entity)
{
    u32 word = FIELD(entity, 0x28C, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x28C, u32) = 0;}
    word = FIELD(entity, 0x290, u32);
    if (word != 0) {func_002393F8(word); FIELD(entity, 0x290, u32) = 0;}
}
ACTOR_INLINE void return_actor34(GeorgeGoalEntity *entity)
{
    func_00170538(entity);
    entity->field0C = 0;
    actor_member(entity, 0);
}
ACTOR_INLINE const GeorgeRotationMatrix *find_actor34_matrix(GeorgeGoalEntity *entity,
                                                            void *primary)
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
            for (index = 0; index < count; ++index) {
                if (records[(u32)index * 0x20u + 0x1Du] == 0x29)
                    return (const GeorgeRotationMatrix *)ADDRESS(matrices, (u32)index << 6);
            }
        }
    }
    return 0;
}
void func_0017CA18(GeorgeGoalEntity *entity)
{
    s32 phase;
    FIELD(entity, 0x93C, float) = FIELD(entity, 0x93C, float) - FIELD(entity, 0x35C, float);
    func_001784D0(entity, 1, FIELD(entity, 0x58, float));
    phase = FIELD(entity, 0x938, s16);
    if (phase == 0) {
        void *effect;
        u32 key;
        request_actor34(entity, 0xBA);
        ensure_effect();
        effect = D_003F2D40;
        func_002BEBA0(&key, D_0042D618);
        FIELD(entity, 0x2EC, void *) = func_00251A88(effect, key);
        FIELD(entity, 0x938, u16) = 50;
    } else if (phase == 50) {
        if (FIELD(entity, 0x93C, float) <= 0.0f) {
            func_002455C0(FIELD(entity, 0x2EC, void *));
            request_actor34(entity, 0xBB);
            FIELD(entity, 0x940, u32) = 0;
            FIELD(entity, 0x938, u16) = 100;
        }
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 1ULL) == 0) return_actor34(entity);
    } else if (phase == 100) {
        float elapsed = FIELD(entity, 0x940, float) + FIELD(entity, 0x35C, float);
        void *data = entity->field18;
        FIELD(entity, 0x940, float) = elapsed;
        if (FIELD(data, 0x37C, float) <= elapsed) {
            u32 word = FIELD(data, 0x108, u32), key;
            const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
            const GeorgeRotationMatrix *matrix;
            GeorgeGeometryFrame frame;
            GeorgeMathVec3 lower, upper;
            void *effect, *handle, *primary;
            float radius;
            if (word != 0) {
                void *object;
                if (FIELD(entity, 0x28C, u32) != 0 || FIELD(entity, 0x290, u32) != 0)
                    func_00190F70(entity);
                object = func_00210078(word, 0, 0);
                if (object != 0) {
                    const GeorgeGoalVirtualWord *pair;
                    FIELD(entity, 0x28C, void *) = func_00236CB8(source, object, 0, 0, 0);
                    FIELD(entity, 0x290, void *) = func_00236CB8(source, object, 0, 0, 0);
                    pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0x20, const u8 *), 0x10);
                    pair->invoke(ADJUST(object, pair->adjustment), 0);
                }
            }
            func_002458E8(FIELD(entity, 0x2EC, void *));
            func_002457D8(FIELD(entity, 0x2EC, void *));
            func_00245B08(FIELD(entity, 0x2EC, void *));
            FIELD(entity, 0x2EC, void *) = 0;
            ensure_effect();
            effect = D_003F2D40;
            func_002BEBA0(&key, D_0042D628);
            handle = func_00251A88(effect, key);
            FIELD(entity, 0x2EC, void *) = handle;
            func_002455C0(handle);
            request_actor34(entity, 0xBC);
            primary = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x938, u16) = 200;
            FIELD(entity, 0x940, u32) = 0;
            matrix = find_actor34_matrix(entity, primary);
            func_002A2200((GeorgeRotationMatrix *)&frame, matrix, source);
            data = entity->field18;
            radius = FIELD(data, 0x0C, float);
            lower.x = frame.position.x + -radius;
            lower.z = frame.position.z + -radius;
            lower.y = frame.position.y + -radius;
            radius = FIELD(data, 0x0C, float);
            upper.x = frame.position.x + radius;
            upper.y = frame.position.y + FIELD(data, 0x08, float);
            upper.z = frame.position.z + radius;
            func_0018E5A8(entity, &lower, &upper, 0x10, 5);
        }
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 1ULL) == 0) return_actor34(entity);
    } else if (phase == 200) {
        const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
        const GeorgeMathVec3 *direction = VECTOR(entity, 0xD0);
        const GeorgeRotationMatrix *matrix = find_actor34_matrix(entity, FIELD(entity, 0x1B0, void *));
        GeorgeGeometryFrame frame;
        GeorgeMathVec4 bounds[2];
        GeorgeMathVec3 motion;
        void *data, *record;
        float radius, height, elapsed;
        func_002A2200((GeorgeRotationMatrix *)&frame, matrix, source);
        data = entity->field18;
        radius = FIELD(data, 0x0C, float);
        bounds[0].x = frame.position.x + -radius;
        bounds[0].z = frame.position.z + -radius;
        bounds[0].y = frame.position.y + -radius;
        radius = FIELD(data, 0x0C, float);
        height = FIELD(data, 0x08, float);
        record = FIELD(entity, 0x300, void *);
        bounds[1].x = frame.position.x + radius;
        bounds[1].y = frame.position.y + height;
        bounds[1].z = frame.position.z + radius;
        bounds[0].w = 0.0f; bounds[1].w = 0.0f;
        func_0030C440(record, bounds);
        func_002A35C0(&motion, direction, FIELD(entity->field18, 0x384, float));
        control_vector(entity, 0xB0, &motion);
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 1ULL) == 0) {
            func_002458E8(FIELD(entity, 0x2EC, void *));
            request_actor34(entity, 0xBD);
            if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
            FIELD(entity, 0x938, u16) = 300;
        }
        elapsed = FIELD(entity, 0x940, float) + FIELD(entity, 0x35C, float);
        data = entity->field18;
        FIELD(entity, 0x940, float) = elapsed;
        if (FIELD(data, 0x380, float) <= elapsed) {
            func_002458E8(FIELD(entity, 0x2EC, void *));
            request_actor34(entity, 0xBD);
            FIELD(entity, 0x938, u16) = 300;
        }
        if (guarded_predicate(entity) != 0) {
            release_actor34_handles(entity);
            func_00170538(entity);
            entity->field0C = 3;
            actor_member(entity, 3);
        }
    } else if (phase == 300) {
        release_actor34_handles(entity);
        control_void(entity, 0xA0);
        if (FIELD(entity, 0x93C, float) <= 0.0f) return_actor34(entity);
    } else if (phase == 400 || phase == 450) {
        func_002458E8(FIELD(entity, 0x2EC, void *));
        release_actor34_handles(entity);
        if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
        control_void(entity, 0xA0);
        request_actor34(entity, phase == 400 ? 0xBE : 0xBF);
        FIELD(entity, 0x938, u16) = 451;
    } else if (phase == 451) {
        control_void(entity, 0xA0);
        if (FIELD(entity, 0x93C, float) <= 0.0f) return_actor34(entity);
    }
}
