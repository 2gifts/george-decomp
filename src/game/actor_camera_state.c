#include "george/actor_camera_state.h"
#include "george/actor_movement.h"
#include "george/ee_math.h"

extern GeorgeDeimosPoolNode *D_003F83D4;
extern GeorgeDeimosValue *D_00474F48;
extern GeorgeDeimosValue D_00474748[];
extern float D_FLT_003F8A70, D_FLT_003F8A74;
extern const u8 D_0042B680[];
extern const char D_0042B0E0[];
extern const char D_0042B0F0[], D_0042B100[], D_0042B120[], D_0042B130[];
extern const char D_0042B148[], D_0042B160[], D_0042B180[], D_0042B190[];
extern const char D_0042B1A8[], D_0042B1B8[], D_0042B1D0[], D_0042B1E8[];
extern const char D_0042B208[], D_0042B220[], D_0042B240[], D_0042B258[];
extern const char D_0042B278[], D_0042B290[], D_0042B2B0[], D_0042B2C0[];
extern const char D_0042B2D8[], D_0042B2E8[], D_0042B300[], D_0042B310[];
extern const char D_0042B328[], D_0042B330[], D_0042B340[], D_0042B358[];
extern const char D_0042B378[], D_0042B388[], D_0042B3A0[], D_0042B3E0[];
extern const char D_0042B428[], D_0042B488[], D_0042B4F8[], D_0042B548[];
extern const char D_0042B590[], D_0042B5D0[];
extern void *func_0023C298(const char *, s32);
extern void *func_002D0B48(u32);
extern void func_002CC938(const char *, ...);
extern void func_002CE6B8(GeorgeDeimosPoolNode *, const char *, const char *,
                        u32, void (*)(s32, s32), u32);
extern void *func_00239FD8(u32);
extern void *func_00238BA0(void *, u32);
extern GeorgeMathVec3 *func_00161C30(s32);
extern void func_00135D10(void *, GeorgeMathVec3 *, const GeorgeMathVec3 *);
extern float func_00135D88(void *, const GeorgeMathVec3 *);
extern void func_00135E88(void *, GeorgeMathVec3 *, float);
extern s32 func_00397178(void);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))
#define TRANSFORM(object) FIELD(object, 8, GeorgeCameraTransform *)

/* These helpers retain the original three-load copy order or all-six-load
 * interpolation capture. They do not replace a general engine vector API. */
static __inline__ void camera_copy(GeorgeMathVec3 *output, const GeorgeMathVec3 *input)
{
    output->x = input->x;
    output->y = input->y;
    output->z = input->z;
}

static __inline__ void camera_blend(GeorgeMathVec3 *output,
                                  const GeorgeMathVec3 *target, float factor)
{
    float x = output->x, y = output->y, z = output->z;
    float tx = target->x, ty = target->y, tz = target->z;
    x += factor * (tx - x);
    y += factor * (ty - y);
    z += factor * (tz - z);
    output->x = x;
    output->y = y;
    output->z = z;
}

void *func_001670A8(void *object, const GeorgeRotationMatrix *input)
{
    GeorgeCameraTransform *transform;
    float x, y, z;
    func_00166CA8(object, input);
    FIELD(object, 0x148, u32) = 0;
    FIELD(object, 0x14C, u32) = 0;
    FIELD(object, 0x150, u32) = 0;
    FIELD(object, 0x15C, u32) = 0;
    FIELD(object, 0x160, void *) = 0;
    FIELD(object, 0x154, float) = 10.0f;
    FIELD(object, 4, const u8 *) = D_0042B680;
    FIELD(object, 0x160, void *) = func_0023C298(D_0042B0E0, -1);
    FIELD(object, 0x148, float) = FIELD(FIELD(object, 0x160, void *), 0, float);
    transform = TRANSFORM(object);
    transform->pose.fields.field00 = 2;
    y = FIELD(object, 0x44, float) + 2.7999999523162841796875f;
    x = FIELD(object, 0x40, float);
    z = FIELD(object, 0x48, float);
    FIELD(object, 0xF0, float) = x;
    FIELD(object, 0xF8, float) = z;
    FIELD(object, 0xF4, float) = y;
    camera_copy(VECTOR(object, 0xFC), VECTOR(object, 0x40));
    camera_copy(VECTOR(object, 0x108), VECTOR(object, 0xF0));
    camera_copy(VECTOR(object, 0x114), VECTOR(object, 0xFC));
    FIELD(object, 0x124, u32) = 0;
    FIELD(object, 0x128, u32) = 0;
    FIELD(object, 0x12C, u32) = 0;
    FIELD(object, 0x130, u32) = 0;
    FIELD(object, 0x134, u32) = 0;
    FIELD(object, 0x138, u32) = 0;
    FIELD(object, 0x13C, u32) = 0;
    FIELD(object, 0x140, u32) = 0;
    FIELD(object, 0x144, u32) = 0;
    FIELD(object, 0x158, u32) = 0;
    FIELD(object, 0x120, u32) = 0;
    return object;
}

