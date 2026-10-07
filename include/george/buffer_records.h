#ifndef GEORGE_BUFFER_RECORDS_H
#define GEORGE_BUFFER_RECORDS_H

#include "george/types.h"
#include "george/compiler.h"

/* Numeric prefixes established by the selected instructions, not complete
 * engine classes or allocation/capacity assertions. */
typedef struct GeorgeBufferRecord {
    u32 field00;
    u32 field04;
    signed char *field08;
} GeorgeBufferRecord;

typedef struct GeorgeBufferRecords {
    u8 unknown00[0x0C];
    GeorgeBufferRecord **field0C;
    GeorgeBufferRecord *field10;
    u8 unknown14[0x0C];
    u32 field20;
    u32 field24;
    const signed char **field28;
} GeorgeBufferRecords;

typedef char george_buffer_record_size[(sizeof(GeorgeBufferRecord) == 12) ? 1 : -1];
typedef char george_buffer_record_data_offset[(offsetof(GeorgeBufferRecord, field08) == 8) ? 1 : -1];
typedef char george_buffer_records_selected_offset[(offsetof(GeorgeBufferRecords, field10) == 0x10) ? 1 : -1];
typedef char george_buffer_records_index_offset[(offsetof(GeorgeBufferRecords, field20) == 0x20) ? 1 : -1];
typedef char george_buffer_records_end_offset[(offsetof(GeorgeBufferRecords, field24) == 0x24) ? 1 : -1];
typedef char george_buffer_records_history_offset[(offsetof(GeorgeBufferRecords, field28) == 0x28) ? 1 : -1];

void func_002A5230(GeorgeBufferRecords *object) GEORGE_SAVE128;
void func_002A5290(GeorgeBufferRecords *object) GEORGE_SAVE128;
void func_002A5398(GeorgeBufferRecords *object) GEORGE_SAVE128;
signed char *func_002A53D0(const GeorgeBufferRecords *object, u32 index);
signed char *func_002A53E8(const GeorgeBufferRecords *object);
void func_002A53F8(GeorgeBufferRecords *object, const signed char *text) GEORGE_SAVE128;

#endif
