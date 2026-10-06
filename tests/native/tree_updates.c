#include <stdio.h>
#include <string.h>
#include "george/tree_updates.h"

/* The explicit native counter substitute covers traversal and wrapped arithmetic
 * only. No original data, machine instructions, or CP0 timing model is used. */
GeorgeTree *D_003FD1E4, *D_003FD1E8, *D_003FD1EC;
const char D_00447040[] = "null";
const char D_00447058[] = "header";
const char D_004470B0[] = "head";
const char D_004470E0[] = "tail";
const char D_00447110[] = "previous";
const char D_00447140[] = "chain";
const char D_00447160[] = "sentinel";
static unsigned checks, failures, tick_index, callbacks, allocations, node_allocations, releases;
static u32 ticks[32], requested_size, requested_alignment;
static GeorgeTree primary, secondary, alternate, controllers[2];
static unsigned char allocation[256] __attribute__((aligned(64)));
static GeorgeTreeNode *callback_log[32], *first, *second, *third, *child, *parent, *injected;
static void *release_log[16];
static int callback_mode, allocation_mode, fail_node, fail_controller, free_mode, tick_mode;
static float expected_elapsed;
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; printf("failure line %d\n", __LINE__); } } while (0)

u32 george_tree_test_counter(void)
{
    u32 value;
    CHECK(tick_index < 32);
    value = ticks[tick_index < 32 ? tick_index : 31];
    if (tick_index == 0) {
        CHECK(D_003FD1E4->current == first);
        CHECK(first->elapsed == expected_elapsed);
        if (tick_mode == 1) first->flags |= 0x10000u;
    }
    ++tick_index;
    return value;
}

void *func_002AEC28(u32 size)
{
    unsigned index = allocations++;
    CHECK(size == 16 && index < 2);
    if (allocation_mode == 2 && index == 0) D_003FD1EC = &secondary;
    return fail_controller ? NULL : &controllers[index];
}

void *func_002AEB60(u32 size, u32 alignment)
{
    ++node_allocations;
    requested_size = size; requested_alignment = alignment;
    if (allocation_mode == 1) {
        D_003FD1E4 = &alternate;
        alternate.current = parent;
    }
    return fail_node ? NULL : allocation;
}

void func_002AEE40(void *memory)
{
    CHECK(releases < 16);
    if (releases < 16) release_log[releases] = memory;
    ++releases;
    if (free_mode == 1 && memory == &primary) D_003FD1EC = &alternate;
}

static void callback(GeorgeTreeNode *node, void *data)
{
    CHECK(callbacks < 32);
    if (callbacks < 32) callback_log[callbacks] = node;
    ++callbacks;
    CHECK(data == node->data);
    CHECK(D_003FD1E4->current == node);
    CHECK(node->elapsed == expected_elapsed);
    if (node == first) {
        if (callback_mode == 1) D_003FD1E4->current = NULL;
        if (callback_mode == 2) {
            D_003FD1E4 = &alternate;
            alternate.current = third;
        }
        if (callback_mode == 3) node->flags |= 0x40000u;
        if (callback_mode == 4) {
            func_002AD9E0(&node->children, (GeorgeListNode *)injected);
            injected->attachment = node;
        }
        if (callback_mode == 5) func_002ADAE0((GeorgeListNode *)second);
        if (callback_mode == 6) node->next = injected;
    }
    if (node == child && callback_mode == 7) parent->cycles = 100;
}

static void node_init(GeorgeTreeNode *node, s32 priority)
{
    memset(node, 0, sizeof *node);
    node->priority = priority; node->update = callback; node->data = node;
    func_002AD9A8(&node->children);
}

static void reset(void)
{
    unsigned i;
    tick_index = callbacks = allocations = node_allocations = releases = 0;
    callback_mode = allocation_mode = fail_node = fail_controller = free_mode = tick_mode = 0;
    first = second = third = child = parent = injected = NULL;
    expected_elapsed = 0.25f;
    memset(callback_log, 0, sizeof callback_log);
    memset(release_log, 0, sizeof release_log);
    memset(allocation, 0xA5, sizeof allocation);
    memset(controllers, 0xA5, sizeof controllers);
    func_002AD350(&primary); func_002AD350(&secondary); func_002AD350(&alternate);
    D_003FD1E4 = D_003FD1E8 = &primary; D_003FD1EC = &secondary;
    for (i = 0; i < 32; ++i) ticks[i] = i * 7u;
}