void func_001671D8(void *object, float elapsed)
{
    GeorgeRotationMatrix *inverse = FRAME(object, 0x50);
    GeorgeRotationMatrix *forward = FRAME(object, 0x90);
    GeorgeMathVec3 point, direction;
    /* Retail reads sp+10/14/18 without a preceding write on the jitter path.
     * This is intentionally indeterminate, not the normalized sp+20 vector.
     * Volatile preserves reads, but portable C cannot promise identical stack
     * placement or defined values. Native tests do not execute that path. */
    volatile GeorgeMathVec3 unwritten_stack_10;
    GeorgeCameraTransform *transform;
    void *configuration;
    float factor, remaining, target, difference, x, y, z, a, b, c;
    GeorgeActorBits64 absolute;
    if (FIELD(object, 0xD8, u32) != 0) return;
    if (FIELD(object, 0xC, void *) == 0) {
        if (D_FLT_003F8A74 == 0.0f) elapsed = D_FLT_003F8A70;
        remaining = FIELD(object, 0x124, float);
        if (0.0f < remaining && FIELD(object, 0x15C, u32) == 0) {
            remaining -= elapsed;
            factor = elapsed / remaining;
            FIELD(object, 0x124, float) = remaining;
            if (0.0f <= factor) factor = george_ee_minimum(factor, 1.0f);
            else factor = 0.0f;
            camera_blend(VECTOR(object, 0xF0), VECTOR(object, 0x108), factor);
            camera_blend(VECTOR(object, 0xFC), VECTOR(object, 0x114), factor);
        } else {
            configuration = FIELD(object, 0x160, void *);
            factor = FIELD(configuration, 4, float);
            if (0.0f < factor && FIELD(object, 0x15C, u32) == 0) {
                camera_blend(VECTOR(object, 0xF0), VECTOR(object, 0x108), factor * elapsed);
                configuration = FIELD(object, 0x160, void *);
                factor = FIELD(configuration, 4, float) * elapsed;
                camera_blend(VECTOR(object, 0xFC), VECTOR(object, 0x114), factor);
            } else {
                camera_copy(VECTOR(object, 0xF0), VECTOR(object, 0x108));
                camera_copy(VECTOR(object, 0xFC), VECTOR(object, 0x114));
                if (0 < FIELD(object, 0x15C, s32))
                    FIELD(object, 0x15C, u32) = FIELD(object, 0x15C, u32) - 1U;
            }
        }
        if (FIELD(object, 0x124, float) < 0.0f) FIELD(object, 0x124, float) = 0.0f;
        if (FIELD(object, 0x120, u32) == 0) {
            FIELD(object, 0x114, float) = FIELD(object, 0x40, float);
            y = FIELD(object, 0x44, float);
            FIELD(object, 0x118, float) = y;
            FIELD(object, 0x11C, float) = FIELD(object, 0x48, float);
            FIELD(object, 0x118, float) = y + FIELD(object, 0x128, float);
        }
        if (0.0f < FIELD(object, 0x148, float)) {
            target = 0.0f;
            if (0.100000001490116119384765625f < FIELD(object, 0x150, float))
                target = FIELD(object, 0x154, float);
            difference = target - FIELD(object, 0x14C, float);
            absolute = func_00374848(difference);
            if (func_00373250(absolute, 0) < 0) absolute = func_00372CC0(0, absolute);
            if (func_00373250(absolute, 0x3FB99999A0000000ULL) <= 0)
                FIELD(object, 0x14C, float) = target;
            else {
                configuration = FIELD(object, 0x160, void *);
                if (0.0f < difference)
                    FIELD(object, 0x14C, float) = FIELD(object, 0x14C, float) +
                        (elapsed * FIELD(configuration, 8, float)) * FIELD(object, 0x154, float);
                else
                    FIELD(object, 0x14C, float) = FIELD(object, 0x14C, float) -
                        (elapsed * FIELD(configuration, 0xC, float)) * FIELD(object, 0x154, float);
            }
        }
        factor = FIELD(object, 0x148, float) *
            (FIELD(object, 0x14C, float) / FIELD(object, 0x154, float));
        x = FIELD(object, 0x114, float) + FIELD(object, 0x30, float) * factor;
        z = FIELD(object, 0x11C, float) + FIELD(object, 0x38, float) * factor;
        y = FIELD(object, 0x118, float) + FIELD(object, 0x34, float) * factor;
        FIELD(object, 0x114, float) = x;
        FIELD(object, 0x11C, float) = z;
        FIELD(object, 0x118, float) = y;
        if (0.0f < FIELD(object, 0xD4, float)) {
            a = (float)(func_00397178() % 101 - 50) * 0.0199999995529651641845703125f;
            b = (float)(func_00397178() % 101 - 50) * 0.0199999995529651641845703125f;
            c = (float)(func_00397178() % 101 - 50) * 0.0199999995529651641845703125f;
            factor = FIELD(object, 0xD0, float);
            point.x = a * factor + FIELD(object, 0xF0, float);
            point.z = c * factor + FIELD(object, 0xF8, float);
            point.y = b * factor + FIELD(object, 0xF4, float);
            direction.x = FIELD(object, 0xFC, float) - point.x;
            direction.z = FIELD(object, 0x104, float) - point.z;
            direction.y = FIELD(object, 0x100, float) - point.y;
            func_002A3538(&direction);
            transform = TRANSFORM(object);
            transform->pose.fields.field04.x = unwritten_stack_10.x;
            transform->pose.fields.field04.y = unwritten_stack_10.y;
            transform->pose.fields.field04.z = unwritten_stack_10.z;
            transform = TRANSFORM(object);
            camera_copy(&transform->pose.fields.field10, &point);
            FIELD(object, 0xD4, float) = FIELD(object, 0xD4, float) - elapsed;
        } else {
            transform = TRANSFORM(object);
            camera_copy(&transform->pose.fields.field04, VECTOR(object, 0xFC));
            transform = TRANSFORM(object);
            camera_copy(&transform->pose.fields.field10, VECTOR(object, 0xF0));
        }
        func_0029A308(&TRANSFORM(object)->pose.prefix, inverse, forward);
        a = FIELD(object, 0x138, float);
        if (0.0f <= a) {
            b = FIELD(object, 0x13C, float);
            c = FIELD(object, 0x140, float) + elapsed;
            transform = TRANSFORM(object);
            a += b * elapsed;
            b -= elapsed * 19.6000003814697265625f;
            FIELD(object, 0x140, float) = c;
            FIELD(object, 0x138, float) = a;
            FIELD(object, 0x13C, float) = b;
            transform->pose.fields.field10.y = transform->pose.fields.field10.y + a;
        }
    }
    factor = FIELD(object, 0x148, float);
    z = FIELD(object, 0x88, float) + FIELD(object, 0x78, float) * factor;
    y = FIELD(object, 0x84, float) + FIELD(object, 0x74, float) * factor;
    x = FIELD(object, 0x80, float) + FIELD(object, 0x70, float) * factor;
    inverse->element[12] = x;
    inverse->element[13] = y;
    inverse->element[15] = 1.0f;
    inverse->element[14] = z;
    func_0029A308(&TRANSFORM(object)->pose.prefix, FRAME(object, 0x50), forward);
}

