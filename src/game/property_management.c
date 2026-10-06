#include "george/property_management.h"

/* Preserve original numeric strcpy; its complete return is unused here. */
extern void *func_00393B74(void *destination,const void *source);

GeorgePropertyOwnedNode *func_002B9530(const signed char *name,float x,float y,float z)
{
    GeorgePropertyOwnedNode *node=(GeorgePropertyOwnedNode *)func_002AEB60(0xC0,4);
    if (node!=NULL) {
        func_002B9440(node,x,y,z);
        func_002B95B8(node,name);
    }
    return node;
}

void func_002B95B8(GeorgePropertyOwnedNode *node,const signed char *name)
{
    signed char *previous=node->name;
    if (previous!=NULL) {
        func_002AEE40(previous);
        node->name=NULL;
    }
    if (name!=NULL) {
        u32 length=func_00295050(name);
        signed char *replacement=(signed char *)func_002AEC28(length+1U);
        node->name=replacement;
        if (replacement!=NULL) func_00393B74(replacement,name);
    }
}

/* Same reviewed inline record release body within 002BA1D0. Preserve the
 * original unlink, fresh payload reload and per-entry numeric free bindings. */
void func_002BA110(GeorgePropertyTextNode *node)
{
    void *payload;
    if (node->link.next!=NULL||node->link.previous!=NULL)
        func_002ADAE0(&node->link);
    payload=node->payload;
    if (payload!=NULL) func_002AEE40(payload);
    func_002AEE40(node);
}

GeorgeList *func_002BA170(void)
{
    GeorgeList *list=(GeorgeList *)func_002AEC28(12);
    if (list!=NULL) func_002AD9A8(list);
    return list;
}

void func_002BA1B0(GeorgeList *list,GeorgeListNode *node)
{
    func_002AD9C0(list,node);
}

u32 func_002BA268(const GeorgeList *list)
{
    return list->head==(const GeorgeListNode *)&list->tail;
}
