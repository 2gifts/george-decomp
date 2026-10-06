#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "george/text_content.h"

const signed char D_004476D0[] = "yes", D_004476D8[] = "on";
const signed char D_004476E0[] = "true", D_004476E8[] = "1";
const signed char D_004476F0[] = "%d", D_004476F8[] = "%d %d";
const signed char D_00447700[] = "%d %d %d";
const signed char D_00447720[] = "%f", D_00447728[] = "%f %f";
const signed char D_00447730[] = "%f %f %f", D_00447740[] = "%f %f %f %f";
static const signed char *formats[2][4] = {
    {D_004476F0,D_004476F8,D_00447700,NULL},
    {D_00447720,D_00447728,D_00447730,D_00447740}
};
static const signed char *truths[] = {D_004476D0,D_004476D8,D_004476E0,D_004476E8};
static GeorgeTextLookupContext context;
static union { u32 words[8]; float floats[8]; signed char text[32]; } output;
static void *pointers[4];
static unsigned checks, failures, group, lanes, mode, mutation;
static unsigned scans, comparisons, copies;
static s32 reported_count;
static const char *expected_text;
static u8 before_scan[4][4];
static u32 *truth_output;
static signed char *copy_output, *copy_result;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures < 20) printf("failure line %u group %u lanes %u mode %u mutation %u\n", \
                            __LINE__,group,lanes,mode,mutation); ++failures; } } while (0)

s32 func_00395350(const signed char *text, const signed char *format, ...)
{
    va_list arguments;
    unsigned index;
    s32 converted;
    ++scans;
    CHECK(text == context.field3030);
    CHECK(format == formats[group][lanes - 1]);
    CHECK(strcmp((const char *)text, expected_text) == 0);
    va_start(arguments,format);
    for (index = 0; index < lanes; ++index) {
        void *pointer = group == 0 ? (void *)va_arg(arguments,s32 *) :
                                    (void *)va_arg(arguments,float *);
        CHECK(pointer == pointers[index]);
        CHECK(memcmp(pointer,before_scan[index],4) == 0);
    }
    va_end(arguments);
    if (mutation == 1) {
        u32 word = 0x11223344;
        memcpy(pointers[lanes - 1],&word,4);
    } else if (mutation == 2) {
        strcpy((char *)context.field3030,group == 0 ? "77 nope" : "7.5 nope");
    }
    va_start(arguments,format);
    converted = vsscanf((const char *)text,(const char *)format,arguments);
    va_end(arguments);
    CHECK(converted == (mutation == 2 ? 1 : mode == 0 ? (s32)lanes :
                        mode == 1 ? 1 : mode == 2 ? 0 : -1));
    return reported_count;
}

static unsigned lower(unsigned byte)
{ return byte >= 'A' && byte <= 'Z' ? byte + 32 : byte; }
s32 func_003983E8(const signed char *left, const signed char *right)
{
    s32 result;
    CHECK(comparisons < 4);
    CHECK(left == truths[comparisons]);
    CHECK(right == context.field3030);
    while (*left != 0 && lower((u8)*left) == lower((u8)*right)) { ++left; ++right; }
    result = (s32)lower((u8)*left) - (s32)lower((u8)*right);
    ++comparisons;
    if (comparisons == 1 && mutation == 1) *truth_output = 0x11223344;
    if (comparisons == 1 && mutation == 2) strcpy((char *)context.field3030,"on");
    return result;
}

signed char *func_00393B74(signed char *destination, const signed char *source)
{
    ++copies;
    CHECK(destination == copy_output);
    CHECK(source == context.field3030);
    do { *destination++ = *source; } while (*source++ != 0);
    return copy_result; /* Controlled callee value, for wrapper propagation. */
}

static s32 invoke_scan(void)
{
    if (group == 0) {
        if (lanes == 1) return func_002B5858(&context,pointers[0]);
        if (lanes == 2) return func_002B5828(&context,pointers[0],pointers[1]);
        return func_002B57F0(&context,pointers[0],pointers[1],pointers[2]);
    }
    if (lanes == 1) return func_002B57C8(&context,pointers[0]);
    if (lanes == 2) return func_002B5798(&context,pointers[0],pointers[1]);
    if (lanes == 3) return func_002B5760(&context,pointers[0],pointers[1],pointers[2]);
    return func_002B5720(&context,pointers[0],pointers[1],pointers[2],pointers[3]);
}

