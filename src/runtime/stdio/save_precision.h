#ifndef GEORGE_STDIO_SAVE_PRECISION_H
#define GEORGE_STDIO_SAVE_PRECISION_H
#include <stdio.h>
#include "george/compiler.h"
extern int __sread(void *, char *, int) GEORGE_SAVE128;
extern int __swrite(void *, const char *, int) GEORGE_SAVE128;
extern fpos_t __sseek(void *, fpos_t, int) GEORGE_SAVE128;
extern int __srget(FILE *) GEORGE_SAVE128;
#endif
