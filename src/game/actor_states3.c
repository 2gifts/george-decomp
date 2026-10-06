#include "george/actor_states3.h"

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

extern GeorgeDeimosValue *D_00474F48;
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[], D_0042D678[], D_0042D690[], D_0042C3E8[];
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
extern void func_001F1758(void *object, void *reference, u32 key, float time);
extern void func_001F2020(void *object, u32 word, float time);
extern void func_001B67C0(void *context, GeorgeGoalEntity *entity, u32 word0,
                        u32 word1, u32 word2, u32 word3);
extern s32 func_00270510(void *object, u32 word, u32 mode0, u32 mode1,
                       u32 owner_word, GeorgeActorRequestCallback callback,
                       GeorgeGoalEntity *context, u32 invoke_word,
                       u32 callback_word, float time);
extern void func_002727D8(void *object);
extern void func_002393F8(u32 word);
extern void func_00235CD8(void *object, u32 key, u32 word);
extern void *func_00238BA0(void *object, u32 key);
extern u32 func_00272C10(void *object);
extern float func_0029B940(float first, float second);
extern float func_002A6E60(void *source);
extern void func_0018FD30(GeorgeGoalEntity *entity, GeorgeActorBits64 mask);
extern void *func_002A6468(const void *array, s32 index);
extern void func_002A2200(GeorgeRotationMatrix *output, const GeorgeRotationMatrix *first,
                        const GeorgeRotationMatrix *second);
extern void func_002A1C08(void *output, const void *input);
extern void *func_00239E40(u32 word, void *output, u32 mode,
                         GeorgeActorAttachmentCallback callback, GeorgeGoalEntity *context,
                         u32 word0, u32 word1);
extern void func_00237608(GeorgeGoalEntity *entity, GeorgeActorAttachmentCallback callback);
extern void func_0026F390(void *object, u32 mode);
extern void *func_0023C230(void *reference, u32 key);
extern void *func_0026EC98(void *storage, void *reference, u32 word, const u8 *text,
                         u32 word0, u32 word1, s32 word2);

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
static __inline__ void captured_control_vector(GeorgeActorControlObject *object,
                                             const GeorgeMathVec3 *vector)
{
    const GeorgeActorVirtualVectorInput *pair = PAIR(object, 0x78, GeorgeActorVirtualVectorInput);
    pair->invoke(ADJUST(object, pair->adjustment), vector);
}
/* The captured primary_gate object is a gate only. Both actual requests reload the
 * actor after stores and after the companion request's callback. */
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

/* Independently expressed registration block, also observed in actor_states2.
 * Keep callback-visible global and range reloads in the original order. */
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
/* GCC2.9 outlines these repeated call sequences despite __inline__. Reusable
 * source templates retain their original in-function placement on both tested
 * compilers, without introducing a fictitious out-of-line retail helper. */
#define FINISH_SCRIPT(entity) do { \
    if (FIELD(entity, 0x3B0, GeorgeDeimosPoolNode *) != 0) { \
        GeorgeDeimosPoolNode *value = func_002D0790((GeorgeScriptObject *)(entity)); \
        GeorgeDeimosValue *output = D_00474F48; \
        output->tag = 4; \
        output->payload.pointer = value; \
        output->subtype = 0; \
        func_002D02A8(FIELD(entity, 0x3B0, GeorgeDeimosPoolNode *), 1, 0); \
    } \
} while (0)
#define NOTIFY_FIELD(entity, offset, key) do { \
    void *object = FIELD(entity, offset, void *); \
    if (object != 0) func_00235CD8(object, key, 0); \
} while (0)