GeorgeDeimosPoolNode *func_001678E0(void)
{
    if (D_003F83D4 == 0) {
        D_003F83D4 = func_002CD348(0);
        func_002CD0B8(D_003F83D4);
#define REGISTER(label, types, count, callback, flags) \
        func_002CE6B8(D_003F83D4, label, types, (u32)(count), callback, flags)
        REGISTER(D_0042B0F0, D_0042B100, 2, func_00168398, 0);
        REGISTER(D_0042B120, D_0042B130, 2, func_001683F0, 0);
        REGISTER(D_0042B148, D_0042B160, 3, func_00167B98, 0);
        REGISTER(D_0042B180, D_0042B190, -1, func_00167CC0, 0);
        REGISTER(D_0042B1A8, D_0042B1B8, 3, func_00167F98, 0);
        REGISTER(D_0042B1D0, D_0042B1E8, 2, func_001683F0, 1);
        REGISTER(D_0042B208, D_0042B220, 3, func_00167B98, 1);
        REGISTER(D_0042B240, D_0042B258, -1, func_00167CC0, 1);
        REGISTER(D_0042B278, D_0042B290, 3, func_00167F98, 1);
        REGISTER(D_0042B2B0, D_0042B2C0, 1, func_001684D0, 0);
        REGISTER(D_0042B2D8, D_0042B2E8, 2, func_001684F8, 0);
        REGISTER(D_0042B300, D_0042B310, 2, func_00168550, 0);
        REGISTER(D_0042B328, D_0042B330, 2, func_001685A8, 0);
        REGISTER(D_0042B340, D_0042B358, 1, func_00168630, 0);
        REGISTER(D_0042B378, D_0042B388, 2, func_00168660, 0);
#undef REGISTER
    }
    return D_003F83D4;
}

