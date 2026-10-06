#include "george/actor_controller.h"
#include "george/ee_math.h"

extern GeorgeDeimosPoolNode *D_003F83D8;
extern GeorgeDeimosValue *D_00474F48;
extern void *D_004961F4;
extern const u8 D_0042BD08[], D_0042BF90[], D_0042BF48[], D_0040DC60[];
extern const GeorgeMathVec3 D_0042BD90;
extern const char D_0042B960[], D_0042B970[], D_0042B980[], D_0042B9A0[], D_0042B9B0[];
extern const char D_0042B9C8[], D_0042B9E0[], D_0042BA00[], D_0042BA18[], D_0042BA38[], D_0042BA50[];
extern const char D_0042BA70[], D_0042BA88[], D_0042BAA8[], D_0042BAB8[];
extern const char D_0042BAD0[], D_0042BB10[], D_0042BB50[], D_0042BB90[], D_0042BBD0[], D_0042BC10[], D_0042BC50[];
extern void *func_002D0B48(u32);
extern void func_002CC938(const char *, ...);
extern void func_002CE6B8(GeorgeDeimosPoolNode *, const char *, const char *, u32, void (*)(s32, s32), u32);
extern s32 func_002D0978(void *, u32);
extern void func_00166F58(void *, u32);
extern u32 func_0029C648(const char *);
extern void func_0023A5D0(u32, const char *, u32, u32, u32, const u8 *, u32, u32);
extern void *func_0016EE68(void *);
extern void func_002A1C08(void *, const void *);
extern void *func_003936A0(void *, s32, u32);
extern void *func_002E2BB0(void *, u32, u32);
extern void *func_00306460(void *, void *, u32);
extern void *func_0022C1E0(void);
extern void func_00311F80(void *, void *);
extern void func_003064F0(void *, u32);
extern void func_002E2CD8(void *, void *, u32, u32);
extern void func_002ADC80(u32, const void *, u32, float *, float);
extern void func_002A1F18(GeorgeRotationMatrix *, const GeorgeMathVec4 *, const GeorgeMathVec3 *);
extern void func_002A1C30(GeorgeRotationMatrix *);
extern void func_002A0B00(GeorgeMathVec4 *, const GeorgeRotationMatrix *);
extern void func_003095D8(void *, const GeorgeMathVec4 *);
extern void func_00307850(void *);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);
extern float func_003734F8(GeorgeActorBits64);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define ADJUST(object, adjustment) ((void *)ADDRESS(object, (s32)(adjustment)))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))

GeorgeDeimosPoolNode *func_0016C098(void)
{
    if (D_003F83D8 == 0) {
        D_003F83D8 = func_002CD348(0);
        func_002CD0B8(D_003F83D8);
#define REGISTER(label, types, callback) \
        func_002CE6B8(D_003F83D8, label, types, 2, callback, 0)
        REGISTER(D_0042B970, D_0042B980, func_0016C2F8);
        REGISTER(D_0042B9A0, D_0042B9B0, func_0016C350);
        REGISTER(D_0042B9C8, D_0042B9E0, func_0016C3A8);
        REGISTER(D_0042BA00, D_0042BA18, func_0016C400);
        REGISTER(D_0042BA38, D_0042BA50, func_0016C458);
        REGISTER(D_0042BA70, D_0042BA88, func_0016C4B0);
        REGISTER(D_0042BAA8, D_0042BAB8, func_0016C508);
#undef REGISTER
    }
    return D_003F83D8;
}

s32 func_0016C238(void *object, u32 key)
{
    if (key == 0x4907C265U) return 1;
    return func_002D0978(object, key) != 0;
}

void func_0016C280(void *object, u32 mode)
{
    FIELD(object, 4, const u8 *) = D_0042BD08;
    func_00166F58(object, mode);
}

/* The seven original setters have identical gates and distinct field/error
 * bindings. Callback parameters and any destination payload remain untouched. */
#define SCALAR_SETTING(name, offset, error_text) \
void name(s32 count, s32 destination) \
{ \
    void *object = func_002D0B48(0x4907C265U); \
    GeorgeDeimosValue *argument = D_00474F48 + 1; \
    (void)count; (void)destination; \
    if ((s16)argument->tag == 2) FIELD(object, offset, float) = argument->payload.scalar; \
    else func_002CC938(error_text); \
}
SCALAR_SETTING(func_0016C2F8, 0xF8, D_0042BAD0)
SCALAR_SETTING(func_0016C350, 0xF4, D_0042BB10)
SCALAR_SETTING(func_0016C3A8, 0xF0, D_0042BB50)
SCALAR_SETTING(func_0016C400, 0x104, D_0042BB90)
SCALAR_SETTING(func_0016C458, 0x100, D_0042BBD0)
SCALAR_SETTING(func_0016C4B0, 0xFC, D_0042BC10)
SCALAR_SETTING(func_0016C508, 0x108, D_0042BC50)
#undef SCALAR_SETTING

