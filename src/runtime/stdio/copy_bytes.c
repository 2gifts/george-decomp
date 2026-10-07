#include "george/stdio_close.h"

void func_003947D8(void *destination, const void *source, u32 count)
{
    unsigned char *out = destination;
    const unsigned char *in = source;
    while (count--) {
        *out++ = *in++;
    }
}
