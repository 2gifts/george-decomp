#include "george/buffer_records.h"
#include "george/string_algorithms.h"

/* The observed 394010 pointer return is discarded at all three call sites.
 * Its complete optimized retail implementation is supporting evidence only. */
extern signed char *func_00394010(signed char *destination,
                                const signed char *source, u32 count);

void func_002A5230(GeorgeBufferRecords *object)
{
    GeorgeBufferRecord *selected;
    const signed char *text;
    if ((s32)object->field20 > 0) {
        object->field20 -= 1u;
    }
    text = *(const signed char **)((u8 *)object->field28
                                  + (s32)(object->field20 << 2));
    selected = object->field10;
    func_00394010(selected->field08, text, selected->field00);
    selected->field04 = func_00295050(selected->field08);
}

void func_002A5290(GeorgeBufferRecords *object)
{
    signed char empty[1];
    const signed char *text;
    GeorgeBufferRecord *selected;
    u32 index = object->field20;
    u32 end = object->field24;
    empty[0] = 0;
    if ((s32)index < (s32)end) {
        index += 1u;
        object->field20 = index;
        if ((s32)index < (s32)end) {
            text = *(const signed char **)((u8 *)object->field28
                                          + (s32)(index << 2));
        } else {
            text = empty;
        }
    } else {
        text = empty;
    }
    selected = object->field10;
    func_00394010(selected->field08, text, selected->field00);
    selected->field04 = func_00295050(selected->field08);
}

void func_002A5398(GeorgeBufferRecords *object)
{
    GeorgeBufferRecord *selected = object->field10;
    u32 count = selected->field04;
    if ((s32)count > 0) {
        signed char *text = selected->field08;
        count -= 1u;
        selected->field04 = count;
        text[count] = 0;
    }
}

signed char *func_002A53D0(const GeorgeBufferRecords *object, u32 index)
{
    GeorgeBufferRecord *selected = *(GeorgeBufferRecord **)
        ((u8 *)object->field0C + (s32)(index << 2));
    return selected->field08;
}

signed char *func_002A53E8(const GeorgeBufferRecords *object)
{
    return object->field10->field08;
}

void func_002A53F8(GeorgeBufferRecords *object, const signed char *text)
{
    GeorgeBufferRecord *selected = object->field10;
    func_00394010(selected->field08, text, selected->field00);
    selected->field04 = func_00295050(selected->field08);
}
