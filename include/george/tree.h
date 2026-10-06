#ifndef GEORGE_TREE_H
#define GEORGE_TREE_H

#include "george/compiler.h"
#include "george/list.h"

typedef struct GeorgeTreeNode GeorgeTreeNode;
typedef void (*GeorgeTreeCallback)(GeorgeTreeNode *node, void *data);

/* The intrusive prefix and embedded list share the reviewed list layout.
 * attachment is a membership marker: one insertion path writes the node itself.
 * The remaining original class identity and untouched fields are unknown. */
struct GeorgeTreeNode {
    GeorgeTreeNode *next;
    GeorgeTreeNode *previous;
    GeorgeTreeCallback update;
    GeorgeTreeCallback destroy;
    GeorgeTreeNode *attachment;
    void *data;
    s32 priority;
    float elapsed;
    float remaining;
    u32 flags;
    u32 unknown28;
    GeorgeList children;
    u32 cycles;
    u32 unknown3C;
};

typedef struct GeorgeTree {
    GeorgeTreeNode *current;
    GeorgeList roots;
} GeorgeTree;

typedef char george_tree_node_size[(sizeof(GeorgeTreeNode) == 0x40) ? 1 : -1];
typedef char george_tree_size[(sizeof(GeorgeTree) == 16) ? 1 : -1];
typedef char george_tree_attachment_offset[(offsetof(GeorgeTreeNode, attachment) == 0x10) ? 1 : -1];
typedef char george_tree_priority_offset[(offsetof(GeorgeTreeNode, priority) == 0x18) ? 1 : -1];
typedef char george_tree_children_offset[(offsetof(GeorgeTreeNode, children) == 0x2C) ? 1 : -1];
typedef char george_tree_cycles_offset[(offsetof(GeorgeTreeNode, cycles) == 0x38) ? 1 : -1];

GeorgeTreeNode *func_002ACDB0(GeorgeTreeCallback callback, u32 data_size) GEORGE_SAVE128;
void func_002ACEA0(GeorgeTreeNode *node) GEORGE_SAVE128;
void func_002ACF60(GeorgeTreeNode *node, u32 flags);
void func_002ACF98(GeorgeTreeNode *node) GEORGE_SAVE128;
void func_002AD080(GeorgeTreeNode *node) GEORGE_SAVE128;
void func_002AD0C8(GeorgeTreeNode *node, GeorgeTreeNode *parent) GEORGE_SAVE128;
void func_002AD160(GeorgeTreeNode *node, s32 priority) GEORGE_SAVE128;
void func_002AD208(void);
void func_002AD210(GeorgeTree *tree, GeorgeTreeNode *node);
GeorgeList *func_002AD2D0(GeorgeTree *tree);
GeorgeTreeNode *func_002AD2D8(const GeorgeTree *tree);
void func_002AD2E0(GeorgeTree *tree, GeorgeTreeNode *node);
void func_002AD2E8(GeorgeTree *tree, GeorgeTreeNode *node);
void func_002AD350(GeorgeTree *tree);
GeorgeTree *func_002AD378(void) GEORGE_SAVE128;
void func_002AD3B8(GeorgeTree *tree) GEORGE_SAVE128;
void func_002AD400(GeorgeTree *tree) GEORGE_SAVE128;

#endif
