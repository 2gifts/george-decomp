#ifndef GEORGE_ARENA_BUFFERS_H
#define GEORGE_ARENA_BUFFERS_H

#include "george/types.h"
#include "george/compiler.h"

typedef struct GeorgeArenaBuffer GeorgeArenaBuffer;
typedef void (*GeorgeArenaReset)(GeorgeArenaBuffer *);
/* Complete observed 24-byte descriptor. The callback's return is not consumed
 * by the recovered initializer; no new callback invocation is invented. */
struct GeorgeArenaBuffer {
    u8 *base;
    u32 capacity;
    u8 *cursor;
    u8 *high_water;
    GeorgeArenaReset reset;
    void *context;
};
typedef char arena_cursor_offset[(offsetof(GeorgeArenaBuffer,cursor)==8)?1:-1];
typedef char arena_callback_offset[(offsetof(GeorgeArenaBuffer,reset)==0x10)?1:-1];
typedef char arena_descriptor_size[(sizeof(GeorgeArenaBuffer)==24)?1:-1];
extern GeorgeArenaBuffer D_0046A0D8;

u8 *func_002B8E00(GeorgeArenaBuffer *,u32 bytes,u32 alignment_exponent);
void func_002B8E88(GeorgeArenaBuffer *);
void func_002B8EA8(void *base,u32 capacity);
void func_002B8EE8(void);
void func_002B8EF0(u32 alignment_exponent);
u8 *func_002B8F28(u32 bytes,u32 alignment_exponent);
u8 *func_002B8FB8(u32 bytes);
void func_002B9040(u32 count,u32 alignment_exponent,void **output) GEORGE_SAVE128;
void func_002B9120(u32 count,u32 alignment_exponent,void **first,void **second) GEORGE_SAVE128;
void func_002B9238(u32 count,u32 alignment_exponent,void **first,void **second,void **third) GEORGE_SAVE128;
void func_002B9388(u32 count,u32 alignment_exponent,void **first,void **second,void **third);

#endif
