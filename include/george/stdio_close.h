#ifndef GEORGE_STDIO_CLOSE_H
#define GEORGE_STDIO_CLOSE_H

#include <stdio.h>
#include "george/types.h"

/* fclose uses the authentic FILE declaration supplied by the target runtime. */
int fclose(FILE *stream);

/* Both observed fread callers ignore the original incidental v0 contents.
 * Void is a caller-compatible convention, not a generic memcpy return ABI. */
void func_003947D8(void *destination, const void *source, u32 count);

#endif
