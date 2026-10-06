#ifndef GEORGE_RING_H
#define GEORGE_RING_H

#include "george/deimos_calls.h"

/* Generic engine helpers operating on the already observed ring layout. */
void *func_002AF208(const GeorgeDeimosRing *ring, u32 index) GEORGE_SAVE128;
void func_002AF258(GeorgeDeimosRing *ring, u32 index) GEORGE_SAVE128;
u32 func_002AF358(const GeorgeDeimosRing *ring, void *output) GEORGE_SAVE128;
u32 func_002AF398(const GeorgeDeimosRing *ring, void *output) GEORGE_SAVE128;

#endif
