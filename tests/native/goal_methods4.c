/* Native 32-bit behavior model; no original executable or assets required. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/goal_methods4.c"

static unsigned checks;
#define CHECK(value) do { ++checks; if (!(value)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #value); exit(1); } } while (0)
#define CLOSE(a, b) CHECK(fabsf((a) - (b)) < 0.00002f)

GeorgeGoalMapOwner *D_003F8C28;
float D_003F8DB8[16];
GeorgeGoalMember D_003F8DF8[5];
GeorgeMathVec3 D_00436BE8;

typedef struct NativeResource {
    GeorgeGoalVirtualObject prefix;
    u8 bytes[0x940];
} NativeResource;
static GeorgeGoalDriveMotion goal, other, third;
static GeorgeGoalOwner owner, replacement_owner;
static GeorgeGoalRoadGeometry road;
static GeorgeGoalIntersectionRecord records[8];
static GeorgeGoalRoadEntry road_entries[8];
static GeorgeMathVec3 vectors[256], position, velocity;
static float basis_words[12];
static NativeResource resource, control;
static u8 resource_table[0x198], control_table[0x198], goal_table[0x18];
static GeorgeDeimosRing ring;
static GeorgeGoalRouteQueueEntry queue[64], pushed[8];
static GeorgeGoalRouteQueueEntry *peek_output, *traffic_context;
static GeorgeGoalMapOwner manager;
static GeorgeGenericMap outer_map, inner_map, replacement_map;
static u32 lookup_keys[32], last_queue_index, route_lookup_word;
static unsigned lookup_calls, push_count, radius_calls, vector_calls;
static unsigned word_calls, scalar_calls, void_calls, map_calls, size_calls;
static u32 word_offsets[8], word_values[8], scalar_offsets[8];
static float scalar_values[8], saved_matrix[16];
static int radius_mode, lookup_mode, size_mode, visitor_mode, context_mode, orientation_mode;
static u8 block[0x180];
static s32 actor_count, predicate_result;
static signed char *dynamic_index;
static const GeorgeMathVec3 *position_pointer;
static void *nested_second;
static u32 flag_words[2];

static const GeorgeMathVec3 *native_position(void *object)
{ (void)object; return position_pointer; }
static const GeorgeMathVec3 *native_velocity(void *object)
{ (void)object; ++vector_calls; return &velocity; }
static const GeorgeMathVec3 *native_basis(void *object)
{ (void)object; return (GeorgeMathVec3 *)basis_words; }
static s32 native_predicate(void *object)
{ (void)object; return predicate_result; }
static float native_radius(void *object)
{
    (void)object;
    ++radius_calls;
    if (radius_mode == 1) {
        if (radius_calls == 1) peek_output->field0C = 20.0f;
        if (radius_calls == 2) traffic_context->field0C = 10.0f;
        if (radius_calls == 3) {
            peek_output->field0C = 5.0f;
            traffic_context->field00 = (u32)&third;
        }
    } else if (radius_mode == 2) goal.field3C = 7.0f;
    return 1.0f;
}
static void native_word30(void *object, u32 value)
{ (void)object; word_offsets[word_calls] = 0x30; word_values[word_calls++] = value; }
static void native_word68(void *object, u32 value)
{ (void)object; word_offsets[word_calls] = 0x68; word_values[word_calls++] = value; }
static void native_scalar20(void *object, float value)
{ (void)object; scalar_offsets[scalar_calls] = 0x20; scalar_values[scalar_calls++] = value; }
static void native_scalar28(void *object, float value)
{ (void)object; scalar_offsets[scalar_calls] = 0x28; scalar_values[scalar_calls++] = value; }
static void native_void(void *object)
{ (void)object; ++void_calls; }
static GeorgeGoalVirtualObject *native_pointer(void *object)
{ (void)object; return &control.prefix; }
static void native_initialize(void *object, u32 value)
{ (void)object; CHECK(value == 0); ++void_calls; }

static void reset(void)
{
    unsigned i;
    memset(&goal, 0, sizeof(goal)); memset(&other, 0, sizeof(other)); memset(&third, 0, sizeof(third));
    memset(&owner, 0, sizeof(owner)); memset(&replacement_owner, 0, sizeof(replacement_owner));
    memset(&road, 0, sizeof(road)); memset(records, 0, sizeof(records));
    memset(road_entries, 0, sizeof(road_entries)); memset(vectors, 0, sizeof(vectors));
    memset(queue, 0, sizeof(queue)); memset(pushed, 0, sizeof(pushed)); memset(&ring, 0, sizeof(ring));
    memset(&resource, 0, sizeof(resource)); memset(&control, 0, sizeof(control));
    memset(resource_table, 0, sizeof(resource_table)); memset(control_table, 0, sizeof(control_table));
    memset(goal_table, 0, sizeof(goal_table)); memset(block, 0, sizeof(block));
    memset(&manager, 0, sizeof(manager)); memset(&outer_map, 0, sizeof(outer_map));
    memset(&inner_map, 0, sizeof(inner_map)); memset(&replacement_map, 0, sizeof(replacement_map));
    memset(basis_words, 0, sizeof(basis_words)); memset(D_003F8DF8, 0, sizeof(D_003F8DF8));
    position.x = position.y = position.z = velocity.x = velocity.y = velocity.z = 0.0f;
    D_00436BE8.x = D_00436BE8.z = 0.0f; D_00436BE8.y = 1.0f;
    basis_words[0] = 1.0f; basis_words[10] = 1.0f;
    resource.prefix.field04 = resource_table; control.prefix.field04 = control_table;
    FIELD(GeorgeGoalVirtualVector, resource_table, 0x28).invoke = native_position;
    FIELD(GeorgeGoalVirtualVector, resource_table, 0x38).invoke = native_velocity;
    FIELD(GeorgeGoalVirtualVector, resource_table, 0x98).invoke = native_basis;
    FIELD(GeorgeGoalVirtualPointer, resource_table, 0x80).invoke = native_pointer;
    FIELD(GeorgeGoalVirtualInt, resource_table, 0x88).invoke = native_predicate;
    FIELD(GeorgeGoalVirtualInt, resource_table, 0x138).invoke = native_predicate;
    FIELD(GeorgeGoalVirtualVoid, resource_table, 0x90).invoke = native_void;
    FIELD(GeorgeGoalVirtualFloatResult, resource_table, 0x190).invoke = native_radius;
    FIELD(GeorgeGoalVirtualWord, control_table, 0x30).invoke = native_word30;
    FIELD(GeorgeGoalVirtualWord, control_table, 0x68).invoke = native_word68;
    FIELD(GeorgeGoalVirtualFloat, control_table, 0x20).invoke = native_scalar20;
    FIELD(GeorgeGoalVirtualFloat, control_table, 0x28).invoke = native_scalar28;
    FIELD(GeorgeGoalVirtualWord, goal_table, 0x10).invoke = native_initialize;
    goal.base.links.unknown00 = (u32)&owner; goal.base.field0C = goal_table;
    goal.field10 = &control.prefix; goal.field14 = &resource.prefix; goal.field40 = &ring;
    other.field14 = &resource.prefix; other.field40 = &ring;
    third.field38 = 10.0f; third.field48 = 0xFF;
    owner.field60 = 0.1f; replacement_owner.field60 = 0.25f;
    ring.count = 1; ring.read = (u8 *)queue; ring.write = (u8 *)(queue + 1); ring.element_size = 0x18;
    road.field38 = records; road.field3C = road_entries; road.field40 = block; road.field48 = vectors;
    for (i = 0; i < 8; ++i) { records[i].field05 = 4; records[i].field18 = records[i].field1C = 0xFFFFFFFFU; }
    manager.field300 = &outer_map; D_003F8C28 = &manager;
    lookup_calls = push_count = radius_calls = vector_calls = word_calls = scalar_calls = void_calls = map_calls = size_calls = 0;
    radius_mode = lookup_mode = size_mode = visitor_mode = context_mode = orientation_mode = 0;
    actor_count = 3; predicate_result = 0; position_pointer = &position;
    nested_second = flag_words;
    FIELD(void *, block, 0xC0) = &nested_second;
    flag_words[0] = 0; flag_words[1] = 0x13579BDFU;
    FIELD(void *, block, 0xC4) = block + 0x120; FIELD(void *, block, 0xDC) = block + 0x140;
}

GeorgeGoalRoad *func_001CCA28(u32 word)
{
    route_lookup_word = word;
    if (lookup_mode == 2) return NULL;
    if (lookup_mode == 1) { owner.field14 = 1; lookup_mode = 0; }
    return (GeorgeGoalRoad *)&road;
}
void *func_002A7C08(GeorgeGenericMap *map, u32 key)
{
    lookup_keys[lookup_calls++] = key;
    if (map == &outer_map) return lookup_mode == 3 ? NULL : &inner_map;
    return lookup_mode == 4 ? &goal : NULL;
}
GeorgeGenericMap *func_002A7F58(u32 buckets)
{ CHECK(buckets == 10); return &inner_map; }
void func_002A7CD0(GeorgeGenericMap *map, u32 key, void *value)
{
    ++map_calls;
    if (map_calls == 1) { CHECK(map == &inner_map); CHECK(key == (u32)value); manager.field300 = &replacement_map; }
    else { CHECK(map == &replacement_map); CHECK(value == &inner_map); CHECK(key == 99); }
}
void func_002A7DC0(GeorgeGenericMap *map, u32 key)
{ ++map_calls; if (map_calls == 1) CHECK(map == &inner_map); else { CHECK(map == &replacement_map); CHECK(key == 99); } }
void func_002A8070(GeorgeGenericMap *map)
{ CHECK(map == &inner_map); manager.field300 = &replacement_map; }
u32 func_002A8128(const void *object)
{ return ((const GeorgeGenericMap *)object)->count; }
u32 func_00297388(const void *object)
{
    (void)object; ++size_calls;
    if (size_mode == 1) { *dynamic_index = 1; block[5] = 2; }
    else if (size_mode == 2) block[5] = 1;
    return 16;
}
void *func_002AF208(const GeorgeDeimosRing *input, u32 index)
{ last_queue_index = index; CHECK(index < 64); return input->read + index * input->element_size; }
u32 func_002AF358(const GeorgeDeimosRing *input, void *output)
{ peek_output = output; memcpy(output, input->read, 0x18); return 1; }
u32 func_002AF398(const GeorgeDeimosRing *input, void *output)
{ CHECK(input->count != 0); memcpy(output, input->read + (input->count - 1) * 0x18, 0x18); return 1; }
u32 func_002AF3F0(GeorgeDeimosRing *input, void *output)
{ memcpy(output, input->read, 0x18); input->read += 0x18; --input->count; return 1; }
u32 func_002AF470(GeorgeDeimosRing *input, const void *value)
{
    CHECK(push_count < 8); pushed[push_count++] = *(const GeorgeGoalRouteQueueEntry *)value;
    memcpy(input->read + input->count * 0x18, value, 0x18); ++input->count;
    return 1;
}
void func_002AF560(GeorgeDeimosRing *input)
{ input->count = 0; input->read = input->write = input->begin; }
float func_002A3538(GeorgeMathVec3 *vector)
{
    float length = sqrtf(dot(vector, vector));
    if (0.0000001f < length) { vector->x /= length; vector->y /= length; vector->z /= length; }
    return length;
}
float func_002A35C0(GeorgeMathVec3 *output, const GeorgeMathVec3 *input, float scale)
{
    GeorgeMathVec3 value = *input; float length = func_002A3538(&value);
    output->x = value.x * scale; output->z = value.z * scale; output->y = value.y * scale;
    return length;
}
s32 func_0016ECE0(void *object)
{ CHECK(object == &control.prefix); return actor_count; }
s32 func_001CF588(void *context, u32 word)
{
    CHECK(context == (void *)position_pointer); CHECK(word == owner.field18.bits);
    if (context_mode == 1) { goal.base.links.unknown00 = (u32)&replacement_owner; return 0; }
    return 1;
}
s32 func_001D1760(void *context, u32 word, u32 index)
{ (void)context; CHECK(word == owner.field14); CHECK(index == OWNER_BYTE(&owner, 0x20)); return 1; }
GeorgeGoalRouteResult *func_001CF340(void *context)
{ (void)context; return NULL; }
GeorgeGoalRouteWord *func_001CE130(void *context, u8 *index)
{ (void)context; (void)index; return NULL; }
float func_001D0EE0(u32 word, u32 index, s32 mode, s32 reverse, const GeorgeMathVec3 *value)
{ CHECK(word == 1); CHECK(index == 0); CHECK(mode == 1); CHECK(reverse == 0); CHECK(value == &position); return 7.0f; }
s32 func_001D11A8(GeorgeGoalIntersectionRecord *first, GeorgeGoalIntersectionRecord *second, s32 mode, s32 reverse)
{ CHECK(first == records); CHECK(second == records + 1); CHECK(mode == -1); CHECK(reverse == 0); return 1; }
float func_001D0010(GeorgeGoalRoadGeometry *input, u32 word, u32 previous, s32 mode,
                    u8 *output_mode, signed char *output_index, u32 *output_word, u8 *output_reverse)
{
    CHECK(input == &road); CHECK(word == 0x80000000U); CHECK(previous == 0); CHECK(mode == -1);
    *output_mode = 2; *output_index = -1; *output_word = 1; *output_reverse = 1; return 3.0f;
}
void func_002A8130(GeorgeGenericMap *map, GeorgeGenericMapVisitor callback, void *context)
{
    (void)map; CHECK(callback == func_001DF420);
    if (visitor_mode == 1) goal.field54 = -5.0f;
    CHECK(context == queue);
}
void func_00297178(void *input, u16 *index, float *value, GeorgeMathVec3 *output)
{ (void)input; (void)index; (void)value; (void)output; }
void func_002A1C60(const void *input, const GeorgeMathVec3 *source, GeorgeMathVec3 *output)
{ (void)input; CHECK(source == output); }
void func_002A1C30(float *matrix)
{ unsigned i; for (i = 0; i < 16; ++i) matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f; }
void func_002A0B00(GeorgeMathVec4 *output, const float *matrix)
{ memcpy(saved_matrix, matrix, sizeof(saved_matrix)); output->x = output->y = output->z = 0; output->w = 1; }
float func_0029C230(float value)
{ return acosf(value); }
void func_001FF6D0(void *object, const GeorgeMathVec4 *value)
{
    (void)object; CHECK(value->w == 1);
    if (orientation_mode == 1) { goal.field3C = 2.0f; goal.base.links.unknown00 = (u32)&replacement_owner; }
}
static GeorgeMathVec3 submitted_motion;
void func_001FF6F8(void *object, const GeorgeMathVec3 *value)
{ CHECK(object == &resource.prefix); submitted_motion = *value; }
s32 func_001D18A8(GeorgeMathVec3 *output, u32 word, u32 index, s32 reverse)
{ (void)word; (void)index; (void)reverse; output->x = 0; output->y = 0; output->z = 1; return 1; }

static void test_maps(void)
{
    reset(); lookup_mode = 3; func_001CCAA8(123, 99);
    CHECK(lookup_keys[0] == 99); CHECK(map_calls == 2);
    reset(); lookup_mode = 4; func_001CCAA8(123, 99);
    CHECK(lookup_calls == 2); CHECK(lookup_keys[1] == 123); CHECK(map_calls == 0);
    reset(); func_001CCB58(123, 99);
    CHECK(lookup_calls == 2); CHECK(lookup_keys[1] == 123); CHECK(map_calls == 2);
    reset(); CHECK(func_001CCBE8(99) == &inner_map); CHECK(lookup_keys[0] == 99);
}
static void test_packed_walk_and_cross(void)
{
    unsigned byte;
    reset();
    for (byte = 0; byte < 256; ++byte) {
        u8 input = byte;
        u32 high = (u8)((s32)(signed char)input >> 4);
        u32 shift = (high * 5U + (input & 15U)) & 31U;
        CHECK(func_001D1A30(&road, 0, &input) == ((flag_words[1] & (1U << shift)) != 0));
    }
    FIELD(u32, road_entries, 0x5C) = 1; CHECK(func_001D1A30(&road, 0, NULL) == 1);
    {
        signed char index = -1; u32 output = 0xDEADBEEF;
        block[5] = 2; dynamic_index = &index; size_mode = 1;
        CHECK(func_001D1AA0(&road, 0, &output, &index) == 1);
        CHECK(output == (u32)(block + 0x140)); CHECK(size_calls == 1);
        index = -1; block[5] = 2; size_mode = 2; output = 0xDEADBEEF;
        CHECK(func_001D1AA0(&road, 0, &output, &index) == 0); CHECK(output == 0xDEADBEEF);
    }
    reset(); records[0].field10 = 1; FIELD(u16, records, 0x12) = 0;
    vectors[0].x = 1; vectors[0].y = 2; vectors[0].z = 3;
    vectors[1].x = 4; vectors[1].y = 5; vectors[1].z = 6;
    CHECK(func_001CFB20(vectors, 0, 256) == 1);
    CLOSE(vectors[0].x, 3); CLOSE(vectors[0].y, -6); CLOSE(vectors[0].z, 3);
    lookup_mode = 2; CHECK(func_001CFB20(vectors, 0, 0) == 0); CLOSE(vectors[0].y, -6);
}
static void test_speed_and_traffic(void)
{
    reset(); goal.field38 = 10; goal.field3C = 9.9f;
    func_001DDA70((GeorgeGoalDrive *)&goal); CLOSE(goal.field3C, 10);
    CLOSE(goal.field54, 26); CHECK(FIELD(u32, &resource, 0x12C) == 0);
    reset(); goal.field38 = 0; goal.field3C = 10; goal.field48 = 2;
    func_001DDA70((GeorgeGoalDrive *)&goal); CLOSE(goal.field3C, 8);
    CHECK(FIELD(u32, &resource, 0x12C) == 1);
    reset(); goal.field38 = goal.field3C = 3; radius_mode = 2;
    func_001DDA70((GeorgeGoalDrive *)&goal); CLOSE(goal.field54, 15.8f);
    reset(); goal.field38 = NAN; goal.field3C = 3;
    func_001DDA70((GeorgeGoalDrive *)&goal); CLOSE(goal.field3C, 3);
    reset(); queue[0].field08 = -1; queue[0].field0C = 5;
    {
        GeorgeGoalRouteQueueEntry entry;
        memset(&entry, 0, sizeof(entry)); entry.field00 = (u32)&goal;
        entry.field08 = -1; entry.field0C = 30; entry.field10 = 10;
        traffic_context = &entry; radius_mode = 1;
        func_001DF420(&outer_map, 0, &other, &entry);
        CHECK(radius_calls == 3); CLOSE(third.field38, 0); CLOSE(third.field58, 0.5f);
        CHECK(third.field48 == 0xFD); CLOSE(goal.field58, 0);
        radius_calls = 0; entry.field00 = (u32)&other;
        func_001DF420(&outer_map, 0, &other, &entry); CHECK(radius_calls == 0);
        entry.field00 = (u32)&goal; entry.field08 = 8;
        func_001DF420(&outer_map, 0, &other, &entry); CHECK(radius_calls == 0);
    }
}
static void test_steering_and_stop(void)
{
    reset(); goal.field80.z = 10; goal.field38 = 1; velocity.z = 2;
    func_001DD580((GeorgeGoalDrive *)&goal);
    CHECK(word_calls == 2); CHECK(word_offsets[0] == 0x68 && word_values[0] == 1);
    CHECK(word_offsets[1] == 0x30 && word_values[1] == 0);
    CLOSE(scalar_values[0], 1); CLOSE(scalar_values[1], -1);
    reset(); goal.field80.z = 10; goal.field38 = 1; velocity.z = 3.3f;
    func_001DD580((GeorgeGoalDrive *)&goal); CHECK(word_values[0] == 1); CLOSE(scalar_values[0], -1);
    reset(); goal.field80.z = 1; goal.field38 = 1; velocity.z = 3;
    owner.field24 = 4; goal.field49 = 0;
    func_001DD580((GeorgeGoalDrive *)&goal);
    CHECK(goal.field44 == 3 && goal.field10 == NULL && goal.field49 == 255);
    CHECK((owner.field24 & 4) == 0 && void_calls == 1);
    reset(); goal.field80.z = 1; goal.field5C = -1; owner.field18.bits = 77;
    func_001DD1E0((GeorgeGoalDrive *)&goal);
    CHECK(vector_calls == 2); CHECK(goal.field44 == 3 && goal.field10 == NULL);
    reset(); goal.field80.z = 1; goal.field5C = -1; owner.field18.bits = 77;
    owner.field14 = 19; OWNER_BYTE(&owner, 0x20) = 6; context_mode = 1;
    func_001DD1E0((GeorgeGoalDrive *)&goal); CHECK(goal.base.links.unknown00 == (u32)&replacement_owner);
    CHECK(goal.field44 == 3); CHECK(void_calls == 1);
    reset(); goal.field5C = NAN; func_001DD1E0((GeorgeGoalDrive *)&goal);
    CHECK(goal.field10 != NULL && void_calls == 0);
}
static void test_expansion_initialization_and_motion(void)
{
    reset(); queue[0].field08 = -1; queue[0].field0A = 0; records[0].field1C = 1;
    FIELD(float, records + 1, 0x28) = 10; FIELD(float, records + 1, 0x2C) = 14;
    CLOSE(func_001DEC18((GeorgeGoalDrive *)&goal), 12);
    CHECK(push_count == 1 && pushed[0].field08 == 1 && pushed[0].field09 == -1);
    CLOSE(pushed[0].field14, 12);
    reset(); queue[0].field08 = -1; records[0].field1C = 0x80000000U;
    FIELD(u16, records + 1, 0x16) = 2U << 3;
    FIELD(float, records + 1, 0x24) = 6; FIELD(float, records + 1, 0x28) = 10;
    CLOSE(func_001DEC18((GeorgeGoalDrive *)&goal), 11);
    CHECK(push_count == 2 && pushed[0].field0B == 0 && pushed[1].field0B == 1);
    CHECK(pushed[1].field0A == 1 && pushed[1].field08 == -1);
    reset(); queue[0].field08 = -1; records[0].field1C = 0xFFFFFFFFU;
    CLOSE(func_001DEC18((GeorgeGoalDrive *)&goal), 100000000.0f); CHECK(push_count == 0 && goal.field45 == 0);
    reset(); goal.field54 = 2; goal.field50 = 1; queue[0].field0B = 1;
    queue[0].field0C = 3; queue[0].field14 = 10; visitor_mode = 1;
    func_001DE6E0((GeorgeGoalDrive *)&goal); CLOSE(queue[0].field0C, 4); CLOSE(queue[0].field10, 2);
    CLOSE(goal.field54, -5); CHECK(push_count == 0);
    reset(); ring.count = 0; owner.field14 = 0; OWNER_BYTE(&owner, 0x22) = 1; lookup_mode = 1;
    FIELD(u16, records, 6) = 0; FIELD(u16, records, 0x12) = 20;
    FIELD(u16, records + 1, 6) = 30; FIELD(u16, records + 1, 0x12) = 50;
    FIELD(float, records + 1, 0x28) = 10; FIELD(float, records + 1, 0x2C) = 14;
    vectors[1].x = 100; vectors[21].x = 1; vectors[31].x = 200; vectors[51].x = 1;
    func_001DE928((GeorgeGoalDrive *)&goal);
    CHECK(push_count == 1 && pushed[0].field04 == 1); CLOSE(pushed[0].field0C, 7);
    CLOSE(pushed[0].field10, 30); CLOSE(pushed[0].field14, 12);
    CLOSE(goal.field80.x, 102); CLOSE(goal.field24.x, 202);
    reset(); goal.field18.z = 10; goal.field3C = 3; goal.field38 = 0; goal.field48 = 1; orientation_mode = 1;
    func_001DE228((GeorgeGoalDrive *)&goal);
    CLOSE(submitted_motion.z, 2); CLOSE(goal.field50, 0.5f);
    CLOSE(saved_matrix[0], 1); CLOSE(saved_matrix[5], 1); CLOSE(saved_matrix[10], 1); CLOSE(saved_matrix[15], 1);
    reset(); goal.field18.z = 0; goal.field3C = 0;
    func_001DE228((GeorgeGoalDrive *)&goal); CLOSE(submitted_motion.z, 0); CLOSE(goal.field50, 0);
}

int main(void)
{
    test_maps(); test_packed_walk_and_cross(); test_speed_and_traffic();
    test_steering_and_stop(); test_expansion_initialization_and_motion();
    printf("goal_methods4: %u checks passed\n", checks);
    return 0;
}