static void flags_and_deadlines(void)
{
    GeorgeTreeNode node;
    unsigned flag_case, time_case;
    union { u32 bits; float scalar; } nan;
    nan.bits = 0x7FC00000u;
    for (time_case = 0; time_case < 4; ++time_case) {
        for (flag_case = 0; flag_case < 8; ++flag_case) {
            u32 starting_flags = (flag_case << 16) | 0x8123u;
            u32 wanted_flags;
            reset(); node_init(&node, 0); first = &node;
            node.flags = starting_flags;
            node.remaining = time_case == 0 ? 1.0f : time_case == 1 ? 0.25f :
                             time_case == 2 ? -1.0f : nan.scalar;
            wanted_flags = (time_case == 1 || time_case == 2) ?
                ((starting_flags | 0x20000u) & 0xFFFEFFFFu) : (starting_flags & 0xFFFDFFFFu);
            func_002AC820(&node, expected_elapsed);
            CHECK(node.flags == wanted_flags);
            CHECK(node.elapsed == expected_elapsed);
            if (time_case == 0) CHECK(node.remaining == 0.75f);
            if (time_case == 1 || time_case == 2) CHECK(node.remaining == 0.0f);
            if (time_case == 3) CHECK(node.remaining != node.remaining);
            CHECK(callbacks == ((wanted_flags & 0x10000u) == 0));
            CHECK(node.cycles == 7 && tick_index == 2);
            CHECK(primary.current == NULL);
        }
    }
    reset(); node_init(&node, 0); first = &node; node.update = NULL;
    ticks[0] = 0xFFFFFFFCu; ticks[1] = 3;
    func_002AC820(&node, expected_elapsed);
    CHECK(node.cycles == 7 && callbacks == 0 && tick_index == 2);
    reset(); node_init(&node, 0); first = &node; node.remaining = 1.0f;
    tick_mode = 1;
    func_002AC820(&node, expected_elapsed);
    CHECK(callbacks == 0 && node.flags == 0x10000u && node.cycles == 7);
}

static void recursive_updates(void)
{
    GeorgeTreeNode root, a, b, extra;
    reset(); node_init(&root, 0); node_init(&a, 1); node_init(&b, 2);
    first = parent = &root; child = &a; second = &b;
    primary.current = &root; func_002AD210(&primary, &a); func_002AD210(&primary, &b);
    ticks[0] = 0xFFFFFFFCu; ticks[1] = 3;
    ticks[2] = 10; ticks[3] = 14; ticks[4] = 20; ticks[5] = 26;
    func_002AC820(&root, expected_elapsed);
    CHECK(callbacks == 3 && callback_log[0] == &root && callback_log[1] == &a && callback_log[2] == &b);
    CHECK(a.cycles == 4 && b.cycles == 6 && root.cycles == 17 && tick_index == 6);
    CHECK(primary.current == NULL);

    reset(); node_init(&root, 0); node_init(&a, 0);
    first = parent = &root; child = &a;
    primary.current = &root; func_002AD210(&primary, &a);
    ticks[0] = 16; ticks[1] = 0; ticks[2] = 0; ticks[3] = 32;
    func_002AC820(&root, expected_elapsed);
    CHECK(root.cycles == 16); /* wrapped difference followed by wrapped addition */
    reset(); node_init(&root, 0); node_init(&a, 0);
    first = parent = &root; child = &a; callback_mode = 7;
    primary.current = &root; func_002AD210(&primary, &a);
    func_002AC820(&root, expected_elapsed);
    CHECK(root.cycles == 107); /* parent cycle value reloaded after child callback */

    reset(); node_init(&root, 0); node_init(&a, 0);
    first = &root; child = &a; callback_mode = 1;
    primary.current = &root; func_002AD210(&primary, &a);
    a.elapsed = 9.0f; a.cycles = 999;
    func_002AC820(&root, expected_elapsed);
    CHECK(callbacks == 1 && tick_index == 2 && a.elapsed == 9.0f && a.cycles == 999);
    reset(); node_init(&root, 0); node_init(&a, 0); node_init(&b, 0);
    first = &root; child = &a; third = &b; callback_mode = 2;
    primary.current = &root; func_002AD210(&primary, &a);
    func_002AC820(&root, expected_elapsed);
    CHECK(callbacks == 2 && primary.current == &root && alternate.current == NULL);
    CHECK(D_003FD1E4 == &alternate && root.cycles == 14);
    reset(); node_init(&root, 0); node_init(&a, 0);
    first = &root; callback_mode = 3;
    primary.current = &root; func_002AD210(&primary, &a);
    func_002AC820(&root, expected_elapsed);
    CHECK(callbacks == 1 && (root.flags & 0x40000u) != 0 && tick_index == 2);
    reset(); node_init(&root, 0); node_init(&a, 1); node_init(&extra, 0);
    first = &root; injected = &extra; callback_mode = 4;
    primary.current = &root; func_002AD210(&primary, &a);
    func_002AC820(&root, expected_elapsed);
    CHECK(callbacks == 3 && callback_log[1] == &extra && callback_log[2] == &a);
    CHECK(root.cycles == 21);
}

