#include <stdio.h>
#include <string.h>
#include "george/algorithm_duplicates.h"

static unsigned checks, failures, compare_calls;
#define CHECK(expression) do { ++checks; if (!(expression)) { ++failures; \
    printf("failure line %d\n", __LINE__); } } while (0)
typedef void **(*Search)(void **, void **, void *const *, GeorgeStartupCompare);
static Search searches[] = {
    func_00104A70,
    func_00108038,
    func_0010C848,
    func_00121E68,
    func_001333A0,
    func_0013C608,
    func_001429C8,
    func_0014FC10,
    func_001554A8,
    func_00157B80,
    func_0015E638,
    func_00162708,
    func_00196BE8,
    func_001999D0,
    func_001AF930,
    func_001B2E58,
    func_001B6B28,
    func_001BBE18,
    func_001C8AC0,
    func_001D6C48,
    func_001D8630,
    func_001EA340,
    func_001ED9C0,
    func_001F0A90,
    func_001F2080,
    func_001F58E0,
    func_00200530,
    func_00200CF0,
    func_002095D0,
    func_00209D90,
    func_00217708,
    func_00230678,
    func_00231F20,
    func_00235390,
    func_0023A3C8,
    func_0023C538,
    func_00242250,
    func_00243D70,
    func_00244E60,
    func_00246B70,
    func_00247880,
    func_002480D0,
    func_00252390,
    func_00253498,
    func_0025E828,
    func_0026EB78,
    func_00273048,
    func_00277398,
    func_0027DA10,
    func_0027ECA0,
    func_00280A50,
    func_002816A0,
    func_00282078,
    func_002BE460,
    func_002BECD0,
    func_002D0B98,
    func_002D2648,
    func_002D7C28
};
typedef struct SearchValue { s32 value; } SearchValue;
static SearchValue keys[2], values[16];
static void **mutable_entries, **mutable_key;
static unsigned mutation;

static int compare(const void *left, const void *right)
{
    s32 key = ((const SearchValue *)left)->value;
    s32 value = ((const SearchValue *)right)->value;
    ++compare_calls;
    if (mutation && compare_calls == 1) {
        CHECK(key == 2 && value == 4);
        *mutable_key = &keys[1];
    } else if (mutation && compare_calls == 2) {
        CHECK(key == 6 && value == 2);
        mutable_entries[2] = &keys[0];
        keys[0].value = 99;
    } else if (mutation && compare_calls == 3) {
        CHECK(key == 6 && value == 3);
    }
    return key < value;
}

static void search_checks(void)
{
    unsigned fn, pattern, length, begin, end, i;
    s32 key;
    void *entries[16], *key_pointer;
    for (fn = 0; fn < sizeof searches / sizeof searches[0]; ++fn) {
        mutation = 0;
        for (pattern = 0; pattern < 3; ++pattern) {
            for (i = 0; i < 16; ++i) {
                values[i].value = pattern == 0 ? (s32)i : pattern == 1 ? (s32)(i / 3) : 3;
                entries[i] = &values[i];
            }
            for (length = 0; length <= 12; ++length)
                for (begin = 0; begin <= length; ++begin)
                    for (end = begin; end <= length; ++end)
                        for (key = -1; key <= 13; ++key) {
                            unsigned expected = begin;
                            while (expected < end && values[expected].value <= key) ++expected;
                            keys[0].value = key; key_pointer = &keys[0]; compare_calls = 0;
                            CHECK(searches[fn](entries + begin, entries + end, &key_pointer, compare) == entries + expected);
                            CHECK(begin != end || compare_calls == 0);
                        }
        }
        key_pointer = &keys[0]; compare_calls = 0;
        CHECK(searches[fn](entries + 12, entries, &key_pointer, compare) == entries + 12);
        CHECK(compare_calls == 0);
        for (i = 0; i < 8; ++i) { values[i].value = (s32)i; entries[i] = &values[i]; }
        keys[0].value = 2; keys[1].value = 6; key_pointer = &keys[0];
        mutable_key = &key_pointer; mutable_entries = entries; mutation = 1; compare_calls = 0;
        CHECK(searches[fn](entries, entries + 8, &key_pointer, compare) == entries + 4);
        CHECK(compare_calls == 3 && key_pointer == &keys[1] && keys[0].value == 99);
    }
}

static unsigned visits, visitor_mutation;
static u32 observed_keys[8];
static void *observed_values[8];
static GeorgeGenericMapNode nodes[6], *buckets[3], *replacement_buckets[3];
static void visitor(GeorgeGenericMap *map, u32 key, void *value, void *context)
{
    CHECK(context == &visits);
    observed_keys[visits] = key; observed_values[visits++] = value;
    if (visitor_mutation == 1 && visits == 1) {
        /* The active node's captured successor must survive its removal. */
        nodes[0].next = NULL; nodes[0].key = 900; nodes[0].value = NULL;
        buckets[0] = NULL;
    } else if (visitor_mutation == 2 && visits == 1) {
        map->bucket_count = 3; map->buckets = replacement_buckets;
    } else if (visitor_mutation == 3 && visits == 1) {
        map->bucket_count = 0;
    }
}

