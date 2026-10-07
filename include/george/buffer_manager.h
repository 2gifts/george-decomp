#ifndef GEORGE_BUFFER_MANAGER_H
#define GEORGE_BUFFER_MANAGER_H

#include "george/types.h"

/* Observed prefixes only; no allocation size or format capacity is inferred. */
typedef struct GeorgeBufferLine {
    s32 limit;
    s32 count;
    signed char *data;
} GeorgeBufferLine;

typedef struct GeorgeBufferManager {
    u32 flags;
    s32 line_count;
    u32 text_limit;
    GeorgeBufferLine **lines;
    GeorgeBufferLine *current;
    void *commands;
    u32 unknown18;
    s32 history_limit;
    s32 navigation;
    s32 history_count;
    signed char **history;
} GeorgeBufferManager;

typedef void (*GeorgeBufferCommand)(GeorgeBufferManager *, char *, u32);
typedef struct GeorgeBufferCommandPrefix {
    GeorgeBufferCommand callback;
    u32 data;
    char prefix[1];
} GeorgeBufferCommandPrefix;

#define GEORGE_BUFFER_OFFSET(type, field, offset) \
    typedef char george_buffer_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
GEORGE_BUFFER_OFFSET(GeorgeBufferLine, limit, 0);
GEORGE_BUFFER_OFFSET(GeorgeBufferLine, count, 4);
GEORGE_BUFFER_OFFSET(GeorgeBufferLine, data, 8);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, flags, 0);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, line_count, 4);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, text_limit, 8);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, lines, 0xC);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, current, 0x10);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, commands, 0x14);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, history_limit, 0x1C);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, navigation, 0x20);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, history_count, 0x24);
GEORGE_BUFFER_OFFSET(GeorgeBufferManager, history, 0x28);
GEORGE_BUFFER_OFFSET(GeorgeBufferCommandPrefix, callback, 0);
GEORGE_BUFFER_OFFSET(GeorgeBufferCommandPrefix, data, 4);
GEORGE_BUFFER_OFFSET(GeorgeBufferCommandPrefix, prefix, 8);
#undef GEORGE_BUFFER_OFFSET

/* Signed ctype indexing and buffer/count pointer arithmetic require the same
 * readable/writable addresses as the original. No null/capacity guards added. */
void func_002A4B70(GeorgeBufferManager *manager, signed char character);
void func_002A4DB8(GeorgeBufferManager *manager, const char *text);
u32 func_002A56C0(const GeorgeBufferManager *manager);

#endif
