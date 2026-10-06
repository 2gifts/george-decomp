#ifndef GEORGE_ARENA_EXTENSIONS_H
#define GEORGE_ARENA_EXTENSIONS_H

#include "george/arena_buffers.h"

void func_002B8C00(u32 count,u32 alignment_exponent,void **first,void **second,
                 void **third,void **fourth) GEORGE_SAVE128;
void func_002B8D80(GeorgeArenaBuffer *,void *base,u32 capacity);
void func_002B8DA0(GeorgeArenaBuffer *,GeorgeArenaReset callback,void *context);
void func_002B8DB0(GeorgeArenaBuffer *);
void func_002B8DD0(GeorgeArenaBuffer *,u32 alignment_exponent);

#endif