static void reset_map(GeorgeGenericMap *map)
{
    unsigned i;
    memset(nodes, 0, sizeof nodes); memset(buckets, 0, sizeof buckets);
    memset(replacement_buckets, 0, sizeof replacement_buckets);
    for (i = 0; i < 6; ++i) { nodes[i].key = i * 3; nodes[i].value = &values[i]; }
    map->flags = 0; map->bucket_count = 3; map->count = 6; map->buckets = buckets;
    buckets[0] = &nodes[0]; nodes[0].next = &nodes[1];
    visits = visitor_mutation = 0;
}

static void map_checks(void)
{
    GeorgeGenericMap map;
    unsigned i;
    reset_map(&map); nodes[2].key = 3; nodes[1].next = &nodes[2];
    for (i = 0; i < 3; ++i) CHECK(func_00219FF0(&map, i * 3) == (i == 2 ? NULL : &values[i]));
    CHECK(func_00219FF0(&map, 3) == &values[1]);
    CHECK(func_00219FF0(&map, 1) == NULL);
    reset_map(&map); nodes[0].key = 0xFFFFFFFFu;
    CHECK(func_00219FF0(&map, 0xFFFFFFFFu) == &values[0]);
    map.bucket_count = 0; map.buckets = NULL; visits = 0;
    func_0021A458(&map, visitor, &visits); CHECK(visits == 0);

    reset_map(&map); buckets[2] = &nodes[2];
    func_0021A458(&map, visitor, &visits);
    CHECK(visits == 3 && observed_keys[0] == 0 && observed_keys[1] == 3 && observed_keys[2] == 6);
    CHECK(observed_values[0] == &values[0] && observed_values[1] == &values[1]);
    reset_map(&map); visitor_mutation = 1;
    func_0021A458(&map, visitor, &visits);
    CHECK(visits == 2 && observed_keys[0] == 0 && observed_keys[1] == 3);
    reset_map(&map); map.bucket_count = 1;
    replacement_buckets[1] = &nodes[3]; replacement_buckets[2] = &nodes[4];
    visitor_mutation = 2; func_0021A458(&map, visitor, &visits);
    CHECK(visits == 4 && observed_keys[2] == 9 && observed_keys[3] == 12);
    reset_map(&map); visitor_mutation = 3;
    func_0021A458(&map, visitor, &visits);
    CHECK(visits == 2 && observed_keys[1] == 3);
}

static GeorgeGoalTimedAction goal;
static GeorgeGoalOwner owners[2];
static GeorgeGoalEntity entities[2];
static s32 gate_result;
static unsigned gate_calls, gate_mutation;
s32 func_00174500(GeorgeGoalEntity *entity)
{
    ++gate_calls; CHECK(entity == &entities[0]);
    if (gate_mutation) {
        goal.base.links.unknown00 = (u32)&owners[1];
        goal.field14 = -7.25f; owners[1].field24 = 0xA510;
    }
    return gate_result;
}

static void action_checks(void)
{
    GeorgeGoalTimedAction expected;
    unsigned mode;
    for (mode = 0; mode < 4; ++mode) {
        memset(&goal, 0x5A, sizeof goal); memset(owners, 0x33, sizeof owners);
        owners[0].field08 = &entities[0]; owners[1].field08 = &entities[1];
        goal.base.links.unknown00 = (u32)&owners[0]; goal.field14 = 2.5f;
        expected = goal; gate_calls = 0; gate_mutation = mode >= 2; gate_result = (mode & 1) ? -1 : 0;
        if (gate_mutation) { expected.base.links.unknown00 = (u32)&owners[1]; expected.field14 = -7.25f; }
        if (gate_result) { expected.field10 = expected.field14; expected.field18 = 0; }
        func_001E1028(&goal);
        CHECK(gate_calls == 1 && memcmp(&goal, &expected, sizeof goal) == 0);
        CHECK(owners[0].field24 == (gate_result && !gate_mutation ? 0x333B : 0x3333));
        CHECK(owners[1].field24 == (gate_mutation ? (gate_result ? 0xA518 : 0xA510) : 0x3333));
    }
    {
        union { u32 alignment; u8 bytes[256]; } storage, expected_storage;
        GeorgeGoalOwner *owner = (GeorgeGoalOwner *)(storage.bytes + 0x40);
        GeorgeGoalTimedAction *alias_goal = (GeorgeGoalTimedAction *)(storage.bytes + 0x54);
        u32 bits = 0x80000000u;
        u16 flags;
        memset(&storage, 0x5A, sizeof storage);
        alias_goal->base.links.unknown00 = (u32)owner;
        owner->field08 = &entities[0];
        memcpy(&alias_goal->field14, &bits, 4);
        expected_storage = storage;
        memcpy(expected_storage.bytes + 0x64, &bits, 4);
        memcpy(&flags, expected_storage.bytes + 0x64, 2); flags |= 8;
        memcpy(expected_storage.bytes + 0x64, &flags, 2);
        expected_storage.bytes[0x6C] = 0;
        gate_result = 1; gate_mutation = gate_calls = 0;
        func_001E1028(alias_goal);
        CHECK(gate_calls == 1 && memcmp(&storage, &expected_storage, sizeof storage) == 0);
    }
}

int main(void)
{
    search_checks(); map_checks(); action_checks();
    printf("algorithm_duplicates: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
