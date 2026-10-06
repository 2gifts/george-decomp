#include "george/deimos_calls.h"
#include "george/deimos_tables.h"

extern s32 D_0048176C;
extern u32 D_0048175C;
extern GeorgeDeimosFrame D_00481770[];
extern GeorgeDeimosValue D_00474748[];
extern u32 D_00474740;
extern GeorgeDeimosValue *D_00474F48;
extern s32 D_00474F4C;
extern GeorgeGenericMap *D_00481760;
extern GeorgeDeimosRing *D_004817F8;
extern void func_002CBC48(void *code, s32 destination);
extern void *func_003934F8(void *destination, const void *source, u32 size);

typedef void (*GeorgeDeimosNativeCall)(s32 count, s32 destination);

void func_002CCC58(GeorgeDeimosPoolNode *node, s32 offset, s32 count, s32 destination)
{
    s32 depth = D_0048176C + 1;
    GeorgeDeimosCallable *callable = (GeorgeDeimosCallable *)node->payload;
    u32 saved_base = D_0048175C;
    u32 base = saved_base + (u32)offset;
    s32 absolute_destination;
    D_00481770[depth].context = (GeorgeDeimosContext *)callable;
    D_00474740 = 0x100u - base;
    D_00481770[depth].unknown04 = 0xFFFFFFFFu;
    D_00474F48 = &D_00474748[base];
    D_0048176C = depth;
    D_0048175C = base;
    absolute_destination = destination == -1 ? -1 : (s32)(saved_base + (u32)destination);
    if (callable->field10 != 0) {
        s32 saved_count = D_00474F4C;
        D_00474F4C = count;
        ((GeorgeDeimosNativeCall)callable->field10)(count, absolute_destination);
        D_00474F4C = saved_count;
    } else {
        func_002CBC48(callable->data0C, absolute_destination);
    }
    depth = D_0048176C;
    D_0048175C = saved_base;
    D_00474740 = 0x100u - saved_base;
    D_00474F48 = &D_00474748[saved_base];
    D_0048176C = depth - 1;
}

void func_002CDF18(u32 key, s32 offset, s32 count, s32 destination)
{
    GeorgeDeimosValue *value = func_002A7C08(D_00481760, key);
    func_002CCC58(value->payload.pointer, offset, count, destination);
}

void func_002D0258(u32 key, s32 count, s32 destination)
{
    func_002CDF18(key, 0, count, destination);
}

void func_002D0280(GeorgeDeimosPoolNode *callable, s32 count, s32 destination)
{
    func_002CCC58(callable, 0, count, destination);
}

/* memcpy precedes all count/pointer reloads, preserving aliased input/output. */
u32 func_002AF3F0(GeorgeDeimosRing *ring, void *output)
{
    if (ring->count == 0) {
        return 0;
    }
    func_003934F8(output, ring->read, ring->element_size);
    {
        u32 count = ring->count - 1;
        u8 *next = ring->read + ring->element_size;
        u8 *end = ring->end;
        ring->count = count;
        ring->read = next;
        if ((u32)next >= (u32)end) {
            ring->read = ring->begin;
        }
    }
    return 1;
}

u32 func_002AF470(GeorgeDeimosRing *ring, const void *input)
{
    if (ring->count == ring->capacity) {
        return 0;
    }
    func_003934F8(ring->write, input, ring->element_size);
    {
        u32 count = ring->count + 1;
        u8 *next = ring->write + ring->element_size;
        u8 *end = ring->end;
        ring->count = count;
        ring->write = next;
        if ((u32)next >= (u32)end) {
            ring->write = ring->begin;
        }
    }
    return 1;
}

static __inline__ int retained_value(u16 tag)
{
    return tag == 5 || (u16)(tag - 3) < 2;
}

/* The observed record holds ten values. The original has no count guard;
 * callers must supply a valid record count. Unused values are left untouched. */
void func_002D02A8(GeorgeDeimosPoolNode *callable, s32 count, s32 offset)
{
    GeorgeDeimosCallRecord record;
    s32 remaining;
    GeorgeDeimosValue *output;
    s32 argument = offset;
    if (D_004817F8->count == D_004817F8->capacity) {
        return;
    }
    record.count = count;
    output = record.values;
    for (remaining = count; remaining > 0; --remaining) {
        *output = D_00474F48[argument];
        if (retained_value(output->tag)) {
            func_002CD0B8(output->payload.pointer);
        }
        ++output;
        ++argument;
    }
    record.callable = callable;
    func_002CD0B8(callable);
    func_002AF470(D_004817F8, &record);
}

void func_002CEA80(GeorgeDeimosCallRecord *record)
{
    s32 index;
    for (index = 0; index < record->count; ++index) {
        D_00474F48[index] = record->values[index];
    }
    func_002CCC58(record->callable, 0, record->count, -1);
    func_002CD130(record->callable);
    record->callable = 0;
    for (index = 0; index < record->count; ++index) {
        GeorgeDeimosValue *value = &record->values[index];
        if (retained_value(value->tag)) {
            func_002CD130(value->payload.pointer);
        }
    }
}

void func_002D03A0(void)
{
    GeorgeDeimosCallRecord record;
    while (D_004817F8->count != 0) {
        func_002AF3F0(D_004817F8, &record);
        func_002CEA80(&record);
    }
}
