#include "george/input_state.h"
#include "george/heap.h"

extern void *func_003936A0(void *memory, s32 value, u32 size);

void func_002AC1C0(GeorgeInputState *state, s32 mode, u32 device)
{
    s32 group, component, index;
    state->mode = mode;
    state->flags = 0x1Au;
    state->device = device;
    state->repeat_interval = 0.13333334028720856f;
    state->repeat_timer = 0.26666668057441711f;
    state->threshold = 0.1599999964237213f;
    state->previous = 0;
    state->held = 0;
    state->pressed = 0;
    state->repeated = 0;
    state->recent = 0;
    state->reservedD4 = 0;
    state->history_index = 0;
    for (index = 0; index < 6; ++index) state->history[index] = 0;
    for (index = 11; index >= 0; --index) state->pressure[index] = 0.0f;
    for (group = 0; group < 2; ++group)
        for (component = 0; component < 2; ++component) {
            GeorgeInputAxis *axis = &state->axes[group][component];
            /* Retail zeroes each triple from its last word backwards. */
            axis->delta = 0.0f;
            axis->current = 0.0f;
            axis->previous = 0.0f;
        }
    state->lock_value = 0.0f;
    state->output_axes[0][0] = 0.0f;
    state->output_axes[1][0] = 0.0f;
    state->previous_output_axes[0][0] = 0.0f;
    state->previous_output_axes[1][0] = 0.0f;
    state->output_axes[0][1] = 0.0f;
    state->output_axes[1][1] = 0.0f;
    state->previous_output_axes[0][1] = 0.0f;
    state->previous_output_axes[1][1] = 0.0f;
    state->lock_timer = 0.0f;
    /* released and four byte-pressure values are deliberately untouched. */
}

void func_002AC310(GeorgeInputState *state, s32 mode, const float *axes,
    const u8 *byte_pressure, u32 buttons, const float *pressure,
    float elapsed, float axis0, float axis1, float axis2, float axis3)
{
    u32 index, flags, old_buttons, pressed, recent;
    float old0, old1, old2, old3, timer;
    if ((state->flags & 1u) != 0) return;
    state->mode = mode;
    if (axes != NULL) {
        for (index = 0; index < 4; ++index) {
            GeorgeInputAxis *axis = &state->axes[index / 2][index % 2];
            float value;
            axis->previous = axis->current;
            value = axes[index];
            axis->current = value;
            axis->delta = value - axis->previous;
        }
    }
    if (byte_pressure != NULL)
        for (index = 0; index < 4; ++index) state->byte_pressure[index] = byte_pressure[index];
    old0 = state->output_axes[0][0];
    old2 = state->output_axes[1][0];
    old1 = state->output_axes[0][1];
    old3 = state->output_axes[1][1];
    flags = state->flags;
    state->previous_output_axes[0][0] = old0;
    state->previous_output_axes[1][0] = old2;
    state->previous_output_axes[0][1] = old1;
    state->previous_output_axes[1][1] = old3;
    state->output_axes[0][0] = axis0;
    state->output_axes[1][0] = axis2;
    state->output_axes[0][1] = axis1;
    state->output_axes[1][1] = axis3;
    if ((flags & 8u) != 0) {
        float value = state->axes[0][0].current;
        if (value > 0.5f) buttons |= 0x20000000u;
        if (value < -0.5f) buttons |= 0x80000000u;
        state->flags = flags & ~8u;
    }
    flags = state->flags;
    if ((flags & 0x10u) != 0) {
        float value = state->axes[0][1].current;
        if (value < -0.5f) buttons |= 0x40000000u;
        if (value > 0.5f) buttons |= 0x10000000u;
        state->flags = flags & ~0x10u;
    }
    old_buttons = state->held;
    index = state->history_index;
    pressed = buttons & ~old_buttons;
    state->released = old_buttons & ~buttons;
    state->previous = old_buttons;
    state->pressed = pressed;
    state->repeated = pressed;
    state->held = buttons;
    state->history[index] = pressed;
    state->history_index = (state->history_index + 1u) % 6u;
    recent = 0;
    for (index = 0; index < 6; ++index) recent |= state->history[index];
    state->recent = recent;
    if (pressure != NULL)
        for (index = 0; index < 12; ++index) state->pressure[index] = pressure[index];
    if (state->previous == buttons) {
        timer = state->repeat_timer - elapsed;
        state->repeat_timer = timer;
        if (timer < 0.0f) {
            state->repeated = state->held;
            state->repeat_timer = state->repeat_interval;
        }
    } else {
        float interval = state->repeat_interval;
        state->repeat_timer = interval + interval;
    }
    timer = state->lock_timer - elapsed;
    state->lock_timer = timer;
    if (timer < 0.0f) state->lock_timer = 0.0f;
}

GeorgeInputState *func_002AC628(s32 mode, u32 device)
{
    GeorgeInputState *state;
    if (mode < 0 || mode >= 3) mode = 0;
    state = func_002AEC28(0xDC);
    if (state != NULL) func_002AC1C0(state, mode, device);
    return state;
}

void func_002AC6B8(GeorgeInputState *state, float timer, float value)
{
    state->lock_value = value;
    state->lock_timer = timer;
}

void func_002AC6C8(GeorgeInputState *state)
{
    func_002AC1C0(state, state->mode, state->device);
}

void func_002AC6E8(GeorgeInputState *state)
{
    state->released = 0;
    state->pressed = 0;
    state->repeated = 0;
    state->recent = 0;
}

GeorgeKeyState *func_002AC700(void)
{
    GeorgeKeyState *state = func_002AEC28(0x108);
    if (state != NULL) {
        state->selected = 0;
        state->repeat_timer = 0.13333334028720856f;
        func_003936A0(state->keys, 0, 0x100);
    }
    return state;
}

void func_002AC778(GeorgeKeyState *state, float elapsed)
{
    u32 index;
    u8 selected;
    for (index = 0; index < 256; ++index) {
        u32 held = state->keys[index] & 1u;
        u32 previous = (state->keys[index] >> 1) & 1u;
        u32 pressed = held & ~previous;
        state->keys[index] = (u8)(held | (held << 1) | (pressed << 2) | (pressed << 3));
    }
    selected = state->selected;
    if (selected != 0) {
        float timer = state->repeat_timer - elapsed;
        state->repeat_timer = timer;
        if (timer <= 0.0f) {
            state->keys[selected] |= 8u;
            state->repeat_timer = 0.066666670143604279f;
        }
    }
}
