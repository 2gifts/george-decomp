#include "george/actor_controls.h"

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern u8 D_003F83F0[];
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042DA78[];
extern void *func_002AEE60(u32);
extern void *func_002481F0(void *);
extern s32 func_00100AA8(const void *, const void *);
extern void func_001007E0(GeorgeActorPointerRange *, void **, void *const *);
extern void func_002BD340(void);
extern s32 func_00396260(void (*)(void));
extern u32 *func_002BEBA0(u32 *, const u8 *);
extern void *func_00251A88(void *, u32);
extern void func_002469C0(void *);
extern void func_002455C0(void *);
extern void func_002457D8(void *);
extern void func_00245B08(void *);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372C68(GeorgeActorBits64, GeorgeActorBits64);
extern float func_003734F8(GeorgeActorBits64);
extern float func_0029B940(float, float);
extern void func_001413B8(void *, const GeorgeMathVec3 *, float);
extern void func_00141378(void *, float);
extern void *func_00210078(u32, u32, u32);
extern void *func_00236CB8(const void *, void *, u32, u32, u32);
extern void func_002393F8(u32);
extern void *func_002A6468(const void *, s32);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *, const GeorgeRotationMatrix *);
extern void func_002A1C08(void *, const void *);
extern void func_002A1C30(float *);
extern void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);
extern void func_00235CD8(void *, u32, u32);
extern s32 func_00270510(void *, u32, u32, u32, u32,
                       GeorgeActorRequestCallback, GeorgeGoalEntity *, u32, u32, float);

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

void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask)
{
    FIELD(entity, 0x190, GeorgeActorBits64) |= mask;
}
void func_0018FD40(GeorgeGoalEntity *entity, GeorgeActorBits64 mask)
{
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~mask;
}

/* Each atan call reloads the matrix. A callback may change actor+484 between
 * calls; these are deliberately repeated original calls, not one pure atan. */
#define REFERENCE_ANGLE(entity, result) do { \
    void *reference = FIELD(entity, 0x484, void *); \
    float probe_angle = func_0029B940(FIELD(reference, 0x28, float), FIELD(reference, 0x20, float)); \
    reference = FIELD(entity, 0x484, void *); \
    if (3.14159274101257324f < probe_angle) { \
        result = func_0029B940(FIELD(reference, 0x28, float), FIELD(reference, 0x20, float)) - 6.28318548202514648f; \
    } else { \
        probe_angle = func_0029B940(FIELD(reference, 0x28, float), FIELD(reference, 0x20, float)); \
        reference = FIELD(entity, 0x484, void *); \
        if (probe_angle < -3.14159274101257324f) \
            result = func_0029B940(FIELD(reference, 0x28, float), FIELD(reference, 0x20, float)) + 6.28318548202514648f; \
        else result = func_0029B940(FIELD(reference, 0x28, float), FIELD(reference, 0x20, float)); \
    } \
} while (0)

#define REFERENCE_BASIS(entity, coefficient_offset, basis_offset) do { \
    void *reference = FIELD(entity, 0x484, void *); \
    float coefficient = FIELD(entity, coefficient_offset, float); \
    float x = coefficient * FIELD(reference, basis_offset, float); \
    float z = coefficient * FIELD(reference, basis_offset + 8, float); \
    float y = coefficient * FIELD(reference, basis_offset + 4, float); \
    x = FIELD(reference, 0x30, float) + x; \
    z = FIELD(reference, 0x38, float) + z; \
    y = FIELD(reference, 0x34, float) + y; \
    FIELD(entity, 0x488, float) = x; \
    FIELD(entity, 0x490, float) = z; \
    FIELD(entity, 0x48C, float) = y; \
} while (0)

