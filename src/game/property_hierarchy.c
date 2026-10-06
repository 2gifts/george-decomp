#include "george/property_hierarchy.h"

extern s32 func_00393A28(const char *left,const char *right);
extern char *func_00398628(const char *text,const char *substring);
#include "property_hierarchy_template.h"

GeorgePropertyHierarchyNode *func_002B9928(GeorgePropertyHierarchyNode *node,const signed char *key)
{
    GeorgePropertyHierarchyNode *result;
    HIERARCHY_FIND_BODY(node,key,result);
    return result;
}

u32 func_002B99B0(GeorgePropertyHierarchyNode *node,const signed char *key,
                  GeorgePropertyHierarchyNode **output)
{
    GeorgePropertyHierarchyNode *child,*next;
    const signed char *name=node->name;
    u32 count=0;
    if (name!=NULL&&func_00398628((const char *)name,(const char *)key)!=NULL) {
        *output++=node;
        count=1;
    }
    child=(GeorgePropertyHierarchyNode *)node->children.head;
    next=(GeorgePropertyHierarchyNode *)child->link.next;
    while (next!=NULL) {
        u32 amount=func_002B99B0(child,key,output);
        child=next;
        next=(GeorgePropertyHierarchyNode *)next->link.next;
        output=(GeorgePropertyHierarchyNode **)((u32)output+amount*4);
        count+=amount;
    }
    return count;
}

u32 func_002B9A58(GeorgePropertyHierarchyNode *node,u32 key,u32 remaining,
                  GeorgePropertyHierarchyNode **output)
{
    u32 count;
    HIERARCHY_TYPE_BODY(node,key,remaining,output,count);
    return count;
}

s32 func_002B9B68(GeorgePropertyHierarchyNode *node,u32 flags,
                  GeorgePropertyHierarchyCallback callback,void *data)
{
    GeorgePropertyHierarchyNode *child,*next;
    s32 result=1;
    if (flags&1) result=callback(node,data);
    if (result!=0&&(flags&2)) {
        child=(GeorgePropertyHierarchyNode *)node->children.head;
        next=(GeorgePropertyHierarchyNode *)child->link.next;
        while (next!=NULL) {
            result=func_002B9B68(child,flags|1,callback,data);
            if (result==0) break;
            child=next;
            next=(GeorgePropertyHierarchyNode *)next->link.next;
        }
    }
    return result;
}

GeorgePropertyHierarchyNode *func_002B9D88(const GeorgeList *list,const signed char *key)
{
    GeorgePropertyHierarchyNode *node=(GeorgePropertyHierarchyNode *)list->head;
    GeorgePropertyHierarchyNode *next=(GeorgePropertyHierarchyNode *)node->link.next;
    GeorgePropertyHierarchyNode *result=NULL;
    while (next!=NULL) {
        HIERARCHY_FIND_BODY(node,key,result);
        if (result!=NULL) break;
        node=next;
        next=(GeorgePropertyHierarchyNode *)next->link.next;
    }
    return result;
}

u32 func_002B9E38(const GeorgeList *list,u32 key,u32 remaining,
                  GeorgePropertyHierarchyNode **output)
{
    GeorgePropertyHierarchyNode *node=(GeorgePropertyHierarchyNode *)list->head;
    GeorgePropertyHierarchyNode *next=(GeorgePropertyHierarchyNode *)node->link.next;
    u32 count=0;
    while (next!=NULL) {
        u32 amount;
        GeorgePropertyHierarchyNode **cursor=(GeorgePropertyHierarchyNode **)((u32)output+count*4);
        HIERARCHY_TYPE_BODY(node,key,remaining-count,cursor,amount);
        count+=amount;
        if (count>=remaining) break;
        node=next;
        next=(GeorgePropertyHierarchyNode *)next->link.next;
    }
    return count;
}
