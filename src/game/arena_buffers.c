#include "george/arena_buffers.h"
#include "arena_buffers_template.h"

extern void *func_003936A0(void *destination,u32 value,u32 length);

u8 *func_002B8E00(GeorgeArenaBuffer *arena,u32 bytes,u32 alignment_exponent)
{
    u32 start;
    GEORGE_ARENA_RESERVE(arena,bytes,alignment_exponent,start);
    return (u8 *)start;
}

void func_002B8E88(GeorgeArenaBuffer *arena)
{
    u8 *cursor=arena->cursor;
    if ((u32)arena->high_water<(u32)cursor) arena->high_water=cursor;
    arena->cursor=arena->base;
}

void func_002B8EA8(void *base,u32 capacity)
{
    D_0046A0D8.base=(u8 *)base;
    D_0046A0D8.capacity=capacity;
    D_0046A0D8.high_water=(u8 *)base;
    D_0046A0D8.reset=func_002B8E88;
    D_0046A0D8.context=NULL;
    D_0046A0D8.cursor=(u8 *)base;
    func_002B8EE8();
}

void func_002B8EE8(void)
{
}

void func_002B8EF0(u32 alignment_exponent)
{
    GEORGE_ARENA_ALIGN(&D_0046A0D8,alignment_exponent);
}

u8 *func_002B8F28(u32 bytes,u32 alignment_exponent)
{
    u32 start;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,bytes,alignment_exponent,start);
    return (u8 *)start;
}

u8 *func_002B8FB8(u32 bytes)
{
    u32 start;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,bytes,4,start);
    return (u8 *)start;
}

void func_002B9040(u32 count,u32 alignment_exponent,void **output)
{
    u32 rounded=(count+3U)&~3U,start;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,rounded*12U,alignment_exponent,start);
    *output=(void *)start;
    if ((s32)count<(s32)rounded)
        func_003936A0((void *)(start+count*12U),0,(rounded-count)*12U);
}

void func_002B9120(u32 count,u32 alignment_exponent,void **first,void **second)
{
    u32 rounded=(count+3U)&~3U,start,other;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,(rounded<<1)*12U,alignment_exponent,start);
    other=start+rounded*12U;
    *first=(void *)start;
    *second=(void *)other;
    if ((s32)count<(s32)rounded) {
        u32 offset=count*12U,length=(rounded-count)*12U;
        func_003936A0((void *)(start+offset),0,length);
        func_003936A0((void *)(other+offset),0,length);
    }
}

void func_002B9238(u32 count,u32 alignment_exponent,void **first,void **second,void **third)
{
    u32 rounded=(count+3U)&~3U,start,other,last;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,(rounded<<1)*12U+(rounded<<2),alignment_exponent,start);
    other=start+rounded*12U;
    last=other+rounded*12U;
    *first=(void *)start;
    *second=(void *)other;
    *third=(void *)last;
    if ((s32)count<(s32)rounded) {
        u32 offset=count*12U,length=(rounded-count)*12U;
        func_003936A0((void *)(start+offset),0,length);
        func_003936A0((void *)(other+offset),0,length);
        func_003936A0((void *)(last+(count<<2)),0,(rounded-count)<<2);
    }
}

void func_002B9388(u32 count,u32 alignment_exponent,void **first,void **second,void **third)
{
    u32 rounded=(count+3U)&~3U,start,other,last;
    GEORGE_ARENA_RESERVE(&D_0046A0D8,rounded*28U,alignment_exponent,start);
    other=start+(rounded<<4);
    last=other+(rounded<<3);
    *first=(void *)start;
    *second=(void *)other;
    *third=(void *)last;
}
