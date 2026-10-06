#include "george/text_parser.h"

extern s32 func_003983E8(const signed char *left, const signed char *right);

/* Every bounded whitespace loop reads the byte before testing its bound. */
#define TEXT_SPACE(character) \
    ((character) == ' ' || (character) == '\t' || \
     (character) == '\n' || (character) == '\r')
#define TEXT_END(context) ((u32)(context)->field08 + (context)->field10)
#define TEXT_SKIP_SPACE(context, cursor) do { \
    signed char character = *(cursor); \
    while (TEXT_SPACE(character)) { \
        if ((u32)(cursor) >= TEXT_END(context)) break; \
        ++(cursor); \
        if (character == '\n') ++(context)->field18; \
        character = *(cursor); \
    } \
} while (0)

/* Both original inline scans reload the code after a successful comparator. */
#define TEXT_TAG_SCAN(context, entries, name) do { \
    const GeorgeTextTagEntry *entry = (entries); \
    if (entry != NULL) { \
        for (;;) { \
            u32 code = entry->code; \
            if (code == 0x8000DEADu) break; \
            if (code == 0x8002DEADu) { \
                (context)->field14 = code; \
                break; \
            } \
            if (func_003983E8((name), entry->text) == 0) { \
                (context)->field14 = entry->code; \
                break; \
            } \
            ++entry; \
        } \
    } \
} while (0)

void func_002B3460(GeorgeTextLookupContext *context,
                   const GeorgeTextTagEntry *entries)
{
    signed char *input = (signed char *)context->field0C;
    signed char *output = context->field1C;
    signed char *raw = output;
    u32 end = TEXT_END(context);
    u32 openings = 0;
    s32 done = 0;

    *raw = 0;
    if ((u32)input >= end) {
        context->field102C = 1;
        return;
    }
    TEXT_SKIP_SPACE(context, input);
    while ((u32)input < TEXT_END(context)) {
        if (*input == '<') {
            if (input[1] == '!' && input[2] == '-' && input[3] == '-') {
                signed char *cursor = input;
                u32 comment_end = TEXT_END(context);
                if ((u32)cursor < comment_end) {
                    do {
                        signed char character = *cursor;
                        ++cursor;
                        if (character == '\n') ++context->field18;
                        if ((u32)cursor >= comment_end) break;
                    } while (*cursor != '-' || cursor[1] != '-' || cursor[2] != '>');
                }
                if (*cursor == '-' && cursor[1] == '-' && cursor[2] == '>')
                    cursor += 3;
                input = cursor;
                TEXT_SKIP_SPACE(context, input);
            } else if (*input == '<') {
                if (input[1] != '/') {
                    u32 previous_openings = openings++;
                    if ((s32)previous_openings <= 0) {
                        signed char *cursor;
                        func_002B3190((GeorgeTextTokenContext *)context,
                                     &input, &output, 2, 1);
                        while (*input == '/' || *input == '?' || *input == '>') {
                            if ((u32)input >= TEXT_END(context)) break;
                            *output++ = *input++;
                        }
                        cursor = input;
                        TEXT_SKIP_SPACE(context, cursor);
                        input = cursor;
                        if (*cursor != 0 && *cursor != '<' &&
                            (u32)cursor < TEXT_END(context)) {
                            do {
                                signed char character = *cursor;
                                ++cursor;
                                *output = character;
                                ++output;
                                input = cursor;
                                if (*cursor == 0 || *cursor == '<') break;
                            } while ((u32)cursor < TEXT_END(context));
                        }
                        cursor = input;
                        TEXT_SKIP_SPACE(context, cursor);
                        input = cursor;
                    }
                    done = 1;
                } else {
                    done = 1;
                    if ((s32)openings <= 0) {
                        signed char *cursor;
                        signed char *start = input;
                        *output++ = *start;
                        input = start + 1;
                        *output++ = start[1];
                        input = start + 2;
                        func_002B3190((GeorgeTextTokenContext *)context,
                                     &input, &output, 2, 1);
                        *output++ = *input++;
                        cursor = input;
                        TEXT_SKIP_SPACE(context, cursor);
                        input = cursor;
                    }
                }
            }
        } else {
            signed char character = *input;
            if ((character == '/' || character == '?') && input[1] == '>') {
                signed char *start = input;
                done = 1;
                *output++ = character;
                input = start + 1;
                *output++ = start[1];
                input = start + 2;
            }
        }
        if ((u32)input >= TEXT_END(context) || done != 0) break;
    }

    *output++ = 0;
    TEXT_SKIP_SPACE(context, input);
    context->field0C = input;
    if ((context->field00 & 1u) != 0) func_002B45A0(raw);
    if ((u32)input >= TEXT_END(context)) context->field102C = 1;
    context->field1030[0] = 0;
    context->field2030[0] = 0;
    context->field14 = 0x8001DEADu;
    context->field3030[0] = 0;
    context->field101C = 0;
    context->field1020 = 0;
    context->field1028 = 0;

    input = raw;
    while (*input != 0) {
        if (*input == '<') {
            if (input[1] == '?' || input[1] == '!') context->field1028 = 1;
            if (*input == '<' && input[1] != '/' &&
                input[1] != '!' && input[1] != '?') {
                signed char *cursor = input;
                signed char *name = context->field1030;
                signed char *name_start = name;
                signed char *attributes = context->field2030;
                signed char *content = context->field3030;
                ++context->field101C;
                for (;;) {
                    if (*cursor == '<' && cursor[1] != '/' &&
                        cursor[1] != '!' && cursor[1] != '?') {
                        ++cursor;
                        continue;
                    }
                    if (TEXT_SPACE(*cursor)) {
                        ++cursor;
                        continue;
                    }
                    break;
                }
                while (!TEXT_SPACE(*cursor) && *cursor != '>' && *cursor != '/')
                    *name++ = *cursor++;
                *name = 0;
                output = attributes;
                func_002B3190((GeorgeTextTokenContext *)context,
                             &cursor, &output, 2, 1);
                *output = 0;
                output = content;
                while (TEXT_SPACE(*cursor) || *cursor == '>') ++cursor;
                func_002B3190((GeorgeTextTokenContext *)context,
                             &cursor, &output, 2, 1);
                *output++ = 0;
                context->field14 = 0x8001DEADu;
                TEXT_TAG_SCAN(context, entries, name_start);
                input = cursor - 1;
            }
        }
        if ((*input == '<' && input[1] == '/') ||
            (*input == '/' && input[1] == '>')) {
            u32 previous_openings = context->field101C;
            ++context->field1020;
            if (previous_openings == 0 && *input == '<' && input[1] == '/') {
                /* The standalone closing-tag name deliberately includes '/'. */
                signed char *cursor = input + 1;
                signed char *name = context->field1030;
                signed char *name_start = name;
                do {
                    *name++ = *cursor++;
                } while (*cursor != '>');
                *name = 0;
                context->field14 = 0x8001DEADu;
                TEXT_TAG_SCAN(context, entries, name_start);
                input = cursor;
            }
        }
        ++input;
    }
    context->field1024 = context->field1024 - context->field1020 + context->field101C;
}

#undef TEXT_TAG_SCAN
#undef TEXT_SKIP_SPACE
#undef TEXT_END
#undef TEXT_SPACE