void func_0016C560(void)
{
    u32 key = func_0029C648(D_0042B960);
    func_0023A5D0(key, D_0042B960, 0, 0, 0x1C, D_0040DC60, 8, 0);
}

void func_0016CED0(void *object, u32 mode)
{
    func_003064F0(object, 0);
    if ((mode & 1U) != 0)
        func_002E2CD8(D_004961F4, object, FIELD(object, 4, u16), 0x27);
}

void *func_0016C5B8(void *object, const GeorgeRotationMatrix *matrix,
                  void *configuration, GeorgeGoalEntity *actor)
{
    void *wheel_source, *child, *manager;
    float x, y, z, first, second;
    func_0016EE68(object);
    FIELD(object, 0x27C, void *) = configuration;
    FIELD(object, 8, void *) = actor;
    FIELD(object, 4, const u8 *) = D_0042BF90;
    FIELD(object, 0xC, u32) = 0;
    FIELD(object, 0x14, u32) = 0;
    FIELD(object, 0x10, u32) = 0;
    func_002A1C08(ADDRESS(object, 0x20), matrix);
    FIELD(object, 0x270, float) = 1000.0f;
    FIELD(object, 0x264, float) = 100.0f;
    FIELD(object, 0x24C, u32) = 0;
    FIELD(object, 0x250, u32) = 0;
    FIELD(object, 0x23C, u32) = 0;
    FIELD(object, 0x230, u32) = 0;
    FIELD(object, 0x254, u32) = 0;
    FIELD(object, 0x258, u32) = 0;
    FIELD(object, 0x25C, u32) = 0;
    FIELD(object, 0x268, u32) = 0;
    FIELD(object, 0x26C, u32) = 0;
    FIELD(object, 0x260, u32) = 0;
    {
        u32 mode = FIELD(FIELD(object, 0x27C, void *), 0x2F0, u32);
        FIELD(object, 0x244, u32) = 0;
        FIELD(object, 0x240, u32) = mode;
    }
    FIELD(object, 0x248, u32) = 0;
    wheel_source = FIELD(actor, 0x21C, void *);
    FIELD(object, 0x234, float) = FIELD(wheel_source, 0x2C, float);
    FIELD(object, 0x238, float) = FIELD(wheel_source, 0x50, float);
    func_003936A0(ADDRESS(object, 0x90), 0, 0x1A0);
    x = FIELD(wheel_source, 0x30, float);
    y = FIELD(FIELD(object, 0x27C, void *), 0x1C, float);
    z = FIELD(wheel_source, 0x38, float);
    FIELD(object, 0x90, float) = x;
    FIELD(object, 0x98, float) = z;
    FIELD(object, 0x94, float) = y;
    x = -FIELD(wheel_source, 0x30, float);
    z = FIELD(wheel_source, 0x38, float);
    y = FIELD(FIELD(object, 0x27C, void *), 0x1C, float);
    FIELD(object, 0xF8, float) = x;
    FIELD(object, 0x100, float) = z;
    FIELD(object, 0xFC, float) = y;
    x = FIELD(wheel_source, 0x54, float);
    y = FIELD(FIELD(object, 0x27C, void *), 0x34, float);
    z = FIELD(wheel_source, 0x5C, float);
    FIELD(object, 0x160, float) = x;
    FIELD(object, 0x168, float) = z;
    FIELD(object, 0x164, float) = y;
    x = -FIELD(wheel_source, 0x54, float);
    z = FIELD(wheel_source, 0x5C, float);
    y = FIELD(FIELD(object, 0x27C, void *), 0x34, float);
    FIELD(object, 0x1C8, float) = x;
    FIELD(object, 0x1D0, float) = z;
    FIELD(object, 0x1CC, float) = y;
    y = FIELD(FIELD(object, 0x27C, void *), 0x1C, float);
    first = FIELD(object, 0x234, float);
    z = FIELD(object, 0x238, float);
    second = FIELD(FIELD(object, 0x27C, void *), 0x34, float);
    first = (y + first) - FIELD(wheel_source, 0x34, float);
    second = (second + z) - FIELD(wheel_source, 0x58, float);
    FIELD(object, 0xBC, float) = first;
    FIELD(object, 0x124, float) = first;
    FIELD(object, 0x1F4, float) = second;
    FIELD(object, 0x18C, float) = second;
    FIELD(object, 0x274, void *) = FIELD(actor, 0x8EC, void *);
    child = func_002E2BB0(D_004961F4, 0xC0, 0x27);
    FIELD(child, 4, u16) = 0xC0;
    func_00306460(child, FIELD(object, 0x274, void *), 0);
    FIELD(child, 0x1C, void *) = object;
    FIELD(child, 0, const u8 *) = D_0042BF48;
    FIELD(object, 0x278, void *) = child;
    manager = func_0022C1E0();
    func_00311F80(manager, FIELD(object, 0x278, void *));
    return object;
}

