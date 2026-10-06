/* Native 32-bit callback and boundary tests; no original code or assets. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/goal_methods5.c"

static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
#define CLOSE(a,b) CHECK(fabsf((a) - (b)) < 0.00002f)

u8 D_004372F0[8], D_004373F0[8], D_00437478[8], D_00437518[8];
GeorgeGoalMember D_003F8EB0[2];
GeorgeMathVec3 D_004373E0;
static GeorgeGoalAngleTarget angle_goal;
static GeorgeGoalEndpoint endpoint;
static GeorgeGoalRouteAction route;
static GeorgeGoalReferenceCandidates selection;
static GeorgeGoalOwner owner, replacement;
static GeorgeGoalEntity entity, replacement_entity;
static GeorgeGoalReferencedObject reference, second_reference;
static GeorgeMathVec3 target;
static GeorgeGoalRoadGeometry geometry;
static GeorgeGoalIntersectionRecord records[8];
static GeorgeGoalRoadEntry road_records[8];
static GeorgeMathVec3 vectors[256];
static int base_mode, release_mode, planar_mode, ray_mode, normalize_mode;
static int lookup_mode, classify_mode, route_call_mode, selection_mode;
static unsigned base_calls, release_calls, lookup_calls, random_calls;
static unsigned route_calls, score_calls, hit_calls, member_calls, motion_calls;
static u32 last_destroy_flags, last_lookup, captured_index;
static GeorgeGoalReferencedObject *released;
static float planar_fraction;
static s32 ray_result, random_value;
static GeorgeGoalRay captured_ray;
static GeorgeGoalEndpoint *active_endpoint;
static GeorgeGoalRouteAction *active_route;
static void *expected_member;
static u8 member_storage[0x10080];
static GeorgeMathVec4 plane;
static u8 original_record[0x20], candidate_record[0x20], collision_resource[0x30];
static u16 hit_indices[1];
static u8 collision_items[0x1C];

static void reset(void)
{
    memset(&angle_goal, 0, sizeof(angle_goal)); memset(&endpoint, 0, sizeof(endpoint));
    memset(&route, 0, sizeof(route)); memset(&selection, 0, sizeof(selection));
    memset(&owner, 0, sizeof(owner)); memset(&replacement, 0, sizeof(replacement));
    memset(&entity, 0, sizeof(entity)); memset(&replacement_entity, 0, sizeof(replacement_entity));
    memset(&reference, 0, sizeof(reference)); memset(&second_reference, 0, sizeof(second_reference));
    memset(&geometry, 0, sizeof(geometry)); memset(records, 0, sizeof(records));
    memset(road_records, 0, sizeof(road_records)); memset(vectors, 0, sizeof(vectors));
    memset(D_003F8EB0, 0, sizeof(D_003F8EB0));
    memset(original_record, 0, sizeof(original_record)); memset(candidate_record, 0, sizeof(candidate_record));
    memset(collision_resource, 0, sizeof(collision_resource));
    owner.field08 = &entity; replacement.field08 = &replacement_entity;
    owner.field60 = 0.1f; replacement.field60 = 0.5f;
    angle_goal.base.links.unknown00 = endpoint.base.links.unknown00 =
      route.base.links.unknown00 = selection.base.links.unknown00 = (u32)&owner;
    geometry.field38 = records; geometry.field3C = road_records; geometry.field48 = vectors;
    target.x = 4; target.y = 8; target.z = 10;
    reference.field06 = 0x40;
    D_004373E0.x = D_004373E0.z = 0; D_004373E0.y = 1;
    active_endpoint = &endpoint; active_route = &route;
    expected_member = NULL;
    planar_fraction = 0.5f; ray_result = 1; random_value = 0;
    base_mode = release_mode = planar_mode = ray_mode = normalize_mode = 0;
    lookup_mode = classify_mode = route_call_mode = selection_mode = 0;
    base_calls = release_calls = lookup_calls = random_calls = route_calls =
      score_calls = hit_calls = member_calls = motion_calls = 0;
    plane.x = plane.y = plane.z = plane.w = 0;
    FIELD(const GeorgeMathVec4 *, candidate_record, 0x18) = &plane;
    FIELD(const GeorgeMathVec4 *, original_record, 0x18) = &plane;
    FIELD(const u16 *, candidate_record, 0x1C) = hit_indices;
    FIELD(const u16 *, original_record, 0x1C) = hit_indices;
    FIELD(const u8 *, collision_resource, 0x2C) = collision_items;
}

GeorgeGameplayGoal *func_0020D2A0(void *storage, void *input_owner)
{
    GeorgeGoalBase *base = storage;
    ++base_calls;
    base->links.unknown00 = base_mode ? (u32)&replacement : (u32)input_owner;
    base->links.field04 = base->links.field08 = 0;
    return &base->links;
}
void func_0020D2C0(GeorgeGoalBase *base, u32 flags)
{ CHECK(base != NULL); last_destroy_flags = flags; }
void func_001CAF88(GeorgeGoalReferencedObject *object)
{
    ++release_calls; released = object;
    if (release_mode) active_endpoint->field14 = &second_reference;
}
const GeorgeMathVec3 *func_001CAFE0(GeorgeGoalReferencedObject *object)
{ CHECK(object == &reference || object == &second_reference); return &target; }
void func_001DC538(GeorgeGoalTimer *timer, float value)
{
    memset(timer, 0, 0x18); timer->field18 = 1.0e9f;
    timer->field1C = value; timer->field20 = timer->field24 = 1;
}
float func_001CDA18(u32 word, const GeorgeMathVec3 *start, const GeorgeMathVec3 *end)
{
    CHECK(word == FIELD(u32, &owner, 0xC)); CHECK(start == POSITION(&entity));
    if (planar_mode) {
        POSITION(&entity)->x = 10; POSITION(&entity)->y = 20; POSITION(&entity)->z = 30;
        active_endpoint->base.links.unknown00 = (u32)&replacement;
    }
    CHECK(end != NULL); return planar_fraction;
}
float func_002A3538(GeorgeMathVec3 *vector)
{
    float length = sqrtf(dot(vector, vector));
    if (0.0000001f < length) { vector->x /= length; vector->y /= length; vector->z /= length; }
    if (normalize_mode) {
        target.x = 20; target.y = 30; target.z = 40;
        active_endpoint->base.links.unknown00 = (u32)&replacement;
    }
    return length;
}
s32 func_001CD728(const GeorgeGoalRay *ray, u32 *word, float *fraction)
{
    captured_ray = *ray; CHECK(*word == 0xFFFFFFFFU);
    *fraction = 0.5f;
    if (ray_mode) { target.x = 100; target.y = 200; target.z = 300; active_endpoint->field14 = &second_reference; }
    return ray_result;
}
GeorgeGoalRoad *func_001CCA28(u32 word)
{
    ++lookup_calls; last_lookup = word;
    if (lookup_mode == 1) return NULL;
    if (lookup_mode == 2 && lookup_calls == 2) {
        records[0].field1C = 0x80000001U;
        BYTE(&owner, 0x21) = 1;
    }
    if (lookup_mode == 3 && lookup_calls == 2) road_records[0].field34 = 0;
    return (GeorgeGoalRoad *)&geometry;
}
GeorgeGoalIntersectionRecord *func_001D1608(u32 word)
{ CHECK(word == owner.field14); return records + (word & 0xFFFFU); }
s32 func_00397178(void)
{ ++random_calls; if (route_call_mode == 3) route.base.links.unknown00 = (u32)&replacement; return random_value; }
s32 func_001DC468(void *context, u32 *word18, u32 *word14, u32 *word20)
{
    CHECK(context == POSITION(&entity)); CHECK(word18 == &owner.field18.bits);
    CHECK(word14 == &owner.field14); CHECK(word20 == &owner.field20);
    return classify_mode;
}
s32 func_001D18A8(GeorgeMathVec3 *output, u32 word, u32 index, s32 reverse)
{
    (void)word; (void)reverse; ++route_calls; captured_index = index;
    output->x = 0; output->y = 0; output->z = 1;
    if (route_call_mode == 1) {
        active_route->base.links.unknown00 = (u32)&replacement;
        BYTE(&replacement, 0x20) = 9; replacement.field14 = 1;
    }
    return 1;
}
s32 func_001D1820(GeorgeMathVec3 *output, u32 word, u32 index, s32 mode, float value)
{
    (void)mode; (void)value;
    ++route_calls;
    if (route_call_mode == 1) { CHECK(word == 1); CHECK(index == 3); }
    output->x = output->y = 0; output->z = 1;
    return route_call_mode == 2 ? 0 : 1;
}
float func_001DC178(const GeorgeMathVec3 *start, const GeorgeMathVec3 *end)
{
    (void)start; (void)end;
    if (route_call_mode == 4) active_route->base.links.unknown00 = (u32)&replacement;
    return 0.25f;
}
void func_001DB968(GeorgeGoalOwner *input_owner, GeorgeGoalMotion *motion)
{
    CHECK(input_owner == &owner); CHECK(motion == &route.field34); ++motion_calls;
    if (route_call_mode == 4) active_route->base.links.unknown00 = (u32)&replacement;
}
s32 func_001CD2A8(const GeorgeMathVec3 *position, void **record, void **resource, u32 unused)
{
    CHECK(unused == 1); ++score_calls; *resource = collision_resource;
    if (score_calls == 1) { CHECK(position == &target); *record = original_record; return 77; }
    *record = selection_mode == 1 ? NULL : candidate_record;
    return selection_mode == 2 ? (s32)FIELD(u32, &owner, 0xC) : 99;
}
void func_001CDEB0(GeorgeMathVec3 *position, const GeorgeMathVec4 *input_plane)
{ CHECK(input_plane == &plane); (void)position; }
s32 func_001CDEF8(const void *item, const GeorgeMathVec3 *start,
                   const GeorgeMathVec3 *end, const void *resource)
{
    CHECK(item == collision_items && resource == collision_resource);
    CHECK(start == &target || end == &target); ++hit_calls;
    return hit_calls <= 2;
}
static void native_member(void *adjusted)
{ ++member_calls; CHECK(adjusted == (expected_member ? expected_member : (void *)active_route)); if (route_call_mode == 4) active_route->base.links.unknown00 = (u32)&replacement; }

static void test_constructors(void)
{
    reset(); base_mode = 1;
    CHECK(func_001E56E8(&angle_goal, &owner, 4.0f, 2.0f, 3.0f) == &angle_goal.base.links);
    CLOSE(angle_goal.field14.angle, 4.0f - 6.2831854820251465f);
    CHECK((replacement.field24 & 0x200) != 0 && owner.field24 == 0);
    CHECK(angle_goal.field10 == 0 && angle_goal.field28 == 1);
    reset(); func_001E56E8(&angle_goal, &owner, -4.0f, 2, 3);
    CLOSE(angle_goal.field14.angle, -4 + 6.2831854820251465f);
    reset(); func_001E56E8(&angle_goal, &owner, NAN, 2, 3); CHECK(isnan(angle_goal.field14.angle));
    reset(); reference.field05 = 255;
    func_001E57C8(&angle_goal, &owner, &reference, 2, 3);
    CHECK(reference.field05 == 0 && angle_goal.field10 == 1 && angle_goal.field28 == 0);
    func_001E58F8(&angle_goal, 3); CHECK(released == &reference && angle_goal.field14.reference == NULL);
    CHECK(last_destroy_flags == 3 && (owner.field24 & 0x200) == 0);
    reset(); FIELD(float, &angle_goal, 0x10) = 1; angle_goal.field14.position.x = 2; angle_goal.field14.position.y = 3;
    func_001E5858(&angle_goal, &owner, ADDRESS(GeorgeMathVec3, &angle_goal, 0x10), 4, 5);
    CLOSE(angle_goal.field14.position.x, 1); CLOSE(angle_goal.field14.position.y, 1); CLOSE(angle_goal.field14.position.z, 1);
    reset(); reference.field05 = 255; func_001E6688(&endpoint, &owner, &reference, 0x18001);
    CHECK(reference.field05 == 0 && endpoint.field12 == (s16)0x8001);
    CHECK(endpoint.field10 == 0 && endpoint.field14 == &reference); CLOSE(endpoint.field18.z, 0);
    release_mode = 1; func_001E6778(&endpoint, 9); CHECK(endpoint.field14 == NULL && last_destroy_flags == 9);
    reset(); endpoint.field18.x = 1; endpoint.field18.y = 2; endpoint.field18.z = 3;
    func_001E6700(&endpoint, &owner, ADDRESS(GeorgeMathVec3, &endpoint, 0x14), 0x10002);
    CHECK(endpoint.field14 == NULL && endpoint.field12 == 2 && endpoint.field10 == 1);
    CLOSE(endpoint.field18.x, 0); CLOSE(endpoint.field18.y, 0); CLOSE(endpoint.field18.z, 0);
    reset(); route.field10 = 99; route.field11 = 88;
    func_001E7520(&route, &owner, &reference, 2, 3);
    CHECK(route.field10 == 99 && route.field11 == 88); CLOSE(route.field14, 3);
    CLOSE(route.field34.field20, 0.75f); CLOSE(route.field68.x, 4);
    reset(); func_001E7520(&route, &owner, NULL, 2, 3); CHECK(route.field11 == 2);
    reset(); func_001E6318(&selection, &owner, &reference, 10); CLOSE(selection.field60, 10);
    func_001E6378(&selection, 5); CHECK(released == &reference && selection.field64 == NULL && last_destroy_flags == 5);
}

static void test_endpoint_and_output(void)
{
    reset(); endpoint.field10 = 0; endpoint.field12 = 1; endpoint.field14 = &reference;
    POSITION(&entity)->y = POSITION(&entity)->z = 2; planar_mode = 1;
    CHECK(func_001E6400(&endpoint) == 1);
    CLOSE(endpoint.field24.x, 12); CLOSE(endpoint.field24.y, 20); CLOSE(endpoint.field24.z, 34);
    CHECK(endpoint.base.links.unknown00 == (u32)&replacement);
    reset(); endpoint.field10 = 0; endpoint.field12 = 1; endpoint.field14 = &reference;
    endpoint.field24.x = 123; planar_fraction = NAN;
    CHECK(func_001E6400(&endpoint) == 1); CLOSE(endpoint.field24.x, 123);
    reset(); endpoint.field10 = 0; endpoint.field14 = &reference;
    normalize_mode = 1; ray_mode = 1; ray_result = 2;
    CHECK(func_001E6400(&endpoint) == 1);
    CLOSE(captured_ray.field00.x, 20); CLOSE(captured_ray.field00.y, 30);
    CLOSE(captured_ray.field18, sqrtf(180));
    CLOSE(endpoint.field24.x, 98); CLOSE(endpoint.field24.y, 196); CLOSE(endpoint.field24.z, 295);
    CHECK(released == &second_reference && endpoint.field14 == NULL);
    reset(); endpoint.field10 = 0; endpoint.field14 = &reference; reference.field06 = 0;
    CHECK(func_001E6400(&endpoint) == 2); CHECK(release_calls == 0);
    reset(); endpoint.field10 = 1; endpoint.field18.x = 1; endpoint.field18.y = 2; endpoint.field18.z = 3;
    ray_result = 0; CHECK(func_001E6400(&endpoint) == 2);
    reset(); endpoint.field24.x = 1; endpoint.field24.y = 2; endpoint.field24.z = 3;
    func_001E67D0(&endpoint, ADDRESS(GeorgeGoalOutput, &endpoint, 0x20));
    CHECK(FIELD(u32, &endpoint, 0x20) == 1); CLOSE(endpoint.field24.x, 1);
}

static void test_plane_alias_and_routes(void)
{
    reset(); records[0].field10 = 1; FIELD(u16, records, 0x12) = 0; FIELD(u16, records, 6) = 0;
    vectors[0].x = 1; vectors[0].y = 2; vectors[0].z = 3;
    vectors[1].x = 4; vectors[1].y = 5; vectors[1].z = 6;
    CHECK(func_001CFBF8(vectors, 0, 256) == 1);
    CLOSE(vectors[0].x, 3); CLOSE(vectors[0].y, -6); CLOSE(vectors[0].z, 3);
    CLOSE(FIELD(float, vectors, 0xC), 54.1f);
    reset(); lookup_mode = 1; vectors[0].x = 99;
    CHECK(func_001CFBF8(vectors, 0, 0) == 0); CLOSE(vectors[0].x, 99);
    reset(); BYTE(&owner, 0x20) = 2; route_call_mode = 1;
    func_001E75F8(&route); CHECK(captured_index == 2); CHECK(route.field10 == 1);
    reset(); records[0].field05 = 4; BYTE(&owner, 0x20) = 0;
    func_001E71E0(&route); CHECK(BYTE(&owner, 0x20) == 1 && route.field10 == 1);
    reset(); records[0].field05 = 4; BYTE(&owner, 0x20) = 2; records[0].field1C = 0xFFFFFFFFU;
    func_001E71E0(&route); CHECK(BYTE(&owner, 0x21) == 1 && BYTE(&owner, 0x20) == 2);
    reset(); BYTE(&owner, 0x21) = 1; records[0].field18 = 1; records[1].field05 = 5;
    func_001E71E0(&route); CHECK(owner.field14 == 1 && BYTE(&owner, 0x20) == 3);
    reset(); route.field11 = 2; func_001E6908(&route); CHECK(lookup_calls == 0);
    reset(); classify_mode = 2; func_001E6908(&route); CHECK(route.field11 == 2 && route.field10 == 1);
    reset(); classify_mode = 1; route.field68.z = 10;
    entity.fieldD0.z = 1; vectors[0].z = 1; SIGNED_BYTE(&owner, 0x22) = 2;
    func_001E6908(&route); CHECK(BYTE(&owner, 0x21) == 1 && SIGNED_BYTE(&owner, 0x22) == -2);
    reset(); records[0].field05 = 2; records[0].field1C = 0x80000000U;
    { unsigned i; for (i = 0; i < 5; ++i) {
        FIELD(u32, road_records, 0x48 + i * 4) = 0xFFFFFFFFU;
        FIELD(u32, road_records + 1, 0x48 + i * 4) = 0xFFFFFFFFU;
    } }
    FIELD(u32, road_records + 1, 0x48) = 2;
    records[2].field04 = 8; records[2].field05 = 5;
    records[2].field1C = FIELD(u32, road_records + 1, 0x40) = 19;
    lookup_mode = 2;
    func_001E71E0(&route);
    CHECK(owner.field14 == 2 && BYTE(&owner, 0x21) == 1 && BYTE(&owner, 0x20) == 4);
    CHECK(SIGNED_BYTE(&owner, 0x22) == 2 && random_calls == 1);
    reset(); route.field68.z = 10;
    { unsigned i; for (i = 0; i < 5; ++i) FIELD(u32, road_records, 0x48 + i * 4) = 0xFFFFFFFFU; }
    FIELD(u32, road_records, 0x48) = 1; records[1].field05 = 2; records[1].field1C = 0xFFFFFFFFU;
    FIELD(u16, records + 1, 6) = 10; vectors[11].x = 2;
    SIGNED_BYTE(&owner, 0x22) = 7;
    func_001E6908(&route);
    CHECK(owner.field14 == 1 && BYTE(&owner, 0x21) == 1 && BYTE(&owner, 0x20) == 1);
    CHECK(SIGNED_BYTE(&owner, 0x22) == 7);
}

static void test_branch_timer_and_scoring(void)
{
    reset();
    { unsigned i; for (i = 0; i < 5; ++i) FIELD(u32, road_records, 0x48 + i * 4) = 0xFFFFFFFFU; }
    records[0].field05 = 4; SIGNED_BYTE(&owner, 0x22) = 2;
    func_001E6ED8(&route, road_records);
    CHECK(SIGNED_BYTE(&owner, 0x22) == -2 && BYTE(&owner, 0x21) == 1 && BYTE(&owner, 0x20) == 3);
    reset();
    { unsigned i; for (i = 0; i < 5; ++i) FIELD(u32, road_records, 0x48 + i * 4) = 0xFFFFFFFFU; }
    FIELD(u32, road_records, 0x48) = 1; records[1].field04 = 4;
    records[1].field1C = FIELD(u32, road_records, 0x40) = 19; records[1].field05 = 5;
    route_call_mode = 3; func_001E6ED8(&route, road_records);
    CHECK(replacement.field14 == 1 && SIGNED_BYTE(&replacement, 0x22) == -2);
    CHECK(BYTE(&replacement, 0x21) == 1 && BYTE(&replacement, 0x20) == 4);
    reset(); route.field14 = -1; CHECK(func_001E6DA0(&route) == 1); CHECK(member_calls == 0);
    reset(); route.field14 = NAN; D_003F8EB0[0].selector = -1; D_003F8EB0[0].target.direct = native_member;
    route_call_mode = 4; CHECK(func_001E6DA0(&route) == 0); CHECK(member_calls == 1);
    CLOSE(replacement.field34, 0.25f); CLOSE(replacement.field30, 6.2831854820251465f);
    reset(); route.field14 = 0.05f; CHECK(func_001E6DA0(&route) == 0); CLOSE(route.field14, -0.05f);
    CHECK(func_001E6DA0(&route) == 1);
    reset();
    {
        GeorgeGoalBits64 virtual_pair = ((GeorgeGoalBits64)(u32)native_member << 32) | 0x7FFFU;
        GeorgeGoalRouteAction *stored = (GeorgeGoalRouteAction *)member_storage;
        memset(member_storage, 0, sizeof(member_storage));
        stored->base.links.unknown00 = (u32)&owner;
        FIELD(const GeorgeGoalBits64 *, stored, 0x18) = &virtual_pair;
        D_003F8EB0[0].selector = 1; D_003F8EB0[0].adjustment = 32767;
        D_003F8EB0[0].target.vtable_offset = 0x18;
        active_route = stored; expected_member = member_storage + 65534;
        CHECK(func_001E6DA0(stored) == 0); CHECK(member_calls == 1);
    }
    reset(); route.field34.field0C.z = 5; route.field5C.z = -1; route.field10 = 1;
    func_001E73C8(&route); CHECK(motion_calls == 1 && owner.field18.bits == 0xFFFFFFFFU && route.field10 == 0);
    CLOSE(route.field34.field20, 1);
    reset(); selection.field64 = &reference; selection.field60 = 20; target.x = target.y = 0; target.z = 4;
    selection_mode = 1; CHECK(func_001E5C48(&selection) == 2); CHECK(score_calls == 6 && random_calls == 0);
    reset(); selection.field64 = &reference; selection.field60 = 20; target.x = target.y = 0; target.z = 4;
    FIELD(u8, original_record, 0x16) = FIELD(u8, candidate_record, 0x16) = 1;
    CHECK(func_001E5C48(&selection) == 1); CHECK(score_calls == 6 && hit_calls == 10 && random_calls == 1);
    CHECK(selection.field4C[0] == 6 && selection.field4C[1] == 4 && selection.field4C[4] == 4);
    CLOSE(selection.field68.x, -4); CLOSE(selection.field68.z, 0);
    reset(); selection.field64 = &reference; selection.field60 = 5; target.x = target.y = 0; target.z = 4;
    CHECK(func_001E5C48(&selection) == 2); CHECK(score_calls == 1);
}

int main(void)
{
    test_constructors(); test_endpoint_and_output(); test_plane_alias_and_routes();
    test_branch_timer_and_scoring();
    printf("goal_methods5: %u checks passed\n", checks);
    return 0;
}
