#ifndef GEORGE_FUNCTION_TEMPLATES_H
#define GEORGE_FUNCTION_TEMPLATES_H

/* Shared recovered C bodies. Each emitted entry is independently compiled,
 * linked and compared in full at its own original address. */
#include "george/types.h"

#define GEORGE_DEFINE_FREE_FORWARD(name, target) \
    void name(void *memory) { target(memory); }

#define GEORGE_DEFINE_SET_BIT0(name) \
    void name(GeorgeFlags18 *object) { object->field18 |= 1; }

#define GEORGE_DEFINE_CLEAR_BIT0(name) \
    void name(GeorgeFlags18 *object) { object->field18 &= ~1u; }

#define GEORGE_DEFINE_FADE_UPDATE(name) \
    void name(GeorgeFade4C *object, float step) { \
        if (object->field18 != 0) { \
            float current = object->field1C; \
            if (0.0f < current) { \
                float scaled = step * object->field4C; \
                object->field1C = current - scaled; \
            } \
        } \
    }

/* Requires george/heap.h for the observed field at offset1C. The macro
 * describes that layout only, without asserting every caller's class. */
#define GEORGE_DEFINE_FIELD1C_BIT0_GETTER(name) \
    u32 name(const GeorgeHeap *object) { return object->flags & 1; }

#endif
