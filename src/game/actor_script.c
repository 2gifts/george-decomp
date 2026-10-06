#include "george/actor_script.h"

extern GeorgeDeimosValue D_00474748[];
extern GeorgeDeimosValue *D_00474F48;
extern void *func_002D0B48(u32 key);
extern void func_002CC938(const char *format, ...);
extern const char D_0042CF38[], D_0042CF98[], D_0042CFF8[], D_0042D0D8[];
extern const char D_0042D300[], D_0042D330[], D_0042D370[], D_0042D3B8[], D_0042D3E8[];
extern void *func_00210078(u32, u32, u32);
extern void *func_00236CB8(const void *, void *, u32, u32, u32);
extern void func_00178D10(GeorgeGoalEntity *, u32);
extern void func_00178E80(GeorgeGoalEntity *, u32, float);
extern s32 func_0014F438(void *, u32);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define SLOT(index) ((GeorgeDeimosValue *)ADDRESS(D_00474748, (u32)(index) << 3))
#define ACTOR_LOOKUP() ((GeorgeGoalEntity *)func_002D0B48(0x69F0BC67U))

/* No payload clearing: each original empty result only writes two halfwords. */
#define EMPTY_RESULT() do { \
    if (destination != -1) { \
        SLOT(destination)->subtype = 0; \
        SLOT(destination)->tag = 0; \
    } \
} while (0)
#define WORD_RESULT(value, result_tag) do { \
    u32 result_word = (u32)(value); \
    if (destination != -1) { \
        SLOT(destination)->tag = result_tag; \
        SLOT(destination)->payload.bits = result_word; \
        SLOT(destination)->subtype = 0; \
    } \
} while (0)
#define SCALAR_RESULT(value) do { \
    float result_scalar = (value); \
    if (destination != -1) { \
        SLOT(destination)->payload.scalar = result_scalar; \
        SLOT(destination)->tag = 2; \
        SLOT(destination)->subtype = 0; \
    } \
} while (0)

void func_00192E70(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) WORD_RESULT(FIELD(entity, 0x19C, u32), 6);
}
void func_00192ED0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosPoolNode *result = func_002D0178((const GeorgeMathVec3 *)ADDRESS(entity, 0x40), FIELD(entity, 0x58, float));
        WORD_RESULT(result, 4);
    }
}
void func_00192F40(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) WORD_RESULT(FIELD(FIELD(entity, 0x1AC, void *), 0x164, u32), 6);
}
void func_00192FA0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) SCALAR_RESULT(FIELD(entity, 0x58, float) * 57.2957763671875f);
}
void func_00193010(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) SCALAR_RESULT(FIELD(entity, 0x36C, float));
}

/* Complete scalar setter family: its empty result runs even on lookup failure
 * and after the original diagnostic callback. */
#define SET_SCALAR(name, member, message) \
void name(s32 argc, s32 destination) { \
    GeorgeGoalEntity *entity = ACTOR_LOOKUP(); \
    (void)argc; \
    if (entity != 0) { \
        GeorgeDeimosValue *arguments = D_00474F48; \
        if ((s16)arguments[1].tag == 2) FIELD(entity, member, float) = arguments[1].payload.scalar; \
        else func_002CC938(message); \
    } \
    EMPTY_RESULT(); \
}
SET_SCALAR(func_00193070, 0x36C, D_0042CF38)
SET_SCALAR(func_00193638, 0x428, D_0042D300)

