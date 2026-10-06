#ifndef GEORGE_PROPERTY_PACK_H
#define GEORGE_PROPERTY_PACK_H

#include "george/list.h"
#include "george/heap.h"
#include "george/property_records.h"
#include "george/string_algorithms.h"

/* Only this observed 0x94-byte record-producing node is described. The shared
 * intrusive-list sentinel may have only its link prefix, never these fields. */
typedef struct GeorgePropertyTextNode {
    GeorgeListNode link;
    signed char name[128];
    u32 kind,length;
    void *payload;
} GeorgePropertyTextNode;

typedef char property_text_node_size[(sizeof(GeorgePropertyTextNode)==0x94)?1:-1];
typedef char property_text_node_name[(offsetof(GeorgePropertyTextNode,name)==8)?1:-1];
typedef char property_text_node_kind[(offsetof(GeorgePropertyTextNode,kind)==0x88)?1:-1];
typedef char property_text_node_length[(offsetof(GeorgePropertyTextNode,length)==0x8C)?1:-1];
typedef char property_text_node_payload[(offsetof(GeorgePropertyTextNode,payload)==0x90)?1:-1];

GeorgePropertyRecord *func_002B9F38(const GeorgeList *list,u32 *output_size) GEORGE_SAVE128;
GeorgePropertyTextNode *func_002BA070(const signed char *name,u32 kind,u32 length,
                                     const void *payload) GEORGE_SAVE128;

#endif
