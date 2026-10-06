#include "george/actor_states2.h"
#include "george/ee_math.h"

/* These helpers express original in-function sequences. GCC 3 otherwise
 * outlines the larger helpers at -O2, inventing a non-retail code dependency.
 * The older EE GCC already inlines these bodies and predates this attribute. */
#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042D640[];
extern void *func_002AEE60(u32 size);
extern void *func_002481F0(void *object);
extern s32 func_00100AA8(const void *first, const void *second);
extern void func_001007E0(GeorgeActorPointerRange *range, void **position, void *const *value);
extern void func_002BD340(void);
extern s32 func_00396260(void (*callback)(void));
extern u32 *func_002BEBA0(u32 *output, const u8 *text);
extern void func_00251DB0(void *object, u32 key, const GeorgeMathVec3 *position);
extern void func_001B67C0(void *context, GeorgeGoalEntity *entity, u32 word0,
                       u32 word1, u32 word2, u32 word3);
extern s32 func_00270510(void *object, u32 word, u32 mode0, u32 mode1,
                       u32 owner_word, GeorgeActorRequestCallback callback,
                       GeorgeGoalEntity *context, u32 invoke_word,
                       u32 callback_word, float time);
extern void func_002393F8(u32 word);
extern void func_00235CD8(void *object, u32 key, u32 word);
extern u32 *func_002AAF50(void *array, s32 index);
extern s32 func_002AAF88(void *array);
extern void func_002AAFD8(void *array);
extern void func_00272A58(void *object);
extern void func_00196980(GeorgeGoalEntity *entity);
extern void func_002A2200(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *first,
                        const GeorgeRotationMatrix *second);
