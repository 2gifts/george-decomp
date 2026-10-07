#ifndef GEORGE_RESOURCE_POINTER_H
#define GEORGE_RESOURCE_POINTER_H

#include "george/types.h"

/* Consumed scalar views only, not original resource classes or capacities. */
typedef unsigned long long GeorgeResourcePointerBits;
typedef struct GeorgeResourcePointerPrefix {
    u16 flags;
    u8 field02, field03;
    u32 field04, field08, field0C;
} GeorgeResourcePointerPrefix;
typedef struct GeorgeResourcePointerPayloadPrefix {
    u32 flags;
    u8 unknown04[24];
    u16 field1C;
    u8 unknown1E[2];
} GeorgeResourcePointerPayloadPrefix;

typedef char resource_pointer_bits_size[(sizeof(GeorgeResourcePointerBits)==8)?1:-1];
typedef char resource_pointer_prefix_size[(sizeof(GeorgeResourcePointerPrefix)==16)?1:-1];
typedef char resource_pointer_byte2_offset[(offsetof(GeorgeResourcePointerPrefix,field02)==2)?1:-1];
typedef char resource_pointer_byte3_offset[(offsetof(GeorgeResourcePointerPrefix,field03)==3)?1:-1];
typedef char resource_pointer_base_offset[(offsetof(GeorgeResourcePointerPrefix,field04)==4)?1:-1];
typedef char resource_pointer_payload_offset[(offsetof(GeorgeResourcePointerPrefix,field08)==8)?1:-1];
typedef char resource_pointer_slot_offset[(offsetof(GeorgeResourcePointerPrefix,field0C)==12)?1:-1];
typedef char resource_pointer_payload_size[(sizeof(GeorgeResourcePointerPayloadPrefix)==32)?1:-1];
typedef char resource_pointer_count_offset[(offsetof(GeorgeResourcePointerPayloadPrefix,field1C)==28)?1:-1];

u32 george_resource_payload_is_kind2_or3(const void *owner);
/* Compatible with the existing callers that discard incidental GPR2. */
void func_002B62E0(void *owner);
void func_002B6308(void *owner);
/* No-change byte or changed sign-extended word bits, not a pointer/Boolean. */
GeorgeResourcePointerBits george_resource_selector(void *owner, u32 index,
                                                   GeorgeResourcePointerBits mode);

#endif
