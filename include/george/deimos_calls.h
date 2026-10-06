#ifndef GEORGE_DEIMOS_CALLS_H
#define GEORGE_DEIMOS_CALLS_H

#include "george/compiler.h"
#include "george/deimos.h"

typedef struct GeorgeDeimosCallRecord {
    s32 count;
    GeorgeDeimosValue values[10];
    GeorgeDeimosPoolNode *callable;
} GeorgeDeimosCallRecord;

typedef struct GeorgeDeimosRing {
    u32 count;
    u8 *read;
    u8 *write;
    u32 element_size;
    u32 capacity;
    u8 *begin;
    u8 *end;
} GeorgeDeimosRing;

typedef char deimos_call_record_size[(sizeof(GeorgeDeimosCallRecord) == 0x58) ? 1 : -1];
typedef char deimos_call_record_callable_offset[(offsetof(GeorgeDeimosCallRecord, callable) == 0x54) ? 1 : -1];
typedef char deimos_ring_size[(sizeof(GeorgeDeimosRing) == 0x1C) ? 1 : -1];
typedef char deimos_ring_capacity_offset[(offsetof(GeorgeDeimosRing, capacity) == 0x10) ? 1 : -1];
typedef char deimos_ring_begin_offset[(offsetof(GeorgeDeimosRing, begin) == 0x14) ? 1 : -1];

void func_002CCC58(GeorgeDeimosPoolNode *callable, s32 offset, s32 count, s32 destination) GEORGE_SAVE128;
void func_002CDF18(u32 key, s32 offset, s32 count, s32 destination) GEORGE_SAVE128;
void func_002D0258(u32 key, s32 count, s32 destination) GEORGE_SAVE128;
void func_002D0280(GeorgeDeimosPoolNode *callable, s32 count, s32 destination) GEORGE_SAVE128;
u32 func_002AF3F0(GeorgeDeimosRing *ring, void *output) GEORGE_SAVE128;
u32 func_002AF470(GeorgeDeimosRing *ring, const void *input) GEORGE_SAVE128;
void func_002D02A8(GeorgeDeimosPoolNode *callable, s32 count, s32 offset) GEORGE_SAVE128;
void func_002D03A0(void) GEORGE_SAVE128;
void func_002CEA80(GeorgeDeimosCallRecord *record) GEORGE_SAVE128;

#endif
