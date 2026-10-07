#include "george/buffer_ui.h"
#include "george/buffer_records.h"
#include "george/accessors.h"
#include "george/deimos.h"
#include "george/deimos_calls.h"
#include "george/string_algorithms.h"

/* These are actual call/address bindings, not additional recovered bodies.
 * Rendering, parsing and the nested completion callback remain opaque. */
typedef unsigned long long BufferUiU64;
typedef signed long long BufferUiS64;
extern void func_002162D0(u32 key, GeorgeDeimosValue *value, void *context);
extern s32 func_002CEB78(const signed char *source, u32 length,
                       const signed char *name);
extern void func_002D04B0(const signed char *name, s32 value);
extern void *func_003936A0(void *destination, s32 value, u32 count);
extern signed char *func_00393758(signed char *destination, const signed char *source);
extern signed char *func_00393B74(signed char *destination, const signed char *source);
extern signed char *func_00398558(const signed char *text, const signed char *set);
extern signed char *func_003985D8(const signed char *text, s32 character);
extern BufferUiU64 func_00374848(float value);
extern void func_0023C908(float x, float y, float r, float g, float b, float a,
                        u32 kind, const signed char *format, ...);
extern void func_002BA680(void);
extern void func_002BA708(void);
extern void func_00290BF0(u32 value);
extern void func_00290CF8(u32 first, u32 second);
extern void func_0028E8A0(u32 value);
extern void func_0028E668(u32 value);
extern void func_0028FE88(float r, float g, float b, float a);
extern void func_0028FBC0(u32 kind);
extern void func_0028FD30(float x, float y, float z);
extern void func_0028C590(void);
extern void func_0023E0D0(u32 value);

extern const signed char D_0043A690[];
extern const signed char D_0043A6E8[];
extern const signed char D_0043A6F0[];
extern const signed char D_0043A6F8[];
extern const signed char D_0043A700[];
extern const signed char D_0043A718[];
extern const signed char D_0043A720[];
extern const signed char D_0043A730[];
extern const signed char D_0043A740[];
extern const signed char D_0043A750[];
extern const signed char D_0043A788[];
extern const signed char D_0043A798[];

#define UI_TEXT(ui, offset) ((signed char *)(ui) + (offset))
#define UI_RECORDS(ui) ((GeorgeBufferRecords *)(ui)->field00)

/* This models the observed value result of EE CVT.W.S, including its sign
 * clamp above biased exponent 0x9D. It does not model FCR flags. Ordinary C
 * conversion is used only where the signed-32 result is representable. */
#if __GNUC__ >= 3
#define UI_INLINE static __inline__ __attribute__((always_inline))
#else
#define UI_INLINE static __inline__
#endif
UI_INLINE s32 buffer_ui_cvtw_value(float value)
{
    union { float scalar; u32 bits; } input;
    input.scalar = value;
    if (((input.bits >> 23) & 0xFF) > 0x9D)
        return (input.bits >> 31) ? (s32)0x80000000U : 0x7FFFFFFF;
    return (s32)value;
}

/* SP+0, +10, +50 and +60 are overlapping address views. The explicit span
 * ends before the original saved RA; it is not a discovered string capacity.
 * Original string operations still require valid, initialized addresses. */
