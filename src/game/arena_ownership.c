#include "george/arena_ownership.h"
#include "george/arena_buffers.h"
#include "george/heap.h"

void func_002B8B68(u32 capacity)
{
    void *base;
    u32 fresh_capacity;
    D_003FD3E4=capacity;
    base=func_002AEC28(capacity);
    fresh_capacity=D_003FD3E4;
    D_003FD3E0=base;
    func_002B8EA8(base,fresh_capacity);
}

void func_002B8BA8(void)
{
    func_002AEE40(D_003FD3E0);
    D_003FD3E0=NULL;
    D_003FD3E4=0;
}

void func_002B8BE0(void)
{
    func_002B8EE8();
}
