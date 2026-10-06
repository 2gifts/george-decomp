#include "george/deimos_interpreter.h"
#include "george/deimos_calls.h"
#include "george/deimos_pool.h"
#include "george/string_algorithms.h"

typedef unsigned long long DeimosBits64;
typedef union DeimosScratch {
    char text[64];
    GeorgeDeimosValue value;
} DeimosScratch;
typedef char deimos_interpreter_bits64[(sizeof(DeimosBits64) == 8) ? 1 : -1];

extern GeorgeDeimosValue *D_00474F48;
extern GeorgeDeimosValue D_00474748[];
extern s32 D_0048176C;
extern GeorgeDeimosFrame D_00481770[];
extern const char D_00448960[], D_00448968[];
extern float func_0037B238(float value, float divisor);
extern DeimosBits64 func_00374848(float value);
extern s32 func_00373250(DeimosBits64 left, DeimosBits64 right);
extern DeimosBits64 func_00372CC0(DeimosBits64 left, DeimosBits64 right);
extern s32 func_003952C8(char *buffer, const char *format, ...);
extern char *func_00393B74(char *destination, const char *source);
extern char *func_00393758(char *destination, const char *source);
extern s32 func_00393A28(const char *left, const char *right);

/* Wrap addresses as the original word shifts/addu instructions do. Signed
 * extended destinations are deliberately not masked back to an 8-bit index. */
#define SLOT(base, index) \
    ((GeorgeDeimosValue *)((u32)(base) + (u32)(index) * 8u))
#define WORD_AT(code, index) (*(const u32 *)((u32)(code) + (u32)(index) * 4u))
#define FIRST(word) (((word) >> 8) & 0xFFu)
#define SECOND(word) (((word) >> 16) & 0xFFu)
#define HIGH(word) ((s32)(word) >> 24)
#define WIDE8(word) ((s32)(word) >> 8)
#define WIDE16(word) ((s32)(word) >> 16)

static __inline__ u32 truth(const GeorgeDeimosValue *value)
{
    s16 tag = (s16)value->tag;
    return tag == 1 ? value->payload.bits : tag != 0;
}

/* String operands retain their pool buffers; floats are formatted in the two
 * reused 64-byte scratch areas. Other tags yield the original null operand.
 * Every scalar is reloaded after callbacks, including conversion/formatting.
 * The threshold is the complete soft-double encoding of float 0.01. */
#define FORMAT_OPERAND(value, scratch, output) do { \
    s16 tag = (s16)(value)->tag; \
    output = NULL; \
    if (tag == 3) { \
        output = (const char *)((GeorgeDeimosPoolNode *)(value)->payload.pointer)->payload; \
    } else if (tag == 2) { \
        DeimosBits64 remainder = func_00374848(func_0037B238((value)->payload.scalar, 1.0f)); \
        if (func_00373250(remainder, 0ULL) < 0) \
            remainder = func_00372CC0(0ULL, remainder); \
        if (func_00373250(remainder, 0x3F847AE140000000ULL) < 0) { \
            func_003952C8((scratch).text, D_00448960, (s32)(value)->payload.scalar); \
        } else { \
            DeimosBits64 converted = func_00374848((value)->payload.scalar); \
            func_003952C8((scratch).text, D_00448968, converted); \
        } \
        output = (scratch).text; \
    } \
} while (0)

