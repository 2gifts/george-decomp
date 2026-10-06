#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "george/deimos_interpreter.h"
#include "george/deimos_calls.h"
#include "george/deimos_pool.h"
#include "george/string_algorithms.h"
typedef unsigned long long DeimosBits64;

GeorgeDeimosValue D_00474748[256];
GeorgeDeimosValue alternate[256];
GeorgeDeimosValue *D_00474F48 = D_00474748;
s32 D_0048176C;
GeorgeDeimosFrame D_00481770[16];
const char D_00448960[] = "%d";
const char D_00448968[] = "%f";
static GeorgeDeimosPoolNode nodes[32];
static unsigned char buffers[32][512];
static GeorgeDeimosValue dictionary[32];
static unsigned count, checks, failures;
static unsigned used[36];
static GeorgeDeimosValue *expected_input, *mutated_slot;
static GeorgeDeimosPoolNode *expected_table, replacement_table;
static GeorgeDeimosPoolNode *last_node;
static unsigned key_mutation, lookup_mutation, compare_mutation, copy_mutation;
static unsigned float_mutation, formatted, iterator_calls, dispatch_calls;
static unsigned allocator_calls, expected_key;
static s32 compare_return;
static unsigned compare_override;
static double last_formatted_double;
static s32 call_offset, call_count, call_destination;

