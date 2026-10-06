/* Callback/alias tests of independently recovered C; no original files. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/game/goal_methods6.c"

static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
#define CLOSE(a,b) CHECK(fabsf((a) - (b)) < 0.00002f)

u8 D_003F8EC0[32], D_00437700[8], D_00437828[8], D_00437878[8];
GeorgeSlotPool D_0045C680;
static GeorgeGoalOwner owner, replacement;
static GeorgeGoalEntity entity, second_entity;
static GeorgeGoalJumpTarget jump;
static GeorgeGoalPair pair;
static GeorgeGoalReferencedObject reference, second_reference;
static GeorgeMathVec3 target;
static u8 children[3][96], child_tables[3][0x38];
static GeorgeGoalBase *child[3];
static u8 vehicle_storage[2][0x80], vehicle_tables[2][0x1C0];
static GeorgeGoalVirtualObject *vehicle[2];
static u8 enter_storage[0xD00];
static GeorgeGoalEnterVehicle *enter;
static float matrices[2][16];
static GeorgeMathVec3 vehicle_position;
static unsigned base_calls, release_calls, jump_calls, child_calls, pool_calls;
static unsigned restart_calls, vector_calls, scalar_calls, predicate_calls, output_calls;
static u32 destroy_flags, word_argument, output_command;
static s32 jump_result, first_result, second_result, predicate_value[4];
static int base_mode, release_mode, jump_mode, child_mode, vehicle_mode, expected_side;
static GeorgeGoalBase *last_restarted;
static GeorgeGoalReferencedObject *last_released;
static const GeorgeMathVec3 *last_target;
static GeorgeGoalEntity *last_entity;
static float last_height;
static void *expected_pointer;
static GeorgeGoalOutput *expected_output;
static s32 predicate_commands[4];

static void reset(void)
{
    unsigned i;
    static const u32 commands[8] = {0,2,3,1,0,3,1,2};
    memset(&owner, 0, sizeof(owner)); memset(&replacement, 0, sizeof(replacement));
    memset(&entity, 0, sizeof(entity)); memset(&second_entity, 0, sizeof(second_entity));
    memset(&jump, 0, sizeof(jump)); memset(&pair, 0, sizeof(pair));
    memset(&reference, 0, sizeof(reference)); memset(&second_reference, 0, sizeof(second_reference));
    memset(children, 0, sizeof(children)); memset(child_tables, 0, sizeof(child_tables));
    memset(vehicle_storage, 0, sizeof(vehicle_storage)); memset(vehicle_tables, 0, sizeof(vehicle_tables));
    memset(matrices, 0, sizeof(matrices)); memset(enter_storage, 0, sizeof(enter_storage));
    memset(predicate_value, 0, sizeof(predicate_value));
    memcpy(D_003F8EC0, commands, sizeof(commands));
    owner.field08 = &entity; replacement.field08 = &second_entity;
    owner.field60 = 0.25f; replacement.field60 = 0.5f;
    jump.base.links.unknown00 = pair.base.links.unknown00 = (u32)&owner;
    target.x = 8; target.y = 10; target.z = 12;
    reference.field06 = 0x40;
    for (i = 0; i < 3; ++i) {
        child[i] = (GeorgeGoalBase *)(children[i] + 24);
        child[i]->field0C = child_tables[i];
    }
    for (i = 0; i < 2; ++i) {
        vehicle[i] = (GeorgeGoalVirtualObject *)(vehicle_storage[i] + 16);
        vehicle[i]->field04 = vehicle_tables[i];
        matrices[i][0] = 1; matrices[i][10] = 1;
        matrices[i][12] = 10; matrices[i][13] = 20; matrices[i][14] = 30;
    }
    enter = (GeorgeGoalEnterVehicle *)enter_storage;
    enter->base.links.unknown00 = (u32)&owner;
    enter->field10 = (u32)&reference; reference.field20.word = (u32)vehicle[0];
    enter->field3E = 7; owner.unknown00 = (u32)vehicle[0];
    vehicle_position.x = vehicle_position.y = vehicle_position.z = 0;
    base_calls = release_calls = jump_calls = child_calls = pool_calls = 0;
    restart_calls = vector_calls = scalar_calls = predicate_calls = output_calls = 0;
    base_mode = release_mode = jump_mode = child_mode = vehicle_mode = 0;
    jump_result = 1; first_result = 7; second_result = -5;
    expected_side = 0; expected_pointer = NULL; expected_output = NULL;
}

GeorgeGameplayGoal *func_0020D2A0(void *storage, void *input_owner)
{
    GeorgeGoalBase *base = storage;
    ++base_calls;
    base->links.unknown00 = base_mode ? (u32)&replacement : (u32)input_owner;
    return &base->links;
}
void func_0020D2C0(GeorgeGoalBase *goal, u32 flags)
{ CHECK(goal != NULL); destroy_flags = flags; }
void func_001CAF88(GeorgeGoalReferencedObject *object)
{
    ++release_calls; last_released = object;
    if (release_mode) { jump.field1C = &second_reference; jump.base.links.unknown00 = (u32)&replacement; }
}
const GeorgeMathVec3 *func_001CAFE0(GeorgeGoalReferencedObject *object)
{
    CHECK(object == &reference);
    if (jump_mode == 1) jump.base.links.unknown00 = (u32)&replacement;
    return &target;
}
s32 func_00177270(GeorgeGoalEntity *input_entity, const GeorgeMathVec3 *position,
                  u32 word, float height)
{
    ++jump_calls; last_entity = input_entity; last_target = position;
    word_argument = word; last_height = height;
    if (jump_mode == 2) { jump.base.links.unknown00 = (u32)&replacement; jump.field2C = 17; }
    return jump_result;
}
void func_001E1028(GeorgeGoalTimedAction *goal)
{ ++restart_calls; last_restarted = (GeorgeGoalBase *)goal; }
void func_002AD748(GeorgeSlotPool *pool, void *slot)
{
    CHECK(pool == &D_0045C680); CHECK(slot == child[0] || slot == child[1] || slot == child[2]);
    ++pool_calls;
}
void func_001F6138(GeorgeGoalVirtualObject *object)
{ CHECK(object == vehicle[0]); FIELD(u32, object, 0x38) += 1U; }

static void callback_this(void *input)
{
    if (expected_pointer != NULL) CHECK(input == expected_pointer);
}
static void child_word(void *input, u32 word)
{
    callback_this(input); ++child_calls; word_argument = word;
    if (child_calls == 1 && child_mode == 1) {
        pair.field14 = child[2]; expected_pointer = ADDRESS(void, child[2], -9);
    }
}
static void child_output(void *input, GeorgeGoalOutput *output)
{
    callback_this(input); CHECK(output == expected_output); ++child_calls;
    output->field00 += 1;
    if (child_calls == 1 && child_mode == 1) {
        pair.field14 = child[2]; expected_pointer = ADDRESS(void, child[2], -9);
    }
}
static void child_void(void *input)
{
    callback_this(input); ++child_calls;
    if (child_calls == 1 && child_mode == 1) {
        pair.field14 = child[2]; expected_pointer = ADDRESS(void, child[2], -9);
    }
    if (child_calls == 1 && child_mode == 2) pair.field1A = 0;
}
static s32 child_update(void *input)
{
    callback_this(input); ++child_calls;
    if (child_calls == 1) {
        if (child_mode == 1) { pair.field14 = child[2]; expected_pointer = ADDRESS(void, child[2], -9); }
        if (child_mode == 2) pair.field1A = 0;
        return first_result;
    }
    CHECK(pair.field18 != 0); /* First completion store is delayed. */
    if (child_mode == 3) pair.field14 = child[0];
    return second_result;
}
static void child_destroy(void *input, u32 flags)
{
    CHECK(flags == 2); callback_this(input); ++child_calls;
    if (child_calls == 1) {
        CHECK(pair.field10 == child[0]);
        if (child_mode == 1) { pair.field14 = child[2]; expected_pointer = ADDRESS(void, child[2], -9); }
    } else {
        CHECK(pair.field10 == NULL && pair.field18 == 0);
        if (child_mode == 1) pair.base.links.unknown00 = (u32)&replacement;
    }
}
static void install_children(void)
{
    unsigned i;
    pair.field10 = child[0]; pair.field14 = child[1]; pair.field18 = -1; pair.field1A = -2;
    for (i = 0; i < 3; ++i) {
        GeorgeGoalVirtualWord *destruct = ADDRESS(GeorgeGoalVirtualWord, child_tables[i], 8);
        GeorgeGoalVirtualWord *word = ADDRESS(GeorgeGoalVirtualWord, child_tables[i], 0x10);
        GeorgeGoalVirtualOutput *output = ADDRESS(GeorgeGoalVirtualOutput, child_tables[i], 0x18);
        GeorgeGoalVirtualVoid *stop = ADDRESS(GeorgeGoalVirtualVoid, child_tables[i], 0x20);
        GeorgeGoalVirtualInt *update = ADDRESS(GeorgeGoalVirtualInt, child_tables[i], 0x28);
        GeorgeGoalVirtualVoid *active = ADDRESS(GeorgeGoalVirtualVoid, child_tables[i], 0x30);
        destruct->adjustment = word->adjustment = output->adjustment = stop->adjustment =
          update->adjustment = active->adjustment = -9;
        destruct->invoke = child_destroy; word->invoke = child_word; output->invoke = child_output;
        stop->invoke = active->invoke = child_void; update->invoke = child_update;
    }
    expected_pointer = ADDRESS(void, child[0], -9);
}

