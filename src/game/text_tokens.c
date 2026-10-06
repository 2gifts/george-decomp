#include "george/text_tokens.h"
#include "george/string_algorithms.h"

/* Inspected original bounded comparison: unsigned byte difference, zero on
 * equal prefixes or an equal NUL, and no character loads for count zero. */
extern s32 func_00393E48(const signed char *left, const signed char *right, u32 count);

void func_002B3190(GeorgeTextTokenContext *context, signed char **input,
                   signed char **output, s32 mode, s32 keep_quotes)
{
    signed char *destination = *output;
    signed char *source = *input;
    signed char quotes[3];
    const signed char **delimiters;
    s32 quote = 0;
    signed char character;
    u32 index;

    quotes[0] = D_004476C8[0];
    quotes[1] = D_004476C8[1];
    quotes[2] = D_004476C8[2];
    character = *source;
    while (character == ' ' || character == '\t' || character == '\n' || character == '\r') {
        if ((u32)source >= (u32)context->field08 + context->field10)
            break;
        ++source;
        if (character == '\n')
            context->field18 += 1u;
        character = *source;
    }

    if (mode == 1)
        delimiters = D_003FD270;
    else if (mode == 2)
        delimiters = D_003FD288;
    else
        delimiters = D_003FD258;

    while (*source != 0) {
        for (index = 0; index < 5; ++index) {
            const signed char *delimiter = delimiters[index];
            if (delimiter != NULL) {
                u32 length = func_00295050(delimiter);
                /* The pointer cell is read again after the length call. */
                if (func_00393E48(source, delimiters[index], length) == 0)
                    break;
            }
        }
        if (index != 5)
            break;

        character = *source;
        if (quote != 0) {
            while (character != 0 && character != quotes[quote]) {
                if (character == '\n') {
                    ++source;
                    context->field18 += 1u;
                } else if (character == '\r') {
                    ++source;
                } else {
                    *destination = character;
                    ++source;
                    ++destination;
                }
                character = *source;
            }
            if (keep_quotes != 0) {
                *destination = quotes[quote];
                ++destination;
            }
            /* Retail advances here even if the quote ended at a NUL. */
            ++source;
            quote = 0;
        } else if (character == '\'' || character == '"') {
            quote = character == '\'' ? 1 : 2;
            if (keep_quotes != 0) {
                *destination = quotes[quote];
                ++destination;
            }
            ++source;
        } else if (character == '\n') {
            ++source;
            context->field18 += 1u;
        } else if (character == '\r') {
            ++source;
        } else {
            *destination = character;
            ++source;
            ++destination;
        }
    }
    *destination = 0;
    *input = source;
    *output = destination;
}

signed char *func_002B3EE0(signed char *input, signed char *name, signed char *value)
{
    signed char quotes[3];
    signed char character;
    s32 quote = 0;

    *name = 0;
    *value = 0;
    character = *input;
    while (character == ' ' || character == '\t' || character == '\r' || character == '\n') {
        ++input;
        character = *input;
    }
    if (character == 0)
        return NULL;

    while (character != 0 && character != '=' && character != ' ' &&
           character != '\t' && character != '\r' && character != '\n') {
        *name = character;
        ++input;
        ++name;
        character = *input;
    }
    *name = 0;
    /* This fresh load observes an input/name terminator alias. */
    character = *input;
    if (character == 0)
        return NULL;

    quotes[0] = D_004476C8[0];
    quotes[1] = D_004476C8[1];
    quotes[2] = D_004476C8[2];
    while (character == '=' || character == ' ' || character == '\t' ||
           character == '\r' || character == '\n' ||
           ((character == '\'' || character == '"') && quote == 0)) {
        ++input;
        if (character == 0)
            return NULL;
        if (character == '\'')
            quote = 1;
        if (character == '"')
            quote = 2;
        character = *input;
    }
    if (character == 0)
        return NULL;

    for (;;) {
        character = *input;
        if (quote == 0 && (character == ' ' || character == '\t' ||
                           character == '\r' || character == '\n'))
            break;
        if (character == quotes[quote] || character == 0)
            break;
        *value = character;
        ++input;
        ++value;
    }
    *value = 0;
    /* The terminator write can change this final source-byte load as well. */
    character = *input;
    if (character == '\'' || character == '"')
        ++input;
    return input;
}
