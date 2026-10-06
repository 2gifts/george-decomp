/*
 * Copyright (C) 1995-2012, The AROS Development Team. All rights reserved.
 *
 * This file is derived from the AROS Development Team's exec AddHead,
 * AddTail, RemHead and Remove routines, revision
 * e8e543e6ca866e26671c8f586d545f80609ef3dd.
 * The contents are subject to the AROS Public License Version 1.1.
 * See LICENSES/AROS-Public-License-1.1.txt or https://www.aros.org/license.html.
 * Software is distributed on an AS IS basis, WITHOUT WARRANTY OF ANY KIND,
 * either express or implied. See that license for rights and limitations.
 *
 * Modifications, 2026-10-06: Adapted the public pointer-link algorithms to
 * GeorgeList/GeorgeListNode and the original entry-point signatures. Removed
 * AROS library-call/debug macros. Preserved observed assignment order and
 * alias-sensitive reloads. RemHead tests the explicit tail sentinel; both
 * removal routines clear removed links as the game does. This is behavioral
 * reuse, not evidence that Papaya used AROS source. The empty guard agrees
 * for valid lists but can differ from AROS for a corrupted list.
 */
#include "george/list.h"

/* Adapted rom/exec/addtail.c: Copyright (C) 1995-2007 AROS Development Team. */
void func_002AD9C0(GeorgeList *list, GeorgeListNode *node)
{
    node->previous = list->tail_previous;
    node->next = (GeorgeListNode *)&list->tail;
    list->tail_previous->next = node;
    list->tail_previous = node;
}

/* Adapted rom/exec/addhead.c: Copyright (C) 1995-2012 AROS Development Team. */
void func_002AD9E0(GeorgeList *list, GeorgeListNode *node)
{
    node->next = list->head;
    node->previous = (GeorgeListNode *)list;
    list->head->previous = node;
    list->head = node;
}

/* Adapted rom/exec/remhead.c: Copyright (C) 1995-2001 AROS Development Team. */
GeorgeListNode *func_002ADA00(GeorgeList *list)
{
    GeorgeListNode *node = list->head;
    GeorgeListNode *next;
    if (node == (GeorgeListNode *)&list->tail)
        return 0;
    next = node->next;
    list->head = next;
    next->previous = (GeorgeListNode *)list;
    node->previous = 0;
    node->next = 0;
    return node;
}

/* Adapted rom/exec/remove.c: Copyright (C) 1995-2001 AROS Development Team. */
void func_002ADAE0(GeorgeListNode *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
    node->next = 0;
    node->previous = 0;
}
