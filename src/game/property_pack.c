#include "george/property_pack.h"

extern char *func_00393B74(char *output,const char *input);
extern void *func_003934F8(void *output,const void *input,u32 length);

GeorgePropertyRecord *func_002B9F38(const GeorgeList *list,u32 *output_size)
{
    const GeorgePropertyTextNode *node;
    GeorgePropertyRecord *allocation,*record;
    u32 size=0,count=0,index=0;

    *output_size=0;
    node=(const GeorgePropertyTextNode *)list->head;
    if ((const void *)node==(const void *)&list->tail) return NULL;
    while (node->link.next!=NULL) {
        size=size+8+node->length;
        ++count;
        node=(const GeorgePropertyTextNode *)node->link.next;
    }
    allocation=(GeorgePropertyRecord *)func_002AEC28(size);
    if (allocation==NULL) return allocation;

    node=(const GeorgePropertyTextNode *)list->head;
    record=allocation;
    if (node->link.next!=NULL) {
        const GeorgePropertyTextNode *next=(const GeorgePropertyTextNode *)node->link.next;
        --count;
        do {
            u32 code=func_0029C648(node->name);
            u8 kind;
            record->code=code;
            kind=*(const u8 *)&node->kind;
            record->flags=0;
            record->kind=kind;
            if (index==count) record->flags=0x80;
            record->step=(u16)((u32)(u16)node->length+8);
            ++index;
            func_003934F8((void *)((u32)record+8),node->payload,node->length);
            record=func_0029A890(record);
            node=next;
            next=(const GeorgePropertyTextNode *)next->link.next;
        } while (next!=NULL);
    }
    *output_size=size;
    return allocation;
}

GeorgePropertyTextNode *func_002BA070(const signed char *name,u32 kind,u32 length,
                                     const void *payload)
{
    GeorgePropertyTextNode *node=(GeorgePropertyTextNode *)func_002AEC28(0x94);
    void *memory;
    if (node!=NULL) {
        node->link.previous=NULL;
        node->link.next=NULL;
        func_00393B74((char *)node->name,(const char *)name);
        node->kind=kind;
        node->length=length;
        memory=func_002AEC28(length);
        node->payload=memory;
        if (memory!=NULL) func_003934F8(memory,payload,length);
    }
    return node;
}
