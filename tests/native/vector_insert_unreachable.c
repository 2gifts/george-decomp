/* Link-only opaque interfaces for a deliberately unexecuted historical GNU
 * stream branch. No fake ostream object is constructed or dereferenced. */
#include <stdlib.h>
unsigned char unreachable_cerr[4] __asm__("_cerr");
void *unreachable_stream(void *,const char *) __asm__("__ZN7ostreamlsEPKc");
void *unreachable_endl(void *) __asm__("__Z4endlR7ostream");
void *unreachable_stream(void *self,const char *text) { (void)self;(void)text;abort();return 0; }
void *unreachable_endl(void *self) { (void)self;abort();return 0; }
