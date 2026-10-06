#ifndef GEORGE_RESOURCE_GROUPS_H
#define GEORGE_RESOURCE_GROUPS_H

#include "george/resource_geometry.h"

/* Observed group layout through A8. The E8-byte allocation has an unexamined
 * tail; this declaration does not identify a complete original class. */
typedef struct GeorgeResourceGroup {
    u16 count;
    u8 flags, field03;
    u8 reserved04[12];
    void *field10;
    u8 reserved14[8];
    u32 field1C;
    const u8 *field20;
    u16 count24, count26;
    GeorgeResourceRecord *field28[16];
    GeorgeResourceRecord *field68[16];
} GeorgeResourceGroup;
typedef char resource_group_holder_offset[(offsetof(GeorgeResourceGroup,field10)==0x10)?1:-1];
typedef char resource_group_first_count_offset[(offsetof(GeorgeResourceGroup,count24)==0x24)?1:-1];
typedef char resource_group_second_count_offset[(offsetof(GeorgeResourceGroup,count26)==0x26)?1:-1];
typedef char resource_group_first_array_offset[(offsetof(GeorgeResourceGroup,field28)==0x28)?1:-1];
typedef char resource_group_second_array_offset[(offsetof(GeorgeResourceGroup,field68)==0x68)?1:-1];
typedef char resource_group_observed_size[(sizeof(GeorgeResourceGroup)==0xA8)?1:-1];

void func_0020F128(GeorgeResourceGroup *,u32 mode) GEORGE_SAVE128;
void func_0020F340(GeorgeResourceGroup *) GEORGE_SAVE128;
void func_0020F440(u32 result,GeorgeResourceGroup *) GEORGE_SAVE128;
void func_0020F5D8(GeorgeResourceGroup *) GEORGE_SAVE128;
GeorgeResourceRecord *func_0020F6F8(void *key,u32 mode,u32 word) GEORGE_SAVE128;
void func_002B6DC0(void *holder);
u32 func_002B7088(void *holder,u32 limit,void **output);
u32 func_002B70E0(void *holder,void **output);
void func_002B6F28(void *holder,u32 index,void *payload);
void func_002B6F48(void *holder,u32 index,void *payload);

#endif
