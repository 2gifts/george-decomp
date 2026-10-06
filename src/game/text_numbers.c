#include "george/text_numbers.h"
#include "george/text_lookup_template.h"

extern s32 func_00395350(const signed char *text, const signed char *format, ...);

s32 func_002B4840(GeorgeTextLookupContext *context, const signed char *key, s32 *first)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_004476F0, first);
    return found;
}

s32 func_002B4960(GeorgeTextLookupContext *context, const signed char *key, s32 *first, s32 *second)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_004476F8, first, second);
    return found;
}

s32 func_002B4A90(GeorgeTextLookupContext *context, const signed char *key,
                  s32 *first, s32 *second, s32 *third)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447700, first, second, third);
    return found;
}

s32 func_002B4BD0(GeorgeTextLookupContext *context, const signed char *key,
                  s32 *first, s32 *second, s32 *third, s32 *fourth)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447710, first, second, third, fourth);
    return found;
}

s32 func_002B4D20(GeorgeTextLookupContext *context, const signed char *key, float *first)
{
    signed char text[0x400];
    s32 found;
    *(u32 *)first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447720, first);
    return found;
}

s32 func_002B4E40(GeorgeTextLookupContext *context, const signed char *key, float *first, float *second)
{
    signed char text[0x400];
    s32 found;
    *(u32 *)first = 0;
    *(u32 *)second = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447728, first, second);
    return found;
}

s32 func_002B4F78(GeorgeTextLookupContext *context, const signed char *key,
                  float *first, float *second, float *third)
{
    signed char text[0x400];
    s32 found;
    *(u32 *)first = 0;
    *(u32 *)second = 0;
    *(u32 *)third = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447730, first, second, third);
    return found;
}

s32 func_002B50C0(GeorgeTextLookupContext *context, const signed char *key,
                  float *first, float *second, float *third, float *fourth)
{
    signed char text[0x400];
    s32 found;
    *(u32 *)first = 0;
    *(u32 *)second = 0;
    *(u32 *)third = 0;
    *(u32 *)fourth = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447740, first, second, third, fourth);
    return found;
}

s32 func_002B5220(GeorgeTextLookupContext *context, const signed char *key, s16 *first)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447750, first);
    return found;
}

s32 func_002B5340(GeorgeTextLookupContext *context, const signed char *key, s16 *first, s16 *second)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    *second = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447758, first, second);
    return found;
}

s32 func_002B5478(GeorgeTextLookupContext *context, const signed char *key,
                  s16 *first, s16 *second, s16 *third)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    *second = 0;
    *third = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447760, first, second, third);
    return found;
}

s32 func_002B55C0(GeorgeTextLookupContext *context, const signed char *key,
                  s16 *first, s16 *second, s16 *third, s16 *fourth)
{
    signed char text[0x400];
    s32 found;
    *first = 0;
    *second = 0;
    *third = 0;
    *fourth = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text, found = 1; goto lookup_done, found = 0; goto lookup_done);
lookup_done:
    if (found != 0) func_00395350(text, D_00447770, first, second, third, fourth);
    return found;
}
