#ifndef GEORGE_DEIMOS_H
#define GEORGE_DEIMOS_H

#include "george/types.h"

/* Names describe observed use; original class names are still unknown. */
typedef union GeorgeDeimosPayload {
    u32 bits;
    float scalar;
    void *pointer;
} GeorgeDeimosPayload;

typedef struct GeorgeDeimosValue {
    u16 tag;
    u16 subtype;
    GeorgeDeimosPayload payload;
} GeorgeDeimosValue;

typedef struct GeorgeDeimosPoolNode {
    u8 type;
    u8 unknown01;
    u16 unknown02;
    u16 references;
    s16 active_slot;
    u32 field08;
    u32 payload;
} GeorgeDeimosPoolNode;

typedef struct GeorgeDeimosHashNode {
    struct GeorgeDeimosHashNode *next;
    u32 key;
    GeorgeDeimosValue value;
} GeorgeDeimosHashNode;

typedef struct GeorgeDeimosHashTable {
    u8 unknown00[8];
    struct GeorgeDeimosHashTable *field08;
    GeorgeDeimosHashNode **buckets;
} GeorgeDeimosHashTable;

typedef struct GeorgeDeimosContext {
    u8 unknown00[0x14];
    void *field14;
} GeorgeDeimosContext;

typedef struct GeorgeDeimosFrame {
    GeorgeDeimosContext *context;
    u32 unknown04;
} GeorgeDeimosFrame;

typedef struct GeorgeDeimosCallable {
    u32 field00;
    u32 field04;
    u32 count08;
    u32 *data0C;
    u32 field10;
    u32 field14;
    u32 field18;
} GeorgeDeimosCallable;

typedef void (*GeorgeDeimosVisitor)(u32 key, GeorgeDeimosValue *value, void *context);

#define DEIMOS_OFFSET(type, member, offset) \
    typedef char deimos_offset_##type##_##member[(offsetof(type, member) == (offset)) ? 1 : -1]
#define DEIMOS_SIZE(type, size) \
    typedef char deimos_size_##type[(sizeof(type) == (size)) ? 1 : -1]
DEIMOS_SIZE(GeorgeDeimosValue, 8);
DEIMOS_SIZE(GeorgeDeimosPoolNode, 16);
DEIMOS_SIZE(GeorgeDeimosHashNode, 16);
DEIMOS_SIZE(GeorgeDeimosFrame, 8);
DEIMOS_SIZE(GeorgeDeimosCallable, 0x1C);
DEIMOS_OFFSET(GeorgeDeimosValue, payload, 4);
DEIMOS_OFFSET(GeorgeDeimosPoolNode, references, 4);
DEIMOS_OFFSET(GeorgeDeimosPoolNode, active_slot, 6);
DEIMOS_OFFSET(GeorgeDeimosPoolNode, payload, 12);
DEIMOS_OFFSET(GeorgeDeimosHashNode, key, 4);
DEIMOS_OFFSET(GeorgeDeimosHashNode, value, 8);
DEIMOS_OFFSET(GeorgeDeimosHashTable, field08, 8);
DEIMOS_OFFSET(GeorgeDeimosHashTable, buckets, 12);
DEIMOS_OFFSET(GeorgeDeimosContext, field14, 0x14);
DEIMOS_OFFSET(GeorgeDeimosCallable, data0C, 0x0C);
DEIMOS_OFFSET(GeorgeDeimosCallable, field18, 0x18);
#undef DEIMOS_OFFSET
#undef DEIMOS_SIZE

void func_002CD0B8(GeorgeDeimosPoolNode *node);
void func_002CC9B8(GeorgeDeimosPoolNode *node);
void func_002CD130(GeorgeDeimosPoolNode *node);
GeorgeDeimosPoolNode *func_002CD248(void);
GeorgeDeimosPoolNode *func_002CD2B0(void);
GeorgeDeimosPoolNode *func_002CD348(GeorgeDeimosHashTable *secondary);
GeorgeDeimosPoolNode *func_002CD528(u32 payload);
GeorgeDeimosPoolNode *func_002CD5A0(u32 size);
GeorgeDeimosPoolNode *func_002CD630(const char *text);
GeorgeDeimosPoolNode *func_002CD6E0(u32 field00, u32 field04, const u32 *data, u32 count, u32 field18);
GeorgeDeimosPoolNode *func_002CD810(u32 field00, u32 field04, u32 field10, u32 field14);
GeorgeDeimosValue *func_002CD990(GeorgeDeimosHashTable *table, u32 key);
void func_002CDA20(GeorgeDeimosHashTable *table, GeorgeDeimosVisitor callback, void *context);
void func_002CDB88(u32 key, GeorgeDeimosValue *value, const u32 *preserved_keys);
void func_002CDC00(GeorgeDeimosHashTable *table, const u32 *preserved_keys);
void *func_002CDF90(void);
void func_002D0560(void *unused, s32 destination);
void func_002CF298(void *unused, s32 destination);
void func_002D05E0(void *unused, s32 destination);

#endif