static void root_walks(void)
{
    GeorgeTreeNode a, b, c, extra;
    reset(); node_init(&a, 1); node_init(&b, 2); node_init(&c, 3); node_init(&extra, 0);
    first = &a; second = &b; third = &c; injected = &extra; callback_mode = 6;
    func_002AD2E8(&primary, &a); func_002AD2E8(&primary, &b); func_002AD2E8(&primary, &c);
    func_002ACBF0(expected_elapsed);
    CHECK(callbacks == 3 && callback_log[0] == &a && callback_log[1] == &b && callback_log[2] == &c);
    CHECK(extra.elapsed == 0.0f && tick_index == 6);

    reset(); node_init(&a, 1); node_init(&b, 2); node_init(&c, 3);
    first = &a; second = &b; third = &c; callback_mode = 5;
    func_002AD2E8(&primary, &a); func_002AD2E8(&primary, &b); func_002AD2E8(&primary, &c);
    func_002ACBF0(expected_elapsed);
    CHECK(callbacks == 1 && b.elapsed == 0.0f && c.elapsed == 0.0f);
    CHECK(b.next == NULL && b.previous == NULL);
    reset(); func_002ACBF0(expected_elapsed);
    CHECK(callbacks == 0 && tick_index == 0);

    reset(); node_init(&a, 0); node_init(&b, 0); first = &a;
    func_002AD2E8(&primary, &a); func_002AD2E8(&secondary, &b);
    D_003FD1E4 = &alternate; func_002ACC50(expected_elapsed);
    CHECK(D_003FD1E4 == &primary && callbacks == 1 && callback_log[0] == &a);
    CHECK(b.elapsed == 0.0f && allocations == 0);
    tick_index = callbacks = 0; first = &b;
    func_002ACD00(expected_elapsed);
    CHECK(D_003FD1E4 == &secondary && callbacks == 1 && callback_log[0] == &b);
}

static void construction_and_contexts(void)
{
    GeorgeTreeNode saved, owner, *node;
    reset(); node_init(&saved, 0); node_init(&owner, 0); parent = &owner;
    primary.current = &saved;
    node = func_002AC9B8(callback, 9);
    CHECK(node == (GeorgeTreeNode *)allocation && primary.current == &saved);
    CHECK(node->attachment == NULL && primary.roots.head == (GeorgeListNode *)node);
    CHECK(saved.children.head == (GeorgeListNode *)&saved.children.tail);
    CHECK(requested_size == 73 && requested_alignment == 6 && node->data == allocation + 64);
    CHECK(node->unknown28 == 0xA5A5A5A5u && node->cycles == 0xA5A5A5A5u && node->unknown3C == 0xA5A5A5A5u);
    reset(); node_init(&saved, 0); primary.current = &saved; fail_node = 1;
    CHECK(func_002AC9B8(callback, 3) == NULL && primary.current == &saved);

    reset(); node_init(&saved, 0); node_init(&owner, 0); parent = &owner;
    primary.current = &saved; allocation_mode = 1;
    node = func_002AC9B8(callback, 0xFFFFFFC0u);
    CHECK(node->attachment == &owner && owner.children.head == (GeorgeListNode *)node);
    CHECK(requested_size == 0 && requested_alignment == 6 && node->data == allocation + 64);
    CHECK(D_003FD1E4 == &alternate && alternate.current == &saved && primary.current == NULL);

    reset(); D_003FD1E4 = D_003FD1E8 = D_003FD1EC = NULL;
    func_002ACB38();
    CHECK(allocations == 2 && D_003FD1E8 == &controllers[0] && D_003FD1EC == &controllers[1]);
    CHECK(D_003FD1E4 == D_003FD1E8 && func_002ACB10() == &controllers[0].roots);
    func_002ACB38(); CHECK(allocations == 2);
    reset(); D_003FD1E4 = D_003FD1E8 = D_003FD1EC = NULL; allocation_mode = 2;
    func_002ACB38();
    CHECK(allocations == 1 && D_003FD1EC == &secondary && D_003FD1E4 == &controllers[0]);
    reset(); D_003FD1E4 = D_003FD1E8 = D_003FD1EC = NULL; fail_controller = 1;
    func_002ACB38();
    CHECK(allocations == 2 && D_003FD1E4 == NULL && D_003FD1E8 == NULL && D_003FD1EC == NULL);

    reset(); free_mode = 1;
    func_002ACBA8();
    CHECK(releases == 2 && release_log[0] == &primary && release_log[1] == &alternate);
    CHECK(D_003FD1E4 == &primary && D_003FD1E8 == NULL && D_003FD1EC == NULL);
}

int main(void)
{
    flags_and_deadlines(); recursive_updates(); root_walks(); construction_and_contexts();
    printf("tree_updates: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
