#include <stdio.h>
#include <string.h>
#include "george/input_state.h"

/* Synthetic values and independently specified state transitions; no original
 * game data or machine instructions are embedded in this harness. */
static unsigned checks, failures, allocations, memset_calls;
static u32 requested_size;
static int fail_allocation;
static unsigned char arena[512] __attribute__((aligned(64)));
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)

void *func_002AEC28(u32 size)
{
    ++allocations; requested_size = size;
    return fail_allocation ? NULL : arena;
}

void *func_003936A0(void *memory, s32 value, u32 size)
{
    ++memset_calls;
    CHECK(memory == arena + 5 && value == 0 && size == 256);
    return memset(memory, value, size);
}

static u32 bits(float value)
{
    union { float scalar; u32 word; } cast;
    cast.scalar = value;
    return cast.word;
}

static float scalar(u32 word)
{
    union { float scalar; u32 word; } cast;
    cast.word = word;
    return cast.scalar;
}

static void reset(void)
{
    allocations = memset_calls = 0; fail_allocation = 0;
    memset(arena, 0xA5, sizeof arena);
}

static void frame(GeorgeInputState *state, u32 buttons, float elapsed)
{
    func_002AC310(state, 17, NULL, NULL, buttons, NULL, elapsed, 7, 8, 9, 10);
}

static void initialization(void)
{
    GeorgeInputState state, *created;
    GeorgeKeyState *keys;
    unsigned group, component, i;
    static const s32 modes[] = { (s32)0x80000000u, -1, 0, 1, 2, 3, 0x7FFFFFFF };
    memset(&state, 0xA5, sizeof state);
    func_002AC1C0(&state, -99, 0xF1234567u);
    CHECK(state.mode == -99 && state.device == 0xF1234567u && state.flags == 0x1A);
    CHECK(bits(state.repeat_interval) == 0x3E088889u && bits(state.repeat_timer) == 0x3E888889u);
    CHECK(bits(state.threshold) == 0x3E23D70Au);
    CHECK(state.previous == 0 && state.held == 0 && state.pressed == 0 && state.repeated == 0 && state.recent == 0);
    CHECK(state.released == 0xA5A5A5A5u && state.reservedD4 == 0 && state.history_index == 0);
    for (i = 0; i < 4; ++i) CHECK(state.byte_pressure[i] == 0xA5);
    for (i = 0; i < 12; ++i) CHECK(bits(state.pressure[i]) == 0);
    for (i = 0; i < 6; ++i) CHECK(state.history[i] == 0);
    for (group = 0; group < 2; ++group) for (component = 0; component < 2; ++component) {
        GeorgeInputAxis *axis = &state.axes[group][component];
        CHECK(bits(axis->previous) == 0 && bits(axis->current) == 0 && bits(axis->delta) == 0);
        CHECK(bits(state.output_axes[group][component]) == 0);
        CHECK(bits(state.previous_output_axes[group][component]) == 0);
    }
    CHECK(bits(state.lock_timer) == 0 && bits(state.lock_value) == 0);
    state.released = 123; memset(state.byte_pressure, 0x3C, 4);
    state.mode = 7; state.device = 0x89ABCDEFu;
    func_002AC6C8(&state);
    CHECK(state.mode == 7 && state.device == 0x89ABCDEFu && state.flags == 0x1A);
    CHECK(state.released == 123 && state.byte_pressure[0] == 0x3C && state.byte_pressure[3] == 0x3C);
    state.previous = 1; state.held = 2; state.pressed = 3; state.repeated = 4; state.recent = 5; state.released = 6;
    func_002AC6E8(&state);
    CHECK(state.previous == 1 && state.held == 2 && state.pressed == 0 && state.repeated == 0 && state.recent == 0 && state.released == 0);
    func_002AC6B8(&state, 0.15f, 1.0f);
    CHECK(state.lock_timer == 0.15f && state.lock_value == 1.0f);

    for (i = 0; i < sizeof modes / sizeof modes[0]; ++i) {
        reset(); created = func_002AC628(modes[i], 0x10203040u);
        CHECK(created == (GeorgeInputState *)arena && allocations == 1 && requested_size == 0xDC);
        CHECK(created->mode == (modes[i] >= 0 && modes[i] < 3 ? modes[i] : 0));
        CHECK(created->device == 0x10203040u && created->released == 0xA5A5A5A5u);
    }
    reset(); fail_allocation = 1;
    CHECK(func_002AC628(1, 99) == NULL && allocations == 1 && arena[0] == 0xA5);
    reset(); keys = func_002AC700();
    CHECK(keys == (GeorgeKeyState *)arena && allocations == 1 && requested_size == 0x108);
    CHECK(memset_calls == 1 && keys->selected == 0 && bits(keys->repeat_timer) == 0x3E088889u);
    for (i = 0; i < 256; ++i) CHECK(keys->keys[i] == 0);
    for (i = 0; i < 3; ++i) CHECK(keys->untouched_padding[i] == 0xA5);
    reset(); fail_allocation = 1;
    CHECK(func_002AC700() == NULL && allocations == 1 && memset_calls == 0);
}

