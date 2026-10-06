#include <stdio.h>
#include <string.h>
#include "george/text_lookup.h"

extern const char _ctype_[257];
const signed char D_004476C8[3] = {0, '\'', '"'};
const signed char *D_003FD258[5], *D_003FD270[5], *D_003FD288[5];
static unsigned checks, failures, comparisons, copies, mutation;
static GeorgeTextLookupContext context;
static GeorgeTextTagEntry *active_entries;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)

u32 func_00295050(const signed char *text) { return (u32)strlen((const char *)text); }
s32 func_00393E48(const signed char *left, const signed char *right, u32 count)
{
    return strncmp((const char *)left, (const char *)right, count);
}

static unsigned lower(unsigned byte)
{
    return byte >= 'A' && byte <= 'Z' ? byte + 32 : byte;
}

s32 func_003983E8(const signed char *left, const signed char *right)
{
    s32 result;
    ++comparisons;
    while (*left != 0 && lower((u8)*left) == lower((u8)*right)) {
        ++left; ++right;
    }
    result = (s32)lower((u8)*left) - (s32)lower((u8)*right);
    if (comparisons == 1) {
        if (mutation == 1) {
            CHECK(context.field14 == 0x8001DEADu);
            active_entries[0].code = 0xDEADBEEFu;
            context.field14 = 33;
        } else if (mutation == 2) {
            active_entries[1].code = 0x8002DEADu;
        } else if (mutation == 3) {
            strcpy((char *)context.field2030, "a=1 B='changed'");
        } else if (mutation == 4) {
            strcpy((char *)context.field3030, "fresh");
        } else if (mutation == 5) {
            strcpy((char *)context.field3030, "target=next");
        }
    }
    return result;
}

signed char *func_00393B74(signed char *destination, const signed char *source)
{
    signed char *start = destination;
    ++copies;
    /* The substitute supports the observed valid forward-copy inputs. */
    do { *destination++ = *source; } while (*source++ != 0);
    if (mutation == 6) {
        strcpy((char *)context.field2030, "replaced=yes");
        context.field14 = 42;
    }
    /* Deliberately different return: callers must ignore it. */
    (void)start;
    return NULL;
}

static void reset(void)
{
    memset(&context, 0, sizeof context);
    comparisons = copies = mutation = 0;
    active_entries = NULL;
}

static void initialization(void)
{
    static const u32 lengths[] = {0, 1, 0x80000000u, 0xFFFFFFFFu};
    static const float values[] = {0.0f, -0.0f, 1.0f, -2.5f};
    unsigned test, index;
    for (test = 0; test < 4; ++test) {
        GeorgeTextLookupContext expected;
        const signed char *input = test == 0 ? NULL : (const signed char *)&context + (test * 37);
        for (index = 0; index < sizeof context; ++index)
            ((u8 *)&context)[index] = (u8)(index * 73u + test * 19u);
        memcpy(&expected, &context, sizeof expected);
        expected.field04 = values[test];
        expected.field0C = input; expected.field10 = lengths[test];
        expected.field14 = 0x8001DEADu; expected.field3030[0] = 0;
        expected.field00 = 0; expected.field08 = input; expected.field18 = 0;
        expected.field1C[0] = 0;
        expected.field101C = expected.field1020 = expected.field1024 = 0;
        expected.field1028 = expected.field102C = 0;
        expected.field1030[0] = expected.field2030[0] = 0;
        CHECK(func_002B4458(&context, input, lengths[test], values[test]) == 1);
        for (index = 0; index < sizeof context; ++index)
            CHECK(((u8 *)&context)[index] == ((u8 *)&expected)[index]);
    }
}

static void tags(void)
{
    GeorgeTextTagEntry entries[] = {
        {9, (const signed char *)"not-this"}, {17, (const signed char *)"TaG"},
        {19, (const signed char *)"tag"}, {0x8000DEADu, NULL}
    };
    reset(); strcpy((char *)context.field1030, "tAg");
    func_002B44A8(&context, entries);
    CHECK(context.field14 == 17 && comparisons == 2);
    CHECK(copies == 0);
    reset(); context.field14 = 99;
    func_002B44A8(&context, NULL);
    CHECK(context.field14 == 0x8001DEADu && comparisons == 0);
    reset(); func_002B44A8(&context, entries + 3);
    CHECK(context.field14 == 0x8001DEADu && comparisons == 0);
    reset(); entries[0].code = 0x8002DEADu;
    func_002B44A8(&context, entries);
    CHECK(context.field14 == 0x8002DEADu && comparisons == 0);
    entries[0].code = 9;
    reset(); strcpy((char *)context.field1030, "NOT-THIS");
    active_entries = entries; mutation = 1;
    func_002B44A8(&context, entries);
    CHECK(context.field14 == 0xDEADBEEFu && comparisons == 1);
    entries[0].code = 9;
    reset(); strcpy((char *)context.field1030, "tag");
    active_entries = entries; mutation = 2;
    func_002B44A8(&context, entries);
    CHECK(context.field14 == 0x8002DEADu && comparisons == 1);

    /* The default write can itself alias the first table code. */
    reset(); strcpy((char *)context.field1030, "alias");
    context.field14 = 0x8000DEADu;
    context.field18 = (u32)(const signed char *)"alias";
    active_entries = (GeorgeTextTagEntry *)&context.field14;
    mutation = 1;
    func_002B44A8(&context, active_entries);
    /* The callback's code write is followed by its same-word tag write33. */
    CHECK(context.field14 == 33 && comparisons == 1);
}

