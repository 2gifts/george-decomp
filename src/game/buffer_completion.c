#include "george/buffer_completion.h"
#include "george/actor_states2.h"
#include "george/heap.h"
#include "george/string_algorithms.h"

/* Reuse only the existing three-word range/descriptor views. Their historical
 * names do not assert a class identity for this opaque registry. */
extern void *D_003F2C44;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_004208A0[];
extern const char D_0043A690[];

/* These complete supporting originals establish numeric contracts; their
 * bodies are not additionally recovered by this batch. */
extern void *func_002BF550(void *object);
extern signed char *func_002BF580(void *object, u32 key);
extern s32 func_00100AA8(const void *left, const void *right);
extern void func_001007E0(GeorgeActorPointerRange *range, void **position,
                        void *const *value);
extern void func_002BD340(void);
extern s32 func_00396260(void (*callback)(void));
extern s32 func_00393E48(const signed char *left, const signed char *right,
                       u32 count);
extern signed char *func_00393B74(signed char *destination,
                                const signed char *source);

/* 0x002162D0: lazily register the opaque resolver, then merge a full key's
 * candidate using the fresh global UI pointer. No allocation guard added. */
void func_002162D0(u32 key, GeorgeDeimosValue *unused_value, void *unused_context)
{
    signed char *candidate;
    (void)unused_value;
    (void)unused_context;
    if (D_003F2C44 == 0) {
        GeorgeActorEffectRecord *record;
        void *registered, *value;
        void **begin, **end, **position;
        D_003F2C44 = func_002BF550(func_002AEE60(0x41C));
        record = (GeorgeActorEffectRecord *)func_002AEE60(12);
        registered = D_003F2C44;
        begin = D_0046A0F0.field00;
        end = D_0046A0F0.field04;
        record->field00 = 5;
        record->field04 = D_004208A0;
        record->field08 = registered;
        value = record;
        position = func_00100C30(begin, end, &value, func_00100AA8);
        if (D_0046A0F0.field04 != D_0046A0F0.field08 &&
            position == D_0046A0F0.field04) {
            if (position != 0)
                *position = value;
            /* The descriptor publication may alias the range's end word. */
            D_0046A0F0.field04 = (void **)((u32)D_0046A0F0.field04 + 4U);
        } else {
            func_001007E0(&D_0046A0F0, position, &value);
        }
        func_00396260(func_002BD340);
    }
    candidate = func_002BF580(D_003F2C44, key);
    func_00217598(D_003F9408, candidate);
}

/* 0x00217598: accepted candidates update the captured UI, while each append
 * obtains its manager from the current global UI. Strings remain unbounded. */
void func_00217598(GeorgeBufferUi *ui, const signed char *candidate)
{
    signed char *input = (signed char *)ui + 0x110;
    signed char *previous = (signed char *)ui + 0x10;
    if (func_00393E48(input, candidate, func_00295050(input)) != 0)
        return;
    if (ui->field0C > 0) {
        u32 index = 0;
        for (;;) {
            u32 length = func_00295050(candidate);
            if (index >= length)
                break;
            if (index >= ui->field210)
                break;
            if (candidate[index] != previous[index])
                ui->field210 = index;
            ++index;
        }
        func_002A4DB8(D_003F9408->field00, (const char *)ui + 0x214);
        func_002A4DB8(D_003F9408->field00, (const char *)previous);
        func_002A4DB8(D_003F9408->field00, D_0043A690);
    } else {
        ui->field210 = func_00295050(candidate);
    }
    func_00393B74(previous, candidate);
    ui->field0C = (s32)((u32)ui->field0C + 1U);
}
