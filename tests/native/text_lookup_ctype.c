/* Compile the unchanged licensed public newlib table for the native harness.
 * These classification macros are the original newlib ctype.h values; MinGW's
 * unrelated host ctype header uses different names. No table bytes are copied. */
#define _CONST const
#define _U 1
#define _L 2
#define _N 4
#define _S 8
#define _P 16
#define _C 32
#define _X 64
#define _B 128
#include "../../src/runtime/ctype_table.c"