static void axis_and_copy_order(void)
{
    GeorgeInputState state, saved;
    float axes[4] = { 1, -2, 3, -4 }, pressure[12];
    u8 byte_pressure[4] = { 11, 22, 33, 44 };
    unsigned i;
    memset(&state, 0xA5, sizeof state); func_002AC1C0(&state, 0, 99);
    state.flags = 0;
    for (i = 0; i < 4; ++i) {
        state.axes[i / 2][i % 2].current = (float)(i + 10);
        state.output_axes[i / 2][i % 2] = (float)(i + 20);
    }
    for (i = 0; i < 12; ++i) pressure[i] = (float)i / 16.0f;
    func_002AC310(&state, 8, axes, byte_pressure, 0, pressure, 0.25f, 7, 8, 9, 10);
    CHECK(state.mode == 8 && state.device == 99);
    for (i = 0; i < 4; ++i) {
        GeorgeInputAxis *axis = &state.axes[i / 2][i % 2];
        CHECK(axis->previous == (float)(i + 10) && axis->current == axes[i]);
        CHECK(axis->delta == axes[i] - (float)(i + 10));
        CHECK(state.previous_output_axes[i / 2][i % 2] == (float)(i + 20));
        CHECK(state.output_axes[i / 2][i % 2] == (float)(i + 7));
        CHECK(state.byte_pressure[i] == byte_pressure[i]);
    }
    for (i = 0; i < 12; ++i) CHECK(state.pressure[i] == pressure[i]);
    saved = state; state.flags |= 1; saved.flags |= 1;
    func_002AC310(&state, -1, axes, byte_pressure, 0xFFFFFFFFu, pressure, 100, 1, 2, 3, 4);
    CHECK(memcmp(&state, &saved, sizeof state) == 0); /* complete disabled no-op */

    func_002AC1C0(&state, 0, 0); state.flags = 0;
    for (i = 0; i < 4; ++i) {
        GeorgeInputAxis *axis = &state.axes[i / 2][i % 2];
        axis->previous = (float)(10 + 30 * i);
        axis->current = (float)(20 + 30 * i);
        axis->delta = (float)(30 + 30 * i);
    }
    func_002AC310(&state, 0, (float *)((u8 *)&state + 0x5C), NULL, 0, NULL, 0, 0, 0, 0, 0);
    /* Values independently derived from the original scalar read/store order:
     * first previous write feeds input0, then current/delta feed later inputs. */
    CHECK(state.axes[0][0].previous == 20 && state.axes[0][0].current == 20 && state.axes[0][0].delta == 0);
    CHECK(state.axes[0][1].previous == 50 && state.axes[0][1].current == 20 && state.axes[0][1].delta == -30);
    CHECK(state.axes[1][0].previous == 80 && state.axes[1][0].current == 0 && state.axes[1][0].delta == -80);
    CHECK(state.axes[1][1].previous == 110 && state.axes[1][1].current == 50 && state.axes[1][1].delta == -60);

    func_002AC1C0(&state, 0, 0); state.flags = 0;
    func_002AC310(&state, 0x3F800000, (float *)&state.mode, NULL, 0, NULL, 0, 0, 0, 0, 0);
    CHECK(state.axes[0][0].current == 1.0f); /* mode is written before source reads */
    func_002AC1C0(&state, 0, 0); state.flags = 0;
    ((u8 *)&state)[0x8B] = 0x7A;
    memset(state.byte_pressure, 0x11, 4);
    func_002AC310(&state, 0, NULL, (u8 *)&state + 0x8B, 0, NULL, 0, 0, 0, 0, 0);
    for (i = 0; i < 4; ++i) CHECK(state.byte_pressure[i] == 0x7A);
    func_002AC1C0(&state, 0, 0); state.flags = 0; state.held = 0x3F800000u;
    func_002AC310(&state, 0, NULL, NULL, 0, (float *)((u8 *)&state + 0x28), 0, 0, 0, 0, 0);
    CHECK(state.released == 0x3F800000u);
    for (i = 0; i < 12; ++i) CHECK(state.pressure[i] == 1.0f);
}

static void axis_flags(void)
{
    GeorgeInputState state;
    unsigned x, y;
    static const u32 values[] = { 0xBF800000u, 0xBF000001u, 0xBF000000u,
                                 0, 0x3F000000u, 0x3F000001u, 0x3F800000u, 0x7FC00000u };
    static const u32 x_buttons[] = { 0x80000000u, 0x80000000u, 0, 0, 0, 0x20000000u, 0x20000000u, 0 };
    static const u32 y_buttons[] = { 0x40000000u, 0x40000000u, 0, 0, 0, 0x10000000u, 0x10000000u, 0 };
    for (x = 0; x < 8; ++x) for (y = 0; y < 8; ++y) {
        u32 wanted = 0x41u | x_buttons[x] | y_buttons[y];
        func_002AC1C0(&state, 0, 0);
        state.axes[0][0].current = scalar(values[x]); state.axes[0][1].current = scalar(values[y]);
        frame(&state, 0x41, 0);
        CHECK(state.flags == 2 && state.held == wanted && state.pressed == wanted && state.repeated == wanted);
        CHECK(state.released == 0 && state.previous == 0 && state.history[0] == wanted && state.recent == wanted);
        CHECK(state.history_index == 1);
        frame(&state, 0x41, 0);
        CHECK(state.held == 0x41 && state.released == (wanted & ~0x41u) && state.pressed == 0);
        CHECK(state.flags == 2); /* conversion request is consumed once */
    }
}