void func_00185178(GeorgeGoalEntity *entity)
{
    float x, negative_z, angle;
    FIELD(entity, 0x7AC, u32) = 0;
    FIELD(entity, 0x7B4, float) = 0.0f;
    FIELD(entity, 0x7A0, float) = FIELD(entity, 0x790, float);
    FIELD(entity, 0x7A4, float) = FIELD(entity, 0x794, float);
    FIELD(entity, 0x7A4, float) = FIELD(entity, 0x7A4, float) - 2.0f;
    FIELD(entity, 0x7A8, float) = FIELD(entity, 0x798, float);
    negative_z = -(FIELD(entity, 0x7A8, float) - FIELD(entity, 0x48, float));
    x = FIELD(entity, 0x7A0, float) - FIELD(entity, 0x40, float);
    angle = func_0029B940(x, negative_z) + 3.14159274101257324f;
    if (3.14159274101257324f < angle)
        angle = (func_0029B940(x, negative_z) + 3.14159274101257324f) - 6.28318548202514648f;
    else {
        angle = func_0029B940(x, negative_z) + 3.14159274101257324f;
        if (angle < -3.14159274101257324f)
            angle = (func_0029B940(x, negative_z) + 3.14159274101257324f) + 6.28318548202514648f;
        else angle = func_0029B940(x, negative_z) + 3.14159274101257324f;
    }
    FIELD(entity, 0x58, float) = angle;
    FIELD(entity, 0x7BC, void *) = 0;
}

