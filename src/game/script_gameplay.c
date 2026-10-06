#include "george/script_gameplay.h"

extern GeorgeGameplayActor *D_003F8AF8;
extern u8 D_0045C680[];
extern GeorgeDeimosValue D_00474748[];
extern GeorgeDeimosValue *D_00474F48;
extern void *func_002AD700(void *allocator, u32 size, s32 flag);
extern void func_002D00A0(GeorgeDeimosHashTable *table, GeorgeMathVec3 *output, float *angle_output);

/* Constructor parameters are recovered from both callers and callee entries. */
extern GeorgeGameplayGoal *func_001E84D0(void *storage, void *owner, u32 object, u32 word1, u32 word2, u32 word3);
extern GeorgeGameplayGoal *func_001E87C0(void *storage, void *actor, u32 word);
extern GeorgeGameplayGoal *func_001E1E78(void *storage, void *owner, u32 word);
extern GeorgeGameplayGoal *func_001E5A58(void *storage, void *owner, u32 object);
extern GeorgeGameplayGoal *func_001E5AA8(void *storage, void *owner, const GeorgeMathVec3 *position);
extern GeorgeGameplayGoal *func_001E6828(void *storage, void *owner, u32 word);
extern GeorgeGameplayGoal *func_001DFD38(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001E04C8(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001D9A80(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001E0EC8(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001E1078(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001D9920(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001E8E40(void *storage, void *owner, u32 word);
extern GeorgeGameplayGoal *func_001DA5E8(void *storage, void *owner, float value0, float value1, float value2);
extern GeorgeGameplayGoal *func_001DC5A0(void *storage, void *owner, s32 value);
extern GeorgeGameplayGoal *func_001DC650(void *storage, void *owner, s32 value);
extern GeorgeGameplayGoal *func_001E1F48(void *storage, void *owner, u32 word);
extern GeorgeGameplayGoal *func_001E5988(void *storage, void *owner, u32 word0, u32 word1);
extern GeorgeGameplayGoal *func_001DC6F8(void *storage, void *owner, float value);
extern GeorgeGameplayGoal *func_001E8950(void *storage, void *owner);
extern GeorgeGameplayGoal *func_001E0C38(void *storage, void *owner, u32 value0, u32 value1, u32 value2, u32 value3, u32 value4);
extern GeorgeGameplayGoal *func_001DC7A8(void *storage, void *owner);
extern GeorgeGameplayGoal *func_001DF028(void *storage, void *owner);
extern GeorgeGameplayGoal *func_001DB930(void *storage, void *owner);
extern GeorgeGameplayGoal *func_001E1DF0(void *storage, void *owner, void *actor);
extern GeorgeGameplayGoal *func_001E8D98(void *storage, void *actor);

/* All callbacks preserve payload bits when clearing an optional result slot. */
#define CLEAR_RESULT() \
    if (destination != -1) { \
        D_00474748[destination].subtype = 0; \
        D_00474748[destination].tag = 0; \
    }
#define APPEND_RESULT() \
    func_001D89D8(&D_003F8AF8->field188, goal); \
    CLEAR_RESULT()

/* mov_entervehicle: capture the first word before the allocator call. */
void func_001D3A68(s32 argc, s32 destination)
{
    u32 object = D_00474F48[0].payload.bits;
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x6C, 0);
    (void)argc;
    if (goal != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        goal = func_001E84D0(goal, D_003F8AF8->field34, object,
                            arguments[1].payload.bits, arguments[2].payload.bits,
                            arguments[3].payload.bits);
    }
    APPEND_RESULT();
}

void func_001D3B20(s32 argc, s32 destination)
{
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x18, 0);
    (void)argc;
    if (goal != 0) {
        goal = func_001E87C0(goal, D_003F8AF8, D_00474F48[0].payload.bits);
    }
    APPEND_RESULT();
}

#define WORD_AFTER_BODY(size, constructor) \
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, size, 0); \
    (void)argc; \
    if (goal != 0) { \
        goal = constructor(goal, D_003F8AF8->field34, D_00474F48[0].payload.bits); \
    } \
    APPEND_RESULT()

void func_001D3BB8(s32 argc, s32 destination)
{
    WORD_AFTER_BODY(0x18, func_001E1E78);
}

/* mov_setpos: dispatch userdata tag 6 and vector-table tag 4 separately. */
void func_001D44D0(s32 argc, s32 destination)
{
    s16 tag = (s16)D_00474F48[0].tag;
    GeorgeGameplayGoal *goal = 0;
    (void)argc;
    if (tag == 6) {
        goal = func_002AD700(D_0045C680, 0x20, 0);
        if (goal != 0) {
            goal = func_001E5A58(goal, D_003F8AF8->field34,
                               D_00474F48[0].payload.bits);
        }
    } else if (tag == 4) {
        GeorgeMathVec3 position;
        func_002D00A0((GeorgeDeimosHashTable *)D_00474F48[0].payload.pointer,
                      &position, 0);
        goal = func_002AD700(D_0045C680, 0x20, 0);
        if (goal != 0) {
            goal = func_001E5AA8(goal, D_003F8AF8->field34, &position);
        }
    }
    APPEND_RESULT();
}

void func_001D48D0(s32 argc, s32 destination)
{
    WORD_AFTER_BODY(0x24, func_001E6828);
}

/* These callbacks read the scalar after allocation, unlike the act_* group. */
#define FLOAT_AFTER_BODY(size, constructor) \
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, size, 0); \
    (void)argc; \
    if (goal != 0) { \
        goal = constructor(goal, D_003F8AF8->field34, D_00474F48[0].payload.scalar); \
    } \
    APPEND_RESULT()

void func_001D4968(s32 argc, s32 destination)
{
    FLOAT_AFTER_BODY(0x74, func_001DFD38);
}

void func_001D4A00(s32 argc, s32 destination)
{
    FLOAT_AFTER_BODY(0x58, func_001E04C8);
}

#define FLOAT_BEFORE_BODY(size, constructor) \
    float value = D_00474F48[0].payload.scalar; \
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, size, 0); \
    (void)argc; \
    if (goal != 0) { goal = constructor(goal, D_003F8AF8->field34, value); } \
    APPEND_RESULT()

void func_001D4E48(s32 argc, s32 destination)
{
    FLOAT_BEFORE_BODY(0x1C, func_001D9A80);
}

void func_001D52D8(s32 argc, s32 destination)
{
    FLOAT_BEFORE_BODY(0x1C, func_001E0EC8);
}

void func_001D5378(s32 argc, s32 destination)
{
    FLOAT_BEFORE_BODY(0x1C, func_001E1078);
}

void func_001D57F0(s32 argc, s32 destination)
{
    FLOAT_BEFORE_BODY(0x1C, func_001D9920);
}

void func_001D5CD0(s32 argc, s32 destination)
{
    WORD_AFTER_BODY(0x18, func_001E8E40);
}

/* gen_lookaround snapshots all three floats before allocating the goal. */
void func_001D5D68(s32 argc, s32 destination)
{
    GeorgeDeimosValue *arguments = D_00474F48;
    float value1 = arguments[1].payload.scalar;
    float value2 = arguments[2].payload.scalar;
    float value0 = arguments[0].payload.scalar;
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x28, 0);
    (void)argc;
    if (goal != 0) {
        goal = func_001DA5E8(goal, D_003F8AF8->field34, value0, value1, value2);
    }
    APPEND_RESULT();
}

