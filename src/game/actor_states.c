#include "george/actor_states.h"
#include "george/deimos_calls.h"

extern GeorgeDeimosValue *D_00474F48;
extern s32 func_0018FCF0(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void func_0018FD40(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern s32 func_001CE080(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second);
extern void func_001CDD90(void *object, u32 word);
extern void func_001C4158(void *object, u32 word);
extern void func_0015F280(void *object, float first, float second);
extern void func_002393F8(u32 word);
extern void func_002727D8(void *object);
extern void func_001F6148(void *object);
extern float func_002A6E60(void *source);
extern void func_00121A80(void *reference, GeorgeMathVec3 *position, GeorgeMathVec3 *direction);
extern void func_001219B8(void *reference);
extern void func_00121988(void *reference);
extern void func_001219E8(void *reference);
extern void func_00121A18(void *reference);
extern GeorgeActorBits64 func_00374848(float value);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern GeorgeActorBits64 func_00372C68(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern s32 func_00373250(GeorgeActorBits64 first, GeorgeActorBits64 second);
extern float func_003734F8(GeorgeActorBits64 value);
/* Both request APIs have nine integer/pointer arguments. Floating arguments
 * occupy the independent f12/f13 bank, not an invented stack float slot. */
extern s32 func_00270510(void *object, u32 word, u32 mode0, u32 mode1,
                       u32 owner_word, GeorgeActorRequestCallback callback,
                       GeorgeGoalEntity *context, u32 invoke_word,
                       u32 callback_word, float time);
extern void func_00272390(void *object, u32 word, u32 mode0, u32 mode1,
                        GeorgeActorRequestCallback callback, GeorgeGoalEntity *context,
                        u32 reuse_word, u32 invoke_word, u32 callback_word,
                        float time, float scale);
extern u32 func_00272C10(void *object);
extern void func_00270858(void *object);
extern void *func_00238BA0(void *object, u32 key);
extern void func_00232BA0(void *object);
extern void *func_00239E40(u32 word, void *output, u32 word0, u32 word1,
                         u32 word2, u32 word3, u32 word4);
extern void func_001787B0(GeorgeGoalEntity *entity);
extern void func_002A1C08(void *output, const void *input);
extern void func_002389E8(void *object, const void *matrix);
extern void func_00235CD8(void *object, u32 key, u32 word);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))
#define CONTROL(entity) FIELD(entity, 0x20, GeorgeActorControlObject *)
#define CONTROL_PAIR(object, offset, type) ((const type *)ADDRESS((object)->field00, offset))
#define VEHICLE(entity) FIELD(entity, 0x730, GeorgeGoalVirtualObject *)

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

static __inline__ void control_vector(GeorgeGoalEntity *entity, u32 offset,
                                      const GeorgeMathVec3 *vector)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualVectorInput *pair = CONTROL_PAIR(object, offset, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}

static __inline__ s32 control_predicate(GeorgeGoalEntity *entity)
{
    GeorgeActorControlObject *object = CONTROL(entity);
    const GeorgeActorVirtualPredicate *pair = CONTROL_PAIR(object, 0xD0, GeorgeActorVirtualPredicate);
    return pair->invoke(ADJUST(object, pair->adjustment), 0.20000000298023224f, 0.25f);
}

void func_0017D908(GeorgeGoalEntity *entity)
{
    float vertical = FIELD(entity, 0x50, float);
    if (vertical < 0.019999999552965164f && -0.019999999552965164f < vertical) {
        float timer = FIELD(entity, 0x560, float) + FIELD(entity, 0x35C, float);
        FIELD(entity, 0x560, float) = timer;
        if (0.029999999329447746f < timer)
            FIELD(entity, 0x50, float) = FIELD(entity, 0x50, float) + -5.0f;
        if (3.0f < FIELD(entity, 0x560, float)) {
            GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
            FIELD(entity, 0x14, u32) = 0;
            FIELD(entity, 0x380, u32) = 0;
            FIELD(entity, 0x190, GeorgeActorBits64) = flags | 0x80ULL;
        }
    }
    if (0.10000000149011612f < FIELD(entity, 0x44, float) - FIELD(entity, 0x80, float))
        FIELD(entity, 0x55C, float) = 0.0f;
    else
        FIELD(entity, 0x55C, float) = FIELD(entity, 0x55C, float) + FIELD(entity, 0x35C, float);
    if (1.0f < FIELD(entity, 0x55C, float)) FIELD(entity, 0x14, u32) = 5;
}

void func_0017EB20(GeorgeGoalEntity *entity)
{
    if (func_0018FCF0(entity, 0x10000000ULL) == 0) {
        void *object = FIELD(entity, 0x364, void *);
        if (object != 0) {
            s32 result = func_001CE080(VECTOR(entity, 0x40), VECTOR(object, 0x40));
            u32 word = FIELD(object, 0x40, u32);
            if (result == 0) {
                func_001CDD90(ADDRESS(object, 0xB4), word);
                word = FIELD(object, 0x40, u32);
            }
            func_001C4158(object, word);
        }
    }
    if (0.0f < FIELD(entity, 0x438, float)) {
        FIELD(entity, 0x438, float) = 0.0f;
        FIELD(entity, 0x190, GeorgeActorBits64) &= 0xFFFFFFFFFFFBFFFFULL;
    }
    FIELD(entity, 0x388, float) = 0.0f;
    if (FIELD(entity->field18, 0x1D8, u32) == 0x45A78000U)
        FIELD(CONTROL(entity), 0x34, u32) = 1;
}

void func_0017EBF0(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x378, void *);
    GeorgeActorBits64 flags;
    void *data;
    float adjustment, current, candidate, increment;
    u32 word;
    if (object != 0) func_0015F280(object, 0.20000000298023224f, 0.75f);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    data = entity->field18;
    adjustment = FIELD(data, 0xA0, float);
    if ((flags & 0x200ULL) != 0 || (flags & 0x100ULL) != 0) return;
    word = FIELD(entity, 0x28C, u32);
    if (word != 0) {
        func_002393F8(word);
        FIELD(entity, 0x28C, u32) = 0;
    }
    word = FIELD(entity, 0x290, u32);
    if (word != 0) {
        func_002393F8(word);
        FIELD(entity, 0x290, u32) = 0;
    }
    data = entity->field18;
    current = FIELD(entity, 0x36C, float);
    candidate = current - adjustment * FIELD(data, 0x198, float);
    increment = FIELD(data, 0x404, float);
    if (current < candidate) candidate = candidate + increment;
    else candidate = candidate - increment;
    FIELD(entity, 0x36C, float) = candidate;
    if (FIELD(entity, 0x38C, GeorgeDeimosPoolNode *) != 0) {
        float value = FIELD(entity, 0x36C, float);
        GeorgeDeimosValue *output = D_00474F48;
        output->payload.scalar = value;
        output->tag = 2;
        output->subtype = 0;
        func_002D02A8(FIELD(entity, 0x38C, GeorgeDeimosPoolNode *), 1, 0);
    }
    FIELD(entity, 0x3C0, u32) = 0;
}

