#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "george/text_values.h"

const signed char D_004476C8[3] = {0, '\'', '"'};
const signed char D_004476D0[] = "yes", D_004476D8[] = "on";
const signed char D_004476E0[] = "true", D_004476E8[] = "1";
const signed char D_00447730[] = "%f %f %f";
const signed char *D_003FD258[5], *D_003FD270[5], *D_003FD288[5];
static const signed char *accepted[] = {D_004476D0, D_004476D8, D_004476E0, D_004476E8};
static GeorgeTextLookupContext context;
static unsigned checks, failures, copies, truth_calls, scan_calls, lower_calls;
static unsigned mutation;
static u32 *active_boolean;
static signed char *active_output;
static float *active_vector;
static s32 scan_return;
static float scan_values[3];
static const char *expected_scan;
static unsigned parse_calls, event_count;
static struct { u32 opened, closed, eof; } events[8];
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)

u32 func_00295050(const signed char *text) { return (u32)strlen((const char *)text); }
s32 func_00393E48(const signed char *left, const signed char *right, u32 count)
{
    return strncmp((const char *)left, (const char *)right, count);
}
static unsigned lower(unsigned byte) { return byte >= 'A' && byte <= 'Z' ? byte + 32 : byte; }

s32 func_003983E8(const signed char *left, const signed char *right)
{
    unsigned index;
    s32 result;
    for (index = 0; index < 4; ++index) {
        if (left == accepted[index]) {
            CHECK(index == truth_calls);
            ++truth_calls;
            if (mutation == 1 && index == 1) {
                CHECK(*active_boolean == 1);
                *active_boolean = 99;
            }
        }
    }
    while (*left != 0 && lower((u8)*left) == lower((u8)*right)) {
        ++left; ++right;
    }
    result = (s32)lower((u8)*left) - (s32)lower((u8)*right);
    return result;
}

signed char *func_00393B74(signed char *destination, const signed char *source)
{
    ++copies;
    do { *destination++ = *source; } while (*source++ != 0);
    return NULL; /* Every caller must ignore this substitute's return. */
}

s32 func_00395350(const signed char *text, const signed char *format, ...)
{
    va_list arguments;
    float *first, *second, *third;
    ++scan_calls;
    CHECK(format == D_00447730);
    CHECK(strcmp((const char *)text, expected_scan) == 0);
    va_start(arguments, format);
    first = va_arg(arguments, float *);
    second = va_arg(arguments, float *);
    third = va_arg(arguments, float *);
    va_end(arguments);
    CHECK(second == first + 1 && third == first + 2);
    CHECK(active_vector[0] == 0 && active_vector[1] == 0 && active_vector[2] == 0);
    if (mutation == 2) {
        active_vector[0] = 90; active_vector[1] = 91; active_vector[2] = 92;
    }
    /* All lanes are written, even when this controlled call reports fewer
     * conversions. This tests ignored return count, not unwritten-lane values. */
    *first = scan_values[0]; *second = scan_values[1]; *third = scan_values[2];
    return scan_return;
}

signed char *func_003984D8(signed char *text)
{
    ++lower_calls;
    CHECK(copies == 1 && *active_output == 0);
    while (*text != 0) {
        *text = (signed char)lower((u8)*text);
        ++text;
    }
    if (mutation == 3)
        strcpy((char *)active_output, "mutation");
    return NULL;
}

void func_002B3460(GeorgeTextLookupContext *received, const void *table)
{
    CHECK(received == &context && table == NULL);
    CHECK(parse_calls < event_count);
    if (parse_calls >= event_count) {
        context.field102C = 1;
        return;
    }
    context.field101C = events[parse_calls].opened;
    context.field1020 = events[parse_calls].closed;
    context.field102C = events[parse_calls].eof;
    ++parse_calls;
}