extern u32 func_00236A10(const GeorgeRotationMatrix *matrix, u32 word,
                      u32 value0, u32 value1, u32 value2, u32 value3);
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void *func_0022C760(u32 word);
extern void *func_00238BA0(void *object, u32 key);
extern s32 func_0018FCF0(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern s32 func_00272C10(void *object);
extern s32 func_00272C30(void *object);
extern GeorgeActorBits64 func_00374848(float value);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372C68(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern s32 func_00373250(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern float func_003734F8(GeorgeActorBits64 value);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define CONTROL_PAIR(object, offset, type) ((const type *)ADDRESS((object)->field00, offset))
#define VEHICLE(entity) FIELD(entity, 0x730, GeorgeGoalVirtualObject *)
#define VEHICLE_PAIR(object, offset, type) ((const type *)ADDRESS((object)->field04, offset))

static __inline__ void control_void(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualVoid *pair = CONTROL_PAIR(object, offset, GeorgeGoalVirtualVoid);
    pair->invoke(ADJUST(object, pair->adjustment));
}
static __inline__ void control_word(GeorgeGoalEntity *entity, u32 offset, u32 word)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualWord *pair = CONTROL_PAIR(object, offset, GeorgeGoalVirtualWord);
    pair->invoke(ADJUST(object, pair->adjustment), word);
}
static __inline__ void control_vector(GeorgeGoalEntity *entity, u32 offset, const GeorgeMathVec3 *vector)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualVectorInput *pair = CONTROL_PAIR(object, offset, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}
static __inline__ void captured_control_vector(GeorgeActorControlObject *object, u32 offset,
                                             const GeorgeMathVec3 *vector)
{
    const GeorgeActorVirtualVectorInput *pair = CONTROL_PAIR(object, offset, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}
static __inline__ s32 control_predicate(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualPredicate *pair = CONTROL_PAIR(object, 0xD0, GeorgeActorVirtualPredicate);
    return pair->invoke(ADJUST(object, pair->adjustment), 0.20000000298023224f, 0.25f);
}
static __inline__ s32 control_int(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualInt *pair = CONTROL_PAIR(object, offset, GeorgeGoalVirtualInt);
    return pair->invoke(ADJUST(object, pair->adjustment));
}
static __inline__ const GeorgeMathVec3 *control_output(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeGoalVirtualVector *pair = CONTROL_PAIR(object, offset, GeorgeGoalVirtualVector);
    return pair->invoke(ADJUST(object, pair->adjustment));
}

/* Repeated original registration blocks reuse the existing upper-bound API.
 * Capture the registry inputs before stores to a potentially aliased record;
 * reload the range's end after both the callback and final record store. */
ACTOR_INLINE void ensure_effect(void)
{
    if (D_003F2D40 == 0) {
        GeorgeActorEffectRecord *record;
        void *registered;
        void *value;
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

static __inline__ void emit_effect(GeorgeGoalEntity *entity)
{
    void *effect;
    u32 key;
    GeorgeMathVec3 position;
    ensure_effect();
    effect = D_003F2D40;
    func_002BEBA0(&key, D_0042D640);
    position.x = FIELD(entity, 0x40, float);
    position.y = FIELD(entity, 0x44, float);
    position.z = FIELD(entity, 0x48, float);
    func_00251DB0(effect, key, &position);
}

static __inline__ void play_pair(GeorgeGoalEntity *entity, u32 word, float first_time, float second_time)
{
    if (FIELD(entity, 0x1B0, void *) != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                         FIELD(entity, 0x368, u32), 0, 0, 0, 0, first_time);
        func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                     FIELD(entity, 0x368, u32), 0, 0, 0, 0, second_time);
    }
}

void func_0017DD10(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 *position = VECTOR(entity, 0x40);
    GeorgeActorBits64 flags;
    float time;
    u32 word, effect_word;
    if (FIELD(entity, 0x52C, signed char) != 0) {
        GeorgeMathVec3 delta, horizontal;
        float gravity, initial, discriminant, horizontal_length;
        control_word(entity, 0x30, 0);
        delta.x = FIELD(entity, 0x514, float) - position->x;
        delta.z = FIELD(entity, 0x51C, float) - position->z;
        delta.y = FIELD(entity, 0x518, float) - position->y;
        gravity = 9.8100004196166992f - FIELD(entity->field18, 0x58, float);
        initial = george_ee_square_root((gravity + gravity) * FIELD(entity, 0x520, float));
        discriminant = initial * initial - (gravity + gravity) * delta.y;
        if (!(0.0f <= discriminant)) discriminant = -discriminant;
        discriminant = george_ee_square_root(discriminant);
        horizontal_length = george_ee_square_root(delta.x * delta.x + delta.z * delta.z);
        time = (initial + discriminant) / gravity;
        horizontal.x = delta.x;
        horizontal.y = 0.0f;
        horizontal.z = delta.z;
        FIELD(entity, 0x528, float) = time;
        func_002A35C0(&horizontal, &horizontal, horizontal_length / time);
        horizontal.y = initial;
        control_vector(entity, 0x78, &horizontal);
    } else {
        GeorgeMathVec3 *motion = VECTOR(entity, 0x54C);
        const GeorgeMathVec3 *current;
        float scale, gravity;
        GeorgeActorControlObject *object;
        control_word(entity, 0x30, 1);
        scale = FIELD(entity, 0x530, float);
        motion->x = FIELD(entity, 0x540, float) * scale;
        motion->y = FIELD(entity, 0x544, float) * scale;
        motion->z = FIELD(entity, 0x548, float) * scale;
        current = control_output(entity, 0x68);
        motion->y = (FIELD(entity, 0x534, float) - current->y) + FIELD(entity, 0x98, float);
        gravity = 9.8000001907348633f - FIELD(entity->field18, 0x58, float);
        object = CONTROL(entity);
        FIELD(entity, 0x528, float) = (FIELD(entity, 0x534, float) + FIELD(entity, 0x534, float)) / gravity;
        captured_control_vector(object, 0x98, motion);
    }
    {
        const GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
        float squared = (motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z;
        if (squared < 0.09000000357627869f && FIELD(entity, 0x530, float) < 0.30000001192092895508f) {
            flags = FIELD(entity, 0x190, GeorgeActorBits64);
            time = george_ee_maximum(FIELD(entity, 0x528, float), 0.0f);
            word = FIELD(entity, 0x538, u32);
            FIELD(entity, 0x190, GeorgeActorBits64) = flags | (1ULL << 35);
        } else {
            time = george_ee_maximum(FIELD(entity, 0x528, float), 0.0f);
            word = FIELD(entity, 0x53C, u32);
        }
        FIELD(entity, 0x4E4, u32) = word;
        play_pair(entity, word, time, time);
    }
    /* Original flag21 branches repeat identical registration/emission blocks. */
    emit_effect(entity);
    effect_word = FIELD(entity->field18, 0x7C, u32);
    if (effect_word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000ULL) == 0)
        func_001B67C0(position, FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                     FIELD(entity, 0x3B4, u32), effect_word, 0, 0);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((flags & 0x80ULL) != 0) {
        FIELD(entity, 0x380, u32) = 0;
        FIELD(entity, 0x190, GeorgeActorBits64) = flags & ~0x80ULL;
    }
    time = FIELD(entity->field18, 0x6C, float);
    FIELD(entity, 0x52F, signed char) = 1;
    FIELD(entity, 0x388, float) = time;
    FIELD(entity, 0x3C4, u32) = 0;
    FIELD(entity, 0x524, u32) = 0;
    FIELD(entity, 0x52D, signed char) = 0;
    FIELD(entity, 0x52E, signed char) = 0;
}

void func_0017E3A0(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 *velocity = VECTOR(entity, 0x4C);
    float step = FIELD(entity, 0x35C, float);
    float timer = FIELD(entity, 0x438, float);
    FIELD(entity, 0x524, float) = FIELD(entity, 0x524, float) + step;
    if (0.0f < timer) FIELD(entity, 0x438, float) = timer - step;
    if (velocity->y < FIELD(entity, 0x3C4, float)) FIELD(entity, 0x3C4, float) = velocity->y;
    if (FIELD(entity, 0x52C, signed char) != 0) {
        control_vector(entity, 0xB8, 0);
        if (FIELD(entity, 0x528, float) <= FIELD(entity, 0x524, float)) {
            s32 result = 0;
            control_word(entity, 0x30, 1);
            if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x80080ULL) == 0)
                result = control_predicate(entity);
            FIELD(entity, 0x14, u32) = result != 0 ? 3 : 5;
        }
        return;
    }
    if (0.0f < FIELD(entity, 0x438, float)) {
        GeorgeMathVec3 scratch;
        func_002A35C0(&scratch, VECTOR(entity, 0x54C), FIELD(entity->field18, 0x10, float));
        control_vector(entity, 0xB8, &scratch);
    } else {
        GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
        GeorgeActorControlObject *object = CONTROL(entity);
        FIELD(entity, 0x190, GeorgeActorBits64) = flags & ~0x40000ULL;
        captured_control_vector(object, 0xB8, VECTOR(entity, 0x68));
    }
    {
        signed char enabled = FIELD(entity, 0x52E, signed char);
        FIELD(entity, 0x74, u32) = 0;
        if (enabled != 0 && FIELD(entity, 0x52D, signed char) == 0 &&
            FIELD(entity, 0x438, float) <= FIELD(entity, 0x74, float)) {
            float ratio = FIELD(entity, 0x524, float) / FIELD(entity, 0x528, float);
            if ((0.30000001192092895508f < ratio && ratio < 0.69999998807907104492f) ||
                FIELD(entity, 0x558, u32) != 0) {
                GeorgeMathVec3 scratch;
                void *data = entity->field18;
                GeorgeActorControlObject *object;
                float gravity, time, second;
                void *animation;
                u32 word;
                scratch.x = 0.0f;
                scratch.z = 0.0f;
                scratch.y = (FIELD(data, 0x40, float) - velocity->y) + FIELD(entity, 0x98, float);
                object = CONTROL(entity);
                gravity = 9.8000001907348633f - FIELD(data, 0x58, float);
                time = (FIELD(data, 0x40, float) + FIELD(data, 0x40, float)) / gravity;
                FIELD(entity, 0x528, float) = time;
                gravity = 9.8000001907348633f - FIELD(data, 0x58, float);
                second = FIELD(data, 0x3C, float);
                second = george_ee_square_root((second * second / (gravity + gravity)) / gravity);
                FIELD(entity, 0x528, float) = time + second;
                captured_control_vector(object, 0x98, &scratch);
                animation = FIELD(entity, 0x1B0, void *);
                time = FIELD(entity, 0x528, float);
                FIELD(entity, 0x52D, signed char) = 1;
                FIELD(entity, 0x4E4, u32) = 0xF;
                FIELD(entity, 0x524, u32) = 0;
                if (animation != 0) {
                    if (FIELD(entity, 0x1B4, void *) != 0)
                        func_00270510(FIELD(entity, 0x1B4, void *), 0xF, 0, 0,
                                     FIELD(entity, 0x368, u32), 0, 0, 0, 0, time);
                    func_00270510(FIELD(entity, 0x1B0, void *), 0xF, 0, 0,
                                 FIELD(entity, 0x368, u32), 0, 0, 0, 0, time);
                }
                emit_effect(entity);
                word = FIELD(entity->field18, 0x80, u32);
                if (word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000ULL) == 0)
                    func_001B67C0(VECTOR(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                                 FIELD(entity, 0x3B4, u32), word, 0, 0);
            }
        }
    }
    {
        s32 result = control_int(entity, 0xD8);
        signed char moving = FIELD(entity, 0x52F, signed char);
        if (result != 0) {
            if (moving != 0) {
                control_word(entity, 0x30, 0);
                control_vector(entity, 0x78, VECTOR(entity, 0x54C));
            } else FIELD(entity, 0x14, u32) = 5;
        } else if (moving != 0) {
            control_word(entity, 0x30, 1);
            control_void(entity, 0x88);
            control_vector(entity, 0x80, VECTOR(entity, 0x54C));
            FIELD(entity, 0x52F, signed char) = 0;
        }
    }
    if (FIELD(entity, 0x528, float) <= FIELD(entity, 0x524, float)) {
        s32 result = 0;
        u32 word;
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x80080ULL) == 0)
            result = control_predicate(entity);
        if (result != 0) FIELD(entity, 0x14, u32) = 3;
        else {
            void *target = 0;
            word = FIELD(entity, 0x410, u32);
            if (word != 0) {
                void *object = func_0022C760(word);
                target = func_00238BA0(FIELD(object, 0xC, void *), 0xD44DAD70U);
            }
            if (target != 0) {
                FIELD(entity, 0x14, u32) = 0;
                func_0018FD30(entity, 1ULL << 42);
                FIELD(entity, 0x414, float) = velocity->x;
                FIELD(entity, 0x418, float) = velocity->y;
                FIELD(entity, 0x41C, float) = velocity->z;
                FIELD(entity, 0x420, void *) = target;
            } else FIELD(entity, 0x14, u32) = 5;
        }
    }
}

