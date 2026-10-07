#ifndef GEORGE_SGI_CONTAINER_DUPLICATES_H
#define GEORGE_SGI_CONTAINER_DUPLICATES_H

#include "george/actor_states2.h"

/* Consumed iterator words only. Node.next at0 and raw unsigned key bits at4
 * do not establish any clone node size, pair/value field, class or lifetime.
 * Some callers treat key bits at4 as a payload pointer. The old constructed
 * SGI pair<const unsigned, void*> is a qualified semantic counterpart only. */
typedef struct GeorgeSgiCloneIteratorPrefix {
    void *node;
    void *table;
} GeorgeSgiCloneIteratorPrefix;

typedef char george_sgi_clone_iterator_words[
    (sizeof(GeorgeSgiCloneIteratorPrefix) == 8) ? 1 : -1];
typedef char george_sgi_clone_iterator_table[
    (offsetof(GeorgeSgiCloneIteratorPrefix, table) == 4) ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif

/* Existing range prefix and actual this/position/count/value-reference ABI.
 * Production is the unchanged whole published SGI method; no numeric
 * forwarding definitions or new template instantiations are introduced. */
void func_0024B0E8(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024B4A8(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024B870(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024BC30(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024BFF8(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024C3C0(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024C788(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_0024CB50(GeorgeActorPointerRange *, void **, u32, void *const *);
void func_002BBDA0(GeorgeActorPointerRange *, void **, u32, void *const *);

GeorgeSgiCloneIteratorPrefix *func_0024D160(GeorgeSgiCloneIteratorPrefix *);
GeorgeSgiCloneIteratorPrefix *func_0024D1F8(GeorgeSgiCloneIteratorPrefix *);
GeorgeSgiCloneIteratorPrefix *func_0024D290(GeorgeSgiCloneIteratorPrefix *);
GeorgeSgiCloneIteratorPrefix *func_0024D3E0(GeorgeSgiCloneIteratorPrefix *);
GeorgeSgiCloneIteratorPrefix *func_0024DE80(GeorgeSgiCloneIteratorPrefix *);

#ifdef __cplusplus
}
#endif

#endif