void func_00167B98(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    void *mode = func_002CDF90();
    GeorgeDeimosValue *arguments = D_00474F48;
    void *object, *path;
    float blend;
    (void)count; (void)destination;
    if ((s16)arguments[1].tag != 6 || (s16)arguments[2].tag != 2) {
        func_002CC938(D_0042B428); return;
    }
    object = func_00239FD8(arguments[1].payload.bits);
    if (object == 0) return;
    path = func_00238BA0(object, 0x53427A93U);
    if (path == 0) return;
    blend = func_00135D88(path, func_00161C30(-1));
    blend += D_00474F48[2].payload.scalar / FIELD(path, 0x30, float);
    if (0.0f <= blend) blend = george_ee_minimum(blend, 1.0f);
    else blend = 0.0f;
    func_00135E88(path, VECTOR(controller, mode != 0 ? 0x114 : 0x108), blend);
    if (mode != 0) FIELD(controller, 0x120, u32) = 1;
}

void func_00167CC0(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    void *mode = func_002CDF90();
    GeorgeDeimosValue *arguments;
    u32 second_word;
    void *first_object, *second_object, *first_path, *second_path;
    GeorgeMathVec3 *output, *reference;
    GeorgeMathVec3 first, second, direction;
    float length, x, y, z, blend;
    (void)destination;
    if (count < 3) { func_002CC938(D_0042B488); return; }
    arguments = D_00474F48;
    if ((s16)arguments[1].tag != 6 || (s16)arguments[2].tag != 6) {
        func_002CC938(D_0042B488); return;
    }
    second_word = arguments[2].payload.bits;
    first_object = func_00239FD8(arguments[1].payload.bits);
    second_object = func_00239FD8(second_word);
    if (first_object == 0 || second_object == 0) return;
    first_path = func_00238BA0(first_object, 0x53427A93U);
    second_path = func_00238BA0(second_object, 0x53427A93U);
    if (first_path == 0 || second_path == 0) return;
    output = VECTOR(controller, 0x108);
    if (mode != 0) {
        output = VECTOR(controller, 0x114);
        FIELD(controller, 0x120, u32) = 1;
    }
    func_00135D10(first_path, &first, func_00161C30(-1));
    func_00135D10(second_path, &second, func_00161C30(-1));
    direction.x = second.x - first.x;
    direction.y = second.y - first.y;
    direction.z = second.z - first.z;
    length = func_002A3538(&direction);
    reference = func_00161C30(-1);
    x = reference->x - first.x;
    y = reference->y - first.y;
    z = reference->z - first.z;
    blend = ((direction.x * x + direction.y * y) + direction.z * z) / length;
    if (count == 4 && (s16)D_00474F48[3].tag == 2)
        blend += D_00474F48[3].payload.scalar / length;
    if (blend < 0.0f) camera_copy(output, &first);
    else if (1.0f < blend) camera_copy(output, &second);
    else {
        x = first.x + blend * (second.x - first.x);
        y = first.y + blend * (second.y - first.y);
        z = first.z + blend * (second.z - first.z);
        output->x = x;
        output->y = y;
        output->z = z;
    }
}

void func_00167F98(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    void *mode = func_002CDF90();
    GeorgeDeimosValue *arguments = D_00474F48;
    void *object, *path;
    (void)count; (void)destination;
    if ((s16)arguments[1].tag != 6 || (s16)arguments[2].tag != 2) {
        func_002CC938(D_0042B4F8); return;
    }
    object = func_00239FD8(arguments[1].payload.bits);
    if (object == 0) return;
    path = func_00238BA0(object, 0x53427A93U);
    if (path == 0) return;
    func_00135E88(path, VECTOR(controller, mode != 0 ? 0x114 : 0x108),
                 D_00474F48[2].payload.scalar);
    if (mode != 0) FIELD(controller, 0x120, u32) = 1;
}

