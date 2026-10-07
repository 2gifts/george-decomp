#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* Native-only byte-width linkage view of genuine separately instantiated
 * SGI allocator storage. No replacement definition or target binding. */
extern void *_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE[16];
/* Inactive GNU diagnostic link label only, following the already qualified
 * resolver observer closure. No ostream object exists or is used: every
 * diagnostic operator immediately fails before interpreting this address. */
unsigned char cerr[256];
void *clear_sgi_head(void)
{
    return _ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE[1];
}
void clear_sgi_unexecuted(void)
{
    fprintf(stderr,"unexpected genuine SGI reference path\n");exit(2);
}
void *george_sgi_engine_allocate(unsigned bytes)
{
    void *result=malloc(bytes);
    if (!result) clear_sgi_unexecuted();
    return result;
}
void george_sgi_engine_release(void *pointer)
{
    free(pointer);
}
/* Platform-library supporting bridge only. Historical empty-vector zero-byte
 * calls retain the observed GNU/native contract, not universal ISO null-input
 * behavior. No target source identity or machine memmove-read validation. */
void *clear_sgi_memmove(void *out,const void *in,unsigned bytes)
{
    return memmove(out,in,bytes);
}
