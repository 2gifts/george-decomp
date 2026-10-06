#include "george/pad_input.h"

/* D4 is observed as a float by the rumble caller. Preserve the shared layout's
 * neutral word view without pointer-punning or inventing an initial value. */
static __inline__ float pad_phase(const GeorgeInputState *state)
{
    union { u32 bits; float scalar; } value;
    value.bits = state->reservedD4;
    return value.scalar;
}

static __inline__ void pad_set_phase(GeorgeInputState *state, float scalar)
{
    union { u32 bits; float scalar; } value;
    value.scalar = scalar;
    state->reservedD4 = value.bits;
}

void func_00295080(void)
{
    s32 index;
    GeorgeInputState *first, *second, *third, *fourth, *extra;
    for (index = 0; index < 4; ++index)
        D_00468388[index] = func_002AC628(1, (u32)index);
    first = D_00468388[0];
    fourth = D_00468388[3];
    second = D_00468388[1];
    third = D_00468388[2];
    D_00468398 = first;
    D_0046839C = second;
    D_004683A0 = third;
    D_004683A4 = fourth;
    extra = func_002AC628(2, 4);
    D_004683A8 = extra;
    extra->flags &= ~0x18U;
    D_004683AC = func_002AC700();
    for (index = 0; index < 4; ++index)
        func_002B2DA8(&D_00468400[index], index, 0, 0);
}

float func_00295B18(float value)
{
    if (value < -127.0f) value = -127.0f;
    if (127.0f < value) value = 127.0f;
    if (0.0f < value) {
        value = (value - 20.0f) * 1.186915874481201171875f;
        if (value < 0.0f) value = 0.0f;
    } else if (value < 0.0f) {
        value = (value + 20.0f) * 1.186915874481201171875f;
        if (0.0f < value) value = 0.0f;
    }
    return value;
}

/* Original radial conversion is also inlined twice in the poller. Keeping
 * this as an inline source template retains the original call graph. */
static __inline__ void pad_radial(float *output_x, float *output_y,
    float *magnitude, float *angle, float x, float y, float threshold)
{
    float length = george_ee_square_root(x * x + y * y);
    float inverse = 1.0f / (1.0f - threshold);
    float scaled = (length * 0.0078740157186985015869140625f - threshold) * inverse;
    float value;
    scaled = george_ee_maximum(scaled, 0.0f);
    *magnitude = george_ee_minimum(scaled, 1.0f);
    value = func_0029B940(y, x);
    *angle = func_0037B238(6.283185482025146484375f - value, 6.283185482025146484375f);
    if (0.0f < length) {
        value = (scaled * x) / length;
        *output_x = -1.0f <= value ? george_ee_minimum(value, 1.0f) : -1.0f;
        value = (scaled * y) / length;
        *output_y = -1.0f <= value ? george_ee_minimum(value, 1.0f) : -1.0f;
    } else {
        *output_x = 0.0f;
        *output_y = 0.0f;
    }
}

void func_00295BD0(float *output_x, float *output_y, float *magnitude,
    float *angle, float x, float y, float threshold)
{
    pad_radial(output_x, output_y, magnitude, angle, x, y, threshold);
}

#define PAD_MAP_BUTTONS(raw, buttons) do { \
    if (raw & 0x8000U) buttons = 0x80000000U; \
    if (raw & 0x2000U) buttons |= 0x20000000U; \
    if (raw & 0x1000U) buttons |= 0x40000000U; \
    if (raw & 0x4000U) buttons |= 0x10000000U; \
    if (raw & 0x0080U) buttons |= 0x00200000U; \
    if (raw & 0x0010U) buttons |= 0x00100000U; \
    if (raw & 0x0040U) buttons |= 0x00080000U; \
    if (raw & 0x0020U) buttons |= 0x00040000U; \
    if (raw & 0x0008U) buttons |= 0x00020000U; \
    if (raw & 0x0002U) buttons |= 0x00010000U; \
    if (raw & 0x0400U) buttons |= 0x00008000U; \
    if (raw & 0x0004U) buttons |= 0x00004000U; \
    if (raw & 0x0001U) buttons |= 0x00002000U; \
    if (raw & 0x0200U) buttons |= 0x00001000U; \
    if (raw & 0x0800U) buttons |= 0x08000000U; \
    if (raw & 0x0100U) buttons |= 0x04000000U; \
} while (0)

void func_00295170(float elapsed)
{
    float axes[4], pressure[12];
    u8 raw_pressure[12];
    s16 pressure_delta[12];
    signed char raw_axes[4];
    u16 axis_delta[4];
    u32 raw_buttons;
    s32 index;
    for (index = 0; index < 4; ++index) {
        GeorgePadDevice *pad = &D_00468400[index];
        GeorgeInputState *state;
        u32 buttons = 0;
        s32 mode = 0, kind, component;
        float left_angle = 0.0f, left_magnitude = 0.0f;
        float right_angle = 0.0f, right_magnitude = 0.0f;
        func_003936A0(axes, 0, sizeof(axes));
        func_003936A0(pressure, 0, sizeof(pressure));
        kind = func_002B2800(pad, &raw_buttons, &raw_axes[0], &raw_axes[1],
            &raw_axes[2], &raw_axes[3], raw_pressure, pressure_delta);
        if (pad->connection_state == 99) {
            func_002B2ED8(pad, &axis_delta[0], &axis_delta[1], &axis_delta[2], &axis_delta[3]);
            if (kind == 1 || kind == 2 || kind == 3) {
                if (kind == 3)
                    for (component = 0; component < 12; ++component)
                        pressure[component] = (float)(signed char)raw_pressure[component] * 0.0039215688593685626983642578125f;
                if (kind == 2 || kind == 3) {
                    pad_radial(&axes[0], &axes[1], &left_magnitude, &left_angle,
                        (float)raw_axes[0], (float)raw_axes[1], D_00468388[index]->threshold);
                    pad_radial(&axes[2], &axes[3], &right_magnitude, &right_angle,
                        (float)raw_axes[2], (float)raw_axes[3], D_00468388[index]->threshold);
                }
                PAD_MAP_BUTTONS(raw_buttons, buttons);
                mode = 1;
            }
        }
        func_002AC310(D_00468388[index], mode, axes, 0, buttons, pressure,
            elapsed, left_angle, left_magnitude, right_angle, right_magnitude);
        state = D_00468388[index];
        if (state->flags & 2U) {
            if (state->lock_timer == 0.0f)
                pad_set_phase(state, 0.5f);
            else
                pad_set_phase(state, pad_phase(state) + ((state->lock_value * 30.0f) * elapsed));
            if (state->lock_timer == 0.0f || pad_phase(state) < 1.0f)
                func_002B2D60(&D_00468400[index], 0, 0);
            else {
                func_002B2D60(&D_00468400[index], 1, 96);
                pad_set_phase(state, pad_phase(state) - (30.0f * elapsed));
            }
        } else {
            state->lock_timer = 0.0f;
            pad_set_phase(state, 0.5f);
            func_002B2D60(&D_00468400[index], 0, 0);
        }
    }
}
