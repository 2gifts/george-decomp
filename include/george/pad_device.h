#ifndef GEORGE_PAD_DEVICE_H
#define GEORGE_PAD_DEVICE_H

#include "george/compiler.h"
#include "george/types.h"

/* SDK-owned DMA prefix followed by the engine wrapper's observed fields. */
typedef struct GeorgePadDevice {
    u8 dma[0x100];
    u32 buttons, accumulated_buttons, toggled_buttons, previous_buttons;
    s32 port, slot;
    u16 raw_buttons;
    u8 untouched11A[2];
    s32 connection_state;
    u32 packet_type;
    s32 kind;
    u32 field128, updates;
    signed char axes[4];
    s16 axis_delta[4];
    u8 pressure[12], motors[6], actuators[6];
    u8 untouched154[0x2C];
} GeorgePadDevice;

#define PAD_OFFSET(member, offset) \
    typedef char pad_device_offset_##member[(offsetof(GeorgePadDevice, member) == (offset)) ? 1 : -1]
PAD_OFFSET(buttons, 0x100);
PAD_OFFSET(port, 0x110);
PAD_OFFSET(raw_buttons, 0x118);
PAD_OFFSET(connection_state, 0x11C);
PAD_OFFSET(packet_type, 0x120);
PAD_OFFSET(kind, 0x124);
PAD_OFFSET(field128, 0x128);
PAD_OFFSET(updates, 0x12C);
PAD_OFFSET(axes, 0x130);
PAD_OFFSET(axis_delta, 0x134);
PAD_OFFSET(pressure, 0x13C);
PAD_OFFSET(motors, 0x148);
PAD_OFFSET(actuators, 0x14E);
typedef char pad_device_stride[(sizeof(GeorgePadDevice) == 0x180) ? 1 : -1];
#undef PAD_OFFSET

extern u32 D_003FD248;

s32 func_002B2800(GeorgePadDevice *pad, u32 *buttons, signed char *left_x,
    signed char *left_y, signed char *right_x, signed char *right_y,
    u8 *pressure, s16 *pressure_delta) GEORGE_SAVE128;
void func_002B2D60(GeorgePadDevice *pad, s32 small_motor, s32 large_motor);
s32 func_002B2DA8(GeorgePadDevice *pad, s32 port, s32 slot, s32 word) GEORGE_SAVE128;
void func_002B2ED8(GeorgePadDevice *pad, u16 *left_x, u16 *left_y,
    u16 *right_x, u16 *right_y);

/* Observed SDK entry ABIs. Their implementations are retained by address. */
s32 func_0038A6B8(s32 port, s32 slot);
s32 func_0038AAE0(s32 port, s32 slot, s32 mode, s32 index);
s32 func_0038AC18(s32 port, s32 slot, s32 mode, s32 lock);
s32 func_0038A820(s32 port, s32 slot);
s32 func_0038A8B0(s32 port, s32 slot, s32 actuator, s32 term);
s32 func_0038AD98(s32 port, s32 slot, u8 *actuators);
s32 func_0038AFD0(s32 port, s32 slot);
s32 func_0038B050(s32 port, s32 slot);
s32 func_0038A640(s32 port, s32 slot, u8 *packet);
s32 func_0038ACD0(s32 port, s32 slot, u8 *motors);
s32 func_00389D98(s32 mode);
s32 func_0038A1F0(s32 port, s32 slot, void *buffer);

#endif
