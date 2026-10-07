#ifndef GEORGE_BUFFER_COMPLETION_H
#define GEORGE_BUFFER_COMPLETION_H

#include "george/buffer_ui.h"
#include "george/deimos.h"

/* Numeric names preserve the observed callback interface. The visitor uses
 * only the full key; value and context are supplied by the real traversals.
 * These effects-only declarations do not prove original source return types. */
void func_002162D0(u32 key, GeorgeDeimosValue *value, void *context) GEORGE_SAVE128;
void func_00217598(GeorgeBufferUi *ui, const signed char *candidate) GEORGE_SAVE128;

#endif
