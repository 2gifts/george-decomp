#include "george/pad_device.h"

s32 func_002B2DA8(GeorgePadDevice *pad, s32 port, s32 slot, s32 word)
{
    s32 index, initialized = 0;
    for (index = 0; index < 6; ++index) {
        pad->motors[index] = 0;
        pad->actuators[index] = 0xFF;
    }
    for (index = 11; index >= 0; --index) pad->pressure[index] = 0;
    pad->actuators[0] = 0;
    pad->actuators[1] = 1;
    pad->port = port;
    pad->slot = slot;
    pad->connection_state = 0;
    pad->raw_buttons = 0;
    pad->updates = 0;
    pad->packet_type = 0;
    if (D_003FD248 == 0) {
        initialized = func_00389D98(0);
        D_003FD248 = 1;
    }
    pad->axes[0] = 0;
    pad->axes[1] = 0;
    pad->axes[2] = 0;
    pad->axes[3] = 0;
    pad->axis_delta[0] = 0;
    pad->axis_delta[1] = 0;
    pad->axis_delta[2] = 0;
    pad->axis_delta[3] = 0;
    func_0038A1F0(port, slot, pad);
    pad->field128 = (u32)word;
    pad->buttons = 0;
    pad->accumulated_buttons = 0;
    pad->toggled_buttons = 0;
    pad->previous_buttons = 0;
    return initialized;
}

void func_002B2D60(GeorgePadDevice *pad, s32 small_motor, s32 large_motor)
{
    if (pad->kind >= 2) {
        pad->motors[0] = small_motor > 0;
        pad->motors[1] = (u8)large_motor;
        func_0038ACD0(pad->port, pad->slot, pad->motors);
    }
}

void func_002B2ED8(GeorgePadDevice *pad, u16 *left_x, u16 *left_y,
    u16 *right_x, u16 *right_y)
{
    if (left_x) *left_x = (u16)pad->axis_delta[0];
    if (left_y) *left_y = (u16)pad->axis_delta[1];
    if (right_x) *right_x = (u16)pad->axis_delta[2];
    if (right_y) *right_y = (u16)pad->axis_delta[3];
}