static void scanner_cases(void)
{
    static const char *texts[2][4] = {
        {"11 -22 33","11 nope","nope",""},
        {"1.25 -2.5 3.75 -4","1.25 nope","nope",""}
    };
    static const s32 integers[] = {11,-22,33};
    static const float floats[] = {1.25f,-2.5f,3.75f,-4.0f};
    static const s32 reports[] = {-1,0,7};
    unsigned aliases, returned, index;
    for (group = 0; group < 2; ++group) for (lanes = 1; lanes <= (group ? 4u : 3u); ++lanes)
    for (mode = 0; mode < 4; ++mode) for (mutation = 0; mutation < 3; ++mutation)
    for (aliases = 0; aliases < 3; ++aliases) for (returned = 0; returned < 3; ++returned) {
        unsigned offsets[4], converted = mutation == 2 ? 1 : mode == 0 ? lanes : mode == 1 ? 1 : 0;
        u8 expected[32];
        memset(&context,0x7E,sizeof context);
        memset(&output,0x7E,sizeof output);
        memset(expected,0x7E,sizeof expected);
        expected_text = texts[group][mode];
        strcpy((char *)context.field3030,expected_text);
        for (index = 0; index < lanes; ++index) {
            offsets[index] = 4 + (aliases == 1 ? 0 : aliases == 2 ? 3 - index : index) * 4;
            pointers[index] = (u8 *)&output + offsets[index];
            memcpy(before_scan[index],expected + offsets[index],4);
        }
        if (mutation == 1) { u32 word = 0x11223344; memcpy(expected + offsets[lanes - 1],&word,4); }
        for (index = 0; index < converted; ++index) {
            if (mutation == 2) {
                s32 integer = 77;
                float real = 7.5f;
                memcpy(expected + offsets[index],group == 0 ? (void *)&integer : (void *)&real,4);
            } else memcpy(expected + offsets[index],group == 0 ? (void *)(integers + index) :
                                                             (void *)(floats + index),4);
        }
        scans = 0;
        reported_count = reports[returned];
        CHECK(invoke_scan() == reported_count);
        CHECK(scans == 1);
        for (index = 0; index < sizeof expected; ++index) CHECK(((u8 *)&output)[index] == expected[index]);
        CHECK(strcmp((char *)context.field3030,mutation == 2 ? (group == 0 ? "77 nope" : "7.5 nope") : expected_text) == 0);
    }
    /* Outputs inside the context retain unwritten words and captured pointer order. */
    for (group = 0; group < 2; ++group) {
        GeorgeTextLookupContext expected;
        lanes = 3; mode = 1; mutation = 0; scans = 0; reported_count = -1;
        memset(&context,0x7E,sizeof context);
        expected_text = texts[group][mode];
        strcpy((char *)context.field3030,expected_text);
        memcpy(&expected,&context,sizeof expected);
        pointers[0] = &context.field00;
        pointers[1] = &context.field04;
        pointers[2] = context.field3030 + 64;
        for (index = 0; index < lanes; ++index) memcpy(before_scan[index],pointers[index],4);
        memcpy(&expected.field00,group == 0 ? (void *)integers : (void *)floats,4);
        CHECK(invoke_scan() == -1);
        for (index = 0; index < sizeof context; ++index) CHECK(((u8 *)&context)[index] == ((u8 *)&expected)[index]);
    }
}