void func_001852C0(GeorgeGoalEntity *entity)
{
    s32 phase = FIELD(entity, 0x7AC, s32);
    void *primary_gate;
    if (phase == 0) {
        GeorgeMathVec3 delta;
        void *object;
        float length;
        u32 key;
        primary_gate = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 1;
        request_pair(entity, primary_gate, 1, 0, 0.0f);
        delta.x = FIELD(entity, 0x7A0, float) - FIELD(entity, 0x40, float);
        delta.z = FIELD(entity, 0x7A8, float) - FIELD(entity, 0x48, float);
        delta.y = FIELD(entity, 0x7A4, float) - FIELD(entity, 0x44, float);
        length = func_002A3538(&delta);
        object = FIELD(entity, 0x2D4, void *);
        FIELD(entity, 0x7B0, float) = length * 0.100000001490116119f;
        if (object == 0) FIELD(entity, 0x7AC, u32) = 100;
        else {
            FIELD(entity, 0x794, float) += -0.259999990463256836f;
            func_002BEBA0(&key, D_0042D678);
            func_001F1758(FIELD(entity, 0x2D4, void *), ADDRESS(entity, 0x760), key, 30.0f);
            FIELD(entity, 0x7AC, u32) = FIELD(entity, 0x7AC, u32) + 1u;
        }
    } else if (phase == 1) {
        void *object;
        primary_gate = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 1;
        request_pair(entity, primary_gate, 1, 0, 0.0f);
        object = FIELD(entity, 0x2D4, void *);
        if (!(FIELD(object, 0x104, float) <= FIELD(object, 0x100, float))) return;
        if (FIELD(entity, 0x3AC, GeorgeDeimosPoolNode *) != 0) {
            GeorgeDeimosPoolNode *value;
            GeorgeDeimosValue *output;
            value = func_002D0790((GeorgeScriptObject *)entity);
            output = D_00474F48;
            output->payload.pointer = value;
            output->subtype = 0;
            output->tag = 4;
            value = func_002D0790(FIELD(entity, 0x7B8, GeorgeScriptObject *));
            output = &D_00474F48[1];
            output->tag = 4;
            output->payload.pointer = value;
            output->subtype = 0;
            func_002D02A8(FIELD(entity, 0x3AC, GeorgeDeimosPoolNode *), 2, 0);
        }
        object = func_00238BA0(FIELD(entity, 0x7B8, void *), 0x8A52D2B3u);
        if (object != 0 && FIELD(object, 0x40, u32) != 0) FIELD(entity, 0x7AC, u32) = 200;
        else {
            void *effect, *handle;
            u32 key, word;
            GeorgeMathVec3 position;
            FIELD(entity, 0x7AC, u32) = 100;
            ensure_effect();
            effect = D_003F2D40;
            func_002BEBA0(&key, D_0042D690);
            position.x = FIELD(entity, 0x40, float);
            position.y = FIELD(entity, 0x44, float);
            position.z = FIELD(entity, 0x48, float);
            handle = func_00251B58(effect, key, &position);
            FIELD(entity, 0x7BC, void *) = handle;
            func_002455C0(handle);
            word = FIELD(entity->field18, 0xD0, u32);
            if (word != 0 && (FIELD(entity, 0x190, GeorgeActorBits64) & 0x100000u) == 0)
                func_001B67C0(ADDRESS(entity, 0x40), FIELD(entity, 0x3B8, GeorgeGoalEntity *),
                             FIELD(entity, 0x3B4, u32), word, 0, 0);
        }
    } else if (phase == 100) {
        GeorgeMathVec3 delta;
        union { GeorgeMathVec3 vector; struct { GeorgeActorBits64 xy; u32 z; } bits; } snapshot;
        GeorgeActorUnalignedPosition *output;
        GeorgeActorControlObject *control;
        float timer = FIELD(entity, 0x7B4, float) + FIELD(entity, 0x35C, float);
        float length;
        primary_gate = FIELD(entity, 0x1B0, void *);
        FIELD(entity, 0x4E4, u32) = 0x49;
        FIELD(entity, 0x7B4, float) = timer;
        request_pair(entity, primary_gate, 0x49, 0, 0.0f);
        snapshot.vector.x = FIELD(entity, 0x40, float);
        output = (GeorgeActorUnalignedPosition *)ADDRESS(FIELD(entity, 0x7BC, void *), 0x14);
        snapshot.vector.y = FIELD(entity, 0x44, float);
        snapshot.vector.z = FIELD(entity, 0x48, float);
        output->field00 = snapshot.bits.xy;
        output->field08 = snapshot.bits.z;
        delta.x = FIELD(entity, 0x7A0, float) - FIELD(entity, 0x40, float);
        delta.z = FIELD(entity, 0x7A8, float) - FIELD(entity, 0x48, float);
        delta.y = FIELD(entity, 0x7A4, float) - FIELD(entity, 0x44, float);
        length = func_002A3538(&delta);
        delta.x *= 10.0f;
        control = CONTROL(entity);
        delta.y *= 10.0f;
        delta.z *= 10.0f;
        captured_control_vector(control, &delta);
        if (length < 0.5f || FIELD(entity, 0x7B0, float) + 0.5f < FIELD(entity, 0x7B4, float)) {
            FINISH_SCRIPT(entity);
            if (FIELD(entity, 0x2D4, void *) != 0) func_001F2020(FIELD(entity, 0x2D4, void *), 0, 30.0f);
            func_002457D8(FIELD(entity, 0x7BC, void *));
            func_00245B08(FIELD(entity, 0x7BC, void *));
            FIELD(entity, 0x7BC, void *) = 0;
            control_void(entity, 0x88);
            FIELD(entity, 0x14, u32) = 3;
        }
    } else if (phase == 200) {
        void *reference, *object;
        float timer, threshold;
        control_void(entity, 0xA0);
        timer = FIELD(entity, 0x7B4, float) + FIELD(entity, 0x35C, float);
        reference = FIELD(entity, 0x7B8, void *);
        FIELD(entity, 0x7B4, float) = timer;
        object = func_00238BA0(reference, 0x8A52D2B3u);
        threshold = object != 0 ? FIELD(object, 0x44, float) : 0.0f;
        if (threshold < FIELD(entity, 0x7B4, float)) {
            primary_gate = FIELD(entity, 0x1B0, void *);
            FIELD(entity, 0x7AC, u32) = FIELD(entity, 0x7AC, u32) + 1u;
            FIELD(entity, 0x4E4, u32) = 0x4A;
            request_pair(entity, primary_gate, 0x4A, func_00194F08, 0.0f);
        }
    } else if (phase == 201) control_void(entity, 0xA0);
    else if (phase == 202) {
        float timer;
        control_void(entity, 0xA0);
        timer = FIELD(entity, 0x7B4, float) - FIELD(entity, 0x35C, float);
        FIELD(entity, 0x7B4, float) = timer;
        if (timer <= 0.0f || (func_00272C10(FIELD(entity, 0x1B0, void *)) & 0x400000u) != 0) {
            func_001F2020(FIELD(entity, 0x2D4, void *), 0, 30.0f);
            FIELD(entity, 0x7AC, u32) = FIELD(entity, 0x7AC, u32) + 1u;
        }
    } else if (phase == 203) {
        control_void(entity, 0xA0);
        if (0.0f < FIELD(FIELD(entity, 0x2D4, void *), 0x104, float)) return;
        FINISH_SCRIPT(entity);
        FIELD(entity, 0x14, u32) = 0;
    }
}