static void reset(void)
{
    memset(&context, 0, sizeof context);
    copies = truth_calls = scan_calls = lower_calls = mutation = 0;
    active_boolean = NULL; active_output = NULL; active_vector = NULL;
    scan_values[0] = 1.5f; scan_values[1] = -2; scan_values[2] = 3.25f;
    scan_return = 3; expected_scan = NULL;
    parse_calls = event_count = 0;
}

static void place(unsigned location, const char *value)
{
    if (location == 0) {
        sprintf((char *)context.field2030, "key='%s' key=later", value);
        strcpy((char *)context.field1030, "key");
        strcpy((char *)context.field3030, "fallback");
    } else if (location == 1) {
        strcpy((char *)context.field2030, "other=1");
        strcpy((char *)context.field1030, "KEY");
        strcpy((char *)context.field3030, value);
    } else {
        strcpy((char *)context.field2030, "other=1");
        strcpy((char *)context.field1030, "other");
        sprintf((char *)context.field3030, "key='%s' key=later", value);
    }
}

static void booleans(void)
{
    static const char *values[] = {"yes", "on", "true", "1", "YES", "On", "TrUe", "no", "0", "false", ""};
    unsigned location, index;
    u32 output;
    for (location = 0; location < 3; ++location) {
        for (index = 0; index < sizeof values / sizeof values[0]; ++index) {
            reset(); place(location, values[index]);
            output = 0xFFFFFFFFu; active_boolean = &output;
            CHECK(func_002B4188(&context, (const signed char *)"key", &output) == 1);
            CHECK(output == (index < 7 ? 1u : 0u));
            CHECK(truth_calls == 4 && copies == 1);
        }
    }
    reset(); output = 0xFFFFFFFFu; active_boolean = &output;
    CHECK(func_002B4188(&context, (const signed char *)"key", &output) == 0);
    CHECK(output == 0 && truth_calls == 0 && copies == 0);
    reset(); place(0, "yes"); active_boolean = &output; mutation = 1;
    CHECK(func_002B4188(&context, (const signed char *)"key", &output) == 1);
    CHECK(output == 99 && truth_calls == 4);
    reset(); strcpy((char *)context.field2030, "key=yes");
    strcpy((char *)context.field1030, "key"); strcpy((char *)context.field3030, "true");
    active_boolean = (u32 *)context.field2030;
    CHECK(func_002B4188(&context, (const signed char *)"key", active_boolean) == 1);
    CHECK(*active_boolean == 1 && truth_calls == 4);
    {
        union { u32 word; signed char text[16]; } key;
        reset(); place(0, "yes"); strcpy((char *)context.field1030, "other");
        strcpy((char *)key.text, "key");
        active_boolean = &key.word;
        CHECK(func_002B4188(&context, key.text, &key.word) == 0);
        CHECK(key.word == 0 && truth_calls == 0);
    }
}

static void vectors(void)
{
    static const s32 returns[] = {-1, 0, 1, 2, 3, 99};
    unsigned location, index;
    float output[4];
    for (location = 0; location < 3; ++location) {
        for (index = 0; index < sizeof returns / sizeof returns[0]; ++index) {
            reset(); place(location, "1.5 -2 3.25");
            output[0] = output[1] = output[2] = -50; output[3] = 123;
            scan_return = returns[index]; expected_scan = "1.5 -2 3.25"; active_vector = output;
            CHECK(func_002B4310(&context, (const signed char *)"key", output) == 1);
            CHECK(output[0] == 1.5f && output[1] == -2 && output[2] == 3.25f && output[3] == 123);
            CHECK(scan_calls == 1 && copies == 1);
        }
    }
    reset(); output[0] = output[1] = output[2] = -1; output[3] = 123;
    active_vector = output;
    CHECK(func_002B4310(&context, (const signed char *)"key", output) == 0);
    CHECK(output[0] == 0 && output[1] == 0 && output[2] == 0 && output[3] == 123);
    CHECK(scan_calls == 0 && copies == 0);
    reset(); place(0, "1.5 -2 3.25"); active_vector = output;
    mutation = 2; expected_scan = "1.5 -2 3.25";
    CHECK(func_002B4310(&context, (const signed char *)"key", output) == 1);
    CHECK(output[0] == 1.5f && output[1] == -2 && output[2] == 3.25f);
    reset(); place(0, "primary"); strcpy((char *)context.field3030, "1.5 -2 3.25");
    active_vector = (float *)context.field2030; expected_scan = "1.5 -2 3.25";
    CHECK(func_002B4310(&context, (const signed char *)"key", active_vector) == 1);
    CHECK(active_vector[0] == 1.5f && active_vector[1] == -2 && active_vector[2] == 3.25f);
}