void func_0017ED18(GeorgeGoalEntity *entity, s32 mode)
{
    GeorgeRotationMatrix identity __attribute__((aligned(16)));
    GeorgeRotationMatrix matrix __attribute__((aligned(16)));
    u32 word;
    unsigned i;
    /* Keep the observed unusual first row; this is not a normal identity. */
    for (i=0; i<16; i++) identity.element[i]=0.0f;
    identity.element[2]=1.0f;
    identity.element[4]=1.0f;
    identity.element[9]=1.0f;
    identity.element[15]=1.0f;
    func_002A2200(&matrix, &identity, (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0));
    word = FIELD(entity, mode == 0 ? 0x3DC : mode == 2 ? 0x3E4 : 0x3E0, u32);
    if (word != 0) func_002393F8(func_00236A10(&matrix, word, 0, 0, 0, 0));
}

void func_0017EE30(GeorgeGoalEntity *entity, s32 mode)
{
    GeorgeMathVec3 position;
    void *effect;
    u32 word;
    u32 offset = 0x3D0U + (u32)mode * 4U;
    if (FIELD(entity, offset, u32) == 0 || FIELD(entity->field18, 0x90, u32) == 0) return;
    ensure_effect();
    position.x = FIELD(entity, 0x40, float);
    effect = D_003F2D40;
    word = FIELD(entity, offset, u32);
    position.y = FIELD(entity, 0x44, float);
    position.z = FIELD(entity, 0x48, float);
    func_00251DB0(effect, word, &position);
}