void func_00186328(GeorgeGoalEntity *entity)
{
    float timer;
    s32 phase;
    u32 word, next;
    void *primary_gate;
    control_void(entity, 0xA0);
    timer = FIELD(entity, 0x7C8, float) - FIELD(entity, 0x35C, float);
    phase = FIELD(entity, 0x7C0, s32);
    FIELD(entity, 0x7C8, float) = timer;
    if (phase == 1 || phase == 101 || phase == 201) return;
    if (phase == 2) {
        NOTIFY_FIELD(entity, 0x214, 0xB95616B6u);
        NOTIFY_FIELD(entity, 0x218, 0xB95616B6u);
        if (FIELD(entity, 0x7C8, float) <= 0.0f) {
            NOTIFY_FIELD(entity, 0x21C, 0xB95616B6u);
            NOTIFY_FIELD(entity, 0x220, 0xB95616B6u);
            FIELD(entity, 0x7C0, u32) = 100;
        }
        return;
    }
    if (phase == 102) {
        /* BNEZL annuls its phase200 store when field7C4 is zero. */
        if (FIELD(entity, 0x7C4, u32) != 0) FIELD(entity, 0x7C0, u32) = 200;
        return;
    }
    if (phase == 202) {
        NOTIFY_FIELD(entity, 0x214, 0x9F79558Fu);
        NOTIFY_FIELD(entity, 0x21C, 0x9F79558Fu);
        NOTIFY_FIELD(entity, 0x220, 0x9F79558Fu);
        if (FIELD(entity, 0x7C8, float) <= 0.0f) {
            NOTIFY_FIELD(entity, 0x218, 0x9F79558Fu);
            FIELD(entity, 0x14, u32) = 0;
        }
        return;
    }
    if (phase == 100) { next = 101; word = 0x37; }
    else if (phase == 200) { next = 201; word = 0x38; }
    else { next = 1; word = 0x36; }
    primary_gate = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x7C0, u32) = next;
    FIELD(entity, 0x4E4, u32) = word;
    if (request_pair(entity, primary_gate, word, func_00195068, 0.0f) == 0)
        FIELD(entity, 0x7C0, u32) = next + 1u;
}

/* Complete state17/state19 bodies differ only in these observed constants. */
#define TIMED_REQUEST(name, phase_offset, timer_offset, word, callback) \
void name(GeorgeGoalEntity *entity) \
{ \
    float timer; s32 phase; void *primary_gate; \
    control_void(entity, 0xA0); \
    timer = FIELD(entity, timer_offset, float) - FIELD(entity, 0x35C, float); \
    phase = FIELD(entity, phase_offset, s32); \
    FIELD(entity, timer_offset, float) = timer; \
    if (phase == 1) return; \
    if (phase == 2) { if (timer <= 0.0f) FIELD(entity, 0x14, u32) = 0; return; } \
    primary_gate = FIELD(entity, 0x1B0, void *); \
    FIELD(entity, phase_offset, u32) = 1; \
    FIELD(entity, 0x4E4, u32) = word; \
    if (request_pair(entity, primary_gate, word, callback, 0.0f) == 0) FIELD(entity, phase_offset, u32) = 2; \
}
TIMED_REQUEST(func_001867D8, 0x7D8, 0x7DC, 0x39, func_001950E8)
TIMED_REQUEST(func_00186AC0, 0x7E8, 0x7EC, 0x3A, func_001952D8)

