#ifndef GEORGE_PROPERTY_RECORDS_H
#define GEORGE_PROPERTY_RECORDS_H

#include "george/byte_order.h"
#include "george/compiler.h"

/* Only the eight-byte header is fixed. Payload extent and the destination's
 * storage capacity depend on the matched descriptor and remain caller-owned. */
typedef struct GeorgePropertyRecord {
    u32 code;
    u8 kind,flags;
    u16 step;
} GeorgePropertyRecord;
typedef u32 (*GeorgePropertyConvert)(const void *payload);
typedef struct GeorgePropertyDescriptor {
    u32 code,type,offset,mask;
    GeorgePropertyConvert convert;
    u32 field14;
} GeorgePropertyDescriptor;
typedef char property_record_header_size[(sizeof(GeorgePropertyRecord)==8)?1:-1];
typedef char property_record_flags_offset[(offsetof(GeorgePropertyRecord,flags)==5)?1:-1];
typedef char property_record_step_offset[(offsetof(GeorgePropertyRecord,step)==6)?1:-1];
typedef char property_descriptor_size[(sizeof(GeorgePropertyDescriptor)==24)?1:-1];
typedef char property_descriptor_convert_offset[(offsetof(GeorgePropertyDescriptor,convert)==16)?1:-1];

void func_0029A508(void *object,GeorgePropertyRecord *record,u32 count,
                   const GeorgePropertyDescriptor *descriptors) GEORGE_SAVE128;
GeorgePropertyRecord *func_0029A890(const GeorgePropertyRecord *record);
u32 func_0029A8B8(GeorgePropertyRecord *record) GEORGE_SAVE128;

#endif
