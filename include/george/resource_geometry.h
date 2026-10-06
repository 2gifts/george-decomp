#ifndef GEORGE_RESOURCE_GEOMETRY_H
#define GEORGE_RESOURCE_GEOMETRY_H

#include "george/geometry_classify6.h"
#include "george/goal_methods4.h"

/* Observed 40-byte resource prefix. Names describe access only. */
typedef struct GeorgeResourceRecord {
    u16 count;
    u8 flags, field03;
    u8 reserved04[12];
    void *field10;
    u8 reserved14[8];
    u32 field1C;
    const u8 *field20;
    struct GeorgeResourceRecord *field24;
} GeorgeResourceRecord;
typedef char resource_count_offset[(offsetof(GeorgeResourceRecord,count)==0)?1:-1];
typedef char resource_flags_offset[(offsetof(GeorgeResourceRecord,flags)==2)?1:-1];
typedef char resource_data_offset[(offsetof(GeorgeResourceRecord,field10)==0x10)?1:-1];
typedef char resource_pair_offset[(offsetof(GeorgeResourceRecord,field20)==0x20)?1:-1];
typedef char resource_prefix_size[(sizeof(GeorgeResourceRecord)==0x28)?1:-1];

void func_0020E920(GeorgeResourceRecord *, u32 mode) GEORGE_SAVE128;
GeorgeResourceRecord *func_0020EAA0(const void *key,u32 mode,u32 word) GEORGE_SAVE128;
GeorgeResourceRecord *func_0020EC10(GeorgeResourceRecord *,const void *key,u32 mode,u32 word) GEORGE_SAVE128;
u32 func_0020EC78(const GeorgeResourceRecord *);
u32 func_0020ECA0(const GeorgeResourceRecord *,const GeorgeRotationMatrix *,const GeorgeGeometryFace *) GEORGE_SAVE128;
void func_0020EEC8(u32 result,GeorgeResourceRecord *) GEORGE_SAVE128;

#endif
