#ifndef GEORGE_FILE_OPERATIONS_H
#define GEORGE_FILE_OPERATIONS_H

#include "george/types.h"
#include "george/compiler.h"

/* Seven observed words; field04 contains either an encoded SDK descriptor or
 * an archive-record pointer. Only the original20-slot scan is established. */
typedef struct GeorgeFileSlot {
    u32 field00;
    u32 field04;
    u32 field08;
    u32 field0C;
    u32 field10;
    u32 field14;
    u32 field18;
} GeorgeFileSlot;

/* Numeric guest addresses, not host DMA ownership or hardware semantics. */
typedef struct GeorgeFileDmaPacket {
    u32 source;
    u32 destination;
    u32 size;
    u32 attribute;
} GeorgeFileDmaPacket;

typedef char george_file_slot_size[(sizeof(GeorgeFileSlot)==28)?1:-1];
typedef char george_file_dma_size[(sizeof(GeorgeFileDmaPacket)==16)?1:-1];
typedef char george_file_slot_10[(offsetof(GeorgeFileSlot,field10)==16)?1:-1];
typedef char george_file_slot_18[(offsetof(GeorgeFileSlot,field18)==24)?1:-1];
typedef char george_file_dma_destination[(offsetof(GeorgeFileDmaPacket,destination)==4)?1:-1];
typedef char george_file_dma_length[(offsetof(GeorgeFileDmaPacket,size)==8)?1:-1];

extern GeorgeFileSlot D_00469BD0[20];
extern u32 D_003FD23C,D_003FD240;
extern void *D_003FD244;

GeorgeFileSlot *func_002B0710(const char *path) GEORGE_SAVE128;
u32 func_002B0F60(u32 encoded_descriptor,u32 destination,s32 count) GEORGE_SAVE128;
u32 func_002B10D8(const char *path) GEORGE_SAVE128;
u32 func_002B1138(const char *path) GEORGE_SAVE128;
/* The observed callers discard v0; this declaration makes no original source
 * return-type claim or defined incidental return on the bypass/cache paths. */
void func_002B1198(u32 encoded_descriptor) GEORGE_SAVE128;
s32 func_002B11F0(u32 encoded_descriptor,void *destination,s32 count) GEORGE_SAVE128;
s32 func_002B1260(u32 encoded_descriptor,const void *source,s32 count) GEORGE_SAVE128;
s32 func_002B1408(u32 encoded_descriptor,s32 offset,s32 whence) GEORGE_SAVE128;
s32 func_002B1468(u32 encoded_descriptor) GEORGE_SAVE128;
s32 func_002B1510(const char *path) GEORGE_SAVE128;

#endif
