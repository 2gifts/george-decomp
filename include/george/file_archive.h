#ifndef GEORGE_FILE_ARCHIVE_H
#define GEORGE_FILE_ARCHIVE_H

#include "george/file_operations.h"
#include "george/deimos_tables.h"

/* Only the observed map pointer at +0x50 is named. This is a byte prefix,
 * not a declaration of an original archive class or storage capacity. */
typedef struct GeorgeArchiveMapView {
    u8 unknown00[0x50];
    GeorgeGenericMap *field50;
} GeorgeArchiveMapView;

typedef char george_archive_map_50[(offsetof(GeorgeArchiveMapView,field50)==0x50)?1:-1];

/* The observed callers discard the close routine's incidental return state.
 * The void declaration models effects and does not identify its source type. */
void func_002B0998(GeorgeFileSlot *slot) GEORGE_SAVE128;
/* Keep the existing opaque context/result interface used by the opener. */
void *func_002B1BA8(void *context,const char *path) GEORGE_SAVE128;

#endif
