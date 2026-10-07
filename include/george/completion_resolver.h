#ifndef GEORGE_COMPLETION_RESOLVER_H
#define GEORGE_COMPLETION_RESOLVER_H

#include "george/actor_states2.h"
#include "george/compiler.h"

/* These numeric views describe only accessed storage. The SGI source uses
 * genuine member templates; their numeric aliases describe the observed
 * this/reference ABI rather than assert original class or typedef names. */
typedef struct GeorgeCompletionNode {
    struct GeorgeCompletionNode *field00;
    u32 field04;
    void *field08;
} GeorgeCompletionNode;

typedef struct GeorgeCompletionTable {
    u8 unknown00[4];
    GeorgeActorPointerRange field04;
    u32 field10;
} GeorgeCompletionTable;

typedef struct GeorgeCompletionIterator {
    GeorgeCompletionNode *field00;
    GeorgeCompletionTable *field04;
} GeorgeCompletionIterator;

typedef struct GeorgeCompletionResolver {
    u8 unknown00[4];
    GeorgeCompletionTable field04;
    u32 field18;
    /* Output slot addresses are computed from +0x1C. No capacity is inferred. */
} GeorgeCompletionResolver;

typedef struct GeorgeCompletionVirtual {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, s32);
} GeorgeCompletionVirtual;

typedef unsigned long long GeorgeCompletionPrime;
typedef char george_completion_prime_width[(sizeof(GeorgeCompletionPrime) == 8) ? 1 : -1];
typedef char george_completion_node_width[(sizeof(GeorgeCompletionNode) == 12) ? 1 : -1];
typedef char george_completion_table_width[(sizeof(GeorgeCompletionTable) == 20) ? 1 : -1];
typedef char george_completion_iterator_width[(sizeof(GeorgeCompletionIterator) == 8) ? 1 : -1];
#define GEORGE_COMPLETION_OFFSET(type, field, offset) \
    typedef char george_completion_offset_##type##_##field[ \
        (offsetof(type, field) == (offset)) ? 1 : -1]
GEORGE_COMPLETION_OFFSET(GeorgeCompletionTable, field04, 4);
GEORGE_COMPLETION_OFFSET(GeorgeCompletionTable, field10, 0x10);
GEORGE_COMPLETION_OFFSET(GeorgeCompletionResolver, field18, 0x18);
GEORGE_COMPLETION_OFFSET(GeorgeCompletionVirtual, invoke, 4);
#undef GEORGE_COMPLETION_OFFSET

#ifdef __cplusplus
extern "C" {
#endif

void *func_002BF550(void *object) GEORGE_SAVE128;
signed char *func_002BF580(void *object, u32 key) GEORGE_SAVE128;
GeorgeCompletionTable *func_002BF748(GeorgeCompletionTable *table) GEORGE_SAVE128;
void func_002BF0F0(GeorgeActorPointerRange *range, void **position,
                   u32 count, void *const *value);
void func_002BEDF0(void *object, s32 flags) GEORGE_SAVE128;
void func_002BF418(GeorgeCompletionTable *table);
GeorgeCompletionIterator *func_002BF4B8(GeorgeCompletionIterator *iterator);
void func_00104B10(void *descriptor, s32 flags) GEORGE_SAVE128;
void func_002BD340(void) GEORGE_SAVE128;

#ifdef __cplusplus
}
#endif

#endif
