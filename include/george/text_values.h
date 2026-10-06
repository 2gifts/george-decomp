#ifndef GEORGE_TEXT_VALUES_H
#define GEORGE_TEXT_VALUES_H

#include "george/text_lookup.h"

extern const signed char D_004476D0[];
extern const signed char D_004476D8[];
extern const signed char D_004476E0[];
extern const signed char D_004476E8[];
extern const signed char D_00447730[];

s32 func_002B4188(GeorgeTextLookupContext *context, const signed char *key,
                  u32 *output) GEORGE_SAVE128;
s32 func_002B4310(GeorgeTextLookupContext *context, const signed char *key,
                  float *output) GEORGE_SAVE128;
void func_002B4538(GeorgeTextLookupContext *context) GEORGE_SAVE128;
s32 func_002B4700(GeorgeTextLookupContext *context, const signed char *key,
                  signed char *output, s32 lowercase) GEORGE_SAVE128;

#endif