static void case_conversion(void)
{
    unsigned code, index;
    signed char text[160];
    for (code = 0; code < 128; ++code) {
        text[0] = (signed char)code; text[1] = 0; text[2] = 0x5A;
        func_002B45A0(text);
        CHECK((u8)text[0] == lower(code));
        CHECK(text[1] == 0 && text[2] == 0x5A);
        CHECK((((u8)_ctype_[code + 1] & 1u) != 0) == (code >= 'A' && code <= 'Z'));
    }
    for (index = 1; index < 128; ++index) text[index - 1] = (signed char)index;
    text[127] = 0; text[128] = 0x5A;
    func_002B45A0(text);
    for (index = 1; index < 128; ++index) CHECK((u8)text[index - 1] == lower(index));
    CHECK(text[127] == 0 && text[128] == 0x5A);
    text[0] = (signed char)0xFF; text[1] = 'A'; text[2] = 0;
    func_002B45A0(text);
    CHECK((u8)text[0] == 0xFF && text[1] == 'a' && text[2] == 0);
    /* 0x80..0xFE yield indices below the proven table and are not executed. */
}

static void values(void)
{
    signed char output[128];
    reset(); strcpy((char *)context.field2030, "K=first k=second");
    strcpy((char *)context.field1030, "k"); strcpy((char *)context.field3030, "fallback");
    CHECK(func_002B45F0(&context, (const signed char *)"k", output) == 1);
    CHECK(strcmp((char *)output, "first") == 0 && copies == 1 && comparisons == 1);

    reset(); strcpy((char *)context.field2030, "other=1");
    strcpy((char *)context.field1030, "Key"); strcpy((char *)context.field3030, "whole value");
    CHECK(func_002B45F0(&context, (const signed char *)"KEY", output) == 1);
    CHECK(strcmp((char *)output, "whole value") == 0 && copies == 1);

    reset(); strcpy((char *)context.field2030, "other=1");
    strcpy((char *)context.field1030, "other"); strcpy((char *)context.field3030, "key=third key=fourth");
    CHECK(func_002B45F0(&context, (const signed char *)"Key", output) == 1);
    CHECK(strcmp((char *)output, "third") == 0 && copies == 1);

    reset(); memset(output, 0x5A, sizeof output);
    strcpy((char *)context.field2030, "other=1");
    strcpy((char *)context.field1030, "other"); strcpy((char *)context.field3030, "other=2");
    CHECK(func_002B45F0(&context, (const signed char *)"key", output) == 0);
    CHECK(copies == 0);
    { unsigned index; for (index = 0; index < sizeof output; ++index) CHECK(output[index] == 0x5A); }

    reset(); strcpy((char *)context.field2030, "key=\"\" tail=2");
    CHECK(func_002B45F0(&context, (const signed char *)"key", output) == 1);
    CHECK(*output == 0 && copies == 1);
    reset(); strcpy((char *)context.field2030, "key"); output[0] = 0x5A;
    CHECK(func_002B45F0(&context, (const signed char *)"key", output) == 0);
    CHECK(*output == 0x5A && copies == 0);

    reset(); strcpy((char *)context.field2030, "a=1 b=2"); mutation = 3;
    CHECK(func_002B45F0(&context, (const signed char *)"b", output) == 1);
    CHECK(strcmp((char *)output, "changed") == 0 && comparisons == 2 && copies == 1);
    reset(); strcpy((char *)context.field1030, "key");
    strcpy((char *)context.field3030, "old"); mutation = 4;
    CHECK(func_002B45F0(&context, (const signed char *)"key", output) == 1);
    CHECK(strcmp((char *)output, "fresh") == 0 && copies == 1);
    reset(); strcpy((char *)context.field1030, "other"); mutation = 5;
    CHECK(func_002B45F0(&context, (const signed char *)"target", output) == 1);
    CHECK(strcmp((char *)output, "next") == 0 && comparisons == 2 && copies == 1);
    reset(); strcpy((char *)context.field2030, "key=stored"); mutation = 6;
    CHECK(func_002B45F0(&context, (const signed char *)"key", output) == 1);
    CHECK(strcmp((char *)output, "stored") == 0 && context.field14 == 42);
    CHECK(strcmp((char *)context.field2030, "replaced=yes") == 0);

    reset(); strcpy((char *)context.field2030, "key=value other=2");
    CHECK(func_002B45F0(&context, (const signed char *)"key", context.field2030) == 1);
    CHECK(strcmp((char *)context.field2030, "value") == 0);
    reset(); strcpy((char *)context.field2030, "key=value");
    strcpy((char *)output, "KEY");
    CHECK(func_002B45F0(&context, output, output) == 1);
    CHECK(strcmp((char *)output, "value") == 0);
}

int main(void)
{
    initialization(); tags(); case_conversion(); values();
    printf("text_lookup: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