static const GeorgeMathVec3 *vehicle_position_call(void *input)
{
    CHECK(input == ADDRESS(void, vehicle[0], -7)); ++vector_calls;
    if (vehicle_mode == 1) {
        owner.field08 = &second_entity; enter->field14 = (u32)vehicle[1];
    }
    return &vehicle_position;
}
static const GeorgeMathVec3 *vehicle_basis_call(void *input)
{
    unsigned which = input == ADDRESS(void, vehicle[1], -7) ? 1U : 0U;
    CHECK(input == ADDRESS(void, vehicle[which], -7)); ++vector_calls;
    if (vehicle_mode == 2 && vector_calls == 4) enter->field14 = (u32)vehicle[1];
    return (GeorgeMathVec3 *)matrices[which];
}
static float vehicle_extent_front(void *input)
{
    CHECK(input == ADDRESS(void, vehicle[vehicle_mode == 2 ? 1 : 0], -7)); ++scalar_calls;
    if (vehicle_mode == 2) {
        matrices[0][12] = 50; matrices[1][10] = 2;
    }
    return 5;
}
static float vehicle_extent_back(void *input)
{ CHECK(input == ADDRESS(void, vehicle[0], -7)); ++scalar_calls; return 8; }
static float vehicle_width(void *input)
{ CHECK(input == ADDRESS(void, vehicle[vehicle_mode == 2 ? 1 : 0], -7)); ++scalar_calls; return 4; }
static s32 vehicle_predicate(void *input, s32 command)
{
    CHECK(input == ADDRESS(void, vehicle[vehicle_mode == 1 ? 1 : 0], -7));
    CHECK(predicate_calls < 4); predicate_commands[predicate_calls] = command;
    if (vehicle_mode == 3) D_003F8EC0[expected_side * 16 + 4] = 255;
    return predicate_value[predicate_calls++];
}
static void vehicle_output(void *input, u32 command, GeorgeMathVec3 *output, GeorgeMathVec3 *scratch)
{
    CHECK(input == ADDRESS(void, vehicle[vehicle_mode == 1 ? 1 : 0], -7));
    CHECK(scratch != NULL); ++output_calls; output_command = command;
    output->x = 100; output->y = 200; output->z = 300;
    if (vehicle_mode == 4) { FIELD(u8, enter, 0x3C) = 3; enter->field3F = 0; }
}
static void vehicle_stop_call(void *input, float value)
{
    CHECK(input == ADDRESS(void, vehicle[0], -7)); CLOSE(value, -1);
    ++child_calls; owner.unknown00 = (u32)vehicle[1];
    vehicle[0]->field04 = vehicle_tables[1];
}
static void vehicle_mode_call(void *input, u32 word)
{ CHECK(input == ADDRESS(void, vehicle[0], 11)); CHECK(word == 1); ++child_calls; }
static void install_vehicle(void)
{
    unsigned i;
    for (i = 0; i < 2; ++i) {
        GeorgeGoalVirtualVector *position = ADDRESS(GeorgeGoalVirtualVector, vehicle_tables[i], 0x28);
        GeorgeGoalVirtualVector *basis = ADDRESS(GeorgeGoalVirtualVector, vehicle_tables[i], 0x98);
        GeorgeGoalVirtualFloatResult *front = ADDRESS(GeorgeGoalVirtualFloatResult, vehicle_tables[i], 0x190);
        GeorgeGoalVirtualFloatResult *back = ADDRESS(GeorgeGoalVirtualFloatResult, vehicle_tables[i], 0x198);
        GeorgeGoalVirtualFloatResult *width = ADDRESS(GeorgeGoalVirtualFloatResult, vehicle_tables[i], 0x188);
        GeorgeGoalVirtualCommand *test = ADDRESS(GeorgeGoalVirtualCommand, vehicle_tables[i], 0x1B8);
        GeorgeGoalVirtualCommandVector *output = ADDRESS(GeorgeGoalVirtualCommandVector, vehicle_tables[i], 0xD8);
        GeorgeGoalVirtualFloat *stop = ADDRESS(GeorgeGoalVirtualFloat, vehicle_tables[i], 0x20);
        GeorgeGoalVirtualWord *mode = ADDRESS(GeorgeGoalVirtualWord, vehicle_tables[i], 0x30);
        position->adjustment = basis->adjustment = front->adjustment = back->adjustment =
          width->adjustment = test->adjustment = output->adjustment = stop->adjustment = -7;
        mode->adjustment = 11;
        position->invoke = vehicle_position_call; basis->invoke = vehicle_basis_call;
        front->invoke = vehicle_extent_front; back->invoke = vehicle_extent_back; width->invoke = vehicle_width;
        test->invoke = vehicle_predicate; output->invoke = vehicle_output;
        stop->invoke = vehicle_stop_call; mode->invoke = vehicle_mode_call;
    }
}

