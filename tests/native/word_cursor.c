#include <stdio.h>
#include "george/word_cursor.h"

#define WORDS 640u
typedef char pointer32[(sizeof(void *) == 4) ? 1 : -1];
typedef char result64[(sizeof(GeorgeWordCursorResult) == 8) ? 1 : -1];
#include "word_cursor_golden.h"

static u32 initial_word(u32 i, u32 salt)
{
    return 0x63CA8701u ^ (i * 0x1030521u) ^ salt;
}

int main(void)
{
    static const u32 boundaries[] = {0, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};
    static const GeorgeWordCursorResult signed_bits[] = {
        0, 0x7FFFFFFFULL, 0xFFFFFFFF80000000ULL, 0xFFFFFFFFFFFFFFFFULL
    };
    unsigned long checks = 0;
    unsigned int i, j;
    for (i = 0; i < 4; ++i) {
        if ((GeorgeWordCursorResult)(signed long long)(s32)boundaries[i] != signed_bits[i])
            return 2;
        ++checks;
    }
    for (i = 0; i < sizeof(word_cursor_golden) / sizeof(word_cursor_golden[0]); ++i) {
        const struct WordCursorGolden *g = &word_cursor_golden[i];
        u32 arena[WORDS], expected[WORDS];
        u32 ci = g->owner + (g->routine == 0 ? 2u : 3u);
        GeorgeWordCursorResult result;
        for (j = 0; j < WORDS; ++j) arena[j] = initial_word(j, g->salt);
        arena[ci] = g->cursor;
        if (g->element_cell != ci) arena[g->element_cell] = g->payload;
        for (j = 0; j < WORDS; ++j) expected[j] = arena[j];
        if (g->routine != 2) expected[ci] = g->cursor + 1u;
        if (g->routine == 0) result = george_word_cursor_read_primary(arena + g->owner);
        else if (g->routine == 1) result = george_word_cursor_read_secondary(arena + g->owner);
        else result = george_word_cursor_peek_secondary(arena + g->owner);
        if (result != g->result) {
            fprintf(stderr, "fixture %u lower64 %016llX != %016llX\n", i, result, g->result);
            return 1;
        }
        ++checks;
        for (j = 0; j < WORDS; ++j) {
            if (arena[j] != expected[j]) {
                fprintf(stderr, "fixture %u cell %u %08X != %08X\n", i, j, arena[j], expected[j]);
                return 1;
            }
            ++checks;
        }
    }
    printf("word cursor: %lu checks\n", checks);
    return 0;
}