void func_0017F588(GeorgeGoalEntity *entity)
{
    float timer = FIELD(entity, 0x568, float) - FIELD(entity, 0x35C, float);
    FIELD(entity, 0x568, float) = timer;
    if (0.0f < timer) {
        GeorgeMathVec3 zero;
        zero.x = 0.0f;
        zero.y = 0.0f;
        zero.z = 0.0f;
        control_vector(entity, 0xB0, &zero);
    }
    timer = FIELD(entity, 0x568, float);
    /* This write is the unconditional retail branch delay slot. */
    FIELD(entity, 0x74, float) = 0.0f;
    if (timer <= 0.0f) {
        const GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
        float length = (motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z;
        FIELD(entity, 0x14, u32) = length < 0.09000000357627869f ? 0 : 2;
    }
}

/* Four complete callback bodies differ only in their two observed offsets. */
#define DURATION_CALLBACK(name, state_offset, time_offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration = func_002A6E60(source); \
    u32 state = FIELD(entity, state_offset, u32); \
    (void)unused; \
    duration = duration * 0.00020833333837799728f; \
    FIELD(entity, state_offset, u32) = state + 1U; \
    FIELD(entity, time_offset, float) = duration; \
}

DURATION_CALLBACK(func_00194B70, 0x588, 0x584)
DURATION_CALLBACK(func_00194C98, 0x710, 0x714)
DURATION_CALLBACK(func_00194D60, 0x72C, 0x728)
DURATION_CALLBACK(func_00194DE0, 0x740, 0x748)