static void button_history_and_repeat(void)
{
    GeorgeInputState state;
    u32 prior = 0, expected_history[6] = { 0 }, recent, value;
    unsigned frame_index, i;
    func_002AC1C0(&state, 0, 0); state.flags = 0;
    for (frame_index = 0; frame_index < 48; ++frame_index) {
        u32 expected_pressed, expected_released;
        value = frame_index < 16 ? 1u << frame_index :
                frame_index < 32 ? (0x80008001u ^ (frame_index * 37u)) : 0;
        expected_pressed = value ^ (value & prior);
        expected_released = prior ^ (prior & value);
        expected_history[frame_index % 6] = expected_pressed;
        recent = 0; for (i = 0; i < 6; ++i) recent |= expected_history[i];
        frame(&state, value, 0);
        CHECK(state.previous == prior && state.held == value);
        CHECK(state.pressed == expected_pressed && state.released == expected_released);
        CHECK(state.repeated == expected_pressed && state.recent == recent);
        CHECK(state.history_index == (frame_index + 1) % 6);
        for (i = 0; i < 6; ++i) CHECK(state.history[i] == expected_history[i]);
        prior = value;
    }
    CHECK(state.recent == 0); /* more than six frames without a fresh press */
    func_002AC1C0(&state, 0, 0); state.flags = 0;
    state.held = 0x90000002u; state.repeat_timer = 0.25f; state.repeat_interval = 0.125f;
    state.lock_timer = 0.5f; state.lock_value = 0.75f;
    frame(&state, 0x90000002u, 0.25f);
    CHECK(state.repeat_timer == 0 && state.repeated == 0 && state.lock_timer == 0.25f);
    frame(&state, 0x90000002u, 0.001f);
    CHECK(state.repeat_timer == 0.125f && state.repeated == 0x90000002u);
    frame(&state, 0x90000001u, 2);
    CHECK(state.repeat_timer == 0.25f && state.repeated == 1 && state.lock_timer == 0 && state.lock_value == 0.75f);
    frame(&state, 0x90000001u, -1);
    CHECK(state.repeat_timer == 1.25f && state.lock_timer == 1);
    state.repeat_timer = state.lock_timer = scalar(0x7FC00000u);
    frame(&state, 0x90000001u, 0.5f);
    CHECK(state.repeat_timer != state.repeat_timer && state.lock_timer != state.lock_timer && state.repeated == 0);
}

static void key_transitions(void)
{
    GeorgeKeyState state;
    unsigned i, selected;
    static const u8 transition[4] = { 0, 15, 0, 3 };
    memset(&state, 0xA5, sizeof state);
    state.selected = 0; state.repeat_timer = 8;
    for (i = 0; i < 256; ++i) state.keys[i] = (u8)i;
    func_002AC778(&state, 1000);
    CHECK(state.repeat_timer == 8 && state.selected == 0);
    for (i = 0; i < 256; ++i) CHECK(state.keys[i] == transition[i % 4]);
    for (i = 0; i < 3; ++i) CHECK(state.untouched_padding[i] == 0xA5);
    func_002AC778(&state, 1);
    for (i = 0; i < 256; ++i) CHECK(state.keys[i] == (i % 2 ? 3 : 0));
    for (selected = 1; selected <= 255; ++selected) {
        memset(state.keys, 0, 256);
        state.selected = (u8)selected; state.repeat_timer = 0.25f;
        func_002AC778(&state, 0.25f);
        CHECK(state.keys[selected] == 8 && bits(state.repeat_timer) == 0x3D888889u);
        CHECK(state.keys[0] == 0); /* selected zero is a marker, not key0 repeat */
        func_002AC778(&state, 0);
        CHECK(state.keys[selected] == 0); /* stale repeat bit cleared on next pass */
    }
    state.selected = 1; state.keys[1] = 1; state.repeat_timer = 1;
    func_002AC778(&state, 0.25f);
    CHECK(state.keys[1] == 15 && state.repeat_timer == 0.75f);
    func_002AC778(&state, 0.25f);
    CHECK(state.keys[1] == 3 && state.repeat_timer == 0.5f);
    state.repeat_timer = scalar(0x7FC00000u);
    func_002AC778(&state, 0.25f);
    CHECK(state.keys[1] == 3 && state.repeat_timer != state.repeat_timer);
}

int main(void)
{
    initialization(); axis_and_copy_order(); axis_flags(); button_history_and_repeat(); key_transitions();
    printf("input_state: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