void func_0017EF88(GeorgeGoalEntity *entity)
{
    s32 mode = 0;
    void *data;
    float minimum;
    control_word(entity, 0x30, 1);
    data = entity->field18;
    minimum = FIELD(entity, 0x3C4, float);
    if (minimum <= -FIELD(data, 0x98, float)) {
        u32 word;
        void *array;
        float duration = FIELD(data, 0x9C, float);
        FIELD(entity, 0x568, float) = duration;
        duration = FIELD(data, 0x9C, float);
        FIELD(entity, 0x4E4, u32) = 0x18;
        play_pair(entity, 0x18, duration, duration);
        func_0017EBF0(entity);
        if (FIELD(entity, 0x23C, void *) != 0)
            func_00235CD8(FIELD(entity, 0x23C, void *), 0xB95616B6U, 1);
        array = FIELD(entity, 0x294, void *);
        if (array != 0) {
            s32 count = func_002AAF88(array);
            s32 index;
            for (index=0; index<count; index++) {
                word = *func_002AAF50(FIELD(entity, 0x294, void *), index);
                if (word != 0) func_002393F8(word);
            }
            func_002AAFD8(FIELD(entity, 0x294, void *));
        }
        if (FIELD(entity, 0x2D8, u32) == 1) {
            func_00272A58(FIELD(entity, 0x1B0, void *));
            if (FIELD(entity, 0x300, u32) != 0) func_00196980(entity);
            FIELD(entity, 0x2D8, u32) = 0;
        }
        word = FIELD(entity->field18, 0xA4, u32);
        if (word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000ULL) == 0)
            func_001B67C0(VECTOR(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                         FIELD(entity, 0x3B4, u32), word, 0, 0);
        mode = 2;
    } else {
        const GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
        float squared;
        u32 word;
        if (minimum <= -FIELD(data, 0x94, float)) mode = 1;
        FIELD(entity, 0x568, u32) = 0;
        squared = (motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z;
        if (squared < 0.09000000357627869f) word = mode == 1 ? 0x17 : 0x14;
        else if (FIELD(data, 0x2C, float) < george_ee_square_root(squared) / FIELD(data, 0x10, float)) word = 0x16;
        else word = 0x15;
        FIELD(entity, 0x4E4, u32) = word;
        play_pair(entity, word, FIELD(entity, 0x568, float), 0.0f);
    }
    func_0017EE30(entity, mode);
    func_0017ED18(entity, mode);
    FIELD(entity, 0x3C4, u32) = 0;
    {
        GeorgeMathVec3 result;
        const GeorgeMathVec3 *input = VECTOR(entity, 0x4C);
        const GeorgeMathVec3 *basis = VECTOR(entity, 0x88);
        float x=input->x, y=input->y, z=input->z;
        float bx=basis->x, by=basis->y, bz=basis->z;
        float dot=(x*bx + y*by) + z*bz;
        result.x = x - dot*bx;
        result.z = z - dot*bz;
        result.y = y - dot*by;
        control_vector(entity, 0x78, &result);
    }
}

static __inline__ s32 vehicle_int(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeGoalVirtualObject *object = VEHICLE(entity);
    const GeorgeGoalVirtualInt *pair = VEHICLE_PAIR(object, offset, GeorgeGoalVirtualInt);
    return pair->invoke(ADJUST(object, pair->adjustment));
}
static __inline__ s32 vehicle_command(GeorgeGoalEntity *entity, u32 offset)
{
    GeorgeGoalVirtualObject *object = VEHICLE(entity);
    s32 command = FIELD(entity, 0x734, s32);
    const GeorgeGoalVirtualCommand *pair = VEHICLE_PAIR(object, offset, GeorgeGoalVirtualCommand);
    return pair->invoke(ADJUST(object, pair->adjustment), command);
}
static __inline__ void vehicle_vector(GeorgeGoalEntity *entity, u32 offset, u32 command_offset,
                                     GeorgeMathVec3 *vector, float *scalar)
{
    GeorgeGoalVirtualObject *object = VEHICLE(entity);
    s32 command = FIELD(entity, command_offset, s32);
    const GeorgeActorVirtualCommandScalarVector *pair = VEHICLE_PAIR(object, offset, GeorgeActorVirtualCommandScalarVector);
    pair->invoke(ADJUST(object, pair->adjustment), command, vector, scalar);
}

/* Original requests repeat this sequence. The animation object is captured
 * before state stores; companion callbacks may replace the later main object. */
static __inline__ s32 vehicle_request(GeorgeGoalEntity *entity, u32 phase, u32 word,
                                    s32 companion_timer)
{
    void *animation = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x740, u32) = phase;
    FIELD(entity, 0x744, u32) = 0;
    FIELD(entity, 0x4E4, u32) = word;
    if (animation != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0)
            func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                         FIELD(entity, 0x368, u32), 0, 0, 0, 0,
                         companion_timer ? FIELD(entity, 0x744, float) : 0.0f);
        return func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                            FIELD(entity, 0x368, u32), func_00194DE0, entity, 0, 0, 0.0f);
    }
    return 0;
}
static __inline__ u32 vehicle_word(GeorgeGoalEntity *entity, void *configuration, u32 offset)
{
    s32 command = FIELD(entity, 0x734, s32);
    return FIELD(configuration, command == 0 || command == 2 ? offset : offset + 4, u32);
}
static __inline__ void vehicle_status(GeorgeGoalEntity *entity, u32 offset)
{
    void *animation = FIELD(entity, 0x1B0, void *);
    if (animation != 0 && (func_00272C10(animation) != 0 ||
                          func_00272C30(FIELD(entity, 0x1B0, void *)) != 0))
        vehicle_command(entity, offset);
}
static __inline__ void separation_gate(GeorgeGoalEntity *entity, const GeorgeMathVec3 *position)
{
    const GeorgeMathVec3 *target = VECTOR(entity, 0x170);
    float x = target->x - position->x;
    float y = target->y - position->y;
    float z = target->z - position->z;
    if (2.0f < george_ee_square_root((x*x + y*y) + z*z) &&
        func_0018FCF0(entity, 0x10000000ULL) == 0)
        FIELD(entity, 0x14, u32) = 0;
}
static __inline__ void interpolate_vehicle(GeorgeGoalEntity *entity,
                                           const GeorgeMathVec3 *first,
                                           const GeorgeMathVec3 *second,
                                           const float *first_angle,
                                           const float *second_angle,
                                           s32 xzy)
{
    float blend = FIELD(entity, 0x744, float) / FIELD(entity, 0x748, float);
    float x = first->x, y = first->y, z = first->z;
    float dx = second->x - x, dy = second->y - y, dz = second->z - z;
    FIELD(entity, 0x40, float) = x + blend * dx;
    if (xzy) {
        FIELD(entity, 0x48, float) = z + blend * dz;
        FIELD(entity, 0x44, float) = y + blend * dy;
    } else {
        FIELD(entity, 0x44, float) = y + blend * dy;
        FIELD(entity, 0x48, float) = z + blend * dz;
    }
    /* Angles are read after the position stores. */
    FIELD(entity, 0x74C, float) = *first_angle * (1.0f - blend) + *second_angle * blend;
}
ACTOR_INLINE void update_vehicle_angle(GeorgeGoalEntity *entity, const float *target)
{
    GeorgeActorBits64 step, current, delta, adjustment, result;
    float target_angle, current_angle;
    s32 comparison, negative;
    step = func_00374848(FIELD(entity, 0x35C, float));
    current = func_00374848(FIELD(entity, 0x74C, float));
    delta = func_00374848(*target - FIELD(entity, 0x74C, float));
    if (func_00373250(delta, 0ULL) < 0) delta = func_00372CC0(0ULL, delta);
    comparison = func_00373250(delta, 0x400921FB60000000ULL);
    target_angle = *target;
    if (comparison > 0) {
        delta = func_00374848(target_angle - FIELD(entity, 0x74C, float));
        negative = func_00373250(delta, 0ULL) < 0;
        target_angle = *target;
        current_angle = FIELD(entity, 0x74C, float);
        if (negative) delta = func_00372CC0(0ULL, delta);
        adjustment = func_00372CC0(delta, 0x401921FB60000000ULL);
        if (target_angle - current_angle < 0.0f)
            step = func_00372D28(step, 0xBFF0000000000000ULL);
        adjustment = func_00372D28(adjustment, step);
        adjustment = func_00372D28(adjustment, 0x4014000000000000ULL);
        result = func_00372C68(current, adjustment);
    } else {
        delta = func_00374848(target_angle - FIELD(entity, 0x74C, float));
        adjustment = func_00372D28(delta, step);
        adjustment = func_00372D28(adjustment, 0x4014000000000000ULL);
        result = func_00372C68(current, adjustment);
    }
    {
        float angle = func_003734F8(result);
        /* The request pointer precedes the angle store in the original. */
        void *animation = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x74C, float) = angle;
        if (animation != 0 && (func_00272C10(animation) != 0 ||
                              func_00272C30(FIELD(entity, 0x1B0, void *)) != 0))
            vehicle_command(entity, 0x100);
    }
}

