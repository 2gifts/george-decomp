#ifndef GEORGE_TEXT_LOOKUP_TEMPLATE_H
#define GEORGE_TEXT_LOOKUP_TEMPLATE_H

#include "george/text_lookup.h"

extern s32 func_003983E8(const signed char *left, const signed char *right);
extern signed char *func_00393B74(signed char *destination, const signed char *source);

/* Shared source for the complete, independently observed lookup sequence.
 * Macro expansion keeps each retail entry's original call graph and scratch
 * ownership; it introduces no new helper call or function-body assembly. All
 * pointer arguments are captured parameters/locals. The success/missing actions
 * must transfer control, preserving the existing entry's early-exit shape. */
#define GEORGE_TEXT_LOOKUP_COPY(context_, key_, destination_, success_, missing_) \
    { \
        signed char name[0x400]; \
        signed char value[0x400]; \
        signed char *cursor = (context_)->field2030; \
        for (;;) { \
            cursor = func_002B3EE0(cursor, name, value); \
            if (cursor == NULL) \
                break; \
            if (func_003983E8((key_), name) == 0) { \
                func_00393B74((destination_), value); \
                success_; \
            } \
        } \
        if (func_003983E8((key_), (context_)->field1030) == 0) { \
            func_00393B74((destination_), (context_)->field3030); \
            success_; \
        } \
        cursor = (context_)->field3030; \
        for (;;) { \
            cursor = func_002B3EE0(cursor, name, value); \
            if (cursor == NULL) { \
                missing_; \
            } \
            if (func_003983E8((key_), name) == 0) { \
                func_00393B74((destination_), value); \
                success_; \
            } \
        } \
    }

#endif
