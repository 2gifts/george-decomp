#ifndef GEORGE_PROPERTY_MANAGEMENT_H
#define GEORGE_PROPERTY_MANAGEMENT_H

#include "george/property_lifecycle.h"
#include "george/string_algorithms.h"

/* Existing observed prefixes and intrusive sentinel geometry are reused.
 * Allocating a node reserves 0xC0; the modeled ownership prefix ends at 0xB8. */
GeorgePropertyOwnedNode *func_002B9530(const signed char *name,float x,float y,float z) GEORGE_SAVE128;
void func_002B95B8(GeorgePropertyOwnedNode *node,const signed char *name) GEORGE_SAVE128;
void func_002BA110(GeorgePropertyTextNode *node) GEORGE_SAVE128;
GeorgeList *func_002BA170(void) GEORGE_SAVE128;
void func_002BA1B0(GeorgeList *list,GeorgeListNode *node);
u32 func_002BA268(const GeorgeList *list);

#endif
