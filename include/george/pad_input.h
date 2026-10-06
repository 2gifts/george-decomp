#ifndef GEORGE_PAD_INPUT_H
#define GEORGE_PAD_INPUT_H

#include "george/input_state.h"
#include "george/pad_device.h"
#include "george/ee_math.h"

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

/* Original custom angle and runtime entries. */
float func_0029B940(float y, float x);
float func_0037B238(float numerator, float denominator);
void *func_003936A0(void *destination, s32 value, u32 size);

#endif
