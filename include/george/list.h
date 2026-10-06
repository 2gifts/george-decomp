#ifndef GEORGE_LIST_H
#define GEORGE_LIST_H

#include "george/types.h"

/* Only the observed next/previous prefix is required of an intrusive node. */
typedef struct GeorgeListNode {
    struct GeorgeListNode *next;
    struct GeorgeListNode *previous;
} GeorgeListNode;

/* Head sentinel overlaps head/tail; tail sentinel overlaps tail/tail_previous.
 * Empty: head=&tail, tail=NULL, tail_previous=&head. */
typedef struct GeorgeList {
    GeorgeListNode *head;
    GeorgeListNode *tail;
    GeorgeListNode *tail_previous;
} GeorgeList;
typedef char george_list_node_size[(sizeof(GeorgeListNode) == 8) ? 1 : -1];
typedef char george_list_header_size[(sizeof(GeorgeList) == 12) ? 1 : -1];

void func_002AD9A8(GeorgeList *list);
void func_002AD9C0(GeorgeList *list, GeorgeListNode *node);
void func_002AD9E0(GeorgeList *list, GeorgeListNode *node);
GeorgeListNode *func_002ADA00(GeorgeList *list);
void func_002ADA30(GeorgeList *destination, GeorgeList *source);
void func_002ADA88(GeorgeList *destination, GeorgeList *source);
void func_002ADAE0(GeorgeListNode *node);
void func_002ADB08(GeorgeListNode *position, GeorgeListNode *node);
void func_002ADB28(GeorgeListNode *position, GeorgeListNode *node);
GeorgeListNode *func_002ADB48(const GeorgeListNode *position);
const char *func_002ADB70(const GeorgeList *list);
GeorgeListNode *func_002ADC50(GeorgeListNode *node);

#endif