static void truth_cases(void)
{
    static const char *texts[] = {"YES","On","true","1","nope","","true ","yesx"};
    unsigned entry, variant, expected_calls;
    for (variant = 0; variant < 2; ++variant) for (entry = 0; entry < 8; ++entry)
    for (mutation = 0; mutation < 3; ++mutation) {
        u32 word = 0x7E7E7E7E, expected_word;
        s32 expected_return = entry < 4;
        memset(&context,0x7E,sizeof context);
        strcpy((char *)context.field3030,texts[entry]);
        comparisons = 0;
        truth_output = &word;
        expected_calls = entry < 4 ? entry + 1 : 4;
        if (mutation == 2 && entry != 0) { expected_calls = 2; expected_return = 1; }
        expected_word = variant == 0 ? 0 : 0x7E7E7E7E;
        if (mutation == 1) expected_word = 0x11223344;
        if (variant == 0 && expected_return) expected_word = 1;
        if (variant == 0) func_002B58A8(&context,&word);
        else CHECK(func_002B5938(&context) == expected_return);
        CHECK(comparisons == expected_calls);
        CHECK(word == expected_word);
        CHECK(strcmp((char *)context.field3030,mutation == 2 ? "on" : texts[entry]) == 0);
    }
    /* The output clear can erase the content before the first comparison. */
    mutation = 0;
    memset(&context,0x7E,sizeof context);
    strcpy((char *)context.field3030,"yes");
    comparisons = 0;
    func_002B58A8(&context,(u32 *)context.field3030);
    CHECK(comparisons == 4);
    CHECK(*(u32 *)context.field3030 == 0);
    strcpy((char *)context.field3030,"yes");
    comparisons = 0;
    CHECK(func_002B5938(&context) == 1);
    CHECK(comparisons == 1);
    CHECK(strcmp((char *)context.field3030,"yes") == 0);
}

static void copy_cases(void)
{
    static const char *texts[] = {"","abc","Case value"};
    unsigned entry, aliases, returned, index;
    signed char sentinel[4];
    for (entry = 0; entry < 3; ++entry) for (aliases = 0; aliases < 2; ++aliases)
    for (returned = 0; returned < 3; ++returned) {
        unsigned length = (unsigned)strlen(texts[entry]) + 1;
        u8 expected[32];
        memset(&context,0x7E,sizeof context);
        memset(&output,0x7E,sizeof output);
        memset(expected,0x7E,sizeof expected);
        strcpy((char *)context.field3030,texts[entry]);
        copy_output = aliases ? context.field1C : output.text;
        copy_result = returned == 0 ? copy_output : returned == 1 ? NULL : sentinel;
        memcpy(expected,texts[entry],length);
        copies = 0;
        CHECK(func_002B5880(&context,copy_output) == copy_result);
        CHECK(copies == 1);
        for (index = 0; index < sizeof expected; ++index) CHECK(((u8 *)copy_output)[index] == expected[index]);
        CHECK(strcmp((char *)context.field3030,texts[entry]) == 0);
    }
}

static void rewind_cases(void)
{
    GeorgeTextLookupContext expected;
    unsigned alias, repeat, index;
    static const unsigned word_offsets[] = {0,0x18,0x101C,0x1020,0x1024,0x1028,0x102C};
    static const unsigned byte_offsets[] = {0x1C,0x1030,0x2030,0x3030};
    const signed char external[] = "external";
    for (alias = 0; alias < 5; ++alias) {
        for (index = 0; index < sizeof context; ++index) ((u8 *)&context)[index] = (u8)(index * 53 + 11);
        context.field08 = alias == 0 ? external : alias == 1 ? context.field1C :
                          alias == 2 ? context.field1030 : alias == 3 ? context.field2030 : context.field3030;
        memcpy(&expected,&context,sizeof expected);
        expected.field0C = context.field08;
        expected.field14 = 0x8001DEAD;
        for (index = 0; index < sizeof word_offsets / sizeof word_offsets[0]; ++index)
            memset((u8 *)&expected + word_offsets[index],0,4);
        for (index = 0; index < sizeof byte_offsets / sizeof byte_offsets[0]; ++index)
            ((u8 *)&expected)[byte_offsets[index]] = 0;
        for (repeat = 0; repeat < 2; ++repeat) {
            func_002B59B0(&context);
            for (index = 0; index < sizeof context; ++index) CHECK(((u8 *)&context)[index] == ((u8 *)&expected)[index]);
        }
    }
}

int main(void)
{
    scanner_cases();
    truth_cases();
    copy_cases();
    rewind_cases();
    printf("text_content: %u checks, %u failures\n",checks,failures);
    return failures ? 1 : 0;
}