void func_001930F8(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 2) {
            float value = FIELD(entity, 0x36C, float) + arguments[1].payload.scalar;
            FIELD(entity, 0x36C, float) = value;
            if (0.0f <= value) value = george_ee_minimum(value, 1.0f);
            else value = 0.0f;
            FIELD(entity, 0x36C, float) = value;
        } else func_002CC938(D_0042CF98);
    }
    EMPTY_RESULT();
}
void func_001931B0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        u32 value = entity->field0C == 13;
        if (destination != -1) {
            SLOT(destination)->payload.bits = value;
            SLOT(destination)->tag = 1;
            SLOT(destination)->subtype = 0;
        }
    }
}
void func_00193218(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 1) {
            GeorgeActorBits64 flags = FIELD(entity, 0x190, GeorgeActorBits64);
            FIELD(entity, 0x190, GeorgeActorBits64) = arguments[1].payload.bits != 0 ? flags | 0x200000ULL : flags & ~0x200000ULL;
        } else func_002CC938(D_0042CFF8);
    }
}
void func_00193298(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 6) {
            u32 word = arguments[1].payload.bits;
            void *reference;
            const GeorgeGoalVirtualWord *pair;
            if (FIELD(entity, 0x298, void *) != 0) func_00191E50(entity);
            reference = func_00210078(word, 0, 0);
            FIELD(entity, 0x298, void *) = func_00236CB8(ADDRESS(entity, 0xB0), reference, 0, 0, 0);
            pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(reference, 0x20, const void *), 0x10);
            pair->invoke(ADDRESS(reference, pair->adjustment), 0);
        }
    }
}
void func_00193348(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) {
        GeorgeDeimosValue *arguments;
        func_00195260(entity);
        arguments = D_00474F48;
        func_001951C0(entity, arguments[1].payload.bits, arguments[2].payload.scalar);
    }
}
void func_001933A0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) func_00195260(entity);
}
void func_001933D0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0 && entity->field0C == 14) {
        GeorgeDeimosPoolNode *result = func_002D0790(FIELD(entity, 0x730, GeorgeScriptObject *));
        WORD_RESULT(result, 4);
    } else EMPTY_RESULT();
}
void func_00193470(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) WORD_RESULT(FIELD(entity->field18, 0xA8, u32), 6);
}
void func_001934D0(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 1) func_00178D10(entity, arguments[1].payload.bits);
        else func_002CC938(D_0042D0D8);
    }
    EMPTY_RESULT();
}
void func_00193558(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosValue *arguments;
        FIELD(entity, 0x190, GeorgeActorBits64) |= 0x40000000ULL;
        arguments = D_00474F48;
        func_00178E80(entity, arguments[1].payload.bits, arguments[2].payload.scalar);
    }
    EMPTY_RESULT();
}
void func_001935D8(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) func_00173648(entity);
}
void func_00193608(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc; (void)destination;
    if (entity != 0) func_00173720(entity);
}

#define SET_CALLBACK(name, member, message) \
void name(s32 argc, s32 destination) { \
    GeorgeGoalEntity *entity = ACTOR_LOOKUP(); \
    (void)argc; \
    if (entity != 0) { \
        if ((s16)D_00474F48[1].tag == 5) { \
            GeorgeDeimosPoolNode *old = FIELD(entity, member, GeorgeDeimosPoolNode *); \
            GeorgeDeimosPoolNode *replacement; \
            if (old != 0) { \
                func_002CD130(old); \
                FIELD(entity, member, GeorgeDeimosPoolNode *) = 0; \
            } \
            replacement = (GeorgeDeimosPoolNode *)D_00474F48[1].payload.pointer; \
            FIELD(entity, member, GeorgeDeimosPoolNode *) = replacement; \
            func_002CD0B8(replacement); \
        } else func_002CC938(message); \
    } \
    EMPTY_RESULT(); \
}
SET_CALLBACK(func_001936C0, 0x3AC, D_0042D330)
SET_CALLBACK(func_00193778, 0x3B0, D_0042D370)

void func_00193830(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 1) {
            if (arguments[1].payload.bits != 0) func_0018FD30(entity, 0x20ULL);
            else func_0018FD40(entity, 0x20ULL);
        } else func_002CC938(D_0042D3B8);
    }
    EMPTY_RESULT();
}
void func_001938D8(s32 argc, s32 destination)
{
    GeorgeGoalEntity *entity = ACTOR_LOOKUP();
    (void)argc;
    if (entity != 0) {
        GeorgeDeimosValue *arguments = D_00474F48;
        if ((s16)arguments[1].tag == 6) {
            u32 result = (u32)func_0014F438(FIELD(entity, 0x374, void *), arguments[1].payload.bits);
            if (destination != -1) {
                SLOT(destination)->payload.bits = result;
                SLOT(destination)->tag = 1;
                SLOT(destination)->subtype = 0;
            }
        } else func_002CC938(D_0042D3E8);
    }
}
