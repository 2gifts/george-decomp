#ifndef GEORGE_TEXT_CONTENT_H
#define GEORGE_TEXT_CONTENT_H

#include "george/text_lookup.h"

/* Scanner/copy results are the known original callee returns preserved at
 * exit. No direct callers establish unused source-level return declarations. */
s32 func_002B5720(const GeorgeTextLookupContext *, float *, float *, float *, float *);
s32 func_002B5760(const GeorgeTextLookupContext *, float *, float *, float *);
s32 func_002B5798(const GeorgeTextLookupContext *, float *, float *);
s32 func_002B57C8(const GeorgeTextLookupContext *, float *);
s32 func_002B57F0(const GeorgeTextLookupContext *, s32 *, s32 *, s32 *);
s32 func_002B5828(const GeorgeTextLookupContext *, s32 *, s32 *);
s32 func_002B5858(const GeorgeTextLookupContext *, s32 *);
signed char *func_002B5880(const GeorgeTextLookupContext *, signed char *);
void func_002B58A8(const GeorgeTextLookupContext *, u32 *) GEORGE_SAVE128;
s32 func_002B5938(const GeorgeTextLookupContext *) GEORGE_SAVE128;
void func_002B59B0(GeorgeTextLookupContext *);

#endif