s32 func_002B2800(GeorgePadDevice *pad, u32 *buttons, signed char *left_x,
    signed char *left_y, signed char *right_x, signed char *right_y,
    u8 *pressure, s16 *pressure_delta)
{
    /* Retail leaves this packet uninitialized when kind <=0 and SDK state is
     * 2 or6. Preserve that first-use contract instead of fabricating zeroes. */
    u8 packet[32] __attribute__((aligned(16)));
    s32 status, mode, extended, request, index;
    s32 raw_axes[4];
    raw_axes[0] = 0;
    raw_axes[1] = 0;
    raw_axes[2] = 0;
    raw_axes[3] = 0;
    pad->updates += 1U;
    status = func_0038A6B8(pad->port, pad->slot);
    if (status == 0) {
        pad->connection_state = 0;
        pad->kind = 0;
    }
    /* The original 78-word table selects only these nine case bodies. All
     * other signed states take the shared default, including negative values. */
    switch (pad->connection_state) {
    case 0:
        if (status == 6 || status == 2) {
            mode = func_0038AAE0(pad->port, pad->slot, 1, 0);
            if (mode != 0) {
                extended = func_0038AAE0(pad->port, pad->slot, 2, 0);
                if (extended >= 1) mode = extended;
                if (mode == 4 || (mode == 7 && pad->kind == 0)) {
                    pad->kind = 1;
                    pad->connection_state = 40;
                } else if (mode == 7)
                    pad->connection_state = 70;
                else
                    pad->connection_state = 99;
            }
        }
        break;
    case 40:
        if (func_0038AAE0(pad->port, pad->slot, 2, 0) == 0) {
            pad->connection_state = 99;
            break;
        }
        pad->connection_state = (s32)((u32)pad->connection_state + 1U);
        /* Fall through to the same main-mode call as original case41. */
    case 41:
        if (func_0038AC18(pad->port, pad->slot, 1, 3) == 1)
            pad->connection_state = (s32)((u32)pad->connection_state + 1U);
        break;
    case 42:
        request = func_0038A820(pad->port, pad->slot);
        if (request == 1)
            pad->connection_state = (s32)((u32)pad->connection_state - 1U);
        if (func_0038A820(pad->port, pad->slot) == 0) {
            pad->connection_state = 0;
            pad->kind = 2;
        }
        break;
    case 70:
        if (func_0038A8B0(pad->port, pad->slot, -1, 0) == 0)
            pad->connection_state = 99;
        else if (func_0038AD98(pad->port, pad->slot, pad->actuators) != 0) {
            pad->connection_state = (s32)((u32)pad->connection_state + 1U);
            func_0038A820(pad->port, pad->slot);
        }
        break;
    case 71:
        request = func_0038A820(pad->port, pad->slot);
        if (request == 1)
            pad->connection_state = (s32)((u32)pad->connection_state - 1U);
        if (func_0038A820(pad->port, pad->slot) == 0)
            pad->connection_state = (s32)((u32)pad->connection_state + 1U);
        break;
    case 72:
        pad->connection_state = func_0038AFD0(pad->port, pad->slot) == 1 ? 76 : 99;
        break;
    case 76:
        if (func_0038B050(pad->port, pad->slot) == 1)
            pad->connection_state = (s32)((u32)pad->connection_state + 1U);
        break;
    case 77:
        request = func_0038A820(pad->port, pad->slot);
        if (request == 1)
            pad->connection_state = (s32)((u32)pad->connection_state - 1U);
        if (func_0038A820(pad->port, pad->slot) == 0) {
            pad->connection_state = 99;
            pad->kind = 3;
        }
        break;
    }
    if (status != 6 && status != 2) {
        if (buttons) *buttons = pad->buttons;
        if (left_x) *left_x = 0;
        if (left_y) *left_y = 0;
        if (right_x) *right_x = 0;
        if (right_y) *right_y = 0;
        return 0;
    }
    if (pad->kind > 0) {
        u32 old_buttons, accumulated, current, toggled;
        pad->previous_buttons = pad->buttons;
        if (func_0038A640(pad->port, pad->slot, packet) == 0) return 0;
        old_buttons = pad->raw_buttons;
        accumulated = pad->accumulated_buttons;
        current = (~(((u32)packet[2] << 8) | packet[3])) & 0xFFFFU;
        pad->raw_buttons = (u16)current;
        toggled = pad->toggled_buttons;
        pad->toggled_buttons = (current & ~old_buttons) ^ toggled;
        pad->accumulated_buttons = accumulated | pad->raw_buttons;
        pad->buttons = pad->raw_buttons;
    }
    if (pad->kind >= 2) {
        raw_axes[0] = (s32)packet[6] - 128;
        raw_axes[1] = (s32)packet[7] - 128;
        raw_axes[2] = (s32)packet[4] - 128;
        raw_axes[3] = (s32)packet[5] - 128;
        for (index = 0; index < 4; ++index)
            pad->axis_delta[index] = (s16)(raw_axes[index] - (s32)pad->axes[index]);
        for (index = 0; index < 4; ++index)
            pad->axes[index] = (signed char)raw_axes[index];
    }
    if (buttons) *buttons = pad->buttons;
    if (left_x) *left_x = (signed char)raw_axes[0];
    if (left_y) *left_y = (signed char)raw_axes[1];
    if (right_x) *right_x = (signed char)raw_axes[2];
    if (right_y) *right_y = (signed char)raw_axes[3];
    if (packet[0] == 0 && pad->kind == 3) {
        for (index = 0; index < 12; ++index) {
            if (pressure_delta)
                pressure_delta[index] = (s16)((s32)packet[index + 8] - (s32)pad->pressure[index]);
            if (pressure) pressure[index] = packet[index + 8];
            pad->pressure[index] = packet[index + 8];
        }
    }
    pad->packet_type = packet[1];
    return pad->kind;
}
