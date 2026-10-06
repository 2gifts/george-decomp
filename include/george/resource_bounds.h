#ifndef GEORGE_RESOURCE_BOUNDS_H
#define GEORGE_RESOURCE_BOUNDS_H

#include "george/resource_groups.h"

/* The 0x40-byte kind-six allocation uses the already observed resource prefix
 * and two vectors. The pointed-to +24 object has a different ownership prefix. */
typedef struct GeorgeResourceBounds {
    GeorgeResourceRecord base;
    GeorgeMathVec3 field28, field34;
} GeorgeResourceBounds;
typedef char resource_bounds_first_offset[(offsetof(GeorgeResourceBounds,field28)==0x28)?1:-1];
typedef char resource_bounds_second_offset[(offsetof(GeorgeResourceBounds,field34)==0x34)?1:-1];
typedef char resource_bounds_size[(sizeof(GeorgeResourceBounds)==0x40)?1:-1];

u32 func_0020F858(const GeorgeResourceGroup *);
GeorgeResourceRecord *func_0020F860(const GeorgeResourceGroup *,u32 index);
GeorgeResourceGroup *func_0020F870(GeorgeResourceGroup *,const void *key,u32 mode,u32 word) GEORGE_SAVE128;
u32 func_0020F8D0(GeorgeResourceGroup *,const GeorgeResourceRecord *child,u32 index) GEORGE_SAVE128;
void func_0020F990(GeorgeResourceGroup *);
void func_0020F9B0(GeorgeResourceGroup *,u32 mode) GEORGE_SAVE128;
void func_0020FA08(GeorgeResourceBounds *,u32 mode) GEORGE_SAVE128;
GeorgeResourceBounds *func_0020FA88(const void *key,u32 mode,u32 word) GEORGE_SAVE128;
void *func_0020FC18(const GeorgeResourceBounds *);
GeorgeMathVec3 *func_0020FC20(GeorgeResourceBounds *);
GeorgeMathVec3 *func_0020FC28(GeorgeResourceBounds *);
GeorgeResourceBounds *func_0020FC30(GeorgeResourceBounds *,const void *key,u32 mode,u32 word) GEORGE_SAVE128;
void func_0020FCC0(GeorgeResourceBounds *,u32 mode) GEORGE_SAVE128;
void func_002B6F60(void *holder,void *payload,u32 index);

#endif
