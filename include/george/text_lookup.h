#ifndef GEORGE_TEXT_LOOKUP_H
#define GEORGE_TEXT_LOOKUP_H

#include "george/text_tokens.h"

/* Observed storage extent; this does not assert a complete engine class. */
typedef struct GeorgeTextLookupContext {
    u32 field00;
    float field04;
    const signed char *field08;
    const signed char *field0C;
    u32 field10;
    u32 field14;
    u32 field18;
    signed char field1C[0x1000];
    u32 field101C;
    u32 field1020;
    u32 field1024;
    u32 field1028;
    u32 field102C;
    signed char field1030[0x1000];
    signed char field2030[0x1000];
    signed char field3030[0x1000];
} GeorgeTextLookupContext;

typedef struct GeorgeTextTagEntry {
    u32 code;
    const signed char *text;
} GeorgeTextTagEntry;

#define GEORGE_TEXT_OFFSET(field, offset) \
    typedef char text_lookup_##field[(offsetof(GeorgeTextLookupContext, field) == (offset)) ? 1 : -1]
GEORGE_TEXT_OFFSET(field04, 4);
GEORGE_TEXT_OFFSET(field08, 8);
GEORGE_TEXT_OFFSET(field0C, 0xC);
GEORGE_TEXT_OFFSET(field10, 0x10);
GEORGE_TEXT_OFFSET(field14, 0x14);
GEORGE_TEXT_OFFSET(field18, 0x18);
GEORGE_TEXT_OFFSET(field1C, 0x1C);
GEORGE_TEXT_OFFSET(field101C, 0x101C);
GEORGE_TEXT_OFFSET(field1020, 0x1020);
GEORGE_TEXT_OFFSET(field1024, 0x1024);
GEORGE_TEXT_OFFSET(field1028, 0x1028);
GEORGE_TEXT_OFFSET(field102C, 0x102C);
GEORGE_TEXT_OFFSET(field1030, 0x1030);
GEORGE_TEXT_OFFSET(field2030, 0x2030);
GEORGE_TEXT_OFFSET(field3030, 0x3030);
#undef GEORGE_TEXT_OFFSET
typedef char text_lookup_context_size[(sizeof(GeorgeTextLookupContext) == 0x4030) ? 1 : -1];
typedef char text_tag_entry_size[(sizeof(GeorgeTextTagEntry) == 8) ? 1 : -1];
typedef char text_tag_entry_pointer[(offsetof(GeorgeTextTagEntry, text) == 4) ? 1 : -1];

s32 func_002B4458(GeorgeTextLookupContext *context, const signed char *input,
                  u32 length, float value);
void func_002B44A8(GeorgeTextLookupContext *context, const GeorgeTextTagEntry *entries) GEORGE_SAVE128;
void func_002B45A0(signed char *text);
s32 func_002B45F0(GeorgeTextLookupContext *context, const signed char *key,
                  signed char *output) GEORGE_SAVE128;

#endif