void func_00185EA0(GeorgeGoalEntity *entity, void *reference,
                   u32 flag0, u32 flag1, const GeorgeMathVec3 *input)
{
    float angle;
    GeorgeActorBits64 flags;
    FIELD(entity, 0x484, void *) = reference;
    func_0018FD40(entity, 0x4000ULL);
    control_void(entity, 0x38);
    REFERENCE_ANGLE(entity, angle);
    FIELD(entity, 0x504, float) = angle;
    FIELD(entity, 0x60, float) = angle;
    if (input != 0) {
        FIELD(entity, 0x494, float) = input->x;
        FIELD(entity, 0x498, float) = input->y;
        FIELD(entity, 0x49C, float) = input->z;
        REFERENCE_BASIS(entity, 0x494, 0x00);
        REFERENCE_BASIS(entity, 0x498, 0x10);
        REFERENCE_BASIS(entity, 0x49C, 0x20);
    } else {
        FIELD(entity, 0x494, u32) = 0;
        FIELD(entity, 0x49C, u32) = 0;
        FIELD(entity, 0x498, u32) = 0;
    }
    func_0018FD40(entity, 0x4000ULL);
    control_void(entity, 0x38);
    REFERENCE_ANGLE(entity, angle);
    FIELD(entity, 0x504, float) = angle;
    FIELD(entity, 0x60, float) = angle;
    reference = FIELD(entity, 0x484, void *);
    FIELD(entity, 0x488, float) = FIELD(reference, 0x30, float);
    FIELD(entity, 0x48C, float) = FIELD(reference, 0x34, float);
    FIELD(entity, 0x490, float) = FIELD(reference, 0x38, float);
    flags = FIELD(entity, 0x190, GeorgeActorBits64) | 0x80000ULL;
    FIELD(entity, 0x190, GeorgeActorBits64) = flags;
    FIELD(entity, 0x190, GeorgeActorBits64) = flag0 != 0 ? flags | (1ULL << 41) : flags & ~(1ULL << 41);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x190, GeorgeActorBits64) = flag1 != 0 ? flags | (1ULL << 40) : flags & ~(1ULL << 40);
}

s32 func_0018B710(GeorgeGoalEntity *entity, const GeorgeMathVec3 *direction,
                  const GeorgeMathVec3 *position)
{
    u32 state = entity->field0C;
    if (state != 0 && state != 2 && state != 4 && state != 29 && state != 30 && state != 40)
        return 0;
    FIELD(entity, 0x468, float) = FIELD(entity, 0x58, float);
    if (direction != 0) {
        FIELD(entity, 0x478, float) = direction->x;
        FIELD(entity, 0x47C, float) = direction->y;
        FIELD(entity, 0x480, float) = direction->z;
    } else {
        FIELD(entity, 0x478, float) = -FIELD(entity, 0xD0, float);
        FIELD(entity, 0x47C, float) = -FIELD(entity, 0xD4, float);
        FIELD(entity, 0x480, float) = -FIELD(entity, 0xD8, float);
    }
    if (position != 0) {
        FIELD(entity, 0x46C, float) = position->x;
        FIELD(entity, 0x470, float) = position->y;
        FIELD(entity, 0x474, float) = position->z;
    } else {
        FIELD(entity, 0x46C, u32) = 0;
        FIELD(entity, 0x474, u32) = 0;
        FIELD(entity, 0x470, u32) = 0;
    }
    FIELD(entity, 0x190, GeorgeActorBits64) |= 1ULL << 49;
    return 1;
}

void func_0018D8A0(GeorgeGoalEntity *entity, const GeorgeMathVec3 *input)
{
    void *object = FIELD(entity, 0x424, void *);
    float x = FIELD(object, 0x74, float), y = FIELD(object, 0x78, float);
    float z = FIELD(object, 0x7C, float), strength, length;
    GeorgeActorBits64 sum, term;
    void *data;
    sum = func_00374848(z);
    if (func_00373250(sum, 0) < 0) sum = func_00372CC0(0, sum);
    term = func_00374848(y);
    if (func_00373250(term, 0) < 0) term = func_00372CC0(0, term);
    sum = func_00372C68(sum, term);
    term = func_00374848(x);
    if (func_00373250(term, 0) < 0) term = func_00372CC0(0, term);
    strength = func_003734F8(func_00372C68(sum, term));
    if (0.0f < strength && FIELD(entity, 0x2F4, void *) == 0) {
        u32 key;
        void *effect;
        ensure_effect();
        effect = D_003F2D40;
        func_002BEBA0(&key, D_0042DA78);
        object = func_00251A88(effect, key);
        FIELD(entity, 0x2F4, void *) = object;
        func_002469C0(object);
        func_002455C0(FIELD(entity, 0x2F4, void *));
    }
    if (strength < 0.0f && FIELD(entity, 0x2F4, void *) != 0) {
        func_002457D8(FIELD(entity, 0x2F4, void *));
        func_00245B08(FIELD(entity, 0x2F4, void *));
        FIELD(entity, 0x2F4, void *) = 0;
    }
    x = input->x; y = input->y; z = input->z;
    data = entity->field18;
    length = george_ee_square_root((x*x + y*y) + z*z);
    if (FIELD(data, 0x14, float) < length) {
        float ratio = length / FIELD(data, 0x10, float);
        float scale = FIELD(data, 0x260, float) * ratio;
        func_001413B8(FIELD(entity, 0x424, void *), input, scale);
    } else {
        object = FIELD(entity, 0x424, void *);
        if (object != 0 && FIELD(object, 0x9C, u32) != 0)
            func_00141378(FIELD(entity, 0x424, void *), 0.949999988079071045f);
        else if (FIELD(entity, 0x2F4, void *) != 0) {
            func_002457D8(FIELD(entity, 0x2F4, void *));
            func_00245B08(FIELD(entity, 0x2F4, void *));
            FIELD(entity, 0x2F4, void *) = 0;
        }
    }
}

