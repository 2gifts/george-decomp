#ifndef GEORGE_ARENA_EXTENSIONS_TEMPLATE_H
#define GEORGE_ARENA_EXTENSIONS_TEMPLATE_H

/* Ordinary C definition copied from reviewed func_002B8E88. The complete
 * original reset bodies are byte-identical. Keep a real natural entry symbol
 * and leave the published arena source/header unchanged. */
#define GEORGE_DEFINE_ARENA_RESET(name) \
void name(GeorgeArenaBuffer *arena) \
{ \
    u8 *cursor=arena->cursor; \
    if ((u32)arena->high_water<(u32)cursor) arena->high_water=cursor; \
    arena->cursor=arena->base; \
}

#endif
