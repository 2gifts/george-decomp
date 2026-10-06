#include "george/script_object.h"

extern const GeorgeScriptVTable D_00448EB0;
extern void func_002AF100(void *object);
extern void func_002CCB10(GeorgeDeimosPoolNode *table, u32 key, const GeorgeDeimosValue *value);

GeorgeScriptObject *func_002D06E8(GeorgeScriptObject *object)
{
    object->table = 0;
    object->vtable = &D_00448EB0;
    return object;
}

void func_002D0700(GeorgeScriptObject *object, u32 flags)
{
    object->vtable = &D_00448EB0;
    func_002D0800(object);
    if ((flags & 1) != 0) {
        func_002AF100(object);
    }
}

/* Lazily create a local table, obtaining its secondary table through dispatch. */
GeorgeDeimosPoolNode *func_002D0790(GeorgeScriptObject *object)
{
    if (object->table == 0) {
        const GeorgeScriptVTable *vtable = object->vtable;
        GeorgeDeimosHashTable *parent = vtable->parent20.function(
            (u8 *)object + vtable->parent20.this_adjust);
        GeorgeDeimosPoolNode *table = func_002CD348(parent);
        object->table = table;
        func_002CD0B8(table);
        vtable = object->vtable;
        vtable->initialize10.function((u8 *)object + vtable->initialize10.this_adjust, object->table);
    }
    return object->table;
}

void func_002D0800(GeorgeScriptObject *object)
{
    if (object->table != 0) {
        GeorgeDeimosValue zero;
        zero.tag = 6;
        zero.payload.bits = 0;
        zero.subtype = 0;
        func_002CCB10(object->table, 0x66A3F26Cu, &zero);
        func_002CD130(object->table);
        object->table = 0;
    }
}
