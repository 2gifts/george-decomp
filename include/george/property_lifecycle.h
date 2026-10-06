#ifndef GEORGE_PROPERTY_LIFECYCLE_H
#define GEORGE_PROPERTY_LIFECYCLE_H

#include "george/property_hierarchy.h"
#include "george/property_pack.h"
#include "george/rotation.h"

/* Observed fields through +B4 only. Allocating callers reserve more bytes;
 * this prefix does not assert the complete original object class or size. */
typedef struct GeorgePropertyOwnedNode GeorgePropertyOwnedNode;
typedef void (*GeorgePropertyDestructor)(GeorgePropertyOwnedNode *);
struct GeorgePropertyOwnedNode {
    GeorgeListNode link;
    GeorgePropertyOwnedNode *field08;
    u32 key;
    u8 field10,field11,field12,field13;
    u32 field14;
    float field18;
    u16 flags,state;
    void *field20;
    GeorgePropertyDestructor destroy;
    void *field28;
    GeorgeMathVec3 position;
    GeorgeMathVec4 rotation;
    GeorgeMathVec3 field48,field54;
    GeorgeList children;
    u32 reserved6C;
    GeorgeRotationMatrix transform;
    signed char *name;
    GeorgeList *records;
};

typedef char property_owned_flags[(offsetof(GeorgePropertyOwnedNode,flags)==0x1C)?1:-1];
typedef char property_owned_key[(offsetof(GeorgePropertyOwnedNode,key)==offsetof(GeorgePropertyHierarchyNode,key))?1:-1];
typedef char property_owned_destroy[(offsetof(GeorgePropertyOwnedNode,destroy)==0x24)?1:-1];
typedef char property_owned_position[(offsetof(GeorgePropertyOwnedNode,position)==0x2C)?1:-1];
typedef char property_owned_rotation[(offsetof(GeorgePropertyOwnedNode,rotation)==0x38)?1:-1];
typedef char property_owned_fields48[(offsetof(GeorgePropertyOwnedNode,field48)==0x48)?1:-1];
typedef char property_owned_fields54[(offsetof(GeorgePropertyOwnedNode,field54)==0x54)?1:-1];
typedef char property_owned_children[(offsetof(GeorgePropertyOwnedNode,children)==offsetof(GeorgePropertyHierarchyNode,children))?1:-1];
typedef char property_owned_transform[(offsetof(GeorgePropertyOwnedNode,transform)==0x70)?1:-1];
typedef char property_owned_name[(offsetof(GeorgePropertyOwnedNode,name)==offsetof(GeorgePropertyHierarchyNode,name))?1:-1];
typedef char property_owned_records[(offsetof(GeorgePropertyOwnedNode,records)==0xB4)?1:-1];
typedef char property_owned_prefix[(sizeof(GeorgePropertyOwnedNode)==0xB8)?1:-1];

void func_002B9440(GeorgePropertyOwnedNode *node,float x,float y,float z) GEORGE_SAVE128;
void func_002B9660(GeorgePropertyOwnedNode *node) GEORGE_SAVE128;
void func_002B9720(GeorgePropertyOwnedNode *node) GEORGE_SAVE128;
void func_002B9AF8(GeorgePropertyOwnedNode *node,GeorgeList *records) GEORGE_SAVE128;
void func_002B9B30(GeorgePropertyOwnedNode *node) GEORGE_SAVE128;
void func_002BA1D0(GeorgeList *records) GEORGE_SAVE128;

#endif
