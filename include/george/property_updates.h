#ifndef GEORGE_PROPERTY_UPDATES_H
#define GEORGE_PROPERTY_UPDATES_H

#include "george/property_lifecycle.h"

/* Actual +20 callable evidence supports one node argument. The time value is
 * stored at node+18; incidental incoming f12 is not a declared callback input. */
typedef void (*GeorgePropertyUpdateCallback)(GeorgePropertyOwnedNode *node);
typedef char property_update_callback_word[(sizeof(GeorgePropertyUpdateCallback)==4)?1:-1];
typedef char property_update_callback_offset[(offsetof(GeorgePropertyOwnedNode,field20)==0x20)?1:-1];
typedef char property_update_time_offset[(offsetof(GeorgePropertyOwnedNode,field18)==0x18)?1:-1];

void func_002B9628(GeorgePropertyOwnedNode *node);
void func_002B9748(GeorgePropertyOwnedNode *node,float time) GEORGE_SAVE128;
void func_002B9898(GeorgePropertyOwnedNode *node) GEORGE_SAVE128;
void func_002B9C08(GeorgeList *list,float time) GEORGE_SAVE128;
void func_002B9D28(GeorgeList *list) GEORGE_SAVE128;

#endif