void func_00194BB8(GeorgeGoalEntity *entity)
{
    control_word(entity, 0x30, 1);
    FIELD(entity, 0x74, u32) = 0;
    FIELD(entity, 0x584, u32) = 0;
    FIELD(entity, 0x588, u32) = 0;
    FIELD(entity, 0x58C, u32) = 0;
}

#define STOP_REQUEST(name) \
void name(GeorgeGoalEntity *entity) \
{ \
    func_002727D8(FIELD(entity, 0x1B0, void *)); \
}
STOP_REQUEST(func_00194C08)
STOP_REQUEST(func_00194D38)

void func_00194CE0(GeorgeGoalEntity *entity)
{
    control_word(entity, 0x30, 1);
    FIELD(entity, 0x710, u32) = FIELD(entity, 0x704, u32) != 0 ? 0 : 100;
    FIELD(entity, 0x718, u32) = 0;
}

void func_00194DA8(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x724, void *);
    if (object != 0) {
        func_001F6148(object);
        FIELD(entity, 0x724, void *) = 0;
    }
}

void func_00194E28(GeorgeGoalEntity *entity)
{
    control_word(entity, 0x30, 0);
    FIELD(entity, 0x78, u32) = 0;
    FIELD(entity, 0x74C, u32) = 0;
    FIELD(entity, 0x750, u32) = 0;
    FIELD(entity, 0x754, u32) = 0;
    control_void(entity, 0x88);
    FIELD(entity, 0x740, u32) = FIELD(entity, 0x73C, u32) != 0 ? 1000 : 0;
}

void func_00194EA8(GeorgeGoalEntity *entity)
{
    void *request = FIELD(entity, 0x1B0, void *);
    GeorgeGoalVirtualObject *vehicle;
    const GeorgeActorVirtualEntity *pair;
    if (request != 0) func_002727D8(request);
    vehicle = VEHICLE(entity);
    FIELD(entity, 0x740, u32) = 0;
    func_001F6148(vehicle);
    vehicle = VEHICLE(entity);
    pair = (const GeorgeActorVirtualEntity *)ADDRESS(vehicle->field04, 0x1B0);
    pair->invoke(ADJUST(vehicle, pair->adjustment), entity);
    FIELD(entity, 0x730, GeorgeGoalVirtualObject *) = 0;
}

u32 func_00195D88(GeorgeGoalEntity *entity, u32 key)
{
    void *data = FIELD(entity, 0x1C, void *);
    s32 count = FIELD(data, 8, s32);
    u8 *entry = ADDRESS(data, 0x14);
    s32 index;
    for (index = 0; index < count; index++) {
        if (FIELD(entry, -8, u32) == key) return FIELD(entry, 0, u32);
        entry = ADDRESS(entry, 0xC);
    }
    return 0xFFFFFFFFU;
}

/* The main request receives captured object/key arguments. The companion
 * key and object are reloaded after the main callback and after the lookup. */
static __inline__ void state_request(GeorgeGoalEntity *entity, void *request,
                                     u32 word, u32 key_offset,
                                     GeorgeActorRequestCallback callback,
                                     float scale, s32 reload_scale)
{
    func_00272390(request, word, 0, 0, callback, entity, 0, 0, 0, 0.0f, scale);
    if (FIELD(entity, 0x1B4, void *) != 0) {
        u32 companion = func_00195D88(entity, FIELD(entity, key_offset, u32));
        if (reload_scale) scale = FIELD(entity, 0x720, float);
        func_00272390(FIELD(entity, 0x1B4, void *), companion, 0, 0, 0, 0,
                     0, 0, 0, 0.0f, scale);
    }
}

