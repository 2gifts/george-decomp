#ifndef GEORGE_CACHE_TRANSFER_H
#define GEORGE_CACHE_TRANSFER_H

#include "george/compiler.h"
#include "george/types.h"

typedef struct GeorgeCacheTransferRecord {
    const void *source;
    u32 offset;
    u32 size;
} GeorgeCacheTransferRecord;

typedef char cache_transfer_record_size[(sizeof(GeorgeCacheTransferRecord) == 12) ? 1 : -1];
typedef char cache_transfer_offset[(offsetof(GeorgeCacheTransferRecord, offset) == 4) ? 1 : -1];
typedef char cache_transfer_length[(offsetof(GeorgeCacheTransferRecord, size) == 8) ? 1 : -1];

extern u32 D_003FD24C;
extern u32 D_003FD250;
extern GeorgeCacheTransferRecord D_00469E00[10];

void func_002B2F40(const void *source, u32 size) GEORGE_SAVE128;
u32 func_002B3150(u32 offset, const void *source, u32 size) GEORGE_SAVE128;

#endif