static void test_jump(void)
{
    reset();
    CHECK(func_001E8A78(&jump, &owner, &target, 0x81234567U, 3, 5) == &jump.base.links);
    CHECK(jump.base.field0C == D_00437700 && jump.field1C == NULL);
    CHECK(jump.field20 == 0x81234567U && jump.field30 == 2 && jump.field31 == 0);
    CLOSE(jump.field10.x, 8); CLOSE(jump.field10.y, 10); CLOSE(jump.field10.z, 12);
    CLOSE(jump.field24, 3); CLOSE(jump.field2C, 5);
    reset(); jump.field10.x = 1; jump.field10.y = 2; jump.field10.z = 3;
    FIELD(float, &jump, 0xC) = 9;
    func_001E8A78(&jump, &owner, ADDRESS(GeorgeMathVec3, &jump, 0xC), 4, 3, 5);
    /* Constructor installs vtable first, then forward-copy overlap causes
     * each newly stored scalar to be used by the next source read. */
    CHECK(FIELD(u32, &jump, 0x10) == (u32)D_00437700);
    CHECK(FIELD(u32, &jump, 0x14) == (u32)D_00437700);
    CHECK(FIELD(u32, &jump, 0x18) == (u32)D_00437700);
    reset(); reference.field05 = 255;
    CHECK(func_001E8B18(&jump, &owner, &reference, 0xABCDEF01U, 2, 4) == &jump.base.links);
    CHECK(reference.field05 == 0 && jump.field1C == &reference && jump.field20 == 0xABCDEF01U);
    reset(); jump.field1C = &reference; release_mode = 1; owner.field24 = replacement.field24 = 0xFFFF;
    func_001E8BA8(&jump, 0x12345678U);
    CHECK(release_calls == 1 && last_released == &reference && jump.field1C == NULL);
    CHECK(owner.field24 == 0xFFFF && replacement.field24 == 0xFFF7 && destroy_flags == 0x12345678U);
    reset(); jump.field1C = &reference; reference.field06 = 0; reference.field20.word = 0; jump.field30 = 8;
    func_001E8C10(&jump); CHECK(jump.field30 == 2 && jump_calls == 0);
    reset(); jump.field10 = target; jump.field24 = 3; jump.field2C = 5; jump.field20 = 0x91234567U;
    FIELD(float, &entity, 0x44) = 2; jump_mode = 2;
    func_001E8C10(&jump);
    CHECK(jump_calls == 1 && last_entity == &entity && last_target == &jump.field10);
    CHECK(word_argument == 0x91234567U && jump.field30 == 0);
    CLOSE(last_height, 11); CLOSE(jump.field28, 17);
    CHECK(owner.field24 == 0 && replacement.field24 == 8);
    reset(); jump.field1C = &reference; jump.field24 = 3; jump_mode = 1;
    FIELD(float, &second_entity, 0x44) = 12; jump_result = 0; jump.field30 = 7;
    func_001E8C10(&jump);
    CHECK(last_entity == &second_entity && last_target == &target && jump.field30 == 7);
    CLOSE(last_height, 3);
    reset(); jump.field10.y = NAN; jump.field24 = 3; func_001E8C10(&jump); CLOSE(last_height, 3);
    reset(); jump.field30 = 9; CHECK(func_001E8CF8(&jump) == 9 && jump.field31 == 0);
    jump.field30 = 0; entity.field0C = 4;
    CHECK(func_001E8CF8(&jump) == 0 && jump.field31 == 0);
    entity.field0C = 0x80000004U; CHECK(func_001E8CF8(&jump) == 0 && jump.field31 == 1);
    jump.field28 = 0.25f; CHECK(func_001E8CF8(&jump) == 0); CLOSE(jump.field28, 0);
    CHECK(func_001E8CF8(&jump) == 1); CLOSE(jump.field28, -0.25f);
    reset(); jump.field31 = 1; jump.field28 = NAN; CHECK(func_001E8CF8(&jump) == 0 && isnan(jump.field28));
    jump.field31 = 2; jump.field28 = 7; CHECK(func_001E8CF8(&jump) == 0); CLOSE(jump.field28, 7);
}