void func_00182B98(GeorgeGoalEntity *entity)
{
    GeorgeMathVec3 position, velocity;
    GeorgeActorBits64 step, current, delta, adjustment, result;
    float target_angle, current_angle, angle_result, timer;
    void *request, *reference;
    s32 state, comparison, negative;
    u32 word;
    func_00121A80(FIELD(entity, 0x580, void *), &position, &velocity);
    velocity.z = (position.z - FIELD(entity, 0x48, float)) * 5.0f;
    velocity.x = (position.x - FIELD(entity, 0x40, float)) * 5.0f;
    velocity.y = (position.y - FIELD(entity, 0x44, float)) * 5.0f;
    control_vector(entity, 0x78, &velocity);
    step = func_00374848(FIELD(entity, 0x35C, float));
    current = func_00374848(FIELD(entity, 0x58, float));
    delta = func_00374848(FIELD(entity, 0x57C, float) - FIELD(entity, 0x58, float));
    if (func_00373250(delta, 0ULL) < 0) delta = func_00372CC0(0ULL, delta);
    comparison = func_00373250(delta, 0x400921FB60000000ULL);
    target_angle = FIELD(entity, 0x57C, float);
    if (comparison > 0) {
        delta = func_00374848(target_angle - FIELD(entity, 0x58, float));
        negative = func_00373250(delta, 0ULL) < 0;
        /* These floats are captured before the optional software subtraction. */
        target_angle = FIELD(entity, 0x57C, float);
        current_angle = FIELD(entity, 0x58, float);
        if (negative) delta = func_00372CC0(0ULL, delta);
        adjustment = func_00372CC0(delta, 0x401921FB60000000ULL);
        if (target_angle - current_angle < 0.0f)
            step = func_00372D28(step, 0xBFF0000000000000ULL);
        adjustment = func_00372D28(adjustment, step);
        adjustment = func_00372D28(adjustment, 0x4014000000000000ULL);
        result = func_00372C68(current, adjustment);
    } else {
        delta = func_00374848(target_angle - FIELD(entity, 0x58, float));
        adjustment = func_00372D28(delta, step);
        adjustment = func_00372D28(adjustment, 0x4014000000000000ULL);
        result = func_00372C68(current, adjustment);
    }
    angle_result = func_003734F8(result);
    timer = FIELD(entity, 0x584, float) - FIELD(entity, 0x35C, float);
    state = FIELD(entity, 0x588, s32);
    FIELD(entity, 0x58, float) = angle_result;
    FIELD(entity, 0x584, float) = timer;
    switch (state) {
    case 1: case 101: case 201: break;
    case 2:
        if (timer <= 0.0f) {
            reference = FIELD(entity, 0x580, void *);
            if (reference != 0) func_001219B8(reference);
            FIELD(entity, 0x588, u32) = 100;
        }
        break;
    case 100:
        request = FIELD(entity, 0x1B0, void *);
        if (request == 0) FIELD(entity, 0x588, u32) = 200;
        else {
            FIELD(entity, 0x588, u32) = 101;
            state_request(entity, request, FIELD(entity, 0x570, u32), 0x570,
                          func_00194B70, 5.0f, 0);
        }
        break;
    case 102:
        request = FIELD(entity, 0x1B0, void *);
        if (request != 0 && (func_00272C10(request) & 0x0F000000U) != 0) {
            reference = FIELD(entity, 0x580, void *);
            if (reference != 0) func_00121988(reference);
        }
        if (FIELD(entity, 0x584, float) <= 0.0f) {
            u32 remaining = FIELD(entity, 0x578, u32) - 1U;
            FIELD(entity, 0x578, u32) = remaining;
            if (0 < (s32)remaining) FIELD(entity, 0x588, u32) = 100;
            else {
                reference = FIELD(entity, 0x580, void *);
                FIELD(entity, 0x588, u32) = 200;
                if (reference != 0) func_001219E8(reference);
            }
        }
        break;
    case 200:
        request = FIELD(entity, 0x1B0, void *);
        word = request != 0 ? FIELD(entity, 0x574, u32) : 0;
        if (word == 0) FIELD(entity, 0x14, u32) = 0;
        else {
            FIELD(entity, 0x588, u32) = 201;
            state_request(entity, request, word, 0x574, func_00194B70, 5.0f, 0);
        }
        break;
    case 202:
        if (timer <= 0.0f) {
            reference = FIELD(entity, 0x580, void *);
            if (reference != 0) func_00121A18(reference);
            FIELD(entity, 0x14, u32) = 0;
        }
        break;
    default:
        request = FIELD(entity, 0x1B0, void *);
        word = request != 0 ? FIELD(entity, 0x56C, u32) : 0;
        if (word == 0) FIELD(entity, 0x588, u32) = 100;
        else {
            FIELD(entity, 0x588, u32) = FIELD(entity, 0x588, u32) + 1U;
            state_request(entity, request, word, 0x56C, func_00194B70, 5.0f, 0);
        }
        break;
    }
}