#define CHECK(condition) do { ++checks; if (!(condition)) { ++failures; \
    printf("FAIL line %d: %s\n", __LINE__, #condition); } } while (0)
#define OP(op,a,b,c) ((u32)(op)|((u32)(a)<<8)|((u32)(b)<<16)|((u32)(c)<<24))
#define WIDE(op,value) ((u32)(op)|((u32)(value)<<8))

static DeimosBits64 double_bits(double value)
{
    DeimosBits64 bits;
    memcpy(&bits, &value, 8);
    return bits;
}
static double bits_double(DeimosBits64 bits)
{
    double value;
    memcpy(&value, &bits, 8);
    return value;
}
DeimosBits64 func_00374848(float value) { return double_bits((double)value); }
s32 func_00373250(DeimosBits64 left, DeimosBits64 right)
{
    double a = bits_double(left), b = bits_double(right);
    return a < b ? -1 : a > b ? 1 : 0;
}
DeimosBits64 func_00372CC0(DeimosBits64 left, DeimosBits64 right)
{
    return double_bits(bits_double(left) - bits_double(right));
}
float func_0037B238(float value, float divisor)
{
    float result = (float)fmod((double)value, (double)divisor);
    CHECK(divisor == 1.0f);
    if (float_mutation) {
        mutated_slot->payload.scalar = 100.25f;
        float_mutation = 0;
    }
    return result;
}
s32 func_003952C8(char *buffer, const char *format, ...)
{
    va_list args;
    int result;
    ++formatted;
    va_start(args, format);
    if (format == D_00448960) result = sprintf(buffer, format, va_arg(args, s32));
    else { CHECK(format == D_00448968); last_formatted_double = bits_double(va_arg(args, DeimosBits64)); result = sprintf(buffer, format, last_formatted_double); }
    va_end(args);
    return result;
}
u32 func_00295050(const signed char *text) { return (u32)strlen((const char *)text); }
char *func_00393B74(char *destination, const char *source)
{
    char *result = strcpy(destination, source);
    if (copy_mutation) {
        strcpy((char *)buffers[31], "redirect:");
        last_node->payload = (u32)buffers[31];
        D_00474F48 = alternate;
    }
    return result;
}
char *func_00393758(char *destination, const char *source) { return strcat(destination, source); }
s32 func_00393A28(const char *left, const char *right)
{
    s32 result = compare_override ? compare_return : strcmp(left, right);
    if (compare_mutation) D_00474F48 = alternate;
    return result;
}
static GeorgeDeimosPoolNode *new_node(u8 type)
{
    GeorgeDeimosPoolNode *node = &nodes[count];
    node->type = type;
    node->payload = (u32)buffers[count];
    ++count;
    ++allocator_calls;
    last_node = node;
    return node;
}
GeorgeDeimosPoolNode *func_002CD528(u32 text)
{
    GeorgeDeimosPoolNode *node = new_node(1);
    node->payload = text;
    return node;
}
GeorgeDeimosPoolNode *func_002CD5A0(u32 size) { CHECK(size <= 512); return new_node(0); }
GeorgeDeimosPoolNode *func_002CD348(GeorgeDeimosHashTable *secondary) { CHECK(secondary == NULL); return new_node(2); }
GeorgeDeimosPoolNode *func_002CD2B0(void) { return new_node(3); }
GeorgeDeimosValue *func_002CDD60(u32 key)
{
    if (lookup_mutation) D_00474F48 = alternate;
    return key == 31 ? NULL : &dictionary[key % 32];
}
void func_002CDCA0(u32 key, const GeorgeDeimosValue *value)
{
    if (expected_input != NULL) CHECK(value == expected_input);
    dictionary[key % 32] = *value;
}
u32 func_002CD908(const GeorgeDeimosValue *value)
{
    u32 key = value->payload.bits;
    if (key_mutation) {
        mutated_slot->payload.pointer = &replacement_table;
        D_00474F48 = alternate;
    }
    return key;
}
GeorgeDeimosValue *func_002CD990(GeorgeDeimosHashTable *table, u32 key)
{
    if (expected_table != NULL) CHECK((GeorgeDeimosPoolNode *)table == expected_table);
    CHECK(key == expected_key);
    if (lookup_mutation) D_00474F48 = alternate;
    return key == 31 ? NULL : &dictionary[key % 32];
}
void func_002CCB10(GeorgeDeimosPoolNode *table, u32 key, const GeorgeDeimosValue *value)
{
    CHECK(table == expected_table);
    CHECK(key == expected_key);
    CHECK(value == expected_input);
    dictionary[key % 32] = *value;
}
void func_002CDAB0(s32 iterator_index, s32 output_index)
{
    GeorgeDeimosPoolNode *node = D_00474F48[iterator_index].payload.pointer;
    GeorgeDeimosTableIterator *iterator = (GeorgeDeimosTableIterator *)node->payload;
    ++iterator_calls;
    CHECK(iterator->table == expected_table);
    CHECK(iterator->bucket == 0);
    CHECK(iterator->current == NULL);
    D_00474F48[output_index] = dictionary[0];
}
void func_002CCC58(GeorgeDeimosPoolNode *node, s32 offset, s32 count_arg, s32 destination)
{
    ++dispatch_calls;
    CHECK(node == expected_table);
    CHECK(offset == call_offset);
    CHECK(count_arg == call_count);
    CHECK(destination == call_destination);
    D_00474F48 = alternate;
}

static void reset(void)
{
    memset(D_00474748, 0, sizeof(D_00474748));
    memset(alternate, 0, sizeof(alternate));
    memset(nodes, 0, sizeof(nodes));
    memset(dictionary, 0, sizeof(dictionary));
    memset(D_00481770, 0, sizeof(D_00481770));
    D_00474F48 = D_00474748;
    D_0048176C = 2;
    count = 0;
    expected_input = mutated_slot = NULL;
    expected_table = NULL;
    key_mutation = lookup_mutation = compare_mutation = copy_mutation = 0;
    float_mutation = formatted = iterator_calls = dispatch_calls = allocator_calls = 0;
    compare_override = 0;
}
static void run(u32 *code, s32 destination)
{
    unsigned i;
    /* Coverage is tracked from the test fixtures, excluding their immediate words. */
    for (i = 0; i < 36; ++i) if (code[0] % 256 == i) used[i] = 1;
    func_002CBC48(code, destination);
}
static GeorgeDeimosValue value(u16 tag, u32 bits)
{
    GeorgeDeimosValue result;
    result.tag = tag;
    result.subtype = 0xA55A;
    result.payload.bits = bits;
    return result;
}
static void basic(void)
{
    u32 code[8];
    unsigned opcode;
    reset();
    D_00474748[3] = value(6, 0xAABBCCDD);
    code[0] = WIDE(1, 3); code[1] = 33; run(code, -1);
    CHECK(D_00474748[3].tag == 0 && D_00474748[3].subtype == 0 && D_00474748[3].payload.bits == 0xAABBCCDD);
    reset();
    code[0] = OP(2, 4, 0xFE, 0xFF); code[1] = 33; run(code, -1);
    CHECK(D_00474748[4].tag == 1 && D_00474748[4].payload.bits == 0xFFFFFFFE);
    for (opcode = 3; opcode <= 5; opcode += 2) {
        reset(); code[0] = WIDE(opcode, 4); code[1] = 0xC0600000; code[2] = 33; run(code, -1);
        CHECK(D_00474748[4].tag == (opcode == 3 ? 2 : 6) && D_00474748[4].subtype == 0);
        CHECK(D_00474748[4].payload.bits == 0xC0600000);
    }
    reset(); code[0] = WIDE(4, 3); memcpy(code + 1, "abcde", 6); code[3] = 33; run(code, -1);
    CHECK(allocator_calls == 1 && D_00474748[3].tag == 3);
    CHECK(strcmp((char *)((GeorgeDeimosPoolNode *)D_00474748[3].payload.pointer)->payload, "abcde") == 0);
    reset(); D_00474748[2] = value(5, 0x12345678); code[0] = OP(6, 2, 3, 0); code[1] = 33; run(code, -1);
    CHECK(memcmp(D_00474748 + 2, D_00474748 + 3, 8) == 0);
    code[0] = OP(6, 2, 2, 0); run(code, -1); CHECK(D_00474748[2].payload.bits == 0x12345678);
    reset(); dictionary[2] = value(3, 0x87654321); lookup_mutation = 1;
    code[0] = WIDE(7, 3); code[1] = 2; code[2] = 33; run(code, -1);
    CHECK(memcmp(alternate + 3, dictionary + 2, 8) == 0 && D_00474748[3].tag == 0);
    alternate[3] = value(6, 0x11112222); code[1] = 31; run(code, -1);
    CHECK(alternate[3].tag == 0 && alternate[3].subtype == 0 && alternate[3].payload.bits == 0x11112222);
    reset(); D_00474748[3] = value(5, 0x44445555); expected_input = D_00474748 + 3;
    code[0] = WIDE(8, 3); code[1] = 2; code[2] = 33; run(code, -1); CHECK(dictionary[2].payload.bits == 0x44445555);
    reset(); dictionary[2] = value(6, 0x11223344); expected_input = dictionary + 2;
    code[0] = 9; code[1] = 2; code[2] = 4; code[3] = 33; run(code, -1);
    CHECK(memcmp(dictionary + 2, dictionary + 4, 8) == 0);
    reset(); code[0] = WIDE(22, 4); code[1] = 33; run(code, -1);
    CHECK(D_00474748[4].tag == 4 && D_00474748[4].subtype == 0 && last_node->type == 2);
    reset(); code[0] = WIDE(35, -7); code[1] = 33; run(code, -1); CHECK(D_00481770[2].unknown04 == (u32)-7);
    reset(); code[0] = 0; code[1] = 255; code[2] = 36; code[3] = OP(2, 3, 99, 0); code[4] = 33;
    run(code, -1); CHECK(D_00474748[3].payload.bits == 99);
}
static void arithmetic_and_truth(void)
{
    unsigned opcode, a, b;
    u32 code[3];
    float inputs[] = {-3.0f, 0.0f, 2.5f, 11.0f};
    for (opcode = 10; opcode <= 13; ++opcode) for (a = 0; a < 4; ++a) for (b = 0; b < 4; ++b) {
        float expected;
        reset(); D_00474748[2].payload.scalar = inputs[a]; D_00474748[3].payload.scalar = inputs[b];
        if (opcode == 10) expected = inputs[a] + inputs[b];
        else if (opcode == 11) expected = inputs[a] - inputs[b];
        else if (opcode == 12) expected = inputs[a] * inputs[b];
        else expected = inputs[a] / inputs[b];
        code[0] = OP(opcode, 2, 3, 2); code[1] = 33; run(code, -1);
        CHECK(D_00474748[2].payload.scalar == expected || (expected != expected && D_00474748[2].payload.scalar != D_00474748[2].payload.scalar));
        CHECK(D_00474748[2].tag == 2 && D_00474748[2].subtype == 0);
    }
    for (a = 0; a < 9; ++a) for (b = 0; b < 3; ++b) {
        unsigned truth_value = a == 0 ? 0 : a == 1 ? b != 0 : 1;
        reset(); D_00474748[2] = value(a == 8 ? 0xFFFF : a, b);
        code[0] = OP(15, 2, 3, 0); code[1] = 33; run(code, -1);
        CHECK(D_00474748[3].payload.bits == !truth_value && D_00474748[3].tag == 1);
    }
    for (opcode = 20; opcode <= 21; ++opcode) for (a = 0; a < 4; ++a) for (b = 0; b < 4; ++b) {
        reset(); D_00474748[2] = value(a/2, a%2); D_00474748[3] = value(b/2, b%2);
        code[0] = OP(opcode, 2, 3, 4); code[1] = 33; run(code, -1);
        CHECK(D_00474748[4].payload.bits == (opcode == 20 ? (a == 3 && b == 3) : (a == 3 || b == 3)));
    }
}
static void strings_and_comparisons(void)
{
    u32 code[8];
    unsigned opcode, a, b;
    static const float integer_edges[] = {2147483520.0f, -2147483648.0f, 0.0f, -0.0f};
    static const char *integer_text[] = {"2147483520tail", "-2147483648tail", "0tail", "0tail"};
    reset(); nodes[0].payload = (u32)"a"; nodes[1].payload = (u32)"b";
    for (opcode = 16; opcode <= 19; ++opcode) {
        reset(); nodes[0].payload = (u32)"a"; nodes[1].payload = (u32)"b";
        D_00474748[2] = value(3, (u32)&nodes[0]); D_00474748[3] = value(3, (u32)&nodes[1]);
        compare_override = compare_mutation = 1; compare_return = -7;
        code[0] = OP(opcode, 2, 3, 4); code[1] = 33; run(code, -1);
        CHECK(alternate[4].payload.bits == (opcode == 17 ? (u32)-7 : opcode == 18 ? 1u : 0u));
        CHECK(alternate[4].tag == 1 && alternate[4].subtype == 0 && D_00474748[4].tag == 0);
    }
    for (opcode = 16; opcode <= 17; ++opcode) for (a = 0; a < 8; ++a) for (b = 0; b < 8; ++b) {
        u32 expected;
        if (a == 3 || b == 3) continue;
        reset(); D_00474748[2] = value(a, 9); D_00474748[3] = value(b, 9);
        if (a == 2) D_00474748[2].payload.scalar = 9.0f;
        if (b == 2) D_00474748[3].payload.scalar = 9.0f;
        expected = a == b;
        if (opcode == 17) expected = !expected;
        code[0] = OP(opcode, 2, 3, 4); code[1] = 33; run(code, -1);
        CHECK(D_00474748[4].payload.bits == expected);
    }
    for (opcode = 16; opcode <= 19; ++opcode) {
        reset(); D_00474748[2] = value(2, 0x7FC00000); D_00474748[3] = value(2, 0x3F800000);
        code[0] = OP(opcode, 2, 3, 4); code[1] = 33; run(code, -1);
        CHECK(D_00474748[4].payload.bits == (opcode == 17));
    }
    reset(); D_00474748[2].tag = 2; D_00474748[2].payload.scalar = -12.0f;
    D_00474748[3].tag = 2; D_00474748[3].payload.scalar = 12.5f;
    code[0] = OP(14, 2, 3, 4); code[1] = 33; run(code, -1);
    CHECK(strcmp((char *)last_node->payload, "-1212.500000") == 0 && formatted == 2);
    reset(); D_00474748[2].tag = 2; D_00474748[2].payload.scalar = 12.5f;
    nodes[30].payload = (u32)"tail"; D_00474748[3] = value(3, (u32)&nodes[30]);
    copy_mutation = float_mutation = 1; mutated_slot = D_00474748 + 2;
    code[0] = OP(14, 2, 3, 4); code[1] = 33; run(code, -1);
    CHECK(strcmp((char *)last_node->payload, "redirect:tail") == 0);
    CHECK(alternate[4].payload.pointer == last_node && D_00474748[4].tag == 0);
    CHECK(formatted == 1);
    CHECK(last_formatted_double == 100.25);
    for (a = 0; a < 4; ++a) {
        reset(); D_00474748[2].tag = 2; D_00474748[2].payload.scalar = integer_edges[a];
        nodes[30].payload = (u32)"tail"; D_00474748[3] = value(3, (u32)&nodes[30]);
        code[0] = OP(14, 2, 3, 4); code[1] = 33; run(code, -1);
        CHECK(strcmp((char *)last_node->payload, integer_text[a]) == 0);
    }
}
static void tables_and_scratch(void)
{
    u32 code[8];
    unsigned opcode;
    for (opcode = 23; opcode <= 26; ++opcode) {
        reset(); dictionary[2] = value(6, 0x99);
        D_00474748[2] = value(4, (u32)&nodes[30]); D_00474748[3] = value(6, 2);
        D_00474748[4] = value(5, 0x99); alternate[4] = value(6, 0x88);
        expected_table = &nodes[30]; expected_key = 2;
        if (opcode == 24 || opcode == 26) { key_mutation = 1; mutated_slot = D_00474748 + 2; expected_table = &replacement_table; }
        if (opcode == 23 || opcode == 25) { code[0] = OP(opcode, 2, 4, 0); code[1] = 2; code[2] = 33; }
        else { code[0] = OP(opcode, 2, 3, 4); code[1] = 33; }
        if (opcode == 25) expected_input = D_00474748 + 4;
        if (opcode == 26) expected_input = alternate + 4;
        lookup_mutation = 1;
        run(code, -1);
        if (opcode <= 24) CHECK(memcmp(alternate + 4, dictionary + 2, 8) == 0);
        else CHECK(dictionary[2].payload.bits == (opcode == 25 ? 0x99u : 0x88u));
    }
    reset(); D_00474748[2] = value(4, (u32)&nodes[30]); expected_table = &nodes[30];
    D_00474748[3] = value(2, 0); D_00474748[3].payload.scalar = 12.5f;
    nodes[29].payload = (u32)"x"; D_00474748[4] = value(3, (u32)&nodes[29]);
    dictionary[0] = value(6, 0xAB);
    code[0] = OP(14, 3, 4, 5); code[1] = OP(27, 6, 2, 7); code[2] = 33;
    run(code, -1); used[27] = 1;
    CHECK(iterator_calls == 1 && D_00474748[6].tag == 7);
    CHECK(D_00474748[6].subtype == ((u16)'5' << 8 | (u16)'.'));
    CHECK(D_00474748[7].payload.bits == 0xAB);
    code[0] = OP(28, 6, 8, 0); code[1] = 33; run(code, -1);
    CHECK(iterator_calls == 2 && D_00474748[8].payload.bits == 0xAB);
}
static void control_and_destinations(void)
{
    u32 code[8];
    unsigned opcode, condition;
    reset(); D_00474748[4] = value(6, 0x1234); code[0] = 33; run(code, 4);
    CHECK(D_00474748[4].tag == 0 && D_00474748[4].subtype == 0 && D_00474748[4].payload.bits == 0x1234);
    reset(); alternate[3] = value(5, 0xABCDEF); D_00474F48 = alternate;
    code[0] = WIDE(34, 3); run(code, 7); CHECK(memcmp(D_00474748 + 7, alternate + 3, 8) == 0);
    D_00474748[7] = value(6, 0x55); run(code, -1); CHECK(D_00474748[7].payload.bits == 0x55);
    reset(); code[0] = WIDE(29, 2); code[1] = OP(2, 4, 99, 0); code[2] = OP(2, 4, 22, 0); code[3] = 33;
    run(code, -1); CHECK(D_00474748[4].payload.bits == 22);
    for (opcode = 30; opcode <= 31; ++opcode) for (condition = 0; condition <= 1; ++condition) {
        reset(); D_00474748[3] = value(1, condition);
        code[0] = OP(opcode, 3, 2, 0); code[1] = OP(2, 4, 9, 0); code[2] = 33; run(code, -1);
        CHECK(D_00474748[4].payload.bits == ((opcode == 30 ? condition : !condition) ? 0u : 9u));
    }
    reset(); D_00474748[3] = value(5, (u32)&nodes[30]); expected_table = &nodes[30];
    call_offset = 255; call_count = 255; call_destination = -1;
    code[0] = OP(32, 3, 255, 255); code[1] = (u32)-1; code[2] = OP(2, 4, 7, 0); code[3] = 33;
    run(code, -1); CHECK(dispatch_calls == 1 && alternate[4].payload.bits == 7 && D_00474748[4].tag == 0);
    /* Negative extended destinations are legitimate offsets within a window. */
    reset(); D_00474F48 = D_00474748 + 8;
    code[0] = WIDE(5, -1); code[1] = 0xDEADBEEF; code[2] = WIDE(34, -1); run(code, 1);
    CHECK(D_00474748[7].tag == 6 && D_00474748[1].payload.bits == 0xDEADBEEF);
}
int main(void)
{
    unsigned i;
    basic(); arithmetic_and_truth(); strings_and_comparisons(); tables_and_scratch(); control_and_destinations();
    for (i = 0; i < 36; ++i) CHECK(used[i]);
    printf("%u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