void func_002163E8(void)
{
    union { BufferUiU64 alignment; signed char bytes[0x110]; } scratch;
    signed char *temporary = scratch.bytes + 0x10;
    signed char *found;
    GeorgeBufferUi *captured = D_003F9408;
    GeorgeDeimosHashTable *table = 0;
    GeorgeDeimosValue *value;
    u32 index = 0;
    u32 key;
    u8 delimiter;

    func_00393B74(UI_TEXT(captured, 0x110), func_002A53E8(UI_RECORDS(captured)));
    func_002A53F8(UI_RECORDS(D_003F9408), D_0043A6E8);
    *UI_TEXT(D_003F9408, 0x214) = 0;
    *(BufferUiU64 *)scratch.bytes = *(const BufferUiU64 *)D_0043A6F0;
    func_003936A0(scratch.bytes + 8, 0, 8);
    while (index < func_00295050(scratch.bytes)) {
        found = func_003985D8(UI_TEXT(D_003F9408, 0x110), scratch.bytes[index]);
        if (found) {
            func_00393B74(temporary, UI_TEXT(D_003F9408, 0x110));
            temporary[(u32)found - (u32)D_003F9408 - 0x10F] = 0;
            func_00393758(UI_TEXT(D_003F9408, 0x214), temporary);
            func_00393B74(temporary, found + 1);
            func_00393B74(UI_TEXT(D_003F9408, 0x110), temporary);
        }
        ++index;
    }
    for (;;) {
        found = func_00398558(UI_TEXT(D_003F9408, 0x110), D_0043A6F8);
        if (!found)
            break;
        func_00393B74(temporary, UI_TEXT(D_003F9408, 0x110));
        temporary[(u32)found - 0x110 - (u32)D_003F9408] = 0;
        key = func_0029C648(temporary);
        value = table ? func_002CD990(table, key) : func_002CDD60(key);
        if (!value || (s16)value->tag != 4)
            break;
        delimiter = *(u8 *)found;
        table = value->payload.pointer;
        scratch.bytes[0x50] = delimiter;
        scratch.bytes[0x51] = 0;
        func_00393B74(scratch.bytes + 0x60, found + 1);
        func_00393B74(UI_TEXT(D_003F9408, 0x110), scratch.bytes + 0x60);
        func_00393758(UI_TEXT(D_003F9408, 0x214), temporary);
        func_00393758(UI_TEXT(D_003F9408, 0x214), scratch.bytes + 0x50);
    }
    D_003F9408->field210 = 0;
    D_003F9408->field0C = 0;
    if (table) {
        func_002CDA20(table, func_002162D0, 0);
        if (table->field08)
            func_002CDA20(table->field08, func_002162D0, 0);
    } else {
        func_002CDDB0(func_002162D0, 0);
    }
    if (D_003F9408->field0C >= 2) {
        func_002A4DB8(D_003F9408->field00, (const char *)UI_TEXT(D_003F9408, 0x214));
        func_002A4DB8(D_003F9408->field00, (const char *)UI_TEXT(D_003F9408, 0x10));
        func_002A4DB8(D_003F9408->field00, (const char *)D_0043A690);
    }
    if (D_003F9408->field0C > 0) {
        *UI_TEXT(D_003F9408, 0x10 + D_003F9408->field210) = 0;
        func_00393B74(temporary, UI_TEXT(D_003F9408, 0x214));
        func_00393758(temporary, UI_TEXT(D_003F9408, 0x10));
    } else {
        func_00393B74(temporary, UI_TEXT(D_003F9408, 0x214));
        func_00393758(temporary, UI_TEXT(D_003F9408, 0x110));
    }
    func_002A53F8(UI_RECORDS(D_003F9408), temporary);
}

void func_002167D8(float delta)
{
    union { BufferUiU64 alignment; signed char bytes[0x100]; } scratch;
    u32 key;
    GeorgeBufferUi *captured;
    BufferUiU64 first, second;
    u32 fourth;
    u8 last;

    D_003F9408->field04 = delta;
    if (D_004683AC[0x65] & 4)
        return;
    if (!func_002A56C0(D_003F9408->field00))
        return;
    key = D_004683AC[4];
    if (!(D_004683AC[5 + key] & 8))
        return;
    if (key == 8) {
        func_002A5398(UI_RECORDS(D_003F9408));
    } else if (key - 0x20 < 0x60) {
        func_002A4B70(D_003F9408->field00, (signed char)key);
    } else if (key == 10 || key == 13) {
        captured = D_003F9408;
        first = *(const BufferUiU64 *)D_0043A700;
        last = D_0043A700[0x14];
        second = *(const BufferUiU64 *)(D_0043A700 + 8);
        fourth = *(const u32 *)(D_0043A700 + 0x10);
        *(BufferUiU64 *)scratch.bytes = first;
        *(BufferUiU64 *)(scratch.bytes + 8) = second;
        scratch.bytes[0x14] = last;
        *(u32 *)(scratch.bytes + 0x10) = fourth;
        func_00393758(scratch.bytes, func_002A53E8(UI_RECORDS(captured)));
        func_00393758(scratch.bytes, D_0043A718);
        func_002A4B70(D_003F9408->field00, (signed char)key);
        if (func_002CEB78(scratch.bytes, func_00295050(scratch.bytes), D_0043A720)) {
            func_002D0258(func_0029C648(D_0043A730), 0, -1);
            func_002D04B0(D_0043A720, 1);
        } else {
            func_002A4DB8(D_003F9408->field00, (const char *)D_0043A740);
        }
    } else if (key == 0xE5) {
        func_002A5230(UI_RECORDS(D_003F9408));
    } else if (key == 0xE7) {
        func_002A5290(UI_RECORDS(D_003F9408));
    } else if (key == 9) {
        func_002163E8();
    }
}

