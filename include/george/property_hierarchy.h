#ifndef GEORGE_PROPERTY_HIERARCHY_H
#define GEORGE_PROPERTY_HIERARCHY_H

#include "george/list.h"
#include "george/compiler.h"

/* Observed prefix only. The original constructor initializes children at +60
 * through the reviewed intrusive-list initializer; no full class is asserted. */
typedef struct GeorgePropertyHierarchyNode {
    GeorgeListNode link;
    u32 field08,key;
    u8 reserved10[0x50];
    GeorgeList children;
    u8 reserved6C[0x44];
    const signed char *name;
} GeorgePropertyHierarchyNode;
typedef s32 (*GeorgePropertyHierarchyCallback)(GeorgePropertyHierarchyNode *,void *);

typedef char property_hierarchy_key[(offsetof(GeorgePropertyHierarchyNode,key)==0xC)?1:-1];
typedef char property_hierarchy_children[(offsetof(GeorgePropertyHierarchyNode,children)==0x60)?1:-1];
typedef char property_hierarchy_name[(offsetof(GeorgePropertyHierarchyNode,name)==0xB0)?1:-1];
typedef char property_hierarchy_prefix[(sizeof(GeorgePropertyHierarchyNode)==0xB4)?1:-1];

GeorgePropertyHierarchyNode *func_002B9928(GeorgePropertyHierarchyNode *node,const signed char *key) GEORGE_SAVE128;
u32 func_002B99B0(GeorgePropertyHierarchyNode *node,const signed char *key,
                  GeorgePropertyHierarchyNode **output) GEORGE_SAVE128;
u32 func_002B9A58(GeorgePropertyHierarchyNode *node,u32 key,u32 remaining,
                  GeorgePropertyHierarchyNode **output) GEORGE_SAVE128;
s32 func_002B9B68(GeorgePropertyHierarchyNode *node,u32 flags,
                  GeorgePropertyHierarchyCallback callback,void *data) GEORGE_SAVE128;
GeorgePropertyHierarchyNode *func_002B9D88(const GeorgeList *list,const signed char *key) GEORGE_SAVE128;
u32 func_002B9E38(const GeorgeList *list,u32 key,u32 remaining,
                  GeorgePropertyHierarchyNode **output) GEORGE_SAVE128;

#endif
