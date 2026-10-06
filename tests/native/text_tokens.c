#include <stdio.h>
#include <string.h>
#include "george/text_tokens.h"
#include "text_tokens_golden.h"

const signed char D_004476C8[3] = {0, '\'', '"'};
const signed char *D_003FD258[5], *D_003FD270[5], *D_003FD288[5];
static const signed char words[][8] = {
    "STOP", "END", "?", "!", ";", "Q", "R", "L", "Z", "", "ab", "xy"
};
static unsigned checks, failures, length_calls, compare_calls;
static unsigned fixture;
static int mutation;
static GeorgeTextTokenContext *active_context;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("fixture %u failure line %d\n", fixture, __LINE__); } } while (0)

u32 func_00295050(const signed char *text)
{
    u32 length = (u32)strlen((const char *)text);
    ++length_calls;
    if (length_calls == 1) {
        if (mutation == 1)
            D_003FD258[0] = words[10];
        else if (mutation == 2)
            D_003FD258[1] = words[8];
        else if (mutation == 3)
            active_context->field18 = 0xFFFFFFFFu;
    }
    return length;
}

s32 func_00393E48(const signed char *left, const signed char *right, u32 count)
{
    u32 index;
    s32 result = 0;
    ++compare_calls;
    if (compare_calls == 1 && mutation == 1) {
        CHECK(right == words[10]);
        CHECK(count == 1); /* Length of the old "?", not the new "ab". */
    }
    for (index = 0; index < count; ++index) {
        unsigned first = (u8)left[index], second = (u8)right[index];
        if (first != second) {
            result = (s32)first - (s32)second;
            break;
        }
        if (first == 0)
            break;
    }
    if (compare_calls == 1 && mutation == 4)
        *(signed char *)left = 0;
    return result;
}

static void reset(void)
{
    unsigned index;
    for (index = 0; index < 5; ++index)
        D_003FD258[index] = D_003FD270[index] = D_003FD288[index] = NULL;
    D_003FD258[0] = words[2]; D_003FD258[2] = words[0];
    D_003FD270[0] = words[3]; D_003FD270[2] = words[1];
    D_003FD288[0] = words[4];
    length_calls = compare_calls = 0;
    mutation = 0;
}

static void golden(void)
{
    unsigned index;
    for (fixture = 0; fixture < sizeof text_token_golden / sizeof text_token_golden[0]; ++fixture) {
        const struct TextTokenGolden *test = &text_token_golden[fixture];
        signed char buffer[128], *input, *output, *result;
        GeorgeTextTokenContext context;
        reset();
        memcpy(buffer, test->initial, sizeof buffer);
        memset(&context, 0, sizeof context);
        active_context = &context;
        mutation = test->mutation;
        if (test->routine == 0) {
            context.field08 = buffer + 32;
            context.field10 = test->length;
            context.field18 = test->line;
            input = buffer + 32;
            output = buffer + test->name;
            func_002B3190(&context, &input, test->same_cell ? &input : &output,
                         test->mode, test->keep);
            CHECK(input - buffer == test->input_end);
            CHECK((test->same_cell ? input : output) - buffer == test->output_end);
        } else {
            result = func_002B3EE0(buffer + 32, buffer + test->name, buffer + test->value);
            CHECK((result == NULL ? -1 : (int)(result - buffer)) == test->result);
        }
        for (index = 0; index < sizeof buffer; ++index)
            CHECK((u8)buffer[index] == test->expected[index]);
        CHECK(context.field18 == test->final_line);
        CHECK(length_calls == test->length_calls);
        CHECK(compare_calls == test->compare_calls);
    }
}

static void examples(void)
{
    signed char text[128], name[64], value[64], output[128];
    signed char *source, *destination, *result;
    GeorgeTextTokenContext context;
    reset();
    memset(&context, 0, sizeof context);
    active_context = &context;
    strcpy((char *)text, " \n\r'ab\nc\rd'?tail");
    context.field08 = text; context.field10 = 64;
    source = text; destination = output;
    func_002B3190(&context, &source, &destination, 0, 1);
    CHECK(strcmp((char *)output, "'abcd'") == 0);
    CHECK(context.field18 == 2);
    CHECK(*source == '?');
    CHECK(destination == output + 6);

    /* A delimiter is tested before the first quoted character is scanned. */
    reset();
    strcpy((char *)text, "\"?inside\"rest");
    source = text; destination = output;
    func_002B3190(&context, &source, &destination, 0, 1);
    CHECK(strcmp((char *)output, "\"") == 0);
    CHECK(source == text + 1);

    /* An unmatched quote steps past its NUL and resumes on the following byte. */
    reset();
    memcpy(text, "\"abc\0X?tail\0", 12);
    source = text; destination = output;
    func_002B3190(&context, &source, &destination, 0, 1);
    CHECK(strcmp((char *)output, "\"abc\"X") == 0);
    CHECK(source == text + 6);

    /* Empty delimiter strings match with a zero count; null cells do not. */
    reset(); D_003FD258[4] = words[9];
    strcpy((char *)text, "data?");
    source = text; destination = output;
    func_002B3190(&context, &source, &destination, 0, 0);
    CHECK(source == text && destination == output && *output == 0);

    reset();
    strcpy((char *)text, "name=\"  two words\" next=x");
    result = func_002B3EE0(text, name, value);
    CHECK(strcmp((char *)name, "name") == 0);
    CHECK(strcmp((char *)value, "two words") == 0);
    CHECK(strcmp((char *)result, " next=x") == 0);

    strcpy((char *)text, "name=\"\" next=x");
    result = func_002B3EE0(text, name, value);
    CHECK(result == text + 7 && *value == 0);
    strcpy((char *)text, "name=\"unclosed");
    result = func_002B3EE0(text, name, value);
    CHECK(result == text + strlen((char *)text));
    CHECK(strcmp((char *)value, "unclosed") == 0);
    strcpy((char *)text, "name");
    CHECK(func_002B3EE0(text, name, value) == NULL);
    CHECK(strcmp((char *)name, "name") == 0 && *value == 0);

    /* Initial output clearing is visible when either output aliases input. */
    strcpy((char *)text, "a=x");
    CHECK(func_002B3EE0(text, text, value) == NULL);
    CHECK(*text == 0 && *value == 0);
    strcpy((char *)text, "a=x");
    CHECK(func_002B3EE0(text, name, text) == NULL);
    CHECK(*text == 0 && *name == 0);
}

int main(void)
{
    golden();
    examples();
    printf("text_tokens: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