void func_00190E00(GeorgeGoalEntity *entity, u32 state)
{
    entity->field0C = state;
    actor_member(entity, state);
}

/* Complete input f12 survives the companion call and is restored for main. */
s32 func_00192C58(GeorgeGoalEntity *entity, u32 word,
                  GeorgeActorRequestCallback callback, GeorgeGoalEntity *context, float time)
{
    FIELD(entity, 0x4E4, u32) = word;
    if (FIELD(entity, 0x1B0, void *) != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                         FIELD(entity, 0x368, u32), 0, 0, 0, 0, time);
        return func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                            FIELD(entity, 0x368, u32), callback, context, 0, 0, time);
    }
    return 0;
}

ACTOR_INLINE void reference_word(void *reference)
{
    const GeorgeGoalVirtualWord *pair = (const GeorgeGoalVirtualWord *)
        ADDRESS(FIELD(reference, 0x20, const void *), 0x10);
    pair->invoke(ADJUST(reference, pair->adjustment), 0);
}
void func_00190EB0(GeorgeGoalEntity *entity, u32 word)
{
    void *reference;
    const void *source;
    if (FIELD(entity, 0x28C, u32) != 0 || FIELD(entity, 0x290, u32) != 0)
        func_00190F70(entity);
    reference = func_00210078(word, 0, 0);
    if (reference != 0) {
        source = ADDRESS(entity, 0xB0);
        FIELD(entity, 0x28C, void *) = func_00236CB8(source, reference, 0, 0, 0);
        FIELD(entity, 0x290, void *) = func_00236CB8(source, reference, 0, 0, 0);
        reference_word(reference);
    }
}
void func_00190F70(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x28C, u32) != 0) {
        func_002393F8(FIELD(entity, 0x28C, u32));
        FIELD(entity, 0x28C, u32) = 0;
    }
    if (FIELD(entity, 0x290, u32) != 0) {
        func_002393F8(FIELD(entity, 0x290, u32));
        FIELD(entity, 0x290, u32) = 0;
    }
}
void func_00191DC8(GeorgeGoalEntity *entity, u32 word)
{
    void *reference;
    if (FIELD(entity, 0x298, u32) != 0) func_00191E50(entity);
    reference = func_00210078(word, 0, 0);
    FIELD(entity, 0x298, void *) = func_00236CB8(ADDRESS(entity, 0xB0), reference, 0, 0, 0);
    /* Retail has no null reference guard: the original callee contract applies. */
    reference_word(reference);
}
void func_00191E50(GeorgeGoalEntity *entity)
{
    if (FIELD(entity, 0x298, u32) != 0) {
        func_002393F8(FIELD(entity, 0x298, u32));
        FIELD(entity, 0x298, u32) = 0;
    }
}

