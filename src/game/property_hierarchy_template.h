/* Ordinary shared source for the original root bodies in their list wrappers.
 * Every actual recursive call retains its original entry binding. Successors
 * are captured before each recursive call, even when the callback changes links. */
#ifndef GEORGE_PROPERTY_HIERARCHY_TEMPLATE_H
#define GEORGE_PROPERTY_HIERARCHY_TEMPLATE_H

#define HIERARCHY_FIND_BODY(current,key_text,result) do { \
    GeorgePropertyHierarchyNode *hierarchy_root=(current),*hierarchy_child,*hierarchy_next; \
    const signed char *name=hierarchy_root->name; \
    (result)=NULL; \
    if (name!=NULL&&func_00393A28((const char *)name,(const char *)(key_text))==0) { \
        (result)=hierarchy_root; \
    } else { \
        hierarchy_child=(GeorgePropertyHierarchyNode *)hierarchy_root->children.head; \
        hierarchy_next=(GeorgePropertyHierarchyNode *)hierarchy_child->link.next; \
        while (hierarchy_next!=NULL) { \
            (result)=func_002B9928(hierarchy_child,(key_text)); \
            if ((result)!=NULL) break; \
            hierarchy_child=hierarchy_next; \
            hierarchy_next=(GeorgePropertyHierarchyNode *)hierarchy_next->link.next; \
        } \
    } \
} while (0)

#define HIERARCHY_TYPE_BODY(current,code,limit,destination,total) do { \
    GeorgePropertyHierarchyNode *hierarchy_root=(current),*hierarchy_child,*hierarchy_next; \
    GeorgePropertyHierarchyNode **hierarchy_cursor=(destination); \
    (total)=0; \
    if (hierarchy_root->key==(code)) { \
        *hierarchy_cursor++=hierarchy_root; \
        (total)=1; \
    } \
    hierarchy_child=(GeorgePropertyHierarchyNode *)hierarchy_root->children.head; \
    hierarchy_next=(GeorgePropertyHierarchyNode *)hierarchy_child->link.next; \
    while (hierarchy_next!=NULL) { \
        u32 hierarchy_amount=func_002B9A58(hierarchy_child,(code),(u32)(limit)-(total),hierarchy_cursor); \
        hierarchy_child=hierarchy_next; \
        hierarchy_next=(GeorgePropertyHierarchyNode *)hierarchy_next->link.next; \
        hierarchy_cursor=(GeorgePropertyHierarchyNode **)((u32)hierarchy_cursor+hierarchy_amount*4); \
        (total)+=hierarchy_amount; \
    } \
} while (0)

#endif
