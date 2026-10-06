#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "george/text_numbers.h"

const signed char D_004476C8[3] = {0, '\'', '"'};
const signed char *D_003FD258[5], *D_003FD270[5], *D_003FD288[5];
const signed char D_004476F0[] = "%d", D_004476F8[] = "%d %d";
const signed char D_00447700[] = "%d %d %d", D_00447710[] = "%d %d %d %d";
const signed char D_00447720[] = "%f", D_00447728[] = "%f %f";
const signed char D_00447730[] = "%f %f %f", D_00447740[] = "%f %f %f %f";
const signed char D_00447750[] = "%hd", D_00447758[] = "%hd %hd";
const signed char D_00447760[] = "%hd %hd %hd", D_00447770[] = "%hd %hd %hd %hd";
static const signed char *formats[3][4] = {
    {D_004476F0,D_004476F8,D_00447700,D_00447710},
    {D_00447720,D_00447728,D_00447730,D_00447740},
    {D_00447750,D_00447758,D_00447760,D_00447770}
};
static GeorgeTextLookupContext context;
static union { u32 words[8]; float floats[8]; s16 halves[16]; } output;
static union { u32 words[8]; signed char text[32]; } key_storage;
static void *pointers[4];
static unsigned checks, failures, group, lanes, width, mode, mutation;
static unsigned comparisons, copies, scans;
static s32 reported_count;
static const char *expected_text;
static u8 before_scan[4][4];
#define CHECK(expression) do { ++checks; if (!(expression)) { \
    if (failures < 20) printf("failure line %u group %u lanes %u mode %u mutation %u\n", \
                            __LINE__, group, lanes, mode, mutation); ++failures; } } while (0)

u32 func_00295050(const signed char *text) { return (u32)strlen((const char *)text); }
s32 func_00393E48(const signed char *left, const signed char *right, u32 count)
{ return strncmp((const char *)left, (const char *)right, count); }
static unsigned lower(unsigned byte) { return byte >= 'A' && byte <= 'Z' ? byte + 32 : byte; }

s32 func_003983E8(const signed char *left, const signed char *right)
{
    s32 result;
    while (*left != 0 && lower((u8)*left) == lower((u8)*right)) { ++left; ++right; }
    result = (s32)lower((u8)*left) - (s32)lower((u8)*right);
    ++comparisons;
    if (comparisons == 1 && mutation == 1) {
        u32 word = 0x11223344;
        memcpy(pointers[0], &word, width);
    }
    return result;
}

signed char *func_00393B74(signed char *destination, const signed char *source)
{
    ++copies;
    do { *destination++ = *source; } while (*source++ != 0);
    return NULL; /* Original callers ignore this return. */
}

s32 func_00395350(const signed char *text, const signed char *format, ...)
{
    va_list arguments;
    unsigned index;
    s32 converted;
    ++scans;
    CHECK(format == formats[group][lanes - 1]);
    CHECK(strcmp((const char *)text, expected_text) == 0);
    va_start(arguments, format);
    for (index = 0; index < lanes; ++index) {
        void *pointer;
        if (group == 0) pointer = va_arg(arguments, s32 *);
        else if (group == 1) pointer = va_arg(arguments, float *);
        else pointer = va_arg(arguments, s16 *);
        CHECK(pointer == pointers[index]);
        CHECK(memcmp(pointer, before_scan[index], width) == 0);
    }
    va_end(arguments);
    if (mutation == 2) {
        u32 word = 0x55667788;
        memcpy(pointers[lanes - 1], &word, width);
    }
    va_start(arguments, format);
    converted = vsscanf((const char *)text, (const char *)format, arguments);
    va_end(arguments);
    CHECK(converted == (mode == 0 ? (s32)lanes : mode == 1 ? 1 : 0));
    return reported_count;
}

static s32 invoke(const signed char *key)
{
    if (group == 0) {
        if (lanes == 1) return func_002B4840(&context,key,pointers[0]);
        if (lanes == 2) return func_002B4960(&context,key,pointers[0],pointers[1]);
        if (lanes == 3) return func_002B4A90(&context,key,pointers[0],pointers[1],pointers[2]);
        return func_002B4BD0(&context,key,pointers[0],pointers[1],pointers[2],pointers[3]);
    }
    if (group == 1) {
        if (lanes == 1) return func_002B4D20(&context,key,pointers[0]);
        if (lanes == 2) return func_002B4E40(&context,key,pointers[0],pointers[1]);
        if (lanes == 3) return func_002B4F78(&context,key,pointers[0],pointers[1],pointers[2]);
        return func_002B50C0(&context,key,pointers[0],pointers[1],pointers[2],pointers[3]);
    }
    if (lanes == 1) return func_002B5220(&context,key,pointers[0]);
    if (lanes == 2) return func_002B5340(&context,key,pointers[0],pointers[1]);
    if (lanes == 3) return func_002B5478(&context,key,pointers[0],pointers[1],pointers[2]);
    return func_002B55C0(&context,key,pointers[0],pointers[1],pointers[2],pointers[3]);
}

static void fill_context(unsigned route, const char *text)
{
    memset(&context, 0x7E, sizeof context);
    strcpy((char *)context.field1030, route == 1 ? "key" : "whole");
    if (route == 0) sprintf((char *)context.field2030, "KEY=\"%s\" key=999", text);
    else strcpy((char *)context.field2030, "other=99");
    if (route == 1) strcpy((char *)context.field3030, text);
    else if (route == 2) sprintf((char *)context.field3030, "key=\"%s\" key=999", text);
    else strcpy((char *)context.field3030, "other=99");
    comparisons = copies = scans = 0;
}