static void test_pairs(void)
{
    unsigned i;
    void (*word_functions[2])(GeorgeGoalPair *, u32) = {func_001E8FC0, func_001E92B8};
    void (*output_functions[2])(GeorgeGoalPair *, GeorgeGoalOutput *) = {func_001E9020, func_001E9318};
    void (*stop_functions[2])(GeorgeGoalPair *) = {func_001E9080, func_001E9378};
    void (*active_functions[2])(GeorgeGoalPair *) = {func_001E9168, func_001E9468};
    reset(); base_mode = 1;
    CHECK(func_001E91D0(&pair, &owner, child[0], child[1]) == &pair.base.links);
    CHECK(pair.field10 == child[0] && pair.field14 == child[1] && pair.field18 == 1 && pair.field1A == 1);
    CHECK(pair.base.field0C == D_00437878 && replacement.field24 == 0x200 && owner.field24 == 0);
    reset(); CHECK(func_001E8EF8(&pair, &owner, child[0], child[1]) == &pair.base.links);
    CHECK(pair.field18 == 1 && pair.field1A == 1 && pair.base.field0C == D_00437828);
    for (i = 0; i < 2; ++i) {
        GeorgeGoalOutput output;
        reset(); install_children(); child_mode = 1;
        word_functions[i](&pair, 0x81234567U);
        CHECK(child_calls == 2 && word_argument == 0x81234567U);
        reset(); install_children(); child_mode = 1; output.field00 = 100; expected_output = &output;
        output_functions[i](&pair, &output); CHECK(child_calls == 2 && output.field00 == 102);
        reset(); install_children(); child_mode = 1; stop_functions[i](&pair); CHECK(child_calls == 2);
        reset(); install_children(); child_mode = 2; active_functions[i](&pair); CHECK(child_calls == 1);
        reset(); install_children(); child_mode = 1; active_functions[i](&pair); CHECK(child_calls == 2);
    }
    reset(); install_children(); child_mode = 1; func_001E8F58(&pair, 0x91234567U);
    CHECK(child_calls == 2 && pool_calls == 2 && pair.field10 == NULL && pair.field14 == NULL);
    CHECK(pair.field18 == 0 && pair.field1A == 0 && destroy_flags == 0x91234567U);
    reset(); install_children(); child_mode = 1; owner.field24 = replacement.field24 = 0xFFFF;
    func_001E9240(&pair, 0x71234567U);
    CHECK(child_calls == 2 && pool_calls == 2 && replacement.field24 == 0xFDFF && owner.field24 == 0xFFFF);
    CHECK(pair.field18 == 0 && pair.field1A == 0 && destroy_flags == 0x71234567U);
    reset(); install_children(); child_mode = 1;
    CHECK(func_001E90D0(&pair) == 7 && child_calls == 2 && pair.field18 == 0 && pair.field1A == 0);
    reset(); install_children(); child_mode = 2;
    CHECK(func_001E90D0(&pair) == 7 && child_calls == 1 && pair.field18 == 0);
    reset(); install_children(); child_mode = 3; expected_pointer = NULL;
    CHECK(func_001E93C8(&pair) == 7 && child_calls == 2 && pair.field18 == 0 && pair.field1A == -2);
    CHECK(restart_calls == 1 && last_restarted == child[0]);
    reset(); install_children(); first_result = 0; second_result = 0; expected_pointer = NULL;
    CHECK(func_001E93C8(&pair) == 0 && restart_calls == 0 && pair.field18 == -1 && pair.field1A == -2);
}

