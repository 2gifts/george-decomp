#include "george/buffer_manager.h"
#include "george/accessors.h"
#include "george/string_algorithms.h"
#include "george/compiler.h"

extern const char _ctype_[257];
extern u32 *func_002AAF50(void *array, s32 index);
extern char *func_00394010(char *destination, const char *source, u32 count);
extern char *func_00398628(const char *text, const char *pattern);

/* The original SLL/ADDU addressing wraps the product and sum to 32 bits. */
static __inline__ GeorgeBufferLine **line_slot(GeorgeBufferLine **lines, s32 index)
{
    return (GeorgeBufferLine **)((u32)lines + (u32)index * 4U);
}

static __inline__ signed char **history_slot(signed char **history, s32 index)
{
    return (signed char **)((u32)history + (u32)index * 4U);
}

static __inline__ void rotate_lines(GeorgeBufferManager *manager)
{
    GeorgeBufferLine **lines = manager->lines;
    s32 index = 1;
    s32 count = manager->line_count;
    GeorgeBufferLine *first = lines[0];

    if (index < count) {
        lines = manager->lines;
        do {
            GeorgeBufferLine **slot = line_slot(lines, index);
            index = (s32)((u32)index + 1U);
            slot[-1] = slot[0];
            count = manager->line_count;
            if (!(index < count))
                break;
            lines = manager->lines;
        } while (1);
    }
    *line_slot(manager->lines, (s32)((u32)index - 1U)) = first;
    first->count = 0;
    first->data[0] = 0;
    manager->current = first;
}

GEORGE_SAVE128 void func_002A4B70(GeorgeBufferManager *manager, signed char character)
{
    if (character == 10 || character == 13) {
        GeorgeBufferLine *line = manager->current;
        signed char *text = line->data;
        s32 index;

        if (func_00295050(text) != 0) {
            s32 count = manager->history_count;
            s32 limit = manager->history_limit;
            if (!(count < limit)) {
                signed char *first = manager->history[0];
                s32 position = 0;
                if (0 < (s32)((u32)limit - 1U)) {
                    signed char **history = manager->history;
                    do {
                        signed char **slot = history_slot(history, position);
                        position = (s32)((u32)position + 1U);
                        slot[0] = slot[1];
                        limit = (s32)((u32)manager->history_limit - 1U);
                        if (!(position < limit))
                            break;
                        history = manager->history;
                    } while (1);
                }
                *history_slot(manager->history, (s32)((u32)manager->history_limit - 1U)) = first;
                (*history_slot(manager->history, (s32)((u32)manager->history_limit - 1U)))[0] = 0;
                count = (s32)((u32)manager->history_limit - 1U);
                manager->history_count = count;
                manager->navigation = count;
                count = manager->history_count;
            }
            func_00394010((char *)*history_slot(manager->history, count), (const char *)text,
                          manager->text_limit);
            count = (s32)((u32)manager->history_count + 1U);
            manager->navigation = count;
            manager->history_count = count;
        }
        rotate_lines(manager);
        index = 0;
        while (index < (s32)func_002AAF88(manager->commands)) {
            GeorgeBufferCommandPrefix *command =
                (GeorgeBufferCommandPrefix *)func_002AAF50(manager->commands, index);
            char *match = func_00398628((const char *)line->data, command->prefix);
            if (match == (char *)line->data) {
                u32 data = command->data;
                GeorgeBufferCommand callback = command->callback;
                callback(manager, match, data);
            }
            index = (s32)((u32)index + 1U);
        }
    } else {
        GeorgeBufferLine *line = manager->current;
        s32 count = line->count;
        /* Retain the signed displacement even below the declared table. The
         * caller must supply readable target addresses; native table evidence
         * covers only nonnegative characters and EOF (-1). */
        u8 classification = *(const u8 *)((u32)_ctype_ + 1U + (u32)(s32)character);
        s32 limit = line->limit;
        signed char lower = (classification & 1U) ?
            (signed char)(character + 32) : character;
        if (count < limit) {
            s32 next = (s32)((u32)count + 1U);
            line->data[count] = lower;
            line->count = next;
            line->data[next] = 0;
        }
    }
}

void func_002A4DB8(GeorgeBufferManager *manager, const char *text)
{
    while (*text != 0) {
        signed char character = (signed char)(u8)*text;
        if (character == 10 || character == 13) {
            ++text;
            rotate_lines(manager);
        } else {
            GeorgeBufferLine *line = manager->current;
            s32 count = line->count;
            s32 limit = line->limit;
            if (count < limit) {
                s32 next = (s32)((u32)count + 1U);
                line->data[count] = character;
                line->count = next;
                line->data[next] = 0;
            }
            limit = line->limit;
            count = line->count;
            ++text;
            if (!(count < limit))
                rotate_lines(manager);
        }
    }
}

u32 func_002A56C0(const GeorgeBufferManager *manager)
{
    return manager->flags & 0x80000000U;
}