void func_002CBC48(void *code, s32 destination)
{
    DeimosScratch scratch0, scratch1;
    u32 pc = 0;
    u32 done = 0;
    do {
        u32 instruction = WORD_AT(code, pc);
        ++pc;
        switch (instruction & 0xFFu) {
        case 0:
            break;
        case 1: {
            GeorgeDeimosValue *output = SLOT(D_00474F48, WIDE8(instruction));
            output->subtype = 0;
            output->tag = 0;
            break;
        }
        case 2: {
            GeorgeDeimosValue *output = SLOT(D_00474F48, FIRST(instruction));
            output->payload.bits = (u32)WIDE16(instruction);
            output->tag = 1;
            output->subtype = 0;
            break;
        }
        case 3: {
            GeorgeDeimosValue *output = SLOT(D_00474F48, WIDE8(instruction));
            output->payload.bits = WORD_AT(code, pc++);
            output->tag = 2;
            output->subtype = 0;
            break;
        }
        case 4: {
            const signed char *text = (const signed char *)((u32)code + pc * 4u);
            s32 output_index = WIDE8(instruction);
            GeorgeDeimosPoolNode *node;
            GeorgeDeimosValue *output;
            pc += (func_00295050(text) + 4u) >> 2;
            node = func_002CD528((u32)text);
            output = SLOT(D_00474F48, output_index);
            output->tag = 3;
            output->payload.pointer = node;
            output->subtype = 0;
            break;
        }
        case 5: {
            GeorgeDeimosValue *output = SLOT(D_00474F48, WIDE8(instruction));
            u32 bits = WORD_AT(code, pc++);
            output->subtype = 0;
            output->tag = 6;
            output->payload.bits = bits;
            break;
        }
        case 6: {
            GeorgeDeimosValue *base = D_00474F48;
            *SLOT(base, WIDE16(instruction)) = *SLOT(base, FIRST(instruction));
            break;
        }
        case 7: {
            GeorgeDeimosValue *found = func_002CDD60(WORD_AT(code, pc++));
            GeorgeDeimosValue *output = SLOT(D_00474F48, WIDE8(instruction));
            if (found != NULL) *output = *found;
            else { output->subtype = 0; output->tag = 0; }
            break;
        }
        case 8: {
            GeorgeDeimosValue *value = SLOT(D_00474F48, WIDE8(instruction));
            u32 key = WORD_AT(code, pc++);
            func_002CDCA0(key, value);
            break;
        }
        case 9: {
            u32 source_key = WORD_AT(code, pc++);
            u32 destination_key = WORD_AT(code, pc++);
            GeorgeDeimosValue *value = func_002CDD60(source_key);
            func_002CDCA0(destination_key, value);
            break;
        }
        case 10:
        case 11:
        case 12:
        case 13: {
            GeorgeDeimosValue *base = D_00474F48;
            float left = SLOT(base, FIRST(instruction))->payload.scalar;
            float right = SLOT(base, SECOND(instruction))->payload.scalar;
            GeorgeDeimosValue *output = SLOT(base, HIGH(instruction));
            float result;
            switch (instruction & 0xFFu) {
            case 10: result = left + right; break;
            case 11: result = left - right; break;
            case 12: result = left * right; break;
            default: result = left / right; break;
            }
            output->tag = 2;
            output->subtype = 0;
            output->payload.scalar = result;
            break;
        }
        case 14: {
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosValue *left = SLOT(base, FIRST(instruction));
            GeorgeDeimosValue *right = SLOT(base, SECOND(instruction));
            const char *left_text, *right_text;
            u32 length;
            GeorgeDeimosPoolNode *node;
            GeorgeDeimosValue *output;
            FORMAT_OPERAND(left, scratch0, left_text);
            FORMAT_OPERAND(right, scratch1, right_text);
            length = func_00295050((const signed char *)left_text);
            length += func_00295050((const signed char *)right_text);
            node = func_002CD5A0(length + 1u);
            func_00393B74((char *)node->payload, left_text);
            func_00393758((char *)node->payload, right_text);
            output = SLOT(D_00474F48, HIGH(instruction));
            output->tag = 3;
            output->payload.pointer = node;
            output->subtype = 0;
            break;
        }
        case 15: {
            GeorgeDeimosValue *base = D_00474F48;
            u32 result = truth(SLOT(base, FIRST(instruction))) == 0;
            GeorgeDeimosValue *output = SLOT(base, WIDE16(instruction));
            output->payload.bits = result;
            output->tag = 1;
            output->subtype = 0;
            break;
        }
        case 16:
        case 17: {
            u32 unequal = (instruction & 0xFFu) == 17;
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosValue *left = SLOT(base, FIRST(instruction));
            GeorgeDeimosValue *right = SLOT(base, SECOND(instruction));
            s16 tag = (s16)left->tag;
            u32 result = unequal;
            GeorgeDeimosValue *output;
            if (tag == (s16)right->tag) {
                if (tag == 2) {
                    result = unequal ? left->payload.scalar != right->payload.scalar :
                                       left->payload.scalar == right->payload.scalar;
                } else if (tag == 3) {
                    s32 comparison = func_00393A28(
                        (const char *)((GeorgeDeimosPoolNode *)left->payload.pointer)->payload,
                        (const char *)((GeorgeDeimosPoolNode *)right->payload.pointer)->payload);
                    /* String inequality keeps strcmp's complete return word. */
                    result = unequal ? (u32)comparison : comparison == 0;
                } else if (tag >= 1 && tag <= 6) {
                    result = unequal ? left->payload.bits != right->payload.bits :
                                       left->payload.bits == right->payload.bits;
                } else {
                    result = unequal ? 0 : 1;
                }
            }
            output = SLOT(D_00474F48, HIGH(instruction));
            output->payload.bits = result;
            output->tag = 1;
            output->subtype = 0;
            break;
        }
        case 18:
        case 19: {
            u32 greater = (instruction & 0xFFu) == 19;
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosValue *left = SLOT(base, FIRST(instruction));
            GeorgeDeimosValue *right = SLOT(base, SECOND(instruction));
            u32 result;
            GeorgeDeimosValue *output;
            if ((s16)left->tag == 3) {
                s32 comparison = func_00393A28(
                    (const char *)((GeorgeDeimosPoolNode *)left->payload.pointer)->payload,
                    (const char *)((GeorgeDeimosPoolNode *)right->payload.pointer)->payload);
                result = greater ? comparison > 0 : (u32)comparison >> 31;
                base = D_00474F48;
            } else {
                result = greater ? right->payload.scalar < left->payload.scalar :
                                   left->payload.scalar < right->payload.scalar;
            }
            output = SLOT(base, HIGH(instruction));
            output->payload.bits = result;
            output->tag = 1;
            output->subtype = 0;
            break;
        }
        case 20:
        case 21: {
            GeorgeDeimosValue *base = D_00474F48;
            u32 left = truth(SLOT(base, FIRST(instruction)));
            u32 right = truth(SLOT(base, SECOND(instruction)));
            u32 result;
            GeorgeDeimosValue *output;
            if ((instruction & 0xFFu) == 20) result = left != 0 && right != 0;
            else result = left != 0 || right != 0;
            if ((instruction & 0xFFu) == 21) base = D_00474F48;
            output = SLOT(base, HIGH(instruction));
            output->payload.bits = result;
            output->tag = 1;
            output->subtype = 0;
            break;
        }
        case 22: {
            s32 output_index = WIDE8(instruction);
            GeorgeDeimosPoolNode *node = func_002CD348(NULL);
            GeorgeDeimosValue *output = SLOT(D_00474F48, output_index);
            output->tag = 4;
            output->payload.pointer = node;
            output->subtype = 0;
            break;
        }
        case 23: {
            GeorgeDeimosPoolNode *table = SLOT(D_00474F48, FIRST(instruction))->payload.pointer;
            u32 key = WORD_AT(code, pc++);
            GeorgeDeimosValue *found = func_002CD990((GeorgeDeimosHashTable *)table, key);
            GeorgeDeimosValue *output = SLOT(D_00474F48, WIDE16(instruction));
            if (found != NULL) *output = *found;
            else { output->subtype = 0; output->tag = 0; }
            break;
        }
        case 24: {
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosValue *table_slot = SLOT(base, FIRST(instruction));
            u32 key = func_002CD908(SLOT(base, SECOND(instruction)));
            GeorgeDeimosValue *found = func_002CD990(table_slot->payload.pointer, key);
            GeorgeDeimosValue *output = SLOT(D_00474F48, HIGH(instruction));
            if (found != NULL) *output = *found;
            else { output->subtype = 0; output->tag = 0; }
            break;
        }
        case 25: {
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosPoolNode *table = SLOT(base, FIRST(instruction))->payload.pointer;
            u32 key = WORD_AT(code, pc++);
            func_002CCB10(table, key, SLOT(base, WIDE16(instruction)));
            break;
        }
        case 26: {
            GeorgeDeimosValue *base = D_00474F48;
            GeorgeDeimosValue *table_slot = SLOT(base, FIRST(instruction));
            u32 key = func_002CD908(SLOT(base, SECOND(instruction)));
            GeorgeDeimosValue *value = SLOT(D_00474F48, HIGH(instruction));
            func_002CCB10(table_slot->payload.pointer, key, value);
            break;
        }
        case 27: {
            GeorgeDeimosPoolNode *table = SLOT(D_00474F48, SECOND(instruction))->payload.pointer;
            s32 iterator_index = (s32)FIRST(instruction);
            s32 output_index = HIGH(instruction);
            GeorgeDeimosPoolNode *node = func_002CD2B0();
            ((GeorgeDeimosTableIterator *)node->payload)->table = table;
            ((GeorgeDeimosTableIterator *)node->payload)->current = NULL;
            ((GeorgeDeimosTableIterator *)node->payload)->bucket = 0;
            /* Same scratch as the left formatted string; subtype is untouched. */
            scratch0.value.tag = 7;
            scratch0.value.payload.pointer = node;
            *SLOT(D_00474F48, iterator_index) = scratch0.value;
            func_002CDAB0(iterator_index, output_index);
            break;
        }
        case 28:
            func_002CDAB0((s32)FIRST(instruction), WIDE16(instruction));
            break;
        case 29:
            pc = (u32)WIDE8(instruction);
            break;
        case 30:
        case 31: {
            u32 condition = truth(SLOT(D_00474F48, FIRST(instruction)));
            if ((instruction & 0xFFu) == 30 ? condition != 0 : condition == 0)
                pc = (u32)WIDE16(instruction);
            break;
        }
        case 32: {
            GeorgeDeimosPoolNode *node = SLOT(D_00474F48, FIRST(instruction))->payload.pointer;
            s32 result_index = (s32)WORD_AT(code, pc++);
            func_002CCC58(node, (s32)SECOND(instruction),
                         (s32)(instruction >> 24), result_index);
            break;
        }
        case 33:
            if (destination != -1) {
                GeorgeDeimosValue *output = SLOT(D_00474748, destination);
                output->subtype = 0;
                output->tag = 0;
            }
            done = 1;
            break;
        case 34: {
            GeorgeDeimosValue *value = SLOT(D_00474F48, WIDE8(instruction));
            if (destination != -1) *SLOT(D_00474748, destination) = *value;
            done = 1;
            break;
        }
        case 35:
            D_00481770[D_0048176C].unknown04 = (u32)WIDE8(instruction);
            break;
        default:
            break;
        }
    } while (done == 0);
}

#undef FORMAT_OPERAND
#undef HIGH
#undef WIDE16
#undef WIDE8
#undef SECOND
#undef FIRST
#undef WORD_AT
#undef SLOT