/* The controller's distinct layout is kept opaque: configuration +27C,
 * physical object +274, and adjusted actor result at the actor's table +E0. */
void func_0016C7E0(void *object, float elapsed)
{
    float length, forward, factor = 1.0f, desired, current, difference;
    float curve[2], x, y, z, w, reciprocal, height_dot;
    GeorgeMathVec3 position;
    GeorgeMathVec4 rotation, converted, conjugate;
    GeorgeRotationMatrix matrix;
    GeorgeActorBits64 step, delta, value;
    void *physical, *actor;
    u32 command;
    length = george_ee_square_root((FIELD(object, 0xC, float) * FIELD(object, 0xC, float) +
                                   FIELD(object, 0x10, float) * FIELD(object, 0x10, float)) +
                                  FIELD(object, 0x14, float) * FIELD(object, 0x14, float));
    FIELD(object, 0x18, float) = length;
    forward = ((FIELD(object, 0xC, float) * FIELD(object, 0x40, float) +
                FIELD(object, 0x10, float) * FIELD(object, 0x44, float)) +
               FIELD(object, 0x14, float) * FIELD(object, 0x48, float)) * 3.6000001430511475f;
    if (1.0f < length) {
        float lateral = ((FIELD(object, 0xC, float) * FIELD(object, 0x20, float) +
                          FIELD(object, 0x10, float) * FIELD(object, 0x24, float)) +
                         FIELD(object, 0x14, float) * FIELD(object, 0x28, float)) / length;
        value = func_00374848(lateral);
        if (func_00373250(value, 0) < 0) value = func_00372CC0(0, value);
        value = func_00372CC0(0x3FF0000000000000ULL, value);
        factor = func_003734F8(value);
    }
    {
        void *config = FIELD(object, 0x27C, void *);
        func_002ADC80(FIELD(config, 0x4C, u32), ADDRESS(config, 0x58), 2, curve, forward * factor);
    }
    FIELD(object, 0x230, float) = curve[1];
    if (FIELD(object, 0, u32) == 0) {
        FIELD(object, 0x23C, u32) = 0;
        FIELD(object, 0x254, u32) = 1;
        FIELD(object, 0x24C, u32) = 0;
    }
    physical = FIELD(object, 0x274, void *);
    position.x = FIELD(physical, 0xE0, float);
    position.y = FIELD(physical, 0xE4, float);
    position.z = FIELD(physical, 0xE8, float);
    rotation.x = -FIELD(physical, 0x120, float);
    rotation.y = -FIELD(physical, 0x124, float);
    rotation.z = -FIELD(physical, 0x128, float);
    rotation.w = FIELD(physical, 0x12C, float);
    func_002A1F18(FRAME(object, 0x20), &rotation, &position);
    physical = FIELD(object, 0x274, void *);
    x = FIELD(physical, 0x180, float);
    z = FIELD(physical, 0x188, float);
    y = FIELD(physical, 0x184, float);
    FIELD(object, 0xC, float) = x;
    FIELD(object, 0x10, float) = y;
    FIELD(object, 0x14, float) = z;
    step = func_00374848(FIELD(FIELD(object, 0x27C, void *), 0x2F4, float) * elapsed);
    if (func_00373250(step, 0) < 0) step = func_00372CC0(0, step);
    difference = FIELD(object, 0x23C, float) - FIELD(object, 0x250, float);
    delta = func_00374848(difference);
    {
        s32 sign = func_00373250(delta, 0);
        desired = FIELD(object, 0x23C, float);
        current = FIELD(object, 0x250, float);
        if (sign < 0) delta = func_00372CC0(0, delta);
    }
    if (func_00373250(step, delta) > 0) {
        FIELD(object, 0x250, float) = desired;
    } else {
        float amount;
        difference = desired - current;
        if (0.0f <= difference)
            amount = FIELD(FIELD(object, 0x27C, void *), 0x2F4, float) * elapsed;
        else
            amount = FIELD(FIELD(object, 0x27C, void *), 0x2F4, float) * -elapsed;
        FIELD(object, 0x250, float) = current + amount;
    }
    /* Capture all original unaligned readonly components before virtual call. */
    position = D_0042BD90;
    actor = FIELD(object, 8, void *);
    height_dot = (FIELD(object, 0x30, float) * position.x +
                  FIELD(object, 0x34, float) * position.y) +
                 FIELD(object, 0x38, float) * position.z;
    if (actor != 0) {
        const GeorgeGoalVirtualFloatResult *method =
            (const GeorgeGoalVirtualFloatResult *)ADDRESS(FIELD(actor, 4, void *), 0xE0);
        float result = method->invoke(ADJUST(actor, method->adjustment));
        if (0.0f < result && height_dot <= 0.3499999940395355f) {
            float timer = FIELD(object, 0x248, float) + elapsed;
            FIELD(object, 0x248, float) = timer;
            if (10.0f <= timer) {
                FIELD(object, 0x248, u32) = 0;
                func_002A1C30(&matrix);
                func_002A0B00(&converted, &matrix);
                x = converted.x; y = converted.y; z = converted.z; w = converted.w;
                reciprocal = george_ee_reciprocal_square_root(1.0f, ((x*x+y*y)+z*z)+w*w);
                w = w * reciprocal;
                x = x * reciprocal;
                y = y * reciprocal;
                z = z * reciprocal;
                conjugate.x = -x; conjugate.y = -y; conjugate.z = -z; conjugate.w = w;
                func_003095D8(FIELD(object, 0x274, void *), &conjugate);
            }
        } else {
            FIELD(object, 0x248, u32) = 0;
        }
    } else {
        FIELD(object, 0x248, u32) = 0;
    }
    command = FIELD(object, 0x244, u32);
    if (command == 0 && FIELD(object, 0x24C, float) != 0.0f) {
        s32 stopped = 0;
        value = func_00374848(FIELD(object, 0xC, float));
        if (func_00373250(value, 0) < 0) value = func_00372CC0(0, value);
        delta = func_00374848(0.05000000074505806f);
        if (func_00373250(value, delta) < 0) {
            value = func_00374848(FIELD(object, 0x10, float));
            if (func_00373250(value, 0) < 0) value = func_00372CC0(0, value);
            delta = func_00374848(0.05000000074505806f);
            if (func_00373250(value, delta) < 0) {
                value = func_00374848(FIELD(object, 0x14, float));
                if (func_00373250(value, 0) < 0) value = func_00372CC0(0, value);
                delta = func_00374848(0.05000000074505806f);
                stopped = func_00373250(value, delta) < 0;
            }
        }
        if (stopped != 0) {
            u32 index, retained = 0;
            for (index = 0; index < 4U; ++index)
                if (FIELD(object, 0xB4U + index * 0x68U, u32) != 0) retained += 1U;
            /* The branch-delay MOVN conditionally retains count+1. */
            if (retained != 4U) {
                FIELD(object, 0x244, u32) = 1;
                command = 1;
                FIELD(object, 0x240, u32) = 2;
                goto publish;
            }
        }
        command = FIELD(object, 0x244, u32);
    }
    if (command == 1) {
        float speed2 = (FIELD(object, 0xC, float) * FIELD(object, 0xC, float) +
                        FIELD(object, 0x10, float) * FIELD(object, 0x10, float)) +
                       FIELD(object, 0x14, float) * FIELD(object, 0x14, float);
        if (9.0f <= speed2) {
            FIELD(object, 0x244, u32) = 0;
            FIELD(object, 0x240, u32) = FIELD(FIELD(object, 0x27C, void *), 0x2F0, u32);
        }
    }
publish:
    command = FIELD(object, 0x254, u32);
    FIELD(object, 0x1B8, u32) = command;
    FIELD(object, 0x220, u32) = command;
    if (command != 0) FIELD(object, 0x258, float) = FIELD(object, 0x258, float) + elapsed;
    else FIELD(object, 0x258, u32) = 0;
    func_00307850(FIELD(object, 0x274, void *));
}
