#ifndef GEORGE_DEIMOS_VECTORS_H
#define GEORGE_DEIMOS_VECTORS_H

#include "george/compiler.h"
#include "george/deimos.h"
#include "george/math_helpers.h"

/* A zero key terminates this observed key/type/payload word stream. */
GeorgeDeimosPoolNode *func_002CFFC0(const u32 *records) GEORGE_SAVE128;
void func_002D00A0(GeorgeDeimosHashTable *table, GeorgeMathVec3 *vector, float *angle) GEORGE_SAVE128;
GeorgeDeimosPoolNode *func_002D0178(const GeorgeMathVec3 *vector, float angle) GEORGE_SAVE128;
void func_002CFE18(void) GEORGE_SAVE128;

#endif
