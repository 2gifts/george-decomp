#include <stdio.h>
#include <string.h>
#include "george/tree.h"

/* Synthetic nodes and callbacks only; this links the recovered tree and the
 * project's reviewed list implementation, without loading original game data. */
GeorgeTree *D_003FD1E4, *D_003FD1E8, *D_003FD1EC;
const char D_00447040[] = "null";
const char D_00447058[] = "header";
const char D_004470B0[] = "head";
const char D_004470E0[] = "tail";
const char D_00447110[] = "previous";
const char D_00447140[] = "chain";
const char D_00447160[] = "sentinel";

static unsigned checks, failures, controller_allocations, node_allocations;
static unsigned releases, callbacks, event_count;
static u32 requested_size, requested_alignment;
static GeorgeTree allocated_trees[2], alternate;
static unsigned char allocated_node[256] __attribute__((aligned(64)));
static void *release_log[32];
static unsigned event_log[64];
static int fail_controller, fail_node, redirect_allocation, callback_mode;
static GeorgeTreeNode *first, *second, *added, *extra_child;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)

void *func_002AEC28(u32 size)
{
    unsigned index = controller_allocations++;
    CHECK(size == sizeof(GeorgeTree));
    CHECK(index < 2);
    if (fail_controller) return NULL;
    return &allocated_trees[index];
}

void *func_002AEB60(u32 size, u32 alignment)
{
    ++node_allocations;
    requested_size = size;
    requested_alignment = alignment;
    if (redirect_allocation) D_003FD1E4 = &alternate;
    return fail_node ? NULL : allocated_node;
}

void func_002AEE40(void *memory)
{
    CHECK(releases < 32);
    if (releases < 32) release_log[releases] = memory;
    ++releases;
    CHECK(event_count < 64);
    if (event_count < 64) event_log[event_count++] = 100u +
        (memory == first ? 1 : memory == second ? 2 : memory == added ? 3 :
         memory == extra_child ? 4 : 0);
}

static void node_init(GeorgeTreeNode *node, s32 priority)
{
    memset(node, 0, sizeof *node);
    node->priority = priority;
    func_002AD9A8(&node->children);
}

static void reset(void)
{
    controller_allocations = node_allocations = releases = callbacks = event_count = 0;
    fail_controller = fail_node = redirect_allocation = callback_mode = 0;
    first = second = added = extra_child = NULL;
    memset(allocated_node, 0xA5, sizeof allocated_node);
    memset(allocated_trees, 0xA5, sizeof allocated_trees);
    memset(release_log, 0, sizeof release_log);
    memset(event_log, 0, sizeof event_log);
    func_002AD350(&alternate);
    D_003FD1E4 = D_003FD1E8 = D_003FD1EC = NULL;
}

static void list_check(GeorgeList *list, GeorgeTreeNode **expected, unsigned count)
{
    GeorgeListNode *node = list->head;
    GeorgeListNode *previous = (GeorgeListNode *)list;
    unsigned i;
    CHECK(list->tail == NULL);
    for (i = 0; i < count; ++i) {
        CHECK(node == (GeorgeListNode *)expected[i]);
        if (node != (GeorgeListNode *)expected[i]) return;
        CHECK(node->previous == previous);
        previous = node;
        node = node->next;
    }
    CHECK(node == (GeorgeListNode *)&list->tail);
    CHECK(node->next == NULL);
    CHECK(node->previous == previous);
    CHECK(list->tail_previous == previous);
    CHECK(func_002ADB70(list) == NULL);
}

static void callback(GeorgeTreeNode *node, void *data)
{
    ++callbacks;
    CHECK(data == node->data);
    CHECK(node->next == NULL && node->previous == NULL);
    CHECK(node->attachment == NULL);
    CHECK(event_count < 64);
    if (event_count < 64) event_log[event_count++] =
        node == first ? 1 : node == second ? 2 : node == added ? 3 : 4;
    if (node == first && callback_mode == 1) {
        /* Its successor was captured before unlinking. This new head must be
         * left behind; a fresh head walk would instead delete it immediately. */
        func_002AD2E8(D_003FD1E4, added);
        node->next = extra_child; /* must not replace the captured successor */
        D_003FD1E4 = &alternate;
        alternate.current = second;
    }
    if (node == first && callback_mode == 2) {
        /* The current node's child head is loaded after this callback. */
        func_002AD9E0(&node->children, (GeorgeListNode *)extra_child);
        extra_child->attachment = node;
    }
}