void func_00183380(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags;
    s32 state, exit_request = 0;
    void *request;
    u32 word;
    float timer;
    control_void(entity, 0xA0);
    timer = FIELD(entity, 0x714, float) - FIELD(entity, 0x35C, float);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x714, float) = timer;
    if ((flags & 0x80080ULL) == 0 && control_predicate(entity) != 0)
        FIELD(entity, 0x14, u32) = 3;
    state = FIELD(entity, 0x710, s32);
    switch (state) {
    case 1: case 101: case 201: break;
    case 2:
        if (FIELD(entity, 0x714, float) <= 0.0f) FIELD(entity, 0x710, u32) = 100;
        break;
    case 100:
        request = FIELD(entity, 0x1B0, void *);
        if (request == 0) FIELD(entity, 0x710, u32) = 200;
        else {
            FIELD(entity, 0x710, u32) = 101;
            state_request(entity, request, FIELD(entity, 0x700, u32), 0x700,
                          func_00194C98, FIELD(entity, 0x720, float), 1);
        }
        break;
    case 102:
        if (FIELD(entity, 0x718, u32) != 0) exit_request = 1;
        else {
            const GeorgeMathVec3 *motion = VECTOR(entity, 0x68);
            float squared = (motion->x * motion->x + motion->y * motion->y) + motion->z * motion->z;
            if (0.25f < squared) exit_request = 1;
            else if (FIELD(entity, 0x714, float) <= 0.0f) {
                s32 remaining = FIELD(entity, 0x70C, s32);
                s32 repeat = 1;
                if (0 < remaining) {
                    u32 reduced = (u32)remaining - 1U;
                    FIELD(entity, 0x70C, u32) = reduced;
                    if (reduced == 0) repeat = 0;
                }
                if (repeat) FIELD(entity, 0x710, u32) = 100;
                else exit_request = 1;
            }
        }
        break;
    case 200:
        request = FIELD(entity, 0x1B0, void *);
        if (request == 0) FIELD(entity, 0x14, u32) = 0;
        else exit_request = 1;
        break;
    case 202:
        if (FIELD(entity, 0x714, float) <= 0.0f) FIELD(entity, 0x14, u32) = 0;
        break;
    default:
        request = FIELD(entity, 0x1B0, void *);
        if (request == 0 || FIELD(entity, 0x704, u32) == 0)
            FIELD(entity, 0x710, u32) = 100;
        else {
            word = FIELD(entity, 0x704, u32);
            FIELD(entity, 0x710, u32) = (u32)state + 1U;
            state_request(entity, request, word, 0x704, func_00194C98,
                          FIELD(entity, 0x720, float), 1);
        }
        break;
    }
    if (exit_request) {
        word = FIELD(entity, 0x708, u32);
        if (word == 0) FIELD(entity, 0x14, u32) = 0;
        else {
            request = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x710, u32) = 201;
            state_request(entity, request, word, 0x708, func_00194C98,
                          FIELD(entity, 0x720, float), 1);
        }
    }
    FIELD(entity, 0x74, u32) = 0;
}

