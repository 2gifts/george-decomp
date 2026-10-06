#include "george/text_content.h"
#include "george/text_numbers.h"
#include "george/text_values.h"

extern s32 func_00395350(const signed char *text, const signed char *format, ...);
extern s32 func_003983E8(const signed char *left, const signed char *right);
extern signed char *func_00393B74(signed char *destination, const signed char *source);

s32 func_002B5720(const GeorgeTextLookupContext *context,
                  float *first, float *second, float *third, float *fourth)
{
    return func_00395350(context->field3030, D_00447740, first, second, third, fourth);
}

s32 func_002B5760(const GeorgeTextLookupContext *context,
                  float *first, float *second, float *third)
{
    return func_00395350(context->field3030, D_00447730, first, second, third);
}

s32 func_002B5798(const GeorgeTextLookupContext *context, float *first, float *second)
{
    return func_00395350(context->field3030, D_00447728, first, second);
}

s32 func_002B57C8(const GeorgeTextLookupContext *context, float *first)
{
    return func_00395350(context->field3030, D_00447720, first);
}

s32 func_002B57F0(const GeorgeTextLookupContext *context,
                  s32 *first, s32 *second, s32 *third)
{
    return func_00395350(context->field3030, D_00447700, first, second, third);
}

s32 func_002B5828(const GeorgeTextLookupContext *context, s32 *first, s32 *second)
{
    return func_00395350(context->field3030, D_004476F8, first, second);
}

s32 func_002B5858(const GeorgeTextLookupContext *context, s32 *first)
{
    return func_00395350(context->field3030, D_004476F0, first);
}

signed char *func_002B5880(const GeorgeTextLookupContext *context, signed char *output)
{
    return func_00393B74(output, context->field3030);
}

void func_002B58A8(const GeorgeTextLookupContext *context, u32 *output)
{
    const signed char *text = context->field3030;
    *output = 0;
    if (func_003983E8(D_004476D0, text) == 0 ||
        func_003983E8(D_004476D8, text) == 0 ||
        func_003983E8(D_004476E0, text) == 0 ||
        func_003983E8(D_004476E8, text) == 0)
        *output = 1;
}

s32 func_002B5938(const GeorgeTextLookupContext *context)
{
    const signed char *text = context->field3030;
    return func_003983E8(D_004476D0, text) == 0 ||
           func_003983E8(D_004476D8, text) == 0 ||
           func_003983E8(D_004476E0, text) == 0 ||
           func_003983E8(D_004476E8, text) == 0;
}

void func_002B59B0(GeorgeTextLookupContext *context)
{
    const signed char *input = context->field08;
    context->field3030[0] = 0;
    context->field0C = input;
    context->field14 = 0x8001DEADu;
    context->field00 = 0;
    context->field18 = 0;
    context->field1C[0] = 0;
    context->field101C = 0;
    context->field1020 = 0;
    context->field1024 = 0;
    context->field1028 = 0;
    context->field102C = 0;
    context->field1030[0] = 0;
    context->field2030[0] = 0;
}
