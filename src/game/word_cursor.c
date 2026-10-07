#include "george/word_cursor.h"

GeorgeWordCursorResult george_word_cursor_read_primary(u32 *owner)
{
    u32 cursor = owner[2];
    u32 successor = cursor + 1u;
    u32 address = (u32)owner + (cursor << 2) + 0x10u;
    u32 element = *(const u32 *)address;
    owner[2] = successor;
    return (GeorgeWordCursorResult)(signed long long)(s32)element;
}

GeorgeWordCursorResult george_word_cursor_read_secondary(u32 *owner)
{
    u32 cursor = owner[3];
    u32 successor = cursor + 1u;
    u32 address = (u32)owner + (cursor << 2) + 0x810u;
    u32 element = *(const u32 *)address;
    owner[3] = successor;
    return (GeorgeWordCursorResult)(signed long long)(s32)element;
}

GeorgeWordCursorResult george_word_cursor_peek_secondary(const u32 *owner)
{
    u32 cursor = owner[3];
    u32 address = (u32)owner + (cursor << 2) + 0x810u;
    u32 element = *(const u32 *)address;
    return (GeorgeWordCursorResult)(signed long long)(s32)element;
}
