/*
 * Build adapter, 2026-10-06.
 * The recovered strcasecmp body calls the public ctype functions. Newlib's
 * GNU C headers normally expand these names as macros. This forced include
 * selects the observed function-call form while leaving upstream C unchanged.
 */
#ifndef GEORGE_CTYPE_FUNCTION_CALLS_H
#define GEORGE_CTYPE_FUNCTION_CALLS_H
#include <ctype.h>
#undef tolower
#undef toupper
#endif