static void initialize_expected(u8 *bytes, const unsigned *offsets)
{
    unsigned index;
    for (index = 0; index < (group == 0 ? 1 : lanes); ++index)
        memset(bytes + offsets[index], 0, width);
    if (mutation == 1) {
        u32 word = 0x11223344;
        memcpy(bytes + offsets[0], &word, width);
    }
    for (index = 0; index < lanes; ++index)
        memcpy(before_scan[index], bytes + offsets[index], width);
}

static void conversion_expected(u8 *bytes, const unsigned *offsets)
{
    static const s32 integers[] = {11,-22,33,44};
    static const float floats[] = {1.25f,-2.5f,3.75f,-4.0f};
    static const s16 halves[] = {-32768,32767,-3,4};
    unsigned index, converted = mode == 0 ? lanes : mode == 1 ? 1 : 0;
    if (mutation == 2) {
        u32 word = 0x55667788;
        memcpy(bytes + offsets[lanes - 1], &word, width);
    }
    for (index = 0; index < converted; ++index) {
        const void *value = group == 0 ? (const void *)(integers + index) :
                            group == 1 ? (const void *)(floats + index) : (const void *)(halves + index);
        memcpy(bytes + offsets[index], value, width);
    }
}

static void basic_cases(void)
{
    unsigned route, aliases, index, returned;
    static const s32 reports[] = {-1,0,7};
    static const char *texts[3][3] = {
        {"11 -22 33 44", "11 nope", "nope"},
        {"1.25 -2.5 3.75 -4", "1.25 nope", "nope"},
        {"-32768 32767 -3 4", "-32768 nope", "nope"}
    };
    for (group = 0; group < 3; ++group) for (lanes = 1; lanes <= 4; ++lanes) {
        width = group == 2 ? 2 : 4;
        for (route = 0; route < 4; ++route) for (mode = 0; mode < 3; ++mode)
        for (aliases = 0; aliases < 3; ++aliases) for (mutation = 0; mutation < 3; ++mutation)
        for (returned = 0; returned < 3; ++returned) {
            unsigned offsets[4];
            u8 expected[32];
            for (index = 0; index < 4; ++index) {
                offsets[index] = 4 + (aliases == 1 ? 0 : aliases == 2 ? 3 - index : index) * width;
                pointers[index] = (u8 *)&output + offsets[index];
            }
            memset(&output, 0x7E, sizeof output);
            memset(expected, 0x7E, sizeof expected);
            initialize_expected(expected, offsets);
            expected_text = texts[group][mode];
            reported_count = reports[returned];
            fill_context(route, expected_text);
            CHECK(invoke((const signed char *)"key") == (route != 3));
            if (route != 3) conversion_expected(expected, offsets);
            for (index = 0; index < sizeof expected; ++index) CHECK(((u8 *)&output)[index] == expected[index]);
            CHECK(copies == (route != 3));
            CHECK(scans == (route != 3));
        }
    }
}

static void initial_alias_cases(void)
{
    unsigned index;
    mutation = 0; mode = 2; reported_count = -7; expected_text = "nope";
    for (group = 0; group < 3; ++group) for (lanes = 1; lanes <= 4; ++lanes) {
        width = group == 2 ? 2 : 4;
        memset(&output, 0x7E, sizeof output);
        memset(&key_storage, 0x7E, sizeof key_storage);
        strcpy((char *)key_storage.text, "key");
        for (index = 0; index < 4; ++index) pointers[index] = (u8 *)&output + 4 + width * index;
        pointers[0] = key_storage.text;
        fill_context(0, expected_text);
        CHECK(invoke(key_storage.text) == 0);
        CHECK(key_storage.text[0] == 0 && scans == 0 && copies == 0);
        CHECK(key_storage.text[width] == (width == 2 ? 'y' : 0x7E));
        if (lanes > 1) {
            memset(&output, 0x7E, sizeof output);
            strcpy((char *)key_storage.text, "key");
            pointers[0] = (u8 *)&output + 4;
            pointers[1] = key_storage.text;
            fill_context(0, expected_text);
            for (index = 0; index < lanes; ++index) {
                if (group != 0 || index == 0) memset(before_scan[index], 0, width);
                else memcpy(before_scan[index], pointers[index], width);
            }
            CHECK(invoke(key_storage.text) == (group == 0));
            CHECK(scans == (group == 0));
            CHECK(group == 0 ? strcmp((char *)key_storage.text,"key") == 0 : key_storage.text[0] == 0);
        }
        /* Clearing an attribute start removes that path; the whole-name
         * fallback uses captured text and scanner pointers into the context. */
        memset(&output, 0x7E, sizeof output);
        for (index = 0; index < 4; ++index) pointers[index] = (u8 *)&output + 4 + width * index;
        fill_context(1, expected_text);
        pointers[0] = context.field2030;
        for (index = 0; index < lanes; ++index) {
            if (group != 0 || index == 0) memset(before_scan[index], 0, width);
            else memcpy(before_scan[index], pointers[index], width);
        }
        CHECK(invoke((const signed char *)"key") == 1);
        CHECK(scans == 1 && copies == 1 && context.field2030[0] == 0);
        CHECK(context.field2030[width] == (width == 2 ? 'h' : 'r'));
    }
}

int main(void)
{
    basic_cases();
    initial_alias_cases();
    printf("text numbers: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