void func_002169C0(void)
{
    s32 rows, index, limit;
    float x, y, text_y;
    const float left = 25.600000381469727f;
    const float bottom = 425.6000061035156f;
    const float step = -15.680000305175781f;
    const float blue = 0.4000000059604645f;
    const float bright = 0.8999999761581421f;
    signed char *text;
    GeorgeBufferUi *captured;
    s32 fps;
    BufferUiU64 double_bits;

    if (!func_002A56C0(D_003F9408->field00))
        return;
    rows = (s32)func_002A53C0(D_003F9408->field00);
    x = (float)(s32)func_002A53C8(D_003F9408->field00) * 5.119999885559082f;
    y = (float)(s32)((u32)rows - 5) * step;
    func_002BA680();
    x += left;
    func_00290BF0(0);
    y += bottom;
    func_0028E8A0(0xB71);
    func_0028E668(0xBE2);
    func_00290CF8(0x302, 0x303);
    func_0028FE88(0.0f, 0.0f, 0.0f, 0.8500000238418579f);
    func_0028FBC0(4);
    func_0028FD30(left, bottom, 0.0f);
    func_0028FD30(left, y, 0.0f);
    func_0028FD30(x, bottom, 0.0f);
    func_0028FD30(left, y, 0.0f);
    func_0028FD30(x, y, 0.0f);
    func_0028FD30(x, bottom, 0.0f);
    func_0028C590();
    func_0028E8A0(0xBE2);
    func_0028FE88(1.0f, 1.0f, 1.0f, 1.0f);
    func_0028FBC0(3);
    func_0028FD30(left, bottom, 0.0f);
    func_0028FD30(left, y, 0.0f);
    func_0028FD30(x, y, 0.0f);
    func_0028FD30(x, bottom, 0.0f);
    func_0028FD30(left, bottom, 0.0f);
    func_0028FD30(left, 402.08001708984375f, 0.0f);
    func_0028FD30(x, 402.08001708984375f, 0.0f);
    func_0028C590();
    captured = D_003F9408;
    fps = buffer_ui_cvtw_value(1.0f / captured->field04);
    func_0023C908(30.720001220703125f, 417.760009765625f,
                 blue, bright, blue, 1.0f, 1, D_0043A750,
                 (BufferUiS64)(s32)captured->field08, (BufferUiS64)fps);
    double_bits = func_00374848(1.0f / D_003F9408->field04);
    text_y = 386.4000244140625f;
    func_0023C908(30.720001220703125f, text_y, blue, bright, blue,
                 1.0f, 1, D_0043A788, double_bits);
    index = 8;
    limit = (s32)((u32)rows - 1);
    while (index < limit) {
        s32 old_index = index;
        index = (s32)((u32)index + 1);
        text = func_002A53D0(UI_RECORDS(D_003F9408), (u32)old_index);
        func_0023C908(30.720001220703125f, text_y, bright, bright,
                     bright, 1.0f, 1, text);
        text_y += step;
    }
    text = func_002A53D0(UI_RECORDS(D_003F9408), (u32)index);
    func_0023C908(30.720001220703125f, text_y, bright, bright,
                 bright, 1.0f, 1, D_0043A798, text);
    func_0028E668(0xB71);
    func_00290BF0(1);
    func_002BA708();
    func_0023E0D0(0);
}

#undef UI_TEXT
#undef UI_RECORDS
#undef UI_INLINE
