#ifndef GEORGE_FILE_ARCHIVE_LIFECYCLE_H
#define GEORGE_FILE_ARCHIVE_LIFECYCLE_H

#include "george/file_archive.h"
#include "george/heap.h"
#include "george/compiler.h"

/* The creator allocates 0x58 bytes. These are observed fields, not an original
 * class or a safe string capacity. The optional prefix copy starts at byte zero
 * and is unbounded. Bytes omitted below are neither read nor initialized here. */
typedef struct GeorgeArchiveContextView {
    u8 unknown00[0x20];
    u32 field20;
    u32 field24;
    u8 field28;
    u8 field29;
    u8 field2A;
    u8 unknown2B;
    u32 field2C;
    void *field30;
    u32 field34;
    u32 field38;
    u8 unknown3C[0x14];
    GeorgeGenericMap *field50;
    GeorgeHeap *field54;
} GeorgeArchiveContextView;

/* Twelve allocated bytes held as a generic-map payload, not a map node. */
typedef struct GeorgeArchiveDirectoryRecord {
    u32 field00;
    u32 field04;
    u32 field08;
} GeorgeArchiveDirectoryRecord;

#define ARCHIVE_LIFECYCLE_OFFSET(type,field,offset) \
    typedef char archive_lifecycle_##type##_##field[(offsetof(type,field)==(offset))?1:-1]
typedef char archive_lifecycle_context_size[(sizeof(GeorgeArchiveContextView)==0x58)?1:-1];
typedef char archive_lifecycle_record_size[(sizeof(GeorgeArchiveDirectoryRecord)==12)?1:-1];
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field20,0x20);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field24,0x24);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field28,0x28);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field29,0x29);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field2A,0x2A);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field2C,0x2C);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field30,0x30);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field34,0x34);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field38,0x38);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field50,0x50);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveContextView,field54,0x54);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveDirectoryRecord,field04,4);
ARCHIVE_LIFECYCLE_OFFSET(GeorgeArchiveDirectoryRecord,field08,8);
#undef ARCHIVE_LIFECYCLE_OFFSET

void *func_002A7E68(GeorgeGenericMap *map,u32 *key_output) GEORGE_SAVE128;
GeorgeGenericMap *func_002A7F58(u32 bucket_count) GEORGE_SAVE128;
void func_002A7FC8(GeorgeGenericMap *map) GEORGE_SAVE128;
u32 func_002B15C8(GeorgeArchiveContextView *context,u32 sector,char *path,u32 depth) GEORGE_SAVE128;
u32 func_002B1900(GeorgeArchiveContextView *context) GEORGE_SAVE128;
GeorgeArchiveContextView *func_002B1D30(const char *prefix,u32 argument5,u32 argument6) GEORGE_SAVE128;
/* Effects only: callers discard incidental v0; no source return type claimed. */
void func_002B1E88(GeorgeArchiveContextView *context) GEORGE_SAVE128;
void func_002B0860(u32 mode,u32 argument5) GEORGE_SAVE128;

#endif
