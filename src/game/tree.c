#include "george/tree.h"
#include "george/heap.h"

extern GeorgeTree *D_003FD1E4, *D_003FD1E8, *D_003FD1EC;

/* Save the successor before destruction, then read that successor's next only
 * after the callback and release. The final list sentinel has a null next. */
#define DESTROY_CHILDREN(head) do { \
    GeorgeTreeNode *destroy_child = (GeorgeTreeNode *)(head); \
    GeorgeTreeNode *destroy_next = destroy_child->next; \
    while (destroy_next != NULL) { \
        func_002ACEA0(destroy_child); \
        destroy_child = destroy_next; \
        destroy_next = destroy_child->next; \
    } \
} while (0)

/* The complete sequence is inlined in retail's delete-children routine. */
#define DESTROY_NODE(node) do { \
    GeorgeTreeCallback callback; \
    if (func_002AD2D8(D_003FD1E4) == (node)) \
        func_002AD2E0(D_003FD1E4, NULL); \
    if ((node)->next != NULL || (node)->previous != NULL) { \
        func_002ADAE0((GeorgeListNode *)(node)); \
        (node)->attachment = NULL; \
    } \
    callback = (node)->destroy; \
    if (callback != NULL) callback(node, (node)->data); \
    DESTROY_CHILDREN((node)->children.head); \
    func_002AEE40(node); \
} while (0)

/* Compare signed priorities; a new equal-priority node precedes the old one.
 * Read no priority from the sentinel, and retain pointer reloads between stores. */
#define INSERT_SORTED(position, node, key) do { \
    if ((position)->next != NULL) { \
        s32 priority_key = (key); \
        while ((position)->priority < priority_key) { \
            (position) = (position)->next; \
            if ((position)->next == NULL) break; \
        } \
    } \
    (position)->previous->next = (node); \
    (node)->previous = (position)->previous; \
    (position)->previous = (node); \
    (node)->next = (position); \
} while (0)

GeorgeTreeNode *func_002ACDB0(GeorgeTreeCallback callback, u32 data_size)
{
    GeorgeTreeNode *node;
    if (D_003FD1E8 == NULL) D_003FD1E8 = func_002AD378();
    if (D_003FD1EC == NULL) D_003FD1EC = func_002AD378();
    if (D_003FD1E4 == NULL) D_003FD1E4 = D_003FD1E8;
    node = func_002AEB60(data_size + 0x40u, 6);
    if (node != NULL) {
        void *data = data_size == 0 ? NULL : (void *)((u32)node + 0x40u);
        node->previous = NULL;
        node->next = NULL;
        node->attachment = NULL;
        node->elapsed = 0.0f;
        node->flags = 0;
        node->data = data;
        node->priority = 2000;
        func_002AD9A8(&node->children);
        node->update = callback;
        node->destroy = NULL;
        node->remaining = 0.0f;
        func_002AD210(D_003FD1E4, node);
    }
    return node;
}

void func_002ACEA0(GeorgeTreeNode *node)
{
    DESTROY_NODE(node);
}

void func_002ACF60(GeorgeTreeNode *node, u32 flags)
{
    GeorgeTreeNode *child = (GeorgeTreeNode *)node->children.head;
    GeorgeTreeNode *next = child->next;
    while (next != NULL) {
        child->flags |= flags;
        child = next;
        next = child->next;
    }
}

void func_002ACF98(GeorgeTreeNode *node)
{
    GeorgeTreeNode *child = (GeorgeTreeNode *)node->children.head;
    GeorgeTreeNode *next = child->next;
    while (next != NULL) {
        DESTROY_NODE(child);
        child = next;
        next = child->next;
    }
}

void func_002AD080(GeorgeTreeNode *node)
{
    if (node->attachment != NULL) {
        func_002ADAE0((GeorgeListNode *)node);
        node->attachment = NULL;
    }
    func_002AD2E8(D_003FD1E4, node);
}

void func_002AD0C8(GeorgeTreeNode *node, GeorgeTreeNode *parent)
{
    GeorgeTreeNode *position;
    func_002ADAE0((GeorgeListNode *)node);
    node->attachment = NULL;
    position = (GeorgeTreeNode *)parent->children.head;
    INSERT_SORTED(position, node, node->priority);
    /* The original stores the inserted node, rather than the parent argument. */
    node->attachment = node;
}

void func_002AD160(GeorgeTreeNode *node, s32 priority)
{
    GeorgeListNode *head = func_002ADC50((GeorgeListNode *)node);
    GeorgeTreeNode *position;
    func_002ADAE0((GeorgeListNode *)node);
    node->priority = priority;
    position = (GeorgeTreeNode *)head->next;
    INSERT_SORTED(position, node, priority);
}

void func_002AD208(void)
{
}

void func_002AD210(GeorgeTree *tree, GeorgeTreeNode *node)
{
    GeorgeTreeNode *current = tree->current;
    if (current != NULL) {
        GeorgeTreeNode *position = (GeorgeTreeNode *)current->children.head;
        INSERT_SORTED(position, node, node->priority);
        node->attachment = tree->current;
    } else {
        GeorgeTreeNode *position = (GeorgeTreeNode *)tree->roots.head;
        INSERT_SORTED(position, node, node->priority);
        node->attachment = NULL;
    }
}

GeorgeList *func_002AD2D0(GeorgeTree *tree) { return &tree->roots; }
GeorgeTreeNode *func_002AD2D8(const GeorgeTree *tree) { return tree->current; }
void func_002AD2E0(GeorgeTree *tree, GeorgeTreeNode *node) { tree->current = node; }

void func_002AD2E8(GeorgeTree *tree, GeorgeTreeNode *node)
{
    GeorgeTreeNode *position = (GeorgeTreeNode *)tree->roots.head;
    INSERT_SORTED(position, node, node->priority);
    node->attachment = NULL;
}

void func_002AD350(GeorgeTree *tree)
{
    tree->current = NULL;
    func_002AD9A8(&tree->roots);
}

GeorgeTree *func_002AD378(void)
{
    GeorgeTree *tree = func_002AEC28(16);
    if (tree != NULL) {
        tree->current = NULL;
        func_002AD9A8(&tree->roots);
    }
    return tree;
}

void func_002AD3B8(GeorgeTree *tree)
{
    DESTROY_CHILDREN(tree->roots.head);
}

void func_002AD400(GeorgeTree *tree)
{
    DESTROY_CHILDREN(tree->roots.head);
    func_002AEE40(tree);
}

#undef INSERT_SORTED
#undef DESTROY_NODE
#undef DESTROY_CHILDREN
