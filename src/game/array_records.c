#include "george/array_records.h"
#include "george/heap.h"

extern void *func_003934F8(void *destination, const void *source, u32 size);
extern void func_00396788(void *data, u32 count, u32 size, GeorgeArrayCompare compare);

void *func_002AAF50(const GeorgeArrayRecords *array, s32 index)
{
    if (index >= 0 && index < (s32)array->used) {
        return (void *)((u32)array->data + array->element_size * (u32)index);
    }
    return 0;
}

GeorgeArrayRecords *func_002AAF98(u32 element_size)
{
    GeorgeArrayRecords *array = func_002AEC28(16);
    if (array != 0) {
        array->element_size = element_size;
        array->capacity = 0;
        array->used = 0;
        array->data = 0;
    }
    return array;
}

void func_002AAFD8(GeorgeArrayRecords *array)
{
    array->used = 0;
}

void func_002AAFE0(GeorgeArrayRecords *array)
{
    if (array->data != 0) {
        func_002AEE40(array->data);
        array->data = 0;
    }
    array->used = 0;
    array->capacity = 0;
}

void func_002AB020(GeorgeArrayRecords *array)
{
    if (array->data != 0) {
        func_002AEE40(array->data);
        array->data = 0;
    }
    array->capacity = 0;
    array->used = 0;
    func_002AEE40(array);
}

s32 func_002AB068(GeorgeArrayRecords *array, s32 additional)
{
    if (additional > 0) {
        u32 capacity = array->capacity + (u32)additional;
        void *memory = func_002AEB60(array->element_size * capacity, 4);
        if (memory != 0) {
            if (array->data != 0) {
                func_003934F8(memory, array->data, array->element_size * array->used);
                /* The copy may change the header; free its freshly read data. */
                func_002AEE40(array->data);
            }
            array->capacity = capacity;
            array->data = memory;
            return 1;
        }
    }
    return 0;
}

s32 func_002AB110(GeorgeArrayRecords *array, const void *element)
{
    if ((s32)array->used >= (s32)array->capacity) {
        u32 capacity = array->capacity + 10u;
        void *memory = func_002AEB60(array->element_size * capacity, 4);
        if (memory != 0) {
            if (array->data != 0) {
                func_003934F8(memory, array->data, array->element_size * array->used);
                func_002AEE40(array->data);
            }
            /* Append publishes data before capacity, unlike reserve. */
            array->data = memory;
            array->capacity = capacity;
        }
        if ((s32)array->used >= (s32)array->capacity) {
            return 0;
        }
    }
    func_003934F8((void *)((u32)array->data + array->used * array->element_size),
                  element, array->element_size);
    array->used = array->used + 1u;
    return 1;
}

u32 func_002AB330(GeorgeArrayRecords *array, s32 index)
{
    if ((u32)index != array->used - 1u) {
        void *destination = 0;
        const void *source = 0;
        s32 last;
        if (index >= 0 && index < (s32)array->used) {
            destination = (void *)((u32)array->data + array->element_size * (u32)index);
        }
        last = (s32)(array->used - 1u);
        if (last >= 0 && last < (s32)array->used) {
            source = (const void *)((u32)array->data + array->element_size * (u32)last);
        }
        /* The original calls copy even if either checked pointer is NULL. */
        func_003934F8(destination, source, array->element_size);
    }
    array->used = array->used - 1u;
    return array->used;
}

void func_002AB3C8(GeorgeArrayRecords *array, GeorgeArrayCompare compare)
{
    if (array->used != 0) {
        func_00396788(array->data, array->used, array->element_size, compare);
    }
}