void func_00183C58(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 first, second, third;
    float first_angle, second_angle, third_angle;
    void *configuration;
    s32 phase;
    {
        float time = FIELD(entity, 0x744, float) + FIELD(entity, 0x35C, float);
        GeorgeGoalVirtualObject *object = VEHICLE(entity);
        const GeorgeGoalVirtualPointer *pair;
        FIELD(entity, 0x744, float) = time;
        pair = VEHICLE_PAIR(object, 0x200, GeorgeGoalVirtualPointer);
        configuration = pair->invoke(ADJUST(object, pair->adjustment));
    }
    vehicle_vector(entity, 0xD8, 0x734, &first, &first_angle);
    vehicle_vector(entity, 0xF0, 0x734, &second, &second_angle);
    vehicle_vector(entity, 0xF0, 0x738, &third, &third_angle);
    phase = FIELD(entity, 0x740, s32);
    switch (phase) {
    case 1: case 101: case 201: case 301: case 401:
    case 2001: case 2101: case 2201:
        break;
    case 2:
        separation_gate(entity, &second);
        {
            const GeorgeMathVec3 *target = VECTOR(entity, 0x170);
            float x = FIELD(entity, 0x40, float), y = FIELD(entity, 0x44, float), z = FIELD(entity, 0x48, float);
            float dx = target->x - x, dy = target->y - y, dz = target->z - z;
            float blend = FIELD(entity, 0x35C, float) * 10.0f;
            FIELD(entity, 0x40, float) = x + blend * dx;
            FIELD(entity, 0x48, float) = FIELD(entity, 0x48, float) + blend * dz;
            FIELD(entity, 0x44, float) = FIELD(entity, 0x44, float) + blend * dy;
        }
        update_vehicle_angle(entity, &first_angle);
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float))
            FIELD(entity, 0x740, u32) = 100;
        break;
    case 100:
        FIELD(entity, 0x740, u32) = 200;
        break;
    case 200:
        if (vehicle_request(entity, 201, vehicle_word(entity, configuration, 0x20), 1) == 0)
            FIELD(entity, 0x740, u32) = 300;
        break;
    case 202:
        separation_gate(entity, &second);
        interpolate_vehicle(entity, &first, &second, &first_angle, &second_angle, 0);
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float))
            FIELD(entity, 0x740, u32) = 300;
        break;
    case 300:
        if ((FIELD(configuration, 0x38, u32) != 0 || FIELD(configuration, 0x3C, u32) != 0) &&
            (FIELD(entity, 0x190, GeorgeActorBits64) & 8ULL) == 0 &&
            vehicle_command(entity, 0x110) != 0 && vehicle_command(entity, 0x118) != 0) {
            if (vehicle_request(entity, 301, vehicle_word(entity, configuration, 0x38), 1) == 0)
                FIELD(entity, 0x740, u32) = 400;
        } else FIELD(entity, 0x740, u32) = 400;
        break;
    case 302:
        FIELD(entity, 0x40, float) = second.x;
        FIELD(entity, 0x44, float) = second.y;
        FIELD(entity, 0x48, float) = second.z;
        {
            void *animation = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x74C, float) = second_angle;
            if (animation != 0 && (func_00272C10(animation) != 0 ||
                                  func_00272C30(FIELD(entity, 0x1B0, void *)) != 0))
                vehicle_command(entity, 0x108);
        }
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float))
            FIELD(entity, 0x740, u32) = 400;
        break;
    case 400:
        {
            s32 next = FIELD(entity, 0x738, s32);
            s32 current = FIELD(entity, 0x734, s32);
            if (next != current) {
                u32 first_word = FIELD(configuration, 0x48, u32);
                if (first_word != 0 || FIELD(configuration, 0x4C, u32) != 0) {
                    u32 word = next == 0 || next == 2 ? first_word : FIELD(configuration, 0x4C, u32);
                    if (vehicle_request(entity, 401, word, 1) != 0) break;
                } else {
                    s32 stopping = FIELD(entity, 0x754, s32);
                    FIELD(entity, 0x734, s32) = next;
                    FIELD(entity, 0x740, u32) = stopping != 0 ? 2000 : 1000;
                    break;
                }
            }
            FIELD(entity, 0x740, u32) = FIELD(entity, 0x754, s32) != 0 ? 2000 : 1000;
        }
        break;
    case 402:
        interpolate_vehicle(entity, &second, &third, &second_angle, &third_angle, 0);
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float)) {
            s32 next = FIELD(entity, 0x738, s32);
            s32 stopping = FIELD(entity, 0x754, s32);
            FIELD(entity, 0x734, s32) = next;
            FIELD(entity, 0x740, u32) = stopping != 0 ? 2000 : 1000;
        }
        break;
    case 1000:
        FIELD(entity, 0x40, float) = second.x;
        FIELD(entity, 0x44, float) = second.y;
        FIELD(entity, 0x48, float) = second.z;
        FIELD(entity, 0x750, u32) = 1;
        {
            /* Capture this object before storing the angle. */
            GeorgeGoalVirtualObject *object = VEHICLE(entity);
            const GeorgeGoalVirtualInt *pair;
            s32 result, command;
            u32 word;
            FIELD(entity, 0x74C, float) = second_angle;
            pair = VEHICLE_PAIR(object, 0xF8, GeorgeGoalVirtualInt);
            result = pair->invoke(ADJUST(object, pair->adjustment));
            command = FIELD(entity, 0x734, s32);
            if (command == result) {
                float blend;
                word = FIELD(configuration, vehicle_int(entity, 0x130) != 0 ? 0xC : 8, u32);
                {
                    void *animation = FIELD(entity, 0x1B0, void *);
                    FIELD(entity, 0x4E4, u32) = word;
                    if (animation != 0) play_pair(entity, word, 0.0f, 0.0f);
                }
                object = VEHICLE(entity);
                {
                    const GeorgeGoalVirtualFloatResult *float_pair = VEHICLE_PAIR(object, 0x128, GeorgeGoalVirtualFloatResult);
                    blend = float_pair->invoke(ADJUST(object, float_pair->adjustment)) * 0.5f + 0.5f;
                }
                {
                    void *animation = FIELD(entity, 0x1B0, void *);
                    if (animation != 0) {
                        if (!(0.0f <= blend)) FIELD(animation, 0x428, float) = 0.0f;
                        else FIELD(FIELD(entity, 0x1B0, void *), 0x428, float) =
                            george_ee_minimum(blend, 0.99900001287460327148f);
                    }
                }
            } else {
                void *animation;
                word = FIELD(configuration, command == 0 || command == 2 ? 0 : 4, u32);
                animation = FIELD(entity, 0x1B0, void *);
                FIELD(entity, 0x4E4, u32) = word;
                if (animation != 0) play_pair(entity, word, 0.0f, 0.0f);
            }
        }
        if (vehicle_command(entity, 0x118) != 0 && vehicle_command(entity, 0x110) != 0 &&
            (FIELD(entity, 0x190, GeorgeActorBits64) & 8ULL) == 0)
            FIELD(entity, 0x740, u32) = 300;
        if (FIELD(entity, 0x754, s32) != 0) {
            FIELD(entity, 0x750, u32) = 0;
            FIELD(entity, 0x740, u32) = 400;
        }
        break;
    case 2000:
        if ((FIELD(configuration, 0x18, u32) != 0 || FIELD(configuration, 0x1C, u32) != 0) &&
            vehicle_command(entity, 0x118) != 0) {
            if (vehicle_request(entity, 2001, vehicle_word(entity, configuration, 0x18), 1) == 0)
                FIELD(entity, 0x740, u32) = 2100;
        } else FIELD(entity, 0x740, u32) = 2100;
        break;
    case 2002:
        FIELD(entity, 0x40, float) = second.x;
        FIELD(entity, 0x44, float) = second.y;
        FIELD(entity, 0x48, float) = second.z;
        {
            void *animation = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x74C, float) = second_angle;
            if (animation != 0 && (func_00272C10(animation) != 0 ||
                                  func_00272C30(FIELD(entity, 0x1B0, void *)) != 0))
                vehicle_command(entity, 0x100);
        }
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float))
            FIELD(entity, 0x740, u32) = 2100;
        break;
    case 2100:
        {
            u32 word = vehicle_word(entity, configuration, 0x28);
            FIELD(entity, 0x740, u32) = 2101;
            FIELD(entity, 0x170, float) = first.x;
            FIELD(entity, 0x744, u32) = 0;
            FIELD(entity, 0x174, float) = first.y;
            FIELD(entity, 0x178, float) = first.z;
            if (vehicle_request(entity, 2101, word, 0) == 0) FIELD(entity, 0x740, u32) = 2200;
        }
        break;
    case 2102:
        interpolate_vehicle(entity, &second, VECTOR(entity, 0x170), &second_angle, &first_angle, 1);
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float)) {
            s32 result = 0;
            if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x80080ULL) == 0)
                result = control_predicate(entity);
            if (result != 0) FIELD(entity, 0x14, u32) = 3;
            else FIELD(entity, 0x740, u32) = 2200;
        }
        break;
    case 2200:
        if (FIELD(configuration, 0x40, u32) != 0 || FIELD(configuration, 0x44, u32) != 0) {
            const GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
            float squared = (motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z;
            if (squared < 0.04000000283122062683f && vehicle_command(entity, 0x118) != 0) {
                if (vehicle_request(entity, 2201, vehicle_word(entity, configuration, 0x40), 1) != 0) break;
            }
        }
        FIELD(entity, 0x740, u32) = 2300;
        break;
    case 2202:
        FIELD(entity, 0x40, float) = FIELD(entity, 0x170, float);
        FIELD(entity, 0x44, float) = FIELD(entity, 0x174, float);
        FIELD(entity, 0x48, float) = FIELD(entity, 0x178, float);
        {
            void *animation = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x74C, float) = first_angle;
            if (animation != 0 && (func_00272C10(animation) != 0 ||
                                  func_00272C30(FIELD(entity, 0x1B0, void *)) != 0))
                vehicle_command(entity, 0x108);
        }
        if (FIELD(entity, 0x748, float) <= FIELD(entity, 0x744, float))
            FIELD(entity, 0x740, u32) = 2300;
        break;
    case 2300:
        {
            s32 result = 0;
            if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x80080ULL) == 0)
                result = control_predicate(entity);
            FIELD(entity, 0x14, u32) = result != 0 ? 3 : 0;
        }
        break;
    default:
        FIELD(entity, 0x170, float) = first.x;
        FIELD(entity, 0x174, float) = first.y;
        FIELD(entity, 0x178, float) = first.z;
        if ((FIELD(configuration, 0x10, u32) != 0 || FIELD(configuration, 0x14, u32) != 0) &&
            vehicle_command(entity, 0x118) != 0 && vehicle_command(entity, 0x110) == 0) {
            if (vehicle_request(entity, 1, vehicle_word(entity, configuration, 0x10), 1) == 0)
                FIELD(entity, 0x740, u32) = 100;
        } else FIELD(entity, 0x740, u32) = 100;
        break;
    }
    if (vehicle_int(entity, 0x168) != 0) {
        FIELD(entity, 0x14, s32) = -1;
        FIELD(entity, 0x3BC, float) = 0.00526351295411586761f;
    }
    FIELD(entity, 0x78, u32) = 0;
}

#undef ACTOR_INLINE
