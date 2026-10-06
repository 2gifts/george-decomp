#include "george/list.h"

extern const char D_00447040[];
extern const char D_00447058[];
extern const char D_004470B0[];
extern const char D_004470E0[];
extern const char D_00447110[];
extern const char D_00447140[];
extern const char D_00447160[];

void func_002AD9A8(GeorgeList *list)
{
    list->tail = 0;
    list->head = (GeorgeListNode *)&list->tail;
    list->tail_previous = (GeorgeListNode *)list;
}

/* Field expressions deliberately reload around stores: the sentinels overlap
 * the header, and a node may alias the observed list fields. */
void func_002ADA30(GeorgeList *destination, GeorgeList *source)
{
    GeorgeListNode *sentinel = (GeorgeListNode *)&source->tail;
    if (source->head != sentinel) {
        source->tail_previous->next = destination->tail_previous->next;
        destination->tail_previous->next = source->head;
        source->head->previous = destination->tail_previous;
        destination->tail_previous = source->tail_previous;
        source->head = sentinel;
        source->tail = 0;
        source->tail_previous = (GeorgeListNode *)source;
    }
}

void func_002ADA88(GeorgeList *destination, GeorgeList *source)
{
    GeorgeListNode *sentinel = (GeorgeListNode *)&source->tail;
    if (source->head != sentinel) {
        source->head->previous = destination->head->previous;
        destination->head->previous = source->tail_previous;
        source->tail_previous->next = destination->head;
        destination->head = source->head;
        source->head = sentinel;
        source->tail = 0;
        source->tail_previous = (GeorgeListNode *)source;
    }
}

void func_002ADB08(GeorgeListNode *position, GeorgeListNode *node)
{
    position->previous->next = node;
    node->previous = position->previous;
    position->previous = node;
    node->next = position;
}

void func_002ADB28(GeorgeListNode *position, GeorgeListNode *node)
{
    position->next->previous = node;
    node->next = position->next;
    position->next = node;
    node->previous = position;
}

GeorgeListNode *func_002ADB48(const GeorgeListNode *position)
{
    GeorgeListNode *next = position->next;
    if (next != 0 && next->next != 0) return next;
    return 0;
}

const char *func_002ADB70(const GeorgeList *list)
{
    GeorgeListNode *head, *last, *sentinel, *previous, *node;
    if (list == 0) return D_00447040;
    head = list->head;
    if (head == 0 || list->tail != 0 || list->tail_previous == 0)
        return D_00447058;
    last = list->tail_previous;
    if (head->previous != (GeorgeListNode *)list) return D_004470B0;
    sentinel = (GeorgeListNode *)&list->tail;
    if (last->next != sentinel) return D_004470E0;
    previous = head;
    node = head->next;
    while (node != 0 && node->next != 0) {
        GeorgeListNode *next = node->next;
        if (node->previous == 0) return D_00447110;
        if (node->previous != previous) return D_00447140;
        previous = node;
        node = next;
    }
    if (head != sentinel && node != sentinel) return D_00447160;
    return 0;
}

GeorgeListNode *func_002ADC50(GeorgeListNode *node)
{
    while (node->previous != 0) node = node->previous;
    return node;
}