void func_00168290(void *object)
{
    float velocity, adjustment, duration, phase;
    if (FIELD(object, 0x138, float) < 0.0f) {
        velocity = FIELD(object, 0x12C, float);
        adjustment = FIELD(object, 0x134, float);
        duration = velocity + velocity;
        FIELD(object, 0x140, float) = 0.0f;
        FIELD(object, 0x13C, float) = velocity;
        FIELD(object, 0x138, float) = 0.0f;
        FIELD(object, 0x158, u32) = 0;
        FIELD(object, 0x144, float) = duration / (9.80000019073486328125f - adjustment);
    } else if (FIELD(object, 0x158, u32) == 0) {
        phase = FIELD(object, 0x140, float) / FIELD(object, 0x144, float);
        if (0.300000011920928955078125f < phase && phase < 0.699999988079071044921875f) {
            velocity = FIELD(object, 0x13C, float);
            adjustment = FIELD(object, 0x130, float);
            FIELD(object, 0x158, u32) = 1;
            FIELD(object, 0x13C, float) = velocity + adjustment;
        }
    }
}

void func_00168398(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    GeorgeDeimosValue *argument = D_00474F48 + 1;
    (void)count; (void)destination;
    if ((s16)argument->tag == 2) FIELD(controller, 0x148, float) = argument->payload.scalar;
    else func_002CC938(D_0042B3A0);
}

void func_001683F0(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    void *mode = func_002CDF90();
    GeorgeDeimosValue *argument = D_00474F48 + 1;
    void *object, *path;
    GeorgeMathVec3 *output;
    (void)count; (void)destination;
    if ((s16)argument->tag != 6) { func_002CC938(D_0042B3E0); return; }
    object = func_00239FD8(argument->payload.bits);
    if (object == 0) return;
    path = func_00238BA0(object, 0x53427A93U);
    if (path == 0) return;
    output = VECTOR(controller, mode != 0 ? 0x114 : 0x108);
    func_00135D10(path, output, func_00161C30(-1));
    if (mode != 0) FIELD(controller, 0x120, u32) = 1;
}

void func_001684D0(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    (void)count; (void)destination;
    if (controller != 0) FIELD(controller, 0x120, u32) = 0;
}

#define NULLABLE_SCALAR(name, offset, error) \
void name(s32 count, s32 destination) \
{ \
    void *controller = func_002D0B48(0x9A825260U); \
    GeorgeDeimosValue *argument; \
    (void)count; (void)destination; \
    if (controller == 0) { func_002CC938(error); return; } \
    argument = D_00474F48 + 1; \
    if ((s16)argument->tag == 2) FIELD(controller, offset, float) = argument->payload.scalar; \
    else func_002CC938(error); \
}
NULLABLE_SCALAR(func_001684F8, 0x128, D_0042B548)
NULLABLE_SCALAR(func_00168550, 0x124, D_0042B590)
#undef NULLABLE_SCALAR

void func_001685A8(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    GeorgeDeimosValue *argument;
    void *object, *path;
    (void)count; (void)destination;
    if (controller == 0) { func_002CC938(D_0042B590); return; }
    argument = D_00474F48 + 1;
    if ((s16)argument->tag != 6) { func_002CC938(D_0042B590); return; }
    object = func_00239FD8(argument->payload.bits);
    if (object == 0) return;
    path = func_00238BA0(object, 0x53427A93U);
    if (path == 0) return;
    FIELD(path, 0x48, float) = -1.0f;
    FIELD(path, 0x44, float) = -1.0f;
}

void func_00168630(s32 count, s32 destination)
{
    void *controller = func_002D0B48(0x9A825260U);
    (void)count; (void)destination;
    if (controller != 0) FIELD(controller, 0x15C, u32) = 1;
}

void func_00168660(s32 count, s32 destination)
{
    GeorgeDeimosValue *argument = D_00474F48 + 1;
    void *object, *path;
    GeorgeDeimosValue *output;
    float fraction;
    (void)count;
    if ((s16)argument->tag != 6) { func_002CC938(D_0042B5D0); return; }
    object = func_00239FD8(argument->payload.bits);
    if (object == 0) return;
    path = func_00238BA0(object, 0x53427A93U);
    if (path == 0) return;
    fraction = func_00135D88(path, func_00161C30(-1));
    if (destination != -1) {
        output = (GeorgeDeimosValue *)ADDRESS(D_00474748, (u32)destination << 3);
        output->payload.scalar = fraction;
        output->tag = 2;
        output->subtype = 0;
    }
}
