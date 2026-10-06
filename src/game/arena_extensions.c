#include "george/arena_extensions.h"
#include "arena_buffers_template.h"
#include "arena_extensions_template.h"

extern void *func_003936A0(void *destination,u32 value,u32 length);

void func_002B8C00(u32 count,u32 alignment_exponent,void **first,void **second,
                 void **third,void **fourth)
{
    u32 rounded=(count+3U)&~3U,start,other,last,tail;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,
        (rounded<<1)*12U+(rounded<<2)+(rounded<<1),alignment_exponent,start);
    other=start+rounded*12U;
    last=other+rounded*12U;
    tail=last+(rounded<<2);
    *first=(void *)start;
    *second=(void *)other;
    *third=(void *)last;
    *fourth=(void *)tail;
    if ((s32)count<(s32)rounded) {
        u32 offset=count*12U,length=(rounded-count)*12U;
        func_003936A0((void *)(start+offset),0,length);
        func_003936A0((void *)(other+offset),0,length);
        func_003936A0((void *)(last+(count<<2)),0,(rounded-count)<<2);
        func_003936A0((void *)(tail+(count<<1)),0,(rounded-count)<<1);
    }
}

void func_002B8D80(GeorgeArenaBuffer *arena,void *base,u32 capacity)
{
    arena->capacity=capacity;
    arena->high_water=(u8 *)base;
    arena->context=NULL;
    arena->base=(u8 *)base;
    arena->cursor=(u8 *)base;
    arena->reset=NULL;
}

void func_002B8DA0(GeorgeArenaBuffer *arena,GeorgeArenaReset callback,void *context)
{
    arena->context=context;
    arena->reset=callback;
}

GEORGE_DEFINE_ARENA_RESET(func_002B8DB0)

void func_002B8DD0(GeorgeArenaBuffer *arena,u32 alignment_exponent)
{
    GEORGE_ARENA_ALIGN(arena,alignment_exponent);
}