static void strings(void)
{
    unsigned location, index;
    static const s32 flags[] = {0, 1, -1};
    signed char output[128];
    for (location = 0; location < 3; ++location) {
        for (index = 0; index < sizeof flags / sizeof flags[0]; ++index) {
            reset(); place(location, "MiXeD/Path.txt");
            output[0] = 0x5A; active_output = output;
            CHECK(func_002B4700(&context, (const signed char *)"key", output, flags[index]) == 1);
            CHECK(strcmp((char *)output, flags[index] ? "mixed/path.txt" : "MiXeD/Path.txt") == 0);
            CHECK(copies == 2 && lower_calls == (flags[index] ? 1u : 0u));
        }
    }
    reset(); output[0] = 0x5A; output[1] = 0x5A; active_output = output;
    CHECK(func_002B4700(&context, (const signed char *)"key", output, 1) == 0);
    CHECK(output[0] == 0 && output[1] == 0x5A && copies == 0 && lower_calls == 0);
    reset(); place(0, "MiXeD"); active_output = output; mutation = 3;
    CHECK(func_002B4700(&context, (const signed char *)"key", output, 1) == 1);
    CHECK(strcmp((char *)output, "mixed") == 0 && copies == 2);
    reset(); place(0, "primary"); strcpy((char *)context.field3030, "FaLlBaCk");
    active_output = context.field2030;
    CHECK(func_002B4700(&context, (const signed char *)"key", active_output, 1) == 1);
    CHECK(strcmp((char *)active_output, "fallback") == 0);
}

static void depths(void)
{
    static const u32 pairs[][2] = {{0,0}, {1,2}, {0xFFFFFFFFu,0}, {0,0x80000000u}, {0x80000001u,0}};
    unsigned index;
    for (index = 0; index < sizeof pairs / sizeof pairs[0]; ++index) {
        reset(); context.field101C = pairs[index][0]; context.field1020 = pairs[index][1];
        func_002B4538(&context); CHECK(parse_calls == 0);
    }
    reset(); context.field101C = 3; context.field102C = 1;
    func_002B4538(&context); CHECK(parse_calls == 0);
    reset(); context.field101C = 2;
    events[0].opened = 1; events[0].closed = 0; events[0].eof = 0;
    events[1].opened = 0; events[1].closed = 1; events[1].eof = 0;
    events[2].opened = 0; events[2].closed = 2; events[2].eof = 0;
    event_count = 3;
    func_002B4538(&context); CHECK(parse_calls == 3);
    reset(); context.field101C = 0x7FFFFFFFu;
    events[0].opened = 1; events[0].closed = 0; events[0].eof = 0; event_count = 1;
    func_002B4538(&context); CHECK(parse_calls == 1);
    reset(); context.field101C = 1;
    events[0].opened = 0; events[0].closed = 0; events[0].eof = 1; event_count = 1;
    func_002B4538(&context); CHECK(parse_calls == 1);
    reset(); context.field101C = 1;
    events[0].opened = 0; events[0].closed = 0xFFFFFFFFu; events[0].eof = 0;
    events[1].opened = 0; events[1].closed = 2; events[1].eof = 0; event_count = 2;
    func_002B4538(&context); CHECK(parse_calls == 2);
}

int main(void)
{
    booleans(); vectors(); strings(); depths();
    printf("text_values: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