static void construction(void)
{
    GeorgeTreeNode *node, parent;
    GeorgeTreeNode *expected[1];
    reset();
    node = func_002ACDB0(callback, 12);
    CHECK(node == (GeorgeTreeNode *)allocated_node);
    CHECK(controller_allocations == 2 && node_allocations == 1);
    CHECK(D_003FD1E8 == &allocated_trees[0]);
    CHECK(D_003FD1EC == &allocated_trees[1]);
    CHECK(D_003FD1E4 == D_003FD1E8);
    CHECK(requested_size == 76 && requested_alignment == 6);
    CHECK(node->data == allocated_node + 64);
    CHECK(node->update == callback && node->destroy == NULL);
    CHECK(node->elapsed == 0.0f && node->remaining == 0.0f);
    CHECK(node->flags == 0 && node->priority == 2000 && node->attachment == NULL);
    CHECK(node->unknown28 == 0xA5A5A5A5u);
    CHECK(node->cycles == 0xA5A5A5A5u && node->unknown3C == 0xA5A5A5A5u);
    list_check(&node->children, NULL, 0);
    expected[0] = node;
    list_check(&D_003FD1E4->roots, expected, 1);
    list_check(&D_003FD1EC->roots, NULL, 0);

    reset();
    D_003FD1E4 = D_003FD1E8 = D_003FD1EC = &alternate;
    node = func_002ACDB0(NULL, 0);
    CHECK(controller_allocations == 0 && node->data == NULL);
    CHECK(requested_size == 64);

    reset();
    node_init(&parent, 0);
    alternate.current = &parent;
    redirect_allocation = 1;
    node = func_002ACDB0(callback, 0xFFFFFFC0u);
    CHECK(requested_size == 0 && requested_alignment == 6);
    CHECK(node->data == allocated_node + 64); /* wrapped size, nonzero argument */
    CHECK(node->attachment == &parent);
    expected[0] = node;
    list_check(&parent.children, expected, 1);
    list_check(&allocated_trees[0].roots, NULL, 0);

    reset();
    fail_node = 1;
    CHECK(func_002ACDB0(callback, 3) == NULL);
    CHECK(controller_allocations == 2 && node_allocations == 1);
    list_check(&allocated_trees[0].roots, NULL, 0);
    reset();
    fail_controller = fail_node = 1;
    CHECK(func_002ACDB0(callback, 9) == NULL);
    CHECK(controller_allocations == 2 && node_allocations == 1);
    CHECK(D_003FD1E4 == NULL && D_003FD1E8 == NULL && D_003FD1EC == NULL);
    reset();
    fail_controller = 1;
    CHECK(func_002AD378() == NULL);
}

static void insertion_permutation(unsigned depth, unsigned used, unsigned *order)
{
    unsigned i, j, k;
    GeorgeTree tree;
    GeorgeTreeNode nodes[5], *expected[5];
    static const s32 priorities[5] = { (s32)0x80000000u, -1, -1, 0, 0x7FFFFFFF };
    if (depth < 5) {
        for (i = 0; i < 5; ++i) if ((used & (1u << i)) == 0) {
            order[depth] = i;
            insertion_permutation(depth + 1, used | (1u << i), order);
        }
        return;
    }
    func_002AD350(&tree);
    for (i = 0; i < 5; ++i) node_init(&nodes[i], priorities[i]);
    for (i = 0; i < 5; ++i) {
        GeorgeTreeNode *new_node = &nodes[order[i]];
        for (j = 0; j < i && expected[j]->priority < new_node->priority; ++j) {}
        for (k = i; k > j; --k) expected[k] = expected[k - 1];
        expected[j] = new_node;
        if (i & 1) func_002AD210(&tree, new_node);
        else func_002AD2E8(&tree, new_node);
        CHECK(new_node->attachment == NULL);
        list_check(&tree.roots, expected, i + 1);
    }
}