/* Optional numeric restart parameter; EE opcode 0x46000064 is trunc.w.s. */
void func_001D5E28(s32 argc, s32 destination)
{
    s32 value = 0;
    GeorgeGameplayGoal *goal;
    if (argc > 0) {
        value = (s32)D_00474F48[0].payload.scalar;
    }
    goal = func_002AD700(D_0045C680, 0x14, 0);
    if (goal != 0) {
        goal = func_001DC5A0(goal, D_003F8AF8->field34, value);
    }
    APPEND_RESULT();
}

/* gen_setmood is inhibited by actor flag 0x400, but always clears its result. */
void func_001D5ED8(s32 argc, s32 destination)
{
    (void)argc;
    if ((D_003F8AF8->field148 & 0x400) == 0) {
        GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x18, 0);
        if (goal != 0) {
            goal = func_001DC650(goal, D_003F8AF8->field34,
                                (s32)D_00474F48[0].payload.scalar);
        }
        func_001D89D8(&D_003F8AF8->field188, goal);
    }
    CLEAR_RESULT();
}

void func_001D5F88(s32 argc, s32 destination)
{
    WORD_AFTER_BODY(0x14, func_001E1F48);
}

void func_001D6020(s32 argc, s32 destination)
{
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x18, 0);
    (void)argc;
    if (goal != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        goal = func_001E5988(goal, D_003F8AF8->field34,
                            arguments[0].payload.bits, arguments[1].payload.bits);
    }
    APPEND_RESULT();
}

void func_001D60C0(s32 argc, s32 destination)
{
    FLOAT_AFTER_BODY(0x18, func_001DC6F8);
}

#define NO_ARGUMENT_BODY(size, constructor) \
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, size, 0); \
    (void)argc; \
    if (goal != 0) { goal = constructor(goal, D_003F8AF8->field34); } \
    APPEND_RESULT()

void func_001D6648(s32 argc, s32 destination)
{
    NO_ARGUMENT_BODY(0x10, func_001E8950);
}

void func_001D66D0(s32 argc, s32 destination)
{
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0xA0, 0);
    (void)argc;
    if (goal != 0) {
        goal = func_001E0C38(goal, D_003F8AF8->field34, 0, 0, 0, 0, 0);
    }
    APPEND_RESULT();
}

void func_001D6948(s32 argc, s32 destination)
{
    NO_ARGUMENT_BODY(0x10, func_001DC7A8);
}

void func_001D6A30(s32 argc, s32 destination)
{
    NO_ARGUMENT_BODY(0x90, func_001DF028);
}

void func_001D6AB8(s32 argc, s32 destination)
{
    NO_ARGUMENT_BODY(0x10, func_001DB930);
}

void func_001D6B40(s32 argc, s32 destination)
{
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x14, 0);
    (void)argc;
    if (goal != 0) {
        GeorgeGameplayActor *actor = D_003F8AF8;
        goal = func_001E1DF0(goal, actor->field34, actor);
    }
    APPEND_RESULT();
}

void func_001D6BC8(s32 argc, s32 destination)
{
    GeorgeGameplayGoal *goal = func_002AD700(D_0045C680, 0x14, 0);
    (void)argc;
    if (goal != 0) {
        goal = func_001E8D98(goal, D_003F8AF8);
    }
    APPEND_RESULT();
}

/* Preserve tail reload after the first store: aliased layouts can change it. */
void func_001D89D8(GeorgeGameplayGoalQueue *queue, GeorgeGameplayGoal *goal)
{
    if (queue->field08 != 0) {
        queue->field08->field04 = goal;
        goal->field08 = queue->field08;
        queue->field08 = goal;
    } else {
        queue->field00 = goal;
        queue->field08 = goal;
    }
}

#undef NO_ARGUMENT_BODY
#undef FLOAT_BEFORE_BODY
#undef FLOAT_AFTER_BODY
#undef WORD_AFTER_BODY
#undef APPEND_RESULT
#undef CLEAR_RESULT