void func_00186DF8(GeorgeGoalEntity *entity)
{
    float timer;
    s32 phase;
    void *primary_gate;
    control_void(entity, 0xA0);
    timer = FIELD(entity, 0x7E4, float) - FIELD(entity, 0x35C, float);
    phase = FIELD(entity, 0x7E0, s32);
    FIELD(entity, 0x3C4, u32) = 0;
    FIELD(entity, 0x7E4, float) = timer;
    if (phase == 101) return;
    if (phase == 103) { FIELD(entity, 0x14, u32) = 0; return; }
    if (phase == 102) {
        primary_gate = FIELD(entity, 0x1B0, void *);
        if (primary_gate != 0 && (func_00272C10(primary_gate) & 0x2000u) != 0 && FIELD(entity, 0x298, void *) != 0) {
            const GeorgeRotationMatrix *matrix = 0;
            const GeorgeRotationMatrix *source = (const GeorgeRotationMatrix *)ADDRESS(entity, 0xB0);
            void *primary = FIELD(entity, 0x1B0, void *);
            if (primary != 0) {
                s32 command = FIELD(primary, 0x0C, s32);
                u32 offset;
                s32 count, index;
                if (command == 3) command = 2;
                offset = (u32)command << 2;
                count = (s32)func_002A6460(FIELD(primary, 0x378u + offset, void *));
                if (count > 0) {
                    const u8 *records = (const u8 *)func_002A6468(FIELD(FIELD(entity, 0x1B0, void *), 0x378u + offset, void *), 0);
                    void *matrices = FIELD(FIELD(entity, 0x1B0, void *), 0x3E8u + offset, void *);
                    for (index = 0; index < count; ++index) {
                        if (records[(u32)index * 0x20u + 0x1Du] == 0x29) {
                            matrix = (const GeorgeRotationMatrix *)ADDRESS(matrices, (u32)index << 6);
                            break;
                        }
                    }
                }
            }
            if (matrix != 0) {
                GeorgeRotationMatrix output __attribute__((aligned(16)));
                void *object;
                float z;
                func_002A2200(&output, matrix, source);
                object = FIELD(entity, 0x298, void *);
                FIELD(object, 0x40, float) = output.element[12];
                FIELD(object, 0x44, float) = output.element[13];
                z = output.element[14];
                FIELD(object, 0x4C, float) = 1.0f;
                FIELD(object, 0x48, float) = z;
                FIELD(object, 0xA0, u32) |= 0x100u;
            } else {
                void *object = FIELD(entity, 0x298, void *);
                func_002A1C08(ADDRESS(object, 0x10), source);
                FIELD(object, 0xA0, u32) |= 0x100u;
            }
            func_00235CD8(FIELD(entity, 0x298, void *), 0x9F79558Fu, 1);
        }
        if (FIELD(entity, 0x7E4, float) <= 0.0f)
            FIELD(entity, 0x7E0, u32) = FIELD(entity, 0x7E0, u32) + 1u;
        return;
    }
    primary_gate = FIELD(entity, 0x1B0, void *);
    FIELD(entity, 0x4E4, u32) = 0x3A;
    if (request_pair(entity, primary_gate, 0x3A, func_001957D0, FIELD(entity, 0x3C4, float)) != 0)
        FIELD(entity, 0x7E0, u32) = FIELD(entity, 0x7E0, u32) + 1u;
    else FIELD(entity, 0x7E0, u32) = 102;
}

void func_00194F08(u32 unused, GeorgeGoalEntity *entity, void *source)
{
    float duration;
    (void)unused;
    duration = func_002A6E60(source);
    FIELD(entity, 0x7AC, u32) = FIELD(entity, 0x7AC, u32) + 1u;
    FIELD(entity, 0x7B4, float) = duration * 0.000208333338377997279f;
}
#define DURATION_CALLBACK(name, phase_offset, timer_offset) \
void name(u32 unused, GeorgeGoalEntity *entity, void *source) \
{ \
    float duration; (void)unused; \
    FIELD(entity, phase_offset, u32) = FIELD(entity, phase_offset, u32) + 1u; \
    duration = func_002A6E60(source); \
    FIELD(entity, timer_offset, float) = duration * 0.000208333338377997279f; \
}
DURATION_CALLBACK(func_00195068, 0x7C0, 0x7C8)
DURATION_CALLBACK(func_001950E8, 0x7D8, 0x7DC)
DURATION_CALLBACK(func_001957D0, 0x7E0, 0x7E4)
DURATION_CALLBACK(func_001952D8, 0x7E8, 0x7EC)

