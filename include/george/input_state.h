#ifndef GEORGE_INPUT_STATE_H
#define GEORGE_INPUT_STATE_H

#include "george/compiler.h"
#include "george/duplicate_helpers.h"

typedef struct GeorgeInputAxis {
    float previous, current, delta;
} GeorgeInputAxis;

/* Observed frame-state layout. Device and reserved words retain neutral names;
 * button labels are not assigned without the corresponding device mapping. */
typedef struct GeorgeInputState {
    s32 mode;
    u32 flags, device;
    float repeat_interval, repeat_timer;
    u32 previous, held, pressed, repeated, recent, released;
    float pressure[12];
    GeorgeInputAxis axes[2][2];
    u8 byte_pressure[4];
    u32 history_index, history[6];
    float output_axes[2][2];
    float previous_output_axes[2][2];
    float lock_timer, lock_value;
    u32 reservedD4;
    float threshold;
} GeorgeInputState;

typedef struct GeorgeKeyState {
    float repeat_timer;
    u8 selected;
    u8 keys[256];
    u8 untouched_padding[3];
} GeorgeKeyState;

#define INPUT_OFFSET(type, member, offset) \
    typedef char input_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
INPUT_OFFSET(GeorgeInputState, flags, 4);
INPUT_OFFSET(GeorgeInputState, previous, 0x14);
INPUT_OFFSET(GeorgeInputState, pressure, 0x2C);
INPUT_OFFSET(GeorgeInputState, axes, 0x5C);
INPUT_OFFSET(GeorgeInputState, byte_pressure, 0x8C);
INPUT_OFFSET(GeorgeInputState, history_index, 0x90);
INPUT_OFFSET(GeorgeInputState, history, 0x94);
INPUT_OFFSET(GeorgeInputState, output_axes, 0xAC);
INPUT_OFFSET(GeorgeInputState, previous_output_axes, 0xBC);
INPUT_OFFSET(GeorgeInputState, lock_timer, 0xCC);
INPUT_OFFSET(GeorgeInputState, lock_value, 0xD0);
INPUT_OFFSET(GeorgeInputState, reservedD4, 0xD4);
INPUT_OFFSET(GeorgeInputState, threshold, 0xD8);
INPUT_OFFSET(GeorgeKeyState, selected, 4);
INPUT_OFFSET(GeorgeKeyState, keys, 5);
typedef char input_state_size[(sizeof(GeorgeInputState) == 0xDC) ? 1 : -1];
typedef char input_key_state_size[(sizeof(GeorgeKeyState) == 0x108) ? 1 : -1];
#undef INPUT_OFFSET

void func_002AC1C0(GeorgeInputState *state, s32 mode, u32 device);
void func_002AC310(GeorgeInputState *state, s32 mode, const float *axes,
    const u8 *byte_pressure, u32 buttons, const float *pressure,
    float elapsed, float axis0, float axis1, float axis2, float axis3) GEORGE_SAVE128;
GeorgeInputState *func_002AC628(s32 mode, u32 device) GEORGE_SAVE128;
void func_002AC6B8(GeorgeInputState *state, float timer, float value);
void func_002AC6C8(GeorgeInputState *state);
void func_002AC6E8(GeorgeInputState *state);
GeorgeKeyState *func_002AC700(void) GEORGE_SAVE128;
void func_002AC778(GeorgeKeyState *state, float elapsed);

/* Destructors2AC698/2AC758 are the existing reviewed duplicate_helpers free
 * wrappers and intentionally have no second implementation in this batch. */

#endif
