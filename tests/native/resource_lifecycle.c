/* Authored resource observations; no original code or table byte arrays. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "george/resource_lifecycle.h"
#include "resource_lifecycle_golden.h"

static union { u32 words[512]; u8 bytes[2048]; } buffer;
static const struct ResourceLifecycleGolden *fixture;
static u32 calls[6], events[128], event_count, checks, failures;
const u8 D_0043A408[16] = {0};
static void virtual_release(void *, u32);
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    if (failures < 8) printf("resource_lifecycle line %u fixture %u mutation %u\n", \
        __LINE__, fixture ? fixture->routine : 0, fixture ? fixture->mutation : 0); \
    ++failures; } } while (0)

static void *pointer(u32 address) { return buffer.bytes + address - 0x20000; }
static u32 native_value(u32 value)
{
    if (value >= 0x20000 && value < 0x20800) return (u32)pointer(value);
    if (value == 0xF00000C0) return (u32)virtual_release;
    if (value == 0x43A408) return (u32)D_0043A408;
    return value;
}
static u32 canonical(u32 value)
{
    if (value >= (u32)buffer.bytes && value < (u32)buffer.bytes + sizeof buffer)
        return value - (u32)buffer.bytes + 0x20000;
    if (value == (u32)virtual_release) return 0xF00000C0;
    if (value == (u32)D_0043A408) return 0x43A408;
    return value;
}
static GeorgeResourceRecord *record(void) { return (GeorgeResourceRecord *)pointer(0x20100); }
static void event(u32 value)
{
    CHECK(event_count < 128);
    if (event_count < 128) events[event_count++] = value;
}
static void observe(u32 index, void *first, u32 second)
{
    ++calls[index];
    event(index); event(canonical((u32)first)); event(second);
    event(record()->flags); event(canonical((u32)record()->field20));
    if (fixture->mutation == 1 && index == 0)
        record()->field10 = pointer(0x20340);
    if (fixture->mutation == 2 && index == 1) {
        record()->field24 = (GeorgeResourceRecord *)pointer(0x20240);
        record()->flags |= 0x20;
        record()->field10 = pointer(0x20340);
    }
    if (fixture->mutation == 3 && index == 2) {
        record()->flags ^= 0x20;
        record()->field10 = pointer(0x20340);
    }
}
void func_00226D78(GeorgeResourceRecord *child)
{ CHECK(child == pointer(0x20200)); observe(0, child, 0); }
void func_00226ED8(GeorgeResourceRecord *child, GeorgeResourceRecord *owner)
{ CHECK(child == pointer(0x20200) && owner == record()); observe(1, child, canonical((u32)owner)); }
static void virtual_release(void *child, u32 mode)
{
    u32 address = canonical((u32)child), adjustment = fixture->adjustment;
    CHECK((address == 0x20200 + adjustment || address == 0x20240 + adjustment) && mode == 0);
    observe(2, child, mode);
}
void func_0021C508(void *holder)
{ CHECK(holder == pointer(0x20300) || holder == pointer(0x20340)); observe(3, holder, 0); }
void func_0021C5D8(void *holder)
{ CHECK(holder == pointer(0x20300) || holder == pointer(0x20340)); observe(4, holder, 0); }
void func_002267D0(GeorgeResourceRecord *owner, u32 mode)
{ CHECK(owner == record() && mode == fixture->mode); observe(5, owner, mode); }

int main(void)
{
    u32 n, i;
    CHECK(sizeof(void *) == 4 && sizeof(GeorgeResourceRecord) == 0x28);
    for (n = 0; n < sizeof resource_lifecycle_golden / sizeof resource_lifecycle_golden[0]; ++n) {
        fixture = &resource_lifecycle_golden[n];
        for (i = 0; i < 512; ++i) buffer.words[i] = native_value(fixture->initial[i]);
        memset(calls, 0, sizeof calls); event_count = 0;
        switch (fixture->routine) {
        case 0: func_0020EF60(record()); break;
        case 1: func_0020EFC8(record()); break;
        case 2: func_0020F008(record()); break;
        default: func_0020F070(record(), fixture->mode); break;
        }
        for (i = 0; i < 512; ++i) CHECK(canonical(buffer.words[i]) == fixture->expected[i]);
        for (i = 0; i < 6; ++i) CHECK(calls[i] == fixture->calls[i]);
        CHECK(event_count == fixture->event_count);
        for (i = 0; i < event_count && i < fixture->event_count; ++i) CHECK(events[i] == fixture->events[i]);
    }
    printf("resource_lifecycle: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
