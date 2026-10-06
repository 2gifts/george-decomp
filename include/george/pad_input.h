#ifndef GEORGE_PAD_INPUT_H
#define GEORGE_PAD_INPUT_H

#include "george/input_state.h"
#include "george/ee_math.h"

/* Only the connection-state word is used by this caller. The pad SDK wrapper
 * owns the other bytes. Four records occupy the observed 0x180-byte stride. */
typedef struct GeorgePadDevice {
    u8 unknown00[0x11C];
    s32 connection_state;
    u8 unknown120[0x60];
} GeorgePadDevice;
typedef char pad_device_state_offset[(offsetof(GeorgePadDevice, connection_state) == 0x11C) ? 1 : -1];
typedef char pad_device_stride[(sizeof(GeorgePadDevice) == 0x180) ? 1 : -1];

extern GeorgeInputState *D_00468388[4];
extern GeorgeInputState *D_00468398, *D_0046839C, *D_004683A0, *D_004683A4;
extern GeorgeInputState *D_004683A8;
extern GeorgeKeyState *D_004683AC;
extern GeorgePadDevice D_00468400[4];

void func_00295080(void) GEORGE_SAVE128;
void func_00295170(float elapsed) GEORGE_SAVE128;
float func_00295B18(float value);
void func_00295BD0(float *output_x, float *output_y, float *magnitude,
    float *angle, float x, float y, float threshold) GEORGE_SAVE128;

/* Exact original entry points and mixed-register ABI, retained until their
 * implementations are independently recovered. Delta output is twelve s16s. */
s32 func_002B2800(GeorgePadDevice *pad, u32 *buttons, signed char *left_x, signed char *left_y,
    signed char *right_x, signed char *right_y, u8 *pressure, s16 *pressure_delta);
void func_002B2ED8(GeorgePadDevice *pad, u16 *left_x, u16 *left_y,
    u16 *right_x, u16 *right_y);
void func_002B2D60(GeorgePadDevice *pad, s32 small_motor, s32 large_motor);
s32 func_002B2DA8(GeorgePadDevice *pad, s32 port, s32 slot, s32 word);
float func_0029B940(float y, float x);
float func_0037B238(float numerator, float denominator);
void *func_003936A0(void *destination, s32 value, u32 size);

#endif
