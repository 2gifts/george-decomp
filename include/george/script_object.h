#ifndef GEORGE_SCRIPT_OBJECT_H
#define GEORGE_SCRIPT_OBJECT_H

#include "george/deimos.h"

/* Observed old EE C++ dispatch entries: signed this adjustment and function. */
typedef struct GeorgeScriptInitializeMethod {
    s16 this_adjust;
    u16 unknown02;
    void (*function)(void *adjusted_this, GeorgeDeimosPoolNode *table);
} GeorgeScriptInitializeMethod;

typedef struct GeorgeScriptParentMethod {
    s16 this_adjust;
    u16 unknown02;
    GeorgeDeimosHashTable *(*function)(void *adjusted_this);
} GeorgeScriptParentMethod;

typedef struct GeorgeScriptVTable {
    u8 unknown00[0x10];
    GeorgeScriptInitializeMethod initialize10;
    u8 unknown18[8];
    GeorgeScriptParentMethod parent20;
} GeorgeScriptVTable;

typedef struct GeorgeScriptObject {
    GeorgeDeimosPoolNode *table;
    const GeorgeScriptVTable *vtable;
} GeorgeScriptObject;

typedef char script_object_size[(sizeof(GeorgeScriptObject) == 8) ? 1 : -1];
typedef char script_method_size[(sizeof(GeorgeScriptParentMethod) == 8) ? 1 : -1];
typedef char script_vtable_initialize_offset[(offsetof(GeorgeScriptVTable, initialize10) == 0x10) ? 1 : -1];
typedef char script_vtable_parent_offset[(offsetof(GeorgeScriptVTable, parent20) == 0x20) ? 1 : -1];
typedef char script_method_function_offset[(offsetof(GeorgeScriptParentMethod, function) == 4) ? 1 : -1];

GeorgeScriptObject *func_002D06E8(GeorgeScriptObject *object);
void func_002D0700(GeorgeScriptObject *object, u32 flags);
GeorgeDeimosPoolNode *func_002D0790(GeorgeScriptObject *object);
void func_002D0800(GeorgeScriptObject *object);

#endif
