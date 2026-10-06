#include "george/property_lifecycle.h"

/* The original identity-matrix initializer uses VU macro instructions. Keep
 * the actual numeric call; this batch recovers its scalar caller only. */
extern void func_002A1C30(GeorgeRotationMatrix *matrix);

void func_002B9440(GeorgePropertyOwnedNode *node,float x,float y,float z)
{
    node->link.previous=NULL;
    node->link.next=NULL;
    func_002AD9A8(&node->children);
    node->field08=NULL;
    node->key=0;
    node->field14=0;
    node->field12=0;
    node->field13=0;
    node->flags=9;
    node->field28=NULL;
    node->field20=NULL;
    node->destroy=NULL;
    node->position.x=x;
    node->position.z=z;
    node->position.y=y;
    node->rotation.x=0.0f;
    node->rotation.y=0.0f;
    node->rotation.w=1.0f;
    node->rotation.z=0.0f;
    node->flags&=0x3FFF;
    func_002A1C30(&node->transform);
    node->field48.x=0.0f;
    node->field48.z=0.0f;
    node->field48.y=0.0f;
    node->field54.x=0.0f;
    node->field54.z=0.0f;
    node->field54.y=0.0f;
    node->field18=0.0f;
    node->records=NULL;
    node->state=0;
    node->field10=0;
    node->field11=0;
    node->name=NULL;
}

void func_002B9660(GeorgePropertyOwnedNode *node)
{
    GeorgePropertyOwnedNode *child,*next;
    GeorgePropertyDestructor destroy;
    signed char *name;
    if (node->records!=NULL) func_002B9B30(node);
    name=node->name;
    if (name!=NULL) {
        func_002AEE40(name);
        node->name=NULL;
    }
    child=(GeorgePropertyOwnedNode *)node->children.head;
    next=(GeorgePropertyOwnedNode *)child->link.next;
    while (next!=NULL) {
        func_002B9660(child);
        child=next;
        next=(GeorgePropertyOwnedNode *)next->link.next;
    }
    if (node->link.next!=NULL||node->link.previous!=NULL)
        func_002ADAE0(&node->link);
    destroy=node->destroy;
    if (destroy!=NULL) destroy(node);
    else func_002AEE40(node);
}

void func_002B9720(GeorgePropertyOwnedNode *node)
{
    func_002ADAE0(&node->link);
    node->field08=NULL;
}

void func_002B9AF8(GeorgePropertyOwnedNode *node,GeorgeList *records)
{
    func_002B9B30(node);
    node->records=records;
}

void func_002B9B30(GeorgePropertyOwnedNode *node)
{
    GeorgeList *records=node->records;
    if (records!=NULL) {
        func_002BA1D0(records);
        node->records=NULL;
    }
}

void func_002BA1D0(GeorgeList *records)
{
    GeorgePropertyTextNode *node=(GeorgePropertyTextNode *)records->head;
    GeorgePropertyTextNode *next=(GeorgePropertyTextNode *)node->link.next;
    while (next!=NULL) {
        void *payload;
        if (node->link.next!=NULL||node->link.previous!=NULL)
            func_002ADAE0(&node->link);
        payload=node->payload;
        if (payload!=NULL) func_002AEE40(payload);
        func_002AEE40(node);
        node=next;
        next=(GeorgePropertyTextNode *)next->link.next;
    }
    func_002AEE40(records);
}