static void test_vehicle(void)
{
    unsigned side, forward, i;
    reset(); install_vehicle(); reference.field20.word = 0;
    func_001E7690(enter); CHECK(enter->field3E == 4 && vector_calls == 0);
    for (side = 0; side < 2; ++side) for (forward = 0; forward < 2; ++forward) {
        reset(); install_vehicle(); expected_side = (int)side;
        FIELD(float, &entity, 0x40) = side ? -3 : 3;
        FIELD(float, &entity, 0x48) = forward ? 3 : -3;
        if (side == 0) predicate_value[0] = 1; /* Skip command2, select3. */
        else enter->field41 = 1; /* Start at command0. */
        func_001E7690(enter);
        CHECK(enter->field3F == 3 && enter->field3E == 7 && output_calls == 1);
        CHECK(vector_calls == 9 && scalar_calls == 4);
        CLOSE(CANDIDATE(enter, 0)->x, 100);
        CLOSE(CANDIDATE(enter, 1)->x, side ? 14 : 6);
        CLOSE(CANDIDATE(enter, 2)->x, side ? 6 : 14);
        CLOSE(CANDIDATE(enter, 1)->y, 20); CLOSE(CANDIDATE(enter, 2)->y, 20);
        CLOSE(CANDIDATE(enter, 1)->z, forward ? 37 : 36);
        CLOSE(CANDIDATE(enter, 2)->z, forward ? 37 : 36);
    }
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = 3;
    func_001E7690(enter); CHECK(output_command == 2 && enter->field3F == 1 && scalar_calls == 0);
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = 3;
    for (i = 0; i < 4; ++i) predicate_value[i] = 1;
    func_001E7690(enter); CHECK(enter->field3E == 2 && predicate_calls == 3 && output_calls == 0);
    CHECK(predicate_commands[0] == 2 && predicate_commands[1] == 3 && predicate_commands[2] == 1);
    reset(); install_vehicle(); vehicle_mode = 1; FIELD(float, &entity, 0x40) = 3;
    FIELD(float, &second_entity, 0x40) = -10;
    func_001E7690(enter); CHECK(output_command == 2 && enter->field3F == 1);
    reset(); install_vehicle(); vehicle_mode = 2; FIELD(float, &entity, 0x40) = 3;
    FIELD(float, &entity, 0x48) = 3; predicate_value[0] = 1;
    func_001E7690(enter);
    /* First matrix is read after extent callback changes its origin;
     * second matrix comes from freshly reloaded replacement vehicle. */
    CLOSE(CANDIDATE(enter, 1)->x, 46); CLOSE(CANDIDATE(enter, 1)->z, 44);
    CLOSE(CANDIDATE(enter, 2)->x, 14); CLOSE(CANDIDATE(enter, 2)->z, 44);
    reset(); install_vehicle(); vehicle_mode = 3; FIELD(float, &entity, 0x40) = 3;
    D_003F8EC0[4] = 128;
    func_001E7690(enter); CHECK(predicate_commands[0] == -128 && output_command == 255);
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = 3; enter->field3F = 255;
    func_001E7690(enter); CHECK(enter->field3F == 0 && output_calls == 1);
    CLOSE(CANDIDATE(enter, 255)->z, 300);
    reset(); install_vehicle(); vehicle_mode = 4; FIELD(float, &entity, 0x40) = 3;
    FIELD(float, &entity, 0x48) = 3;
    func_001E7690(enter); CHECK(output_command == 2 && enter->field3F == 3);
    CLOSE(CANDIDATE(enter, 1)->x, 6);
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = NAN;
    func_001E7690(enter); CHECK(output_command == 3 && enter->field3F == 1);
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = 1;
    func_001E7690(enter); CHECK(output_command == 3 && enter->field3F == 1);
    reset(); install_vehicle(); FIELD(float, &entity, 0x40) = 3;
    FIELD(float, &entity, 0x48) = 3; matrices[0][9] = NAN; predicate_value[0] = 1;
    func_001E7690(enter); CLOSE(CANDIDATE(enter, 1)->z, 36);
    reset(); install_vehicle(); func_001E8988(&jump.base); CHECK(child_calls == 2);
}

int main(void)
{
    test_jump(); test_pairs(); test_vehicle();
    printf("goal_methods6: %u checks passed\n", checks);
    return 0;
}
