#include <stdio.h>
#include <string.h>
#include "george/text_parser.h"
#include "text_parser_golden.h"

const signed char D_004476C8[3] = {0, '\'', '"'};
const signed char *D_003FD258[5], *D_003FD270[5], *D_003FD288[5];
/* The retail token helper compares internal scratch addresses to the original
 * input window. Keep fixture input below context scratch, as in the decoder;
 * independent globals can be placed in the opposite order by native GCC. */
static struct { signed char buffer[256]; GeorgeTextLookupContext context; } storage;
#define context storage.context
#define buffer storage.buffer
static GeorgeTextLookupContext expected;
static GeorgeTextTagEntry tags[4];
static unsigned checks, failures, length_calls, compare_calls, tag_calls, mutation;
static unsigned fixture;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures < 20) printf("fixture %u failure line %u\n", fixture, __LINE__); \
    ++failures; } } while (0)

u32 func_00295050(const signed char *text)
{
    u32 length = (u32)strlen((const char *)text);
    ++length_calls;
    if (length_calls == 1 && mutation == 1) context.field18 = 0xFFFFFFFFu;
    return length;
}

s32 func_00393E48(const signed char *left, const signed char *right, u32 count)
{
    ++compare_calls;
    return strncmp((const char *)left, (const char *)right, count);
}

static unsigned lower(unsigned byte) { return byte >= 'A' && byte <= 'Z' ? byte + 32 : byte; }
s32 func_003983E8(const signed char *left, const signed char *right)
{
    s32 result;
    while (*left != 0 && lower((u8)*left) == lower((u8)*right)) { ++left; ++right; }
    result = (s32)lower((u8)*left) - (s32)lower((u8)*right);
    ++tag_calls;
    if (tag_calls == 1) {
        if (mutation == 2) {
            tags[0].code = 0x12345678;
            context.field14 = 99;
        } else if (mutation == 3) tags[1].code = 0x8002DEAD;
        else if (mutation == 4) {
            context.field101C = 4;
            context.field1020 = 2;
            context.field1024 = 0xFFFFFFFEu;
        } else if (mutation == 5) context.field0C = buffer + 7;
    }
    return result;
}

/* Unused when text_lookup.c is linked for its authentic lowercase helper. */
signed char *func_00393B74(signed char *destination, const signed char *source)
{
    return (signed char *)strcpy((char *)destination, (const char *)source);
}

static void run_fixture(const struct ParserGolden *golden)
{
    unsigned index;
    signed char *source;
    static const signed char *names[] = {(const signed char *)"tag", (const signed char *)"other",
                                       (const signed char *)"/tag"};
    memset(&context, 0x7E, sizeof context);
    memset(buffer, 0x7E, sizeof buffer);
    source = golden->alias < 0 ? buffer + 32 : (signed char *)&context + golden->alias;
    memcpy(source, golden->text, golden->text_size);
    memset(source + golden->text_size, 0, 4);
    context.field00 = golden->flags;
    context.field08 = source;
    context.field0C = source + golden->cursor;
    context.field10 = golden->length;
    context.field18 = golden->line;
    context.field1024 = golden->depth;
    tags[0].code = 41; tags[0].text = names[0];
    tags[1].code = 42; tags[1].text = names[1];
    tags[2].code = tags[3].code = 0x8000DEAD;
    tags[2].text = tags[3].text = names[0];
    if (golden->entries == 2) { tags[0].text = names[1]; tags[1].text = names[0]; }
    else if (golden->entries == 3) tags[0].code = 0x8002DEAD;
    else if (golden->entries == 4) tags[0].code = 0x8000DEAD;
    else if (golden->entries == 5) tags[0].text = names[2];
    expected = context;
    for (index = 0; index < golden->patch_count; ++index)
        ((u8 *)&expected)[golden->patches[index].offset] = golden->patches[index].byte;
    expected.field0C = (golden->cursor_kind ? (signed char *)&context : buffer) + golden->cursor_offset;
    length_calls = compare_calls = tag_calls = 0;
    mutation = golden->mutation;
    for (index = 0; index < golden->iterations; ++index)
        func_002B3460(&context, golden->entries ? tags : NULL);
    for (index = 0; index < sizeof context; ++index) {
        if (((u8 *)&context)[index] != ((u8 *)&expected)[index] && failures < 20)
            printf("context offset %X actual %u expected %u\n", index,
                   ((u8 *)&context)[index], ((u8 *)&expected)[index]);
        CHECK(((u8 *)&context)[index] == ((u8 *)&expected)[index]);
    }
    for (index = 0; index < sizeof buffer; ++index) CHECK((u8)buffer[index] == golden->expected_buffer[index]);
    CHECK(length_calls == golden->length_calls);
    CHECK(compare_calls == golden->compare_calls);
    CHECK(tag_calls == golden->tag_calls);
}

int main(void)
{
    unsigned index;
    D_003FD258[0] = (const signed char *)" ";
    D_003FD258[1] = (const signed char *)"\t";
    D_003FD270[0] = (const signed char *)"\r";
    D_003FD270[1] = (const signed char *)"\n";
    D_003FD288[0] = (const signed char *)"</";
    D_003FD288[1] = (const signed char *)"/>";
    D_003FD288[2] = (const signed char *)">";
    for (index = 0; index < sizeof parser_golden / sizeof *parser_golden; ++index) {
        fixture = index;
        run_fixture(parser_golden + index);
    }
    printf("text parser: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
