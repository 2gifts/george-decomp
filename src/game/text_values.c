#include "george/text_values.h"
#include "george/text_lookup_template.h"

/* Original two-GPR parser call; its return is unused by the depth helper. */
extern void func_002B3460(GeorgeTextLookupContext *context, const void *table);
extern s32 func_00395350(const signed char *text, const signed char *format, ...);
extern signed char *func_003984D8(signed char *text);

s32 func_002B4188(GeorgeTextLookupContext *context, const signed char *key, u32 *output)
{
    signed char text[0x400];
    s32 found;
    *output = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text,
                           found = 1; goto lookup_done,
                           found = 0; goto lookup_done);
lookup_done:
    if (found != 0) {
        /* All four calls occur, even after one comparison set the output. */
        if (func_003983E8(D_004476D0, text) == 0)
            *output = 1;
        if (func_003983E8(D_004476D8, text) == 0)
            *output = 1;
        if (func_003983E8(D_004476E0, text) == 0)
            *output = 1;
        if (func_003983E8(D_004476E8, text) == 0)
            *output = 1;
    }
    return found;
}

s32 func_002B4310(GeorgeTextLookupContext *context, const signed char *key, float *output)
{
    signed char text[0x400];
    float parsed[3];
    float first, second, third;
    s32 found;
    ((u32 *)output)[0] = 0;
    ((u32 *)output)[1] = 0;
    ((u32 *)output)[2] = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text,
                           found = 1; goto lookup_done,
                           found = 0; goto lookup_done);
lookup_done:
    if (found != 0) {
        /* Retail ignores conversion count. Unwritten lanes remain original
         * indeterminate stack values on partial/failed conversion; no zero
         * initialization or deterministic value is invented for that path. */
        func_00395350(text, D_00447730, &parsed[0], &parsed[1], &parsed[2]);
        first = parsed[0];
        second = parsed[1];
        third = parsed[2];
        output[0] = first;
        output[2] = third;
        output[1] = second;
    }
    return found;
}

void func_002B4538(GeorgeTextLookupContext *context)
{
    u32 remaining = context->field101C - context->field1020;
    while ((s32)remaining > 0 && context->field102C == 0) {
        func_002B3460(context, NULL);
        remaining += context->field101C - context->field1020;
    }
}

s32 func_002B4700(GeorgeTextLookupContext *context, const signed char *key,
                  signed char *output, s32 lowercase)
{
    signed char text[0x400];
    s32 found;
    *output = 0;
    GEORGE_TEXT_LOOKUP_COPY(context, key, text,
                           found = 1; goto lookup_done,
                           found = 0; goto lookup_done);
lookup_done:
    if (found != 0) {
        if (lowercase != 0)
            func_003984D8(text);
        func_00393B74(output, text);
    }
    return found;
}
