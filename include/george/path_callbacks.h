#ifndef GEORGE_PATH_CALLBACKS_H
#define GEORGE_PATH_CALLBACKS_H

#include "george/path_curves.h"

/* Numeric callback signatures follow the actual original table dispatch.
 * Endpoint storage has format-specific scalar/packed offsets, not an inferred
 * original class layout. The third integer argument is the output storage. */
void func_002C03E8(const void *, const void *, void *, float);
void func_002C0560(const void *, const void *, void *, float);
void func_002C0C18(const void *, const void *, void *, float) GEORGE_SAVE128;
void func_002C19A8(const void *, const void *, void *, float);
void func_002C1AD0(const void *, const void *, void *, float);
void func_002C1BB0(const void *, const void *, void *, float);
void func_002C1E20(const void *, const void *, void *, float);
void func_002C1EF8(const void *, const void *, void *, float);
void func_002C1F50(const void *, const void *, void *, float);
void func_002C1F90(const void *, const void *, void *, float) GEORGE_SAVE128;
void func_002C2088(const void *, const void *, void *, float) GEORGE_SAVE128;
void func_002C21B0(const void *, const void *, void *, float) GEORGE_SAVE128;
void func_002C2300(const void *, const void *, void *, float);
void func_002C23C8(const GeorgeMathVec3 *, const void *, const void *,
                 float *, float *) GEORGE_SAVE128;
void func_002C2440(const GeorgeMathVec3 *, const void *, const void *,
                 float *, float *) GEORGE_SAVE128;

#endif
