#ifndef GEORGE_ARRAY_RECORDS_H
#define GEORGE_ARRAY_RECORDS_H

#include "george/types.h"
#include "george/compiler.h"

/* Four observed words in the allocated sixteen-byte header. Element storage
 * is byte-addressed; these names do not establish an original engine class. */
typedef struct GeorgeArrayRecords {
    u32 element_size;
    u32 capacity;
    u32 used;
    void *data;
} GeorgeArrayRecords;

typedef s32 (*GeorgeArrayCompare)(const void *, const void *);
typedef char george_array_records_size[(sizeof(GeorgeArrayRecords) == 16) ? 1 : -1];
typedef char george_array_records_used_offset[(offsetof(GeorgeArrayRecords, used) == 8) ? 1 : -1];
typedef char george_array_records_data_offset[(offsetof(GeorgeArrayRecords, data) == 12) ? 1 : -1];

void *func_002AAF50(const GeorgeArrayRecords *array, s32 index);
GeorgeArrayRecords *func_002AAF98(u32 element_size) GEORGE_SAVE128;
void func_002AAFD8(GeorgeArrayRecords *array);
void func_002AAFE0(GeorgeArrayRecords *array) GEORGE_SAVE128;
void func_002AB020(GeorgeArrayRecords *array) GEORGE_SAVE128;
s32 func_002AB068(GeorgeArrayRecords *array, s32 additional) GEORGE_SAVE128;
s32 func_002AB110(GeorgeArrayRecords *array, const void *element) GEORGE_SAVE128;
u32 func_002AB330(GeorgeArrayRecords *array, s32 index) GEORGE_SAVE128;
void func_002AB3C8(GeorgeArrayRecords *array, GeorgeArrayCompare compare);

#endif
