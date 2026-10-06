#include "george/tree_updates.h"
#include "george/heap.h"

extern GeorgeTree *D_003FD1E4, *D_003FD1E8, *D_003FD1EC;

/* The bootstrap and node initialization sequences reuse the already reviewed
 * constructor source. Retail inlines them in its root-only constructor. */
#define BOOTSTRAP_TREES() do { \
    if (D_003FD1E8 == NULL) D_003FD1E8 = func_002AD378(); \
    if (D_003FD1EC == NULL) D_003FD1EC = func_002AD378(); \
    if (D_003FD1E4 == NULL) D_003FD1E4 = D_003FD1E8; \
} while (0)

#define INITIALIZE_NODE(node, callback, data_size) do { \
    void *data = (data_size) == 0 ? NULL : (void *)((u32)(node) + 0x40u); \
    (node)->previous = NULL; \
    (node)->next = NULL; \
    (node)->attachment = NULL; \
    (node)->elapsed = 0.0f; \
    (node)->flags = 0; \
    (node)->data = data; \
    (node)->priority = 2000; \
    func_002AD9A8(&(node)->children); \
    (node)->update = (callback); \
    (node)->destroy = NULL; \
    (node)->remaining = 0.0f; \
    func_002AD210(D_003FD1E4, node); \
} while (0)

#define UPDATE_ROOTS(tree, elapsed) do { \
    GeorgeList *root_list = func_002AD2D0(tree); \
    GeorgeTreeNode *root_node = (GeorgeTreeNode *)root_list->head; \
    GeorgeTreeNode *root_next = root_node->next; \
    while (root_next != NULL) { \
        func_002AC820(root_node, elapsed); \
        root_node = root_next; \
        root_next = root_node->next; \
    } \
} while (0)

void func_002AC820(GeorgeTreeNode *node, float elapsed)
{
    u32 before, flags;
    float remaining;
    GeorgeTreeCallback callback;
    func_002AD2E0(D_003FD1E4, node);
    remaining = node->remaining - elapsed;
    node->elapsed = elapsed;
    node->remaining = remaining;
    if (remaining <= 0.0f) {
        node->remaining = 0.0f;
        node->flags = (node->flags | 0x20000u) & 0xFFFEFFFFu;
    } else {
        node->flags &= 0xFFFDFFFFu;
    }
    before = george_tree_read_count();
    flags = node->flags;
    if ((flags & 0x10000u) == 0 || (flags & 0x20000u) != 0) {
        callback = node->update;
        if (callback != NULL) callback(node, node->data);
    }
    /* subu and the following sw keep only the wrapped low32 result. */
    node->cycles = george_tree_read_count() - before;
    if (func_002AD2D8(D_003FD1E4) != NULL) {
        func_002AD2E0(D_003FD1E4, NULL);
        if ((node->flags & 0x40000u) == 0) {
            GeorgeTreeNode *child = (GeorgeTreeNode *)node->children.head;
            GeorgeTreeNode *next = child->next;
            while (next != NULL) {
                u32 child_cycles;
                func_002AC820(child, elapsed);
                child_cycles = child->cycles;
                node->cycles = node->cycles + child_cycles;
                child = next;
                next = child->next;
            }
        }
    }
}

GeorgeTreeNode *func_002AC9B8(GeorgeTreeCallback callback, u32 data_size)
{
    GeorgeTreeNode *saved, *node;
    BOOTSTRAP_TREES();
    saved = func_002AD2D8(D_003FD1E4);
    func_002AD2E0(D_003FD1E4, NULL);
    BOOTSTRAP_TREES();
    node = func_002AEB60(data_size + 0x40u, 6);
    if (node != NULL) INITIALIZE_NODE(node, callback, data_size);
    /* Allocation may replace the active controller. Restore to its reloaded
     * slot, as retail does, even when allocation failed. */
    func_002AD2E0(D_003FD1E4, saved);
    return node;
}

GeorgeList *func_002ACB10(void)
{
    return func_002AD2D0(D_003FD1E8);
}

void func_002ACB38(void)
{
    BOOTSTRAP_TREES();
}

void func_002ACBA8(void)
{
    func_002AD400(D_003FD1E8);
    func_002AD400(D_003FD1EC);
    D_003FD1E8 = NULL;
    D_003FD1EC = NULL;
    /* The active slot intentionally remains untouched. */
}

void func_002ACBF0(float elapsed)
{
    UPDATE_ROOTS(D_003FD1E4, elapsed);
}

void func_002ACC50(float elapsed)
{
    BOOTSTRAP_TREES();
    D_003FD1E4 = D_003FD1E8;
    UPDATE_ROOTS(D_003FD1E8, elapsed);
}

void func_002ACD00(float elapsed)
{
    BOOTSTRAP_TREES();
    D_003FD1E4 = D_003FD1EC;
    UPDATE_ROOTS(D_003FD1EC, elapsed);
}

#undef UPDATE_ROOTS
#undef INITIALIZE_NODE
#undef BOOTSTRAP_TREES
