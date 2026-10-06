#include "george/property_updates.h"
#include "property_updates_template.h"

/* Preserve the original complete VU transform call. Only its scalar callers
 * are reconstructed here; no VU instruction body or scheduling is replaced. */
extern void func_002A1F18(GeorgeRotationMatrix *output,const GeorgeMathVec4 *rotation,
                         const GeorgeMathVec3 *position);

void func_002B9628(GeorgePropertyOwnedNode *node)
{
    u16 flags=node->flags;
    PROPERTY_DEFER_CAPTURED(node,flags);
}

void func_002B9748(GeorgePropertyOwnedNode *node,float time)
{
    GeorgePropertyUpdateCallback callback;
    node->field18=time;
    callback=(GeorgePropertyUpdateCallback)node->field20;
    PROPERTY_UPDATE_AFTER_CAPTURE(node,time,callback);
}

void func_002B9898(GeorgePropertyOwnedNode *node)
{
    u16 mode=node->flags&0xC000;
    if (mode==0x8000) {
        float z;
        node->transform.element[12]=node->position.x;
        node->transform.element[13]=node->position.y;
        z=node->position.z;
        node->transform.element[15]=1.0f;
        node->transform.element[14]=z;
    } else if (mode!=0xC000) {
        func_002A1F18(&node->transform,&node->rotation,&node->position);
    }
    node->flags|=0xC000;
}

void func_002B9C08(GeorgeList *list,float time)
{
    GeorgePropertyOwnedNode *node=(GeorgePropertyOwnedNode *)list->head;
    GeorgePropertyOwnedNode *next=(GeorgePropertyOwnedNode *)node->link.next;
    while (next!=NULL) {
        GeorgePropertyUpdateCallback callback=(GeorgePropertyUpdateCallback)node->field20;
        node->field18=time;
        PROPERTY_UPDATE_AFTER_CAPTURE(node,time,callback);
        node=next;
        next=(GeorgePropertyOwnedNode *)next->link.next;
    }
}

void func_002B9D28(GeorgeList *list)
{
    GeorgePropertyOwnedNode *node=(GeorgePropertyOwnedNode *)list->head;
    GeorgePropertyOwnedNode *next=(GeorgePropertyOwnedNode *)node->link.next;
    while (next!=NULL) {
        u16 flags=node->flags;
        PROPERTY_DEFER_CAPTURED(node,flags);
        node=next;
        next=(GeorgePropertyOwnedNode *)next->link.next;
    }
}
