#include "george/types.h"
#include "george/function_templates.h"

/*
 * Reconstructed from SLUS_216.68. See config/recovered_functions.json for status.
 * Addresses and field offsets are evidence; gameplay names are provisional.
 * Floating expressions must be compiled without fast-math or contraction.
 * Host IEEE floating-point is not a bit-exact model of PS2 exceptional values.
 */

/* Matched. 0x001100D0, 0x10 bytes: set bit 0 of the word at +0x18. */
GEORGE_DEFINE_SET_BIT0(func_001100D0)

/* Matched. 0x001100E0, 0x14 bytes: clear bit 0, preserving every other bit. */
GEORGE_DEFINE_CLEAR_BIT0(func_001100E0)

/* 0x001102F8, 0x34 bytes: adjust two flags, write low 3 bits, advance by 24. */
u8 *func_001102F8(GeorgeFlags18 *object, u32 *output)
{
    u32 flags = object->field18;

    if ((flags & 1) != 0 && (flags & 4) != 0) {
        object->field18 = flags & ~1u;
        flags = object->field18;
    }
    *output = flags & 7;
    return (u8 *)output + 0x18;
}

/* Matched. 0x00111FA0, 0x18 bytes: conditional scalar, otherwise positive zero. */
float func_00111FA0(const GeorgeScalar44 *object)
{
    if (object->field3C != 0) {
        return object->field44;
    }
    return 0.0f;
}

/* Matched. 0x001134E8, 0x30 bytes: copy fields, then signed-int reciprocal. */
const GeorgeRateInput *func_001134E8(GeorgeRate48 *object, const GeorgeRateInput *input)
{
    float reciprocal = 1.0f;

    /* The PS2 compiler narrows to the low 16 bits in two's-complement form. */
    object->field40 = (s16)input->field00;
    object->field44 = input->field08;
    reciprocal /= (float)input->field04;
    object->field48 = reciprocal;
    return input;
}

/* Matched. 0x00113518, 0x20 bytes: signed halfword expansion and two word copies. */
GeorgeRateOutput *func_00113518(const GeorgeRate48 *object, GeorgeRateOutput *output)
{
    output->field00 = object->field40;
    output->field04 = object->field30;
    output->field08 = object->field44;
    return output;
}

/* 0x0011C078, 0x54 bytes: normalized scalar selected by two word flags. */
float func_0011C078(const GeorgeTimer50 *object)
{
    if (object->field30 != 0) {
        float ratio = object->field34 / object->field50;
        return 1.0f - ratio;
    }
    if (object->field2C == 0) {
        return 1.0f;
    }
    return object->field34 / object->field50;
}

/* Matched with scheduling disabled. 0x0011C108, 0x14 bytes: reset four fields. */
void func_0011C108(GeorgeTimer50 *object)
{
    object->field34 = 0.0f;
    object->field28 = 0;
    object->field30 = 0;
    object->field2C = 0;
}

/* 0x0011C120, 0x3C bytes: select state and complement a scalar in its range. */
void func_0011C120(GeorgeTimer50 *object)
{
    float current = object->field34;
    object->field2C = 1;
    object->field30 = 0;
    if (current == 0.0f) {
        object->field34 = object->field50;
    } else {
        object->field34 = object->field50 - current;
    }
}

/* 0x0011C160, 0x5C bytes: decrement while enabled and handle a negative result. */
void func_0011C160(GeorgeTimer50 *object, float step)
{
    u32 state;
    float current;

    if (object->field28 == 0) {
        return;
    }
    state = object->field2C;
    if (state == 0 && object->field30 == 0) {
        return;
    }
    current = object->field34 - step;
    object->field34 = current;
    /* The original tests strictly below zero: exactly zero stays enabled. */
    if (current < 0.0f) {
        if (state != 0) {
            object->field28 = 0;
        }
        object->field34 = 0.0f;
        object->field30 = 0;
        object->field2C = 0;
    }
}

/* 0x0011F0E8, 0x40 bytes: normalize a word to a halfword and two endpoints. */
const u32 *func_0011F0E8(GeorgeBlend20 *object, const u32 *input)
{
    if (*input != 0) {
        object->field18 = 1;
        object->field20 = 1.0f;
        object->field1C = 0.0f;
    } else {
        object->field20 = 0.0f;
        object->field1C = 1.0f;
        object->field18 = 0;
    }
    return input + 1;
}

/* Matched. 0x0011FE00, 0x44 bytes: independent comparisons; second takes priority. */
void func_0011FE00(GeorgeFade4C *object, u32 value)
{
    if (value == object->field40) {
        object->field18 = 1;
    }
    if (value == object->field44) {
        object->field18 = 0;
    }
    if (value == object->field48) {
        object->field1C = 1.0f;
    }
}

/* Matched. 0x0011FFB0, 0x40 bytes: subtract a scaled step from a positive scalar. */
/* The shared body preserves the original possible negative overshoot. */
GEORGE_DEFINE_FADE_UPDATE(func_0011FFB0)