#define STOP_REQUEST(name) \
void name(GeorgeGoalEntity *entity) \
{ void *object = FIELD(entity, 0x1B0, void *); if (object != 0) func_002727D8(object); }
STOP_REQUEST(func_001950C0)
STOP_REQUEST(func_00195140)
STOP_REQUEST(func_00195330)
STOP_REQUEST(func_00195828)

void func_00194F50(GeorgeGoalEntity *entity)
{
    void *object;
    if (FIELD(entity, 0x7BC, void *) != 0) {
        func_002457D8(FIELD(entity, 0x7BC, void *));
        func_00245B08(FIELD(entity, 0x7BC, void *));
        FIELD(entity, 0x7BC, void *) = 0;
    }
    object = FIELD(entity, 0x2D4, void *);
    if (object != 0 && 0.0f < FIELD(object, 0x104, float)) func_001F2020(object, 0, 30.0f);
}
void func_00194FD8(GeorgeGoalEntity *entity)
{
    GeorgeActorBits64 flags;
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~((GeorgeActorBits64)1 << 19);
    func_0018FD30(entity, 0x4000);
    control_void(entity, 0x38);
    flags = FIELD(entity, 0x190, GeorgeActorBits64);
    FIELD(entity, 0x60, u32) = 0;
    FIELD(entity, 0x484, u32) = 0;
    FIELD(entity, 0x190, GeorgeActorBits64) = flags & ~((GeorgeActorBits64)1 << 41) & ~((GeorgeActorBits64)1 << 40);
}
void func_00195168(GeorgeGoalEntity *entity, void *object)
{
    FIELD(entity, 0x4E8, void *) = object;
    func_00235CD8(object, 0x9F79558Fu, 1);
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~((GeorgeActorBits64)1 << 39);
}
void func_001951C0(GeorgeGoalEntity *entity, u32 word, float height)
{
    GeorgeRotationMatrix matrix __attribute__((aligned(16)));
    float x, y, z;
    func_002A1C08(&matrix, ADDRESS(entity, 0xB0));
    y = matrix.element[13] + height;
    x = matrix.element[12];
    z = matrix.element[14];
    FIELD(entity, 0x4EC, float) = height;
    matrix.element[12] = x;
    matrix.element[13] = y;
    matrix.element[14] = z;
    func_00239E40(word, &matrix, 0, func_00195168, entity, 0, 0);
    FIELD(entity, 0x190, GeorgeActorBits64) |= (GeorgeActorBits64)1 << 39;
}
void func_00195260(GeorgeGoalEntity *entity)
{
    void *object = FIELD(entity, 0x4E8, void *);
    if (object != 0) {
        func_00235CD8(object, 0xB95616B6u, 1);
        func_002393F8(FIELD(entity, 0x4E8, u32));
        FIELD(entity, 0x4E8, void *) = 0;
    }
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & ((GeorgeActorBits64)1 << 39)) != 0)
        func_00237608(entity, func_00195168);
}
void func_00195358(GeorgeGoalEntity *entity, void *reference)
{
    void *object = FIELD(entity, 0x1B4, void *);
    void *map, *secondary, *storage;
    if (object != 0) {
        func_0026F390(object, 3);
        FIELD(entity, 0x1B4, void *) = 0;
    }
    FIELD(entity, 0x1C, void *) = 0;
    map = func_0023C230(reference, 0xFFFFFFFFu);
    FIELD(entity, 0x1C, void *) = map;
    secondary = func_0023C230(FIELD(map, 0, void *), 0x937AB034u);
    storage = func_002AEE60(0x45C);
    FIELD(entity, 0x1B4, void *) = func_0026EC98(storage, secondary,
        FIELD(FIELD(entity, 0x1C, void *), 4, u32), D_0042C3E8, 0, 0, -1);
}