/* These two complete bodies differ only in source member/record selection. */
#define FIND_MATRIX(name, gate_offset, collection_offset) \
GeorgeRotationMatrix *name(GeorgeGoalEntity *entity, u32 identifier) { \
    void *object = FIELD(entity, gate_offset, void *); \
    u32 command, offset, i; \
    s32 count; \
    const u8 *records; \
    GeorgeRotationMatrix *matrix; \
    if (object == 0) return 0; \
    command = FIELD(FIELD(entity, 0x1B0, void *), 0x0C, u32); \
    if (command == 3) command = 2; \
    offset = command << 2; \
    count = (s32)func_002A6460(FIELD(object, 0x378 + offset, const void *)); \
    if (count <= 0) return 0; \
    object = FIELD(entity, collection_offset, void *); \
    records = func_002A6468(FIELD(object, 0x378 + offset, const void *), 0); \
    object = FIELD(entity, collection_offset, void *); \
    matrix = FIELD(object, 0x3E8 + offset, GeorgeRotationMatrix *); \
    for (i = 0; i < (u32)count; ++i) { \
        if (records[0x1D] == identifier) return matrix; \
        matrix = (GeorgeRotationMatrix *)ADDRESS(matrix, 0x40); \
        records = ADDRESS(records, 0x20); \
    } \
    return 0; \
}
FIND_MATRIX(func_00192748, 0x1B0, 0x1B0)
FIND_MATRIX(func_00192818, 0x1B4, 0x1B4)

#define RELEASE_STORED(name, object_offset, word_offset) \
void name(GeorgeGoalEntity *entity) { \
    void *object = FIELD(entity, object_offset, void *); \
    if (object != 0) { \
        FIELD(entity, word_offset, u32) = FIELD(object, 8, u32); \
        func_002393F8((u32)object); \
        FIELD(entity, object_offset, void *) = 0; \
    } \
}
RELEASE_STORED(func_001913A0, 0x228, 0x460)
RELEASE_STORED(func_001913E0, 0x224, 0x464)

#define CREATE_STORED(name, cleanup, object_offset, word_offset, identifier) \
void name(GeorgeGoalEntity *entity, u32 word) { \
    GeorgeRotationMatrix matrix; \
    GeorgeRotationMatrix *source; \
    void *reference, *object; \
    if (FIELD(entity, object_offset, void *) != 0) cleanup(entity); \
    if (word == 0) word = FIELD(entity, word_offset, u32); \
    if (word == 0) return; \
    source = func_00192748(entity, identifier); \
    func_002A2200(&matrix, source, (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0)); \
    reference = func_00210078(word, 0, 0); \
    if (reference != 0) { \
        object = func_00236CB8(&matrix, reference, 1, 0, 0); \
        FIELD(entity, object_offset, void *) = object; \
        func_002A1C08(ADDRESS(object, 0x10), &matrix); \
        FIELD(object, 0xA0, u32) |= 0x100; \
        reference_word(reference); \
    } \
}
CREATE_STORED(func_001911F0, func_001913A0, 0x228, 0x460, 0x62)
CREATE_STORED(func_001912C8, func_001913E0, 0x224, 0x464, 0x61)

/* Shared complete repeated attachment block. The caller-specific phase gate
 * and phase store stay outside this source macro. No private helper binding. */
#define CREATE_ATTACHMENT(entity, word) do { \
    GeorgeRotationMatrix matrix; \
    GeorgeMathVec3 translation; \
    GeorgeRotationMatrix *source; \
    void *object; \
    func_00191DC8(entity, word); \
    source = func_00192748(entity, 0x29); \
    func_002A1C60(ADDRESS(entity, 0xF0), VECTOR(source, 0x30), &translation); \
    func_002A1C30(matrix.element); \
    matrix.element[12] = translation.x; \
    matrix.element[13] = translation.y; \
    matrix.element[15] = 1.0f; \
    matrix.element[14] = translation.z; \
    object = FIELD(entity, 0x298, void *); \
    func_002A1C08(ADDRESS(object, 0x10), &matrix); \
    FIELD(object, 0xA0, u32) |= 0x100; \
    func_00235CD8(FIELD(entity, 0x298, void *), 0x9F79558FU, 1); \
} while (0)

void func_00191D08(GeorgeGoalEntity *entity)
{
    u32 word = FIELD(entity->field18, 0x10C, u32);
    if (word != 0) CREATE_ATTACHMENT(entity, word);
}
void func_00191670(GeorgeGoalEntity *entity, u32 mode)
{
    if (FIELD(entity, 0x938, s16) < 400) {
        u32 word = FIELD(entity->field18, 0x10C, u32);
        if (word != 0) CREATE_ATTACHMENT(entity, word);
        FIELD(entity, 0x938, s16) = mode != 0 ? 400 : 450;
    }
}
