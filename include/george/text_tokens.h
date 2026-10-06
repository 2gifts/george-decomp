#ifndef GEORGE_TEXT_TOKENS_H
#define GEORGE_TEXT_TOKENS_H

#include "george/compiler.h"
#include "george/types.h"

/* Observed prefix only; the larger adjacent parser is not reconstructed here. */
typedef struct GeorgeTextTokenContext {
    u8 unknown00[8];
    const signed char *field08;
    u8 unknown0C[4];
    u32 field10;
    u8 unknown14[4];
    u32 field18;
} GeorgeTextTokenContext;

typedef char text_token_base_offset[(offsetof(GeorgeTextTokenContext, field08) == 8) ? 1 : -1];
typedef char text_token_length_offset[(offsetof(GeorgeTextTokenContext, field10) == 0x10) ? 1 : -1];
typedef char text_token_line_offset[(offsetof(GeorgeTextTokenContext, field18) == 0x18) ? 1 : -1];

/* Only the five pointer cells actually read by each helper are declared. */
extern const signed char *D_003FD258[5];
extern const signed char *D_003FD270[5];
extern const signed char *D_003FD288[5];
extern const signed char D_004476C8[3];

void func_002B3190(GeorgeTextTokenContext *context, signed char **input,
                   signed char **output, s32 mode, s32 keep_quotes) GEORGE_SAVE128;
signed char *func_002B3EE0(signed char *input, signed char *name,
                          signed char *value) GEORGE_SAVE128;

#endif