static void membership(void)
{
    GeorgeTree tree;
    GeorgeTreeNode parent, other, a, b, c;
    GeorgeTreeNode *expected[3];
    reset();
    func_002AD350(&tree);
    D_003FD1E4 = &tree;
    node_init(&parent, 10); node_init(&other, 20);
    node_init(&a, 1); node_init(&b, 1); node_init(&c, -2);
    func_002AD2E8(&tree, &parent);
    func_002AD2E0(&tree, &parent);
    CHECK(func_002AD2D8(&tree) == &parent && func_002AD2D0(&tree) == &tree.roots);
    func_002AD210(&tree, &a); func_002AD210(&tree, &b); func_002AD210(&tree, &c);
    expected[0] = &c; expected[1] = &b; expected[2] = &a;
    list_check(&parent.children, expected, 3);
    CHECK(a.attachment == &parent && b.attachment == &parent && c.attachment == &parent);
    a.flags = 1; b.flags = 2; c.flags = 4;
    func_002ACF60(&parent, 0x80000008u);
    CHECK(a.flags == 0x80000009u && b.flags == 0x8000000Au && c.flags == 0x8000000Cu);
    CHECK(parent.flags == 0);
    func_002ACF60(&other, 0xFFFFFFFFu);
    list_check(&other.children, NULL, 0);

    func_002AD160(&c, 2);
    expected[0] = &b; expected[1] = &a; expected[2] = &c;
    list_check(&parent.children, expected, 3);
    CHECK(c.priority == 2 && c.attachment == &parent);
    func_002AD0C8(&c, &other);
    CHECK(c.attachment == &c); /* retain the original self marker */
    expected[0] = &c;
    list_check(&other.children, expected, 1);
    func_002AD160(&c, (s32)0x80000000u);
    CHECK(c.attachment == &c);
    func_002AD080(&c);
    CHECK(c.attachment == NULL);
    expected[0] = &c; expected[1] = &parent;
    list_check(&tree.roots, expected, 2);
    list_check(&other.children, NULL, 0);
    func_002AD0C8(&c, &parent);
    expected[0] = &c; expected[1] = &b; expected[2] = &a;
    list_check(&parent.children, expected, 3);
    CHECK(c.attachment == &c);
    func_002AD208();
}

static void destruction(void)
{
    GeorgeTree tree;
    GeorgeTreeNode parent, a, b, c, child;
    GeorgeTreeNode *expected[1];
    reset();
    func_002AD350(&tree); D_003FD1E4 = &tree;
    node_init(&parent, 0); node_init(&a, 1); node_init(&b, 2); node_init(&child, 0);
    first = &parent; second = &a; extra_child = &child;
    parent.destroy = a.destroy = child.destroy = callback;
    parent.data = &b; a.data = &a; child.data = &tree;
    func_002AD2E8(&tree, &parent); tree.current = &parent;
    func_002AD210(&tree, &a); tree.current = &a; func_002AD210(&tree, &child);
    tree.current = &parent;
    func_002ACEA0(&parent);
    CHECK(tree.current == NULL && callbacks == 3 && releases == 3);
    CHECK(event_count == 6 && event_log[0] == 1 && event_log[1] == 2 &&
          event_log[2] == 4 && event_log[3] == 104 && event_log[4] == 102 && event_log[5] == 101);
    CHECK(release_log[0] == &child && release_log[1] == &a && release_log[2] == &parent);
    list_check(&tree.roots, NULL, 0);

    reset(); func_002AD350(&tree); D_003FD1E4 = &tree;
    node_init(&a, 1); node_init(&b, 2); node_init(&c, -1); node_init(&child, 0);
    first = &a; second = &b; added = &c; extra_child = &child;
    a.destroy = b.destroy = callback;
    func_002AD2E8(&tree, &a); func_002AD2E8(&tree, &b);
    tree.current = &a; callback_mode = 1;
    func_002AD3B8(&tree);
    CHECK(tree.current == NULL && alternate.current == NULL);
    CHECK(D_003FD1E4 == &alternate);
    CHECK(callbacks == 2 && releases == 2);
    CHECK(release_log[0] == &a && release_log[1] == &b);
    expected[0] = &c; list_check(&tree.roots, expected, 1);
    CHECK(child.next == NULL && child.previous == NULL);

    reset(); func_002AD350(&tree); D_003FD1E4 = &tree;
    node_init(&parent, 0); node_init(&a, 1); node_init(&b, 2); node_init(&child, 0);
    first = &a; second = &b; extra_child = &child;
    a.destroy = b.destroy = child.destroy = callback;
    func_002AD2E8(&tree, &parent); tree.current = &parent;
    func_002AD210(&tree, &a); func_002AD210(&tree, &b);
    callback_mode = 2;
    func_002ACF98(&parent);
    CHECK(tree.current == &parent && callbacks == 3 && releases == 3);
    CHECK(release_log[0] == &child && release_log[1] == &a && release_log[2] == &b);
    list_check(&parent.children, NULL, 0);
    expected[0] = &parent; list_check(&tree.roots, expected, 1);
    callbacks = releases = event_count = 0;
    func_002AD400(&tree);
    CHECK(releases == 2 && release_log[0] == &parent && release_log[1] == &tree);
    CHECK(tree.current == NULL);
    reset(); func_002AD350(&tree); D_003FD1E4 = &tree;
    func_002AD3B8(&tree); CHECK(releases == 0);
    func_002AD400(&tree); CHECK(releases == 1 && release_log[0] == &tree);
}

int main(void)
{
    unsigned order[5];
    construction();
    insertion_permutation(0, 0, order);
    membership();
    destruction();
    printf("tree: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
