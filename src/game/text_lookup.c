#include "george/text_lookup.h"
#include "george/text_lookup_template.h"

/* Independently proven unchanged newlib 1.8.1 table in runtime/ctype_table.c.
 * Preserve the observed signed-byte + 1 index, including its valid-index
 * precondition; converting the character to unsigned would change addresses. */
extern const char _ctype_[257];

/* Complete original call bodies establish case-insensitive compare and string
 * copy ABIs. Keep original numeric bindings; no implementation is imported. */
extern s32 func_003983E8(const signed char *left, const signed char *right);
extern signed char *func_00393B74(signed char *destination, const signed char *source);

s32 func_002B4458(GeorgeTextLookupContext *context, const signed char *input,
                  u32 length, float value)
{
    context->field04 = value;
    context->field0C = input;
    context->field10 = length;
    context->field14 = 0x8001DEADu;
    context->field3030[0] = 0;
    context->field00 = 0;
    context->field08 = input;
    context->field18 = 0;
    context->field1C[0] = 0;
    context->field101C = 0;
    context->field1020 = 0;
    context->field1024 = 0;
    context->field1028 = 0;
    context->field102C = 0;
    context->field1030[0] = 0;
    context->field2030[0] = 0;
    return 1;
}

void func_002B44A8(GeorgeTextLookupContext *context, const GeorgeTextTagEntry *entries)
{
    context->field14 = 0x8001DEADu;
    if (entries == NULL)
        return;
    for (;;) {
        u32 code = entries->code;
        if (code == 0x8000DEADu)
            return;
        if (code == 0x8002DEADu) {
            context->field14 = code;
            return;
        }
        if (func_003983E8(context->field1030, entries->text) == 0) {
            /* The original reloads this word after the comparison call. */
            context->field14 = entries->code;
            return;
        }
        ++entries;
    }
}

void func_002B45A0(signed char *text)
{
    signed char character = *text;
    while (character != 0) {
        s32 code = character;
        if (((u8)_ctype_[code + 1] & 1u) != 0)
            character = (signed char)(code + 0x20);
        *text = character;
        ++text;
        character = *text;
    }
}

s32 func_002B45F0(GeorgeTextLookupContext *context, const signed char *key,
                  signed char *output)
{
    GEORGE_TEXT_LOOKUP_COPY(context, key, output, return 1, return 0);
}