void func_00183730(GeorgeGoalEntity *entity)
{
    void *object;
    u32 key, word = 0;
    control_void(entity, 0x88);
    object = FIELD(entity, 0x1C8, void *);
    FIELD(entity, 0x72C, u32) = 0;
    FIELD(entity, 0x728, u32) = 0;
    func_00232BA0(func_00238BA0(object, 0x1A6B0F5DU));
    key = FIELD(entity, 0x3BC, u32);
    if (key == 0x260105ECU) word = 0x4D;
    else if (key == 0xCB426EF9U || key == 0xCAD99652U || key == 0xEC789588U ||
             key == 0x1111150CU || key == 0x728F0147U || key == 0x75188880U || key == 0x794F22DAU)
        word = (FIELD(entity, 0x190, GeorgeActorBits64) & (1ULL << 45)) != 0 ? 0x4F : 0x4E;
    else FIELD(entity, 0x72C, u32) = 1;
    if (word != 0) {
        void *request = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = word;
        if (request != 0) {
            if (FIELD(entity, 0x1B4, void *) != 0)
                func_00270510(FIELD(entity, 0x1B4, void *), word, 0, 0,
                    FIELD(entity, 0x368, u32), 0, 0, 0, 0, 0.0f);
            func_00270510(FIELD(entity, 0x1B0, void *), word, 0, 0,
                    FIELD(entity, 0x368, u32), func_00194D60, entity, 0, 0, 0.0f);
        }
    }
    key = FIELD(entity->field18, 0x27C, u32);
    if (key != 0) func_00239E40(key, ADDRESS(entity, 0xB0), 0, 0, 0, 0, 0);
    if (FIELD(entity, 0x3BC, u32) != 0x3BAC798CU) func_001787B0(entity);
}

void func_001839D0(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags;
    void *request, *object;
    float timer;
    u32 state, word;
    s32 predicate = 0;
    control_void(entity, 0x88);
    func_0018FD40(entity, 0x02000000ULL);
    func_0018FD30(entity, 1ULL << 43);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((flags & 0x80080ULL) == 0) predicate = control_predicate(entity);
    if (predicate != 0) control_word(entity, 0xB8, 0);
    else control_void(entity, 0xA0);
    timer = FIELD(entity, 0x728, float) - FIELD(entity, 0x35C, float);
    state = FIELD(entity, 0x72C, u32);
    FIELD(entity, 0x728, float) = timer;
    if (state != 1) goto finish;
    request = FIELD(entity, 0x1B0, void *);
    word = request != 0 ? func_00272C10(request) : 0;
    if ((word & 0x2000U) != 0 || FIELD(entity, 0x3BC, u32) == 0x3BAC798CU) {
        object = FIELD(entity, 0x288, void *);
        if (object != 0) {
            func_002A1C08(ADDRESS(object, 0x10), ADDRESS(entity, 0xF0));
            FIELD(object, 0xA0, u32) |= 0x100U;
            func_002389E8(FIELD(entity, 0x288, void *), ADDRESS(entity, 0xF0));
            func_00235CD8(FIELD(entity, 0x288, void *), 0x9F79558FU, 1);
        }
    }
    request = FIELD(entity, 0x1B0, void *);
    if (request == 0) goto finish;
    if ((func_00272C10(request) & 0x8000U) != 0) {
        flags = FIELD(entity, 0x190, GeorgeActorBits64);
        object = FIELD(entity, 0x228, void *);
        FIELD(entity, 0x190, GeorgeActorBits64) = flags | (1ULL << 44);
        if (object != 0) func_00235CD8(object, 0x9F79558FU, 1);
    }
    request = FIELD(entity, 0x1B0, void *);
    if (request == 0) goto finish;
    if ((func_00272C10(request) & 0x4000U) != 0) control_word(entity, 0x30, 0);
    request = FIELD(entity, 0x1B0, void *);
    if (request != 0 && FIELD(entity, 0x728, float) <= 0.0f) {
        func_00270858(request);
        request = FIELD(entity, 0x1B4, void *);
        if (request != 0) func_00270858(request);
        if (FIELD(entity, 0x3A8, GeorgeDeimosPoolNode *) != 0)
            func_002D02A8(FIELD(entity, 0x3A8, GeorgeDeimosPoolNode *), 0, 0);
        FIELD(entity, 0x72C, u32) = FIELD(entity, 0x72C, u32) + 1U;
    }
finish:
    FIELD(entity, 0x74, float) = FIELD(entity, 0x78, float);
}
