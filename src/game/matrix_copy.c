#include "george/matrix_copy_identity.h"

/* All object representations are captured before publication, including
 * shifted overlap. The original reads rows 3,0,1,2 before writing 0,1,2,3. */
void func_002A1C08(void *output, const void *input)
{
    const unsigned char *source = (const unsigned char *)input;
    unsigned char *destination = (unsigned char *)output;
    unsigned char captured[64];
    unsigned i;
    for (i = 48; i < 64; ++i) captured[i] = source[i];
    for (i = 0; i < 48; ++i) captured[i] = source[i];
    for (i = 0; i < 64; ++i) destination[i] = captured[i];
}
