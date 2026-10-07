#ifndef GEORGE_WORD_CURSOR_H
#define GEORGE_WORD_CURSOR_H

#include "george/types.h"

/* Scalar storage convention and sign-extended lower64 bits, not an original
 * class, declared return type, capacity, or upper128 ABI assertion. */
typedef unsigned long long GeorgeWordCursorResult;
GeorgeWordCursorResult george_word_cursor_read_primary(u32 *owner);
GeorgeWordCursorResult george_word_cursor_read_secondary(u32 *owner);
GeorgeWordCursorResult george_word_cursor_peek_secondary(const u32 *owner);

#endif
