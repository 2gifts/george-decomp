#ifndef GEORGE_DEIMOS_INTERPRETER_H
#define GEORGE_DEIMOS_INTERPRETER_H

#include "george/deimos.h"

/* code is a word-addressed bytecode stream. destination is an absolute slot
 * in the shared value array; -1 discards the result. */
void func_002CBC48(void *code, s32 destination) GEORGE_SAVE128;

#endif
