#include "george/actor_core.h"
#include "george/accessors.h"
#include "george/ee_math.h"

extern u8 D_003F83F0[];
extern void *D_003F2D40;
extern GeorgeActorPointerRange D_0046A0F0;
extern const u8 D_00421160[];
extern void *func_002AEE60(u32);
extern void *func_002481F0(void *);
extern s32 func_00100AA8(const void *, const void *);
extern void func_001007E0(GeorgeActorPointerRange *, void **, void *const *);
extern void func_002BD340(void);
extern s32 func_00396260(void (*)(void));
extern void *func_00251A88(void *, u32);
extern void func_002455C0(void *);
extern void func_00246A18(void *, float);
extern void func_00245D40(void *, float, float);
extern void *func_00161B80(u32);
extern void func_001A7118(u32);
extern void func_00190D80(GeorgeGoalEntity *, u32);
extern void func_0022D5D0(void *);
extern void func_00195480(GeorgeGoalEntity *);
extern void func_00195550(GeorgeGoalEntity *);
extern void func_00195620(GeorgeGoalEntity *);
extern void func_002D08E8(GeorgeGoalEntity *);
extern void func_002B8878(void *);
extern void func_002AF100(void *);
extern void *func_0022C1E0(void);
extern void func_00311680(void *, void *);
extern void func_00312C00(void *, void *);
extern void func_00317910(void *);
extern void func_002393F8(u32);
extern void func_0026F390(void *, u32);
extern void func_00237608(GeorgeGoalEntity *, GeorgeActorAttachmentCallback);
extern void func_002AB020(void *);
extern void *func_002AAF50(void *, s32);
extern void func_0021C088(void *, u32);
extern void func_001910C0(GeorgeGoalEntity *, void *);
extern void func_00191788(GeorgeGoalEntity *, void *);
extern void func_00194790(GeorgeGoalEntity *, void *);
extern void func_00194A40(GeorgeGoalEntity *, void *);
extern void func_00194D58(GeorgeGoalEntity *, void *);
extern void func_00195668(GeorgeGoalEntity *, void *);
extern void func_00196418(GeorgeGoalEntity *, void *);
extern void func_001968D0(GeorgeGoalEntity *, void *);
extern u32 func_001A6FE0(GeorgeGoalEntity *);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *, const GeorgeRotationMatrix *);
extern void func_002A1C08(void *, const void *);
extern void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);
extern float func_0029B940(float, float);
extern const u8 D_0042C330[];
extern const GeorgeMathVec4 D_004872E0;
extern GeorgeMathVec3 D_FLT_00469F00;
extern s32 func_0012AB70(void *, const GeorgeMathVec3 *);
extern s32 func_0013AA70(void *, const GeorgeMathVec3 *);
extern s32 func_00146AC8(void *, const GeorgeMathVec3 *, GeorgeMathVec3 *, float);
extern void func_0015F280(void *, float, float);
extern void func_00173E00(GeorgeGoalEntity *, const GeorgeMathVec3 *);
extern void *func_00175590(GeorgeGoalEntity *, u32);
extern void func_001767B8(GeorgeGoalEntity *);
extern void func_00182898(GeorgeGoalEntity *, float);
extern s32 func_0018FCF0(GeorgeGoalEntity *, GeorgeActorBits64);
extern s32 func_00191420(GeorgeGoalEntity *, u32, u32);
extern s32 func_00191A78(GeorgeGoalEntity *);
extern void func_00191AB8(GeorgeGoalEntity *, GeorgeRotationMatrix *);
extern void func_00191B00(GeorgeGoalEntity *, GeorgeRotationMatrix *);
extern s32 func_00192A18(void *);
extern float func_00192DA8(GeorgeGoalEntity *);
extern void func_001F1410(void *, const GeorgeRotationMatrix *, float);
extern void func_00215100(GeorgeMathVec3 *, const GeorgeMathVec3 *, const GeorgeMathVec3 *,
                        u32, u32, float, float, float, float);
extern void func_0021BF28(void *, const GeorgeMathVec3 *, float);
extern void func_0022D3F0(void *);
extern void func_00235CD8(void *, u32, u32);
extern u32 func_00236A10(const GeorgeRotationMatrix *, u32, u32, u32, u32, u32);
extern s32 func_00238D50(void *);
extern void func_00251DB0(void *, u32, const GeorgeMathVec3 *);
extern void func_0026FE30(void *, const GeorgeRotationMatrix *, u32, float, float);
extern s32 func_00271F70(void *);
extern void func_00271F90(void *, u32);
extern void func_00272BD8(void *);
extern u32 func_00272C10(void *);
extern void func_00272C40(void *, u32, u32, float, float, float);
extern void func_00276FF0(void *, GeorgeMathVec3 *);
extern u32 *func_002BEBA0(u32 *, const u8 *);
extern void func_003565B8(const GeorgeMathVec4 *, const GeorgeMathVec4 *, void *, float);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64, GeorgeActorBits64);
extern float func_003734F8(GeorgeActorBits64);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

/* Reuse the reviewed eight-byte member/pair representation. The caller
 * selects whether its final adjustment retains the captured state. */
ACTOR_INLINE void actor_member(GeorgeGoalEntity *entity, u32 state, u32 offset, u32 fresh_adjustment)
{
    GeorgeGoalMember *member = (GeorgeGoalMember *)ADDRESS(D_003F83F0, state * 28U + offset);
    s32 selector = member->selector, adjustment;
    void (*invoke)(void *);
    GeorgeGoalVirtualVoid pair;
    if (selector == 0) return;
    if (selector < 0) invoke = member->target.direct;
    else {
        const GeorgeGoalVirtualVoid *table = FIELD(entity, member->target.vtable_offset, const GeorgeGoalVirtualVoid *);
        pair = *(const GeorgeGoalVirtualVoid *)ADDRESS(table, (u32)selector * 8U - 8U);
        invoke = pair.invoke;
    }
    if (fresh_adjustment != 0) state = entity->field0C;
    adjustment = ((GeorgeGoalMember *)ADDRESS(D_003F83F0, state * 28U + offset))->adjustment;
    if (selector > 0) adjustment += pair.adjustment;
    invoke(ADJUST(entity, adjustment));
}

/* Complete registry block reused at each original occurrence. */
#define ENSURE_EFFECT() do { \
    if (D_003F2D40 == 0) { \
        GeorgeActorEffectRecord *record; \
        void *registered, *value; \
        void **begin, **end, **position; \
        D_003F2D40 = func_002481F0(func_002AEE60(0x1B4)); \
        record = (GeorgeActorEffectRecord *)func_002AEE60(12); \
        registered = D_003F2D40; begin = D_0046A0F0.field00; end = D_0046A0F0.field04; \
        record->field00 = 9; record->field04 = D_00421160; record->field08 = registered; \
        value = record; \
        position = func_00100C30(begin, end, &value, func_00100AA8); \
        if (D_0046A0F0.field04 != D_0046A0F0.field08 && position == D_0046A0F0.field04) { \
            if (position != 0) *position = value; \
            D_0046A0F0.field04 = (void **)ADDRESS(D_0046A0F0.field04, 4); \
        } else func_001007E0(&D_0046A0F0, position, &value); \
        func_00396260(func_002BD340); \
    } \
} while (0)

/* Each expansion reloads its own slot after all preceding callbacks. */
#define RELEASE_WORD(offset) do { \
    u32 value = FIELD(entity, offset, u32); \
    if (value != 0) { func_002393F8(value); FIELD(entity, offset, u32) = 0; } \
} while (0)
#define RELEASE_NODE(offset) do { \
    GeorgeDeimosPoolNode *node = FIELD(entity, offset, GeorgeDeimosPoolNode *); \
    if (node != 0) { func_002CD130(node); FIELD(entity, offset, u32) = 0; } \
} while (0)
#define RELEASE_OR_ATTACHMENT(offset, config, callback) do { \
    u32 value = FIELD(entity, offset, u32); \
    if (value != 0) { func_002393F8(value); FIELD(entity, offset, u32) = 0; } \
    else if (FIELD(entity->field18, config, u32) != 0) func_00237608(entity, callback); \
} while (0)

void func_0016FC98(GeorgeGoalEntity *entity)
{
    void *object;
    s32 index;
    u32 state;
    const GeorgeGoalVirtualWord *word_pair;
    const GeorgeGoalVirtualVoid *void_pair;
    FIELD(entity, 0x190, GeorgeActorBits64) |= 0x1000000000ULL;
    if (entity->field0C == 39 && func_001A7570(entity) == 0)
        func_001A7118(FIELD(func_00161B80(0), 0x34, u32));
    state = entity->field0C;
    actor_member(entity, state, 8, 0);
    if (FIELD(entity, 0x300, void *) != 0) func_00196980(entity);
    if (FIELD(entity, 0x304, void *) != 0) {
        void *manager = func_0022C1E0();
        func_00312C00(manager, FIELD(entity, 0x304, void *));
        func_00317910(FIELD(entity, 0x304, void *));
        object = FIELD(entity, 0x2FC, void *);
        FIELD(entity, 0x304, u32) = 0;
        if (object != 0) {
            word_pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0, const u8 *), 0x18);
            word_pair->invoke(ADJUST(object, word_pair->adjustment), 3);
        }
        FIELD(entity, 0x2FC, u32) = 0;
    }
    object = FIELD(entity, 0x384, void *);
    if (object != 0) func_0021C088(object, 3);
    FIELD(entity, 0x384, u32) = 0;
    func_00190D80(entity, 0);
    RELEASE_NODE(0x38C); RELEASE_NODE(0x390); RELEASE_NODE(0x394);
    RELEASE_NODE(0x398); RELEASE_NODE(0x39C); RELEASE_NODE(0x3A0);
    RELEASE_NODE(0x3A4); RELEASE_NODE(0x3A8); RELEASE_NODE(0x3AC); RELEASE_NODE(0x3B0);
    RELEASE_WORD(0x1C8); RELEASE_WORD(0x1CC);
    func_0022D5D0(FIELD(entity, 0x1D0, void *));
    object = FIELD(entity, 0x208, void *);
    FIELD(entity, 0x1D0, u32) = 0;
    if (object != 0) { func_002393F8((u32)object); FIELD(entity, 0x208, u32) = 0; }
    RELEASE_WORD(0x214); RELEASE_WORD(0x218); RELEASE_WORD(0x21C);
    RELEASE_WORD(0x220); RELEASE_WORD(0x224); RELEASE_WORD(0x228); RELEASE_WORD(0x234);
    RELEASE_OR_ATTACHMENT(0x288, 0x280, func_00194D58);
    RELEASE_OR_ATTACHMENT(0x238, 0xF8, func_001968D0);
    RELEASE_OR_ATTACHMENT(0x240, 0x114, func_00196418);
    RELEASE_OR_ATTACHMENT(0x23C, 0x140, func_00191788);
    RELEASE_OR_ATTACHMENT(0x264, 0xFC, func_001910C0);
    for (index = 0; index < FIELD(entity->field18, 0x288, s32); index = (s32)((u32)index + 1U)) {
        u32 value = FIELD(entity, 0x244U + ((u32)index << 2), u32);
        if (value != 0) { func_002393F8(value); FIELD(entity, 0x244U + ((u32)index << 2), u32) = 0; }
        else func_00237608((GeorgeGoalEntity *)ADDRESS(entity, 0x5D0U + ((u32)index << 4)), func_00194A40);
    }
    for (index = 0; index < FIELD(entity->field18, 0x2EC, s32); index = (s32)((u32)index + 1U)) {
        u32 value = FIELD(entity, 0x268U + ((u32)index << 2), u32);
        if (value != 0) { func_002393F8(value); FIELD(entity, 0x268U + ((u32)index << 2), u32) = 0; }
        else func_00237608((GeorgeGoalEntity *)ADDRESS(entity, 0x65CU + ((u32)index << 4)), func_00194790);
    }
    object = FIELD(entity, 0x294, void *);
    if (object != 0) {
        s32 count = (s32)func_002AAF88(object);
        for (index = 0; index < count; index = (s32)((u32)index + 1U)) {
            void *entry = func_002AAF50(FIELD(entity, 0x294, void *), index);
            object = FIELD(entry, 0, void *);
            if (object != 0) func_002393F8((u32)object);
        }
        func_002AB020(FIELD(entity, 0x294, void *));
    }
    func_00191E50(entity); func_00195480(entity); func_00195550(entity); func_00195620(entity);
    for (index = 0; index < FIELD(entity, 0x2C2, s16); index = (s32)((u32)index + 1U))
        func_00237608((GeorgeGoalEntity *)ADDRESS(entity, 0x2A8U + (u32)index * 12U), func_00195668);
    for (index = 0; index < FIELD(entity, 0x2C0, s16); index = (s32)((u32)index + 1U)) {
        u32 value = FIELD(entity, 0x2B4U + (u32)index * 12U, u32);
        func_002393F8(value);
        FIELD(entity, 0x2B4U + (u32)index * 12U, u32) = 0;
    }
    object = FIELD(entity, 0x1B0, void *);
    if (object != 0) { func_0026F390(object, 3); FIELD(entity, 0x1B0, u32) = 0; }
    object = FIELD(entity, 0x1B4, void *);
    if (object != 0) { func_0026F390(object, 3); FIELD(entity, 0x1B4, u32) = 0; }
    RELEASE_WORD(0x1A4);
    if (FIELD(entity, 0x1A0, void *) != 0) {
        void *manager = func_0022C1E0();
        func_00311680(manager, FIELD(entity, 0x1A0, void *));
        func_00317910(FIELD(entity, 0x1A0, void *));
        FIELD(entity, 0x1A0, u32) = 0;
    }
    object = FIELD(entity, 0x454, void *);
    if (object != 0) {
        word_pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0, const u8 *), 0x28);
        word_pair->invoke(ADJUST(object, word_pair->adjustment), 3);
        FIELD(entity, 0x454, u32) = 0;
    }
    object = FIELD(entity, 0x374, void *);
    if (object != 0) { func_002AF100(object); FIELD(entity, 0x374, u32) = 0; }
    object = FIELD(entity, 0x1F8, void *);
    if (object != 0) func_002B8878(object);
    object = FIELD(entity, 0x20, void *);
    FIELD(entity, 8, u32) = 0;
    FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x1000000000ULL;
    if (object != 0) {
        void_pair = (const GeorgeGoalVirtualVoid *)ADDRESS(FIELD(object, 0, const u8 *), 0x10);
        void_pair->invoke(ADJUST(object, void_pair->adjustment));
        object = FIELD(entity, 0x20, void *);
        if (object != 0) {
            word_pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0, const u8 *), 8);
            word_pair->invoke(ADJUST(object, word_pair->adjustment), 3);
        }
    }
    func_00195260(entity);
    func_002D08E8(entity);
}

void func_001703F0(GeorgeGoalEntity *entity)
{
    void *effect;
    if (FIELD(entity, 0x400, u32) == 0 || FIELD(entity, 0x404, void *) != 0) return;
    ENSURE_EFFECT();
    effect = func_00251A88(D_003F2D40, FIELD(entity, 0x400, u32));
    FIELD(entity, 0x404, void *) = effect;
    func_002455C0(effect);
    func_00246A18(FIELD(entity, 0x404, void *), 0.0f);
    func_00245D40(FIELD(entity, 0x404, void *), 1.0f, 1.0f);
}

/* Lookup-based attachments share the complete multiply/captured-copy sequence.
 * Only the original +214 path performs a second nonnull gate after callbacks. */
#define UPDATE_ATTACHMENT(offset, key, second_gate) do { \
    if (FIELD(entity, offset, void *) != 0) { \
        const GeorgeRotationMatrix *source = func_00192748(entity, key); \
        func_002A2200(&frame, source, input); \
        object = FIELD(entity, offset, void *); \
        if ((second_gate) == 0 || object != 0) { \
            func_002A1C08(ADDRESS(object, 0x10), &frame); \
            FIELD(object, 0xA0, u32) |= 0x100U; \
        } \
    } \
} while (0)
#define UPDATE_CACHED_ATTACHMENT(offset, cached, output) do { \
    if (FIELD(entity, offset, void *) != 0) { \
        func_002A2200(output, FIELD(entity, cached, const GeorgeRotationMatrix *), input); \
        object = FIELD(entity, offset, void *); \
        func_002A1C08(ADDRESS(object, 0x10), output); \
        FIELD(object, 0xA0, u32) |= 0x100U; \
    } \
} while (0)

void func_00171FB8(GeorgeGoalEntity *entity, GeorgeRotationMatrix *input)
{
    GeorgeMathVec3 position;
    GeorgeRotationMatrix frame, second_frame;
    void *object = FIELD(entity, 0x4E8, void *);
    s32 index;
    if (object != 0) {
        float x = input->element[12];
        float height = FIELD(entity, 0x4EC, float);
        float y = input->element[13] + height;
        float z = input->element[14];
        position.x = x; position.y = y; position.z = z;
        FIELD(object, 0x40, float) = x;
        FIELD(object, 0x44, float) = y;
        FIELD(object, 0x4C, float) = 1.0f;
        FIELD(object, 0x48, float) = z;
        FIELD(object, 0xA0, u32) |= 0x100U;
    }
    object = FIELD(entity, 0x1A4, void *);
    if (object != 0) {
        float z;
        FIELD(object, 0x40, float) = input->element[12];
        FIELD(object, 0x44, float) = input->element[13];
        z = input->element[14];
        FIELD(object, 0x4C, float) = 1.0f;
        FIELD(object, 0x48, float) = z;
        FIELD(object, 0xA0, u32) |= 0x100U;
    }
    if (FIELD(entity, 0x1C8, void *) != 0) {
        GeorgeGoalEntityData *data = entity->field18;
        position.x = input->element[12];
        position.y = input->element[13];
        position.z = input->element[14];
        if (FIELD(data, 4, u32) != 0) position.y = position.y + FIELD(data, 0xC, float);
        else position.y = position.y + (FIELD(data, 0xC, float) + FIELD(data, 8, float) * 0.5f);
        object = FIELD(entity, 0x1C8, void *);
        FIELD(object, 0x40, float) = position.x;
        FIELD(object, 0x44, float) = position.y;
        FIELD(object, 0x4C, float) = 1.0f;
        FIELD(object, 0x48, float) = position.z;
        FIELD(object, 0xA0, u32) |= 0x100U;
    }
    object = FIELD(entity, 0x1CC, void *);
    if (object != 0) {
        float factor = FIELD(entity->field18, 0x3F0, float);
        float z = FIELD(entity, 0x48, float) + input->element[10] * factor;
        float y = FIELD(entity, 0x44, float) + input->element[9] * factor;
        float x = FIELD(entity, 0x40, float) + input->element[8] * factor;
        position.z = z; position.y = y; position.x = x;
        FIELD(object, 0x40, float) = x;
        FIELD(object, 0x44, float) = position.y;
        FIELD(object, 0x4C, float) = 1.0f;
        FIELD(object, 0x48, float) = position.z;
        FIELD(object, 0xA0, u32) |= 0x100U;
    }
    FIELD(entity, 0x1BC, const GeorgeRotationMatrix *) = func_00192748(entity, 0x61);
    FIELD(entity, 0x1C0, const GeorgeRotationMatrix *) = func_00192748(entity, 0x62);
    UPDATE_ATTACHMENT(0x214, 0x23, 1);
    UPDATE_ATTACHMENT(0x218, 0x83, 0);
    UPDATE_ATTACHMENT(0x21C, 0x81, 0);
    UPDATE_ATTACHMENT(0x220, 0x82, 0);
    UPDATE_CACHED_ATTACHMENT(0x224, 0x1BC, &frame);
    UPDATE_CACHED_ATTACHMENT(0x228, 0x1C0, &frame);
    UPDATE_ATTACHMENT(0x29C, 0x86, 0);
    UPDATE_ATTACHMENT(0x2A4, 0x27, 0);
    for (index = 0; index < FIELD(entity, 0x2C0, s16); index = (s32)((u32)index + 1U)) {
        u32 offset = 0x2B4U + (u32)index * 12U;
        const GeorgeRotationMatrix *source = func_00192748(entity, FIELD(entity, offset + 8U, u32));
        func_002A2200(&frame, source, input);
        object = FIELD(entity, offset, void *);
        func_002A1C08(ADDRESS(object, 0x10), &frame);
        FIELD(object, 0xA0, u32) |= 0x100U;
    }
    UPDATE_CACHED_ATTACHMENT(0x28C, 0x1BC, &second_frame);
    UPDATE_CACHED_ATTACHMENT(0x290, 0x1C0, &second_frame);
    if (func_001A6FE0(entity) != 0) {
        const GeorgeRotationMatrix *source = func_00192748(entity, 0x62);
        float x, y, z;
        func_002A1C60(input, VECTOR(source, 0x30), &position);
        func_002A1C08(ADDRESS(entity, 0x4A0), input);
        x = position.x; y = position.y; z = position.z;
        FIELD(entity, 0x4DC, float) = 1.0f;
        FIELD(entity, 0x4D0, float) = x;
        FIELD(entity, 0x4D4, float) = y;
        FIELD(entity, 0x4D8, float) = z;
    }
}

s32 func_00172550(GeorgeGoalEntity *entity)
{
    s32 state = (s32)entity->field0C;
    u32 substate;
    if (state >= 6) {
        if (state != 12) return 0;
        func_00170538(entity);
        entity->field0C = 0;
        actor_member(entity, 0, 0, 1);
    } else if (state < 2 && state != 0) return 0;
    substate = FIELD(entity, 0x2D8, u32);
    if (substate != 0) {
        if (substate != 4) return 0;
        if (FIELD(entity, 0x334, float) < FIELD(entity->field18, 0x370, float)) return 0;
        if (FIELD(entity, 0x354, u32) == 0) return 0;
        func_001910C8(entity);
    }
    if (FIELD(entity, 0x264, void *) != 0) {
        void *record = FIELD(entity, 0x230, void *);
        FIELD(entity, 0x350, u32) = 1;
        FIELD(entity, 0x2D8, u32) = 4;
        FIELD(entity, 0x32C, u32) = 0x58;
        FIELD(entity, 0x330, u32) = 0;
        FIELD(entity, 0x2E8, u32) = 0;
        FIELD(entity, 0x354, u32) = 0;
        FIELD(entity, 0x334, u32) = 0;
        if (record != 0) FIELD(record, 0x30, float) = 1.0f;
        return 1;
    }
    return 0;
}

ACTOR_INLINE void core_control_scalar(GeorgeGoalEntity *entity, u32 offset, float value)
{
    void *object = FIELD(entity, 0x20, void *);
    const GeorgeGoalVirtualFloat *pair = (const GeorgeGoalVirtualFloat *)ADDRESS(FIELD(object, 0, const u8 *), offset);
    pair->invoke(ADJUST(object, pair->adjustment), value);
}
ACTOR_INLINE void core_control_void(GeorgeGoalEntity *entity, u32 offset)
{
    void *object = FIELD(entity, 0x20, void *);
    const GeorgeGoalVirtualVoid *pair = (const GeorgeGoalVirtualVoid *)ADDRESS(FIELD(object, 0, const u8 *), offset);
    pair->invoke(ADJUST(object, pair->adjustment));
}
ACTOR_INLINE s32 core_control_integer(GeorgeGoalEntity *entity, u32 offset)
{
    void *object = FIELD(entity, 0x20, void *);
    const GeorgeGoalVirtualInt *pair = (const GeorgeGoalVirtualInt *)ADDRESS(FIELD(object, 0, const u8 *), offset);
    return pair->invoke(ADJUST(object, pair->adjustment));
}
ACTOR_INLINE u32 core_contact_tag(void *object)
{
    const GeorgeGoalVirtualInt *pair = (const GeorgeGoalVirtualInt *)ADDRESS(FIELD(object, 4, const u8 *), 0x30);
    return (u32)pair->invoke(ADJUST(object, pair->adjustment));
}

/* Every atan occurrence reloads the actor's matrix pointer. The original
 * explicitly repeats the call when checking each boundary and final value. */
ACTOR_INLINE float core_matrix_angle(GeorgeGoalEntity *entity)
{
    const GeorgeRotationMatrix *matrix = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
    float angle = func_0029B940(matrix->element[10], matrix->element[8]);
    if (3.1415927410125732f < angle) {
        matrix = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        return func_0029B940(matrix->element[10], matrix->element[8]) - 6.2831854820251465f;
    }
    matrix = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
    angle = func_0029B940(matrix->element[10], matrix->element[8]);
    if (angle < -3.1415927410125732f) {
        matrix = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        return func_0029B940(matrix->element[10], matrix->element[8]) + 6.2831854820251465f;
    }
    matrix = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
    return func_0029B940(matrix->element[10], matrix->element[8]);
}

void func_00170600(GeorgeGoalEntity *entity, float elapsed)
{
    GeorgeActorCoreScratch scratch;
    GeorgeRotationMatrix *matrix;
    GeorgeMathVec3 *vector0 = VECTOR(&scratch, 0);
    GeorgeMathVec3 *vector10 = VECTOR(&scratch, 0x10);
    GeorgeMathVec3 *vector20 = VECTOR(&scratch, 0x20);
    GeorgeMathVec3 *vector30 = VECTOR(&scratch, 0x30);
    GeorgeMathVec3 *vector40 = VECTOR(&scratch, 0x40);
    GeorgeActorBits64 bits;
    void *object;
    u32 index, key, command;
    float value = FIELD(entity, 0x4FC, float);
    if (0.0f < value) FIELD(entity, 0x4FC, float) = value - elapsed;
    bits = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((bits & 0x100000000ULL) != 0) {
        if ((bits & 0x04000000ULL) == 0) return;
        if (func_00271F70(FIELD(entity, 0x1B0, void *)) == 0) return;
        func_00272BD8(FIELD(entity, 0x1B0, void *));
        func_0021BF28(FIELD(entity, 0x384, void *), VECTOR(entity, 0x40), 1.0f);
        func_0026FE30(FIELD(entity, 0x1B0, void *), FRAME(entity, 0xF0), 0, elapsed, 0.0f);
        if (FIELD(entity, 0x1B4, void *) != 0) {
            const GeorgeRotationMatrix *source = func_00192748(entity, 0x86);
            FIELD(entity, 0x1C4, const GeorgeRotationMatrix *) = source;
            func_002A2200(FRAME(entity, 0x130), source, FRAME(entity, 0xF0));
            func_00272BD8(FIELD(entity, 0x1B4, void *));
            func_0026FE30(FIELD(entity, 0x1B4, void *), FRAME(entity, 0x130), 0,
                          FIELD(entity, 0x35C, float), 0.0f);
            if (FIELD(entity, 0x2A0, void *) != 0) {
                source = func_00192818(entity, 0x29);
                func_002A2200(FRAME(&scratch, 0), source, FRAME(entity, 0x130));
                object = FIELD(entity, 0x2A0, void *);
                func_002A1C08(ADDRESS(object, 0x10), FRAME(&scratch, 0));
                FIELD(object, 0xA0, u32) |= 0x100U;
            }
        }
        func_00171FB8(entity, FRAME(entity, 0xF0));
        FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x04000000ULL;
        return;
    }
    FIELD(entity, 0x1FC, u32) = 0;
    FIELD(entity, 0x35C, float) = elapsed;
    if ((bits & 0x800000000000ULL) != 0) {
        func_00170538(entity);
        entity->field0C = 32;
        actor_member(entity, 32, 0, 1);
        FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x800000000000ULL;
        if (FIELD(entity, 0x180, signed char) == 1 && FIELD(entity->field18, 0x194, u32) != 0)
            func_00235CD8(FIELD(entity, 0x1CC, void *), 0x9F79558FU, 0);
    }
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x2000000000000ULL) != 0) {
        func_00170538(entity);
        entity->field0C = 28;
        actor_member(entity, 28, 0, 1);
        FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x2000000000000ULL;
    }
    if (entity->field0C != 4 && entity->field0C != 33) {
        FIELD(entity, 0x42C, u32) = 0;
        FIELD(entity, 0x434, u32) = 0;
        FIELD(entity, 0x430, u32) = 0;
    }
    bits = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((bits & 0x40000000000ULL) != 0) {
        GeorgeGoalEntityData *data = entity->field18;
        float height;
        FIELD(entity, 0x190, GeorgeActorBits64) = bits & ~0x40000000000ULL;
        height = FIELD(data, 0x58, float) - 9.800000190734863f;
        if (func_00146AC8(FIELD(entity, 0x420, void *), VECTOR(entity, 0x414), vector0, height) != 0)
            func_00173E00(entity, vector0);
    }
    value = FIELD(entity, 0x1D4, float);
    if (value != 0.0f && 0.0f < FIELD(entity, 0x36C, float)) {
        float timer = FIELD(entity, 0x1DC, float) - elapsed;
        FIELD(entity, 0x1DC, float) = timer;
        if (timer < 0.0f) {
            GeorgeGoalEntityData *data;
            FIELD(entity, 0x1DC, float) = timer + FIELD(entity, 0x1D8, float);
            data = entity->field18;
            func_00192078(entity, 0x728F0147U, value * FIELD(data, 0x198, float));
            object = FIELD(entity, 0x378, void *);
            if (object != 0) {
                data = entity->field18;
                func_0015F280(object, FIELD(data, 0x1CC, float), FIELD(data, 0x1D0, float));
            }
            if (FIELD(entity->field18, 0x154, u32) != 0) func_00195850(entity, 0x50, 0xF114E180U, 0);
            else func_00191420(entity, 0x50, 0);
        }
    }
    if (FIELD(entity, 0x36C, float) <= 0.0f && entity->field0C != 13)
        func_00192180(entity, FIELD(entity, 0x3C0, u32));
    if (FIELD(entity, 0x208, void *) != 0) {
        const GeorgeRotationMatrix *source = func_00192748(entity, 0x89);
        if (source != 0) {
            func_002A2200(FRAME(&scratch, 0), source, FRAME(entity, 0xB0));
            object = FIELD(entity, 0x208, void *);
            func_002A1C08(ADDRESS(object, 0x10), FRAME(&scratch, 0));
            FIELD(object, 0xA0, u32) |= 0x100U;
            if (FIELD(entity, 0x20C, u32) != 0) {
                float timer = FIELD(entity, 0x210, float) + elapsed;
                FIELD(entity, 0x210, float) = timer;
                if (2.0f <= timer && entity->field0C == 0) {
                    func_00235CD8(FIELD(entity, 0x208, void *), 0x9F79558FU, 0);
                    FIELD(entity, 0x210, u32) = 0;
                }
            } else if (func_00238D50(FIELD(entity, 0x208, void *)) != 0) {
                func_002393F8(FIELD(entity, 0x208, u32));
                FIELD(entity, 0x208, u32) = 0;
                FIELD(entity, 0x210, u32) = 0;
            }
        }
    }
    bits = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((bits & 0x80ULL) != 0) {
        float timer = FIELD(entity, 0x380, float) + elapsed;
        FIELD(entity, 0x380, float) = timer;
        if (3.0f < timer) {
            FIELD(entity, 0x380, u32) = 0;
            FIELD(entity, 0x190, GeorgeActorBits64) = bits & ~0x80ULL;
        }
    }
    for (index = 0; index < FIELD(entity, 0x4F4, u32); ++index) {
        u8 *entries = FIELD(entity, 0x4F8, u8 *);
        object = FIELD(entries, index * 8U, void *);
        if (object != 0) {
            u32 tag = core_contact_tag(object);
            u32 active = 0;
            float change = 0.0f;
            if (tag == 0xCA152581U) {
                entries = FIELD(entity, 0x4F8, u8 *);
                object = FIELD(entries, index * 8U, void *);
                if (func_0013AA70(object, VECTOR(entity, 0x40)) != 0) {
                    change = elapsed * func_0013B308(object);
                    active = 1;
                }
            } else {
                entries = FIELD(entity, 0x4F8, u8 *);
                object = FIELD(entries, index * 8U, void *);
                if (core_contact_tag(object) == 0x7FB6A6E3U) {
                    entries = FIELD(entity, 0x4F8, u8 *);
                    object = FIELD(entries, index * 8U, void *);
                    if (func_0012AB70(object, VECTOR(entity, 0x40)) != 0) {
                        change = elapsed * FIELD(object, 0x94, float);
                        active = 1;
                    }
                }
            }
            if (active != 0) {
                entries = FIELD(entity, 0x4F8, u8 *);
                func_00191E88(entity, FIELD(entries, index * 8U + 4U, u32) == 0, change);
                entries = FIELD(entity, 0x4F8, u8 *);
                FIELD(entries, index * 8U + 4U, u32) = 1;
            }
        }
    }
    core_control_scalar(entity, 0xC0, elapsed);
    core_control_scalar(entity, 0xE0, elapsed);
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x80000ULL) != 0) {
        const GeorgeRotationMatrix *source;
        float x, y, z, factor, bx, by, bz, angle;
        core_control_void(entity, 0x88);
        source = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        FIELD(entity, 0x40, float) = source->element[12];
        FIELD(entity, 0x44, float) = source->element[13];
        FIELD(entity, 0x48, float) = source->element[14];
        func_002A1C08(FRAME(entity, 0xB0), FIELD(entity, 0x484, const GeorgeRotationMatrix *));
        source = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        factor = FIELD(entity, 0x494, float);
        bx = factor * source->element[0]; bz = factor * source->element[2]; by = factor * source->element[1];
        x = FIELD(entity, 0x40, float) + bx;
        y = FIELD(entity, 0x44, float) + by;
        z = FIELD(entity, 0x48, float) + bz;
        FIELD(entity, 0x40, float) = x; FIELD(entity, 0x44, float) = y; FIELD(entity, 0x48, float) = z;
        source = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        factor = FIELD(entity, 0x498, float);
        bx = factor * source->element[4]; bz = factor * source->element[6]; by = factor * source->element[5];
        x = FIELD(entity, 0x40, float) + bx; z = z + bz; y = y + by;
        FIELD(entity, 0x40, float) = x; FIELD(entity, 0x48, float) = z; FIELD(entity, 0x44, float) = y;
        source = FIELD(entity, 0x484, const GeorgeRotationMatrix *);
        factor = FIELD(entity, 0x49C, float);
        bx = factor * source->element[8]; bz = factor * source->element[10]; by = factor * source->element[9];
        x = FIELD(entity, 0x40, float) + bx; z = z + bz; y = y + by;
        FIELD(entity, 0x40, float) = x; FIELD(entity, 0x48, float) = z; FIELD(entity, 0x44, float) = y;
        angle = core_matrix_angle(entity);
        bits = FIELD(entity, 0x190, GeorgeActorBits64);
        FIELD(entity, 0x60, float) = angle;
        if ((bits & 0x20000000000ULL) != 0) {
            FIELD(entity, 0x7C, float) = angle; FIELD(entity, 0x58, float) = angle; FIELD(entity, 0x64, float) = angle;
        }
        if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x10000000000ULL) != 0) {
            x = FIELD(entity, 0x40, float) - FIELD(entity, 0x488, float);
            y = FIELD(entity, 0x44, float) - FIELD(entity, 0x48C, float);
            z = FIELD(entity, 0x48, float) - FIELD(entity, 0x490, float);
            FIELD(entity, 0x4C, float) = x; FIELD(entity, 0x50, float) = y; FIELD(entity, 0x54, float) = z;
            if (0.0f < elapsed) {
                float reciprocal;
                FIELD(entity, 0x488, float) = FIELD(entity, 0x40, float);
                reciprocal = 1.0f / elapsed;
                FIELD(entity, 0x48C, float) = FIELD(entity, 0x44, float);
                FIELD(entity, 0x490, float) = FIELD(entity, 0x48, float);
                FIELD(entity, 0x4C, float) = FIELD(entity, 0x4C, float) * reciprocal;
                y = FIELD(entity, 0x50, float) * reciprocal; z = FIELD(entity, 0x54, float) * reciprocal;
                FIELD(entity, 0x50, float) = y; FIELD(entity, 0x54, float) = z;
            }
            func_00177E48(entity, VECTOR(entity, 0x4C));
        }
    }
    command = entity->field0C;
    FIELD(entity, 0x14, s32) = -1;
    actor_member(entity, command, 0x10, 1);
    if (FIELD(entity, 0x14, s32) >= 0 && FIELD(entity, 0x14, u32) != entity->field0C) {
        func_00170538(entity);
        command = FIELD(entity, 0x14, u32);
        entity->field0C = command;
        actor_member(entity, command, 0, 1);
    }
    if ((FIELD(entity, 0x190, GeorgeActorBits64) & 0x08000000ULL) != 0) {
        if (entity->field0C != 31) {
            func_00170538(entity); entity->field0C = 9; actor_member(entity, 9, 0, 1);
        } else func_00195850(entity, 0x54, 0xB71B6C44U, 0);
        if (FIELD(entity->field18, 0x150, u32) != 0 && FIELD(entity, 0x2D8, u32) == 7)
            FIELD(entity, 0x5C4, u32) = 201;
        FIELD(entity, 0x190, GeorgeActorBits64) &= ~0x08000000ULL;
    }
    func_00176E10(entity);
    core_control_scalar(entity, 0xC8, elapsed);
    if (FIELD(entity, 0x1A0, void *) != 0) {
        object = FIELD(entity, 0x1B0, void *);
        if (object != 0 && FIELD(object, 0x43C, void *) != 0)
            func_00276FF0(FIELD(object, 0x43C, void *), vector0);
        else {
            vector0->z = FIELD(entity, 0x48, float); vector0->x = FIELD(entity, 0x40, float); vector0->y = FIELD(entity, 0x44, float);
            value = func_00192DA8(entity);
            vector0->y = vector0->y + value;
        }
        value = 1.0f / george_ee_maximum(elapsed, 0.01666666753590107f);
        vector20->x = vector0->x; vector20->y = vector0->y; vector20->z = vector0->z;
        FIELD(&scratch, 0x2C, u32) = 0;
        *(GeorgeMathVec4 *)vector10 = *(GeorgeMathVec4 *)vector20;
        func_003565B8((const GeorgeMathVec4 *)vector10, &D_004872E0, FIELD(entity, 0x1A0, void *), value);
    }
    bits = FIELD(entity, 0x190, GeorgeActorBits64);
    if ((bits & 0x01000000ULL) == 0) {
        if ((bits & 0x44ULL) == 0x44ULL || func_00191A78(entity) != 0) {
            s32 result = core_control_integer(entity, 0x18);
            FIELD(entity, 0x410, void *) = func_00175590(entity, result == 0);
        } else FIELD(entity, 0x80, float) = FIELD(entity, 0x44, float);
    }
    func_001767B8(entity);
    object = FIELD(entity, 0x1D0, void *);
    command = FIELD(object, 0x10, u32);
    FIELD(object, 0, float) = FIELD(entity, 0x40, float);
    FIELD(object, 4, float) = FIELD(entity, 0x44, float);
    FIELD(object, 8, float) = FIELD(entity, 0x48, float);
    if (command != 0) func_0022D3F0(object);
    if (FIELD(entity->field18, 0x41C, u32) != 0 && func_0018FCF0(entity, 2ULL) == 0)
        func_00182898(entity, elapsed);
    {
        float x = FIELD(entity, 0xE0, float) - D_FLT_00469F00.x;
        float y = FIELD(entity, 0xE4, float) - D_FLT_00469F00.y;
        float z = FIELD(entity, 0xE8, float) - D_FLT_00469F00.z;
        float square = (x * x + y * y) + z * z;
        vector0->x = x; vector0->y = y; vector0->z = z;
        if (entity->field0C == 14) command = square < 900.0f ? 1 : 2;
        else if (square < 289.0f) command = 0;
        else if (square < 900.0f) command = 1;
        else if (square < 2025.0f) command = 2;
        else { command = 3; if (func_00192A18(entity) != 0) command = 2; }
        object = FIELD(entity, 0x1B0, void *);
        if (object != 0) func_00271F90(object, command);
        object = FIELD(entity, 0x1B4, void *);
        if (object != 0) func_00271F90(object, command);
    }
    {
        float z = FIELD(entity, 0x54, float) - FIELD(entity, 0x9C, float);
        float x = FIELD(entity, 0x4C, float) - FIELD(entity, 0x94, float);
        float y = FIELD(entity, 0x50, float) - FIELD(entity, 0x98, float);
        vector0->z = z; vector0->x = x; vector0->y = y;
    }
    object = FIELD(entity, 0x1B0, void *);
    if (object != 0) {
        float length;
        func_00272C40(object, 0, 0, 0.0f, 0.0f, 0.0f);
        length = george_ee_square_root((vector0->x * vector0->x + vector0->y * vector0->y) + vector0->z * vector0->z);
        command = (u32)func_0018FCF0(entity, 2ULL);
        func_0026FE30(FIELD(entity, 0x1B0, void *), FRAME(entity, 0xF0), command, elapsed, length);
        if (FIELD(entity->field18, 0x90, u32) != 0 &&
            (func_00272C10(FIELD(entity, 0x1B0, void *)) & 0xC0000000U) != 0) {
            if (FIELD(entity, 0x3CC, u32) != 0) {
                ENSURE_EFFECT();
                vector10->x = FIELD(entity, 0x40, float); vector10->y = FIELD(entity, 0x44, float); vector10->z = FIELD(entity, 0x48, float);
                func_00251DB0(D_003F2D40, FIELD(entity, 0x3CC, u32), vector10);
            } else {
                void *effect;
                ENSURE_EFFECT();
                effect = D_003F2D40;
                func_002BEBA0(&key, D_0042C330);
                vector10->x = FIELD(entity, 0x40, float); vector10->y = FIELD(entity, 0x44, float); vector10->z = FIELD(entity, 0x48, float);
                func_00251DB0(effect, key, vector10);
            }
        }
        if (FIELD(entity, 0x3E8, GeorgeActorBits64) != 0 && FIELD(entity, 0x3F0, s32) >= 0 && FIELD(entity, 0x3F4, s32) >= 0) {
            const GeorgeRotationMatrix *source = 0;
            u32 status = func_00272C10(FIELD(entity, 0x1B0, void *));
            if ((s32)status < 0) {
                object = FIELD(entity, 0x1B0, void *);
                index = FIELD(entity, 0x3F0, u32);
                source = FRAME(FIELD(object, 0x3D8, void *), index << 6);
            } else if ((func_00272C10(FIELD(entity, 0x1B0, void *)) & 0x40000000U) != 0) {
                object = FIELD(entity, 0x1B0, void *);
                index = FIELD(entity, 0x3F4, u32);
                source = FRAME(FIELD(object, 0x3D8, void *), index << 6);
            }
            if (source != 0) {
                float projection, x, y, z, nx, ny, nz, distance;
                u32 emit;
                vector30->x = FIELD(entity, 0xE0, float); vector30->y = FIELD(entity, 0xE4, float); vector30->z = FIELD(entity, 0xE8, float);
                vector30->y = func_00191A78(entity) != 0 ? FIELD(entity, 0x84, float) : FIELD(entity, 0x80, float);
                projection = (FIELD(entity, 0x88, float) * vector30->x + FIELD(entity, 0x8C, float) * vector30->y) + FIELD(entity, 0x90, float) * vector30->z;
                func_002A1C60(FRAME(entity, 0xB0), VECTOR(source, 0x30), vector10);
                emit = FIELD(entity, 0x3E8, u32);
                vector40->x = FIELD(entity, 0xD0, float); vector40->y = FIELD(entity, 0xD4, float); vector40->z = FIELD(entity, 0xD8, float);
                x = vector10->x + vector40->x * 0.05000000074505806f;
                y = vector10->y + vector40->y * 0.05000000074505806f;
                z = vector10->z + vector40->z * 0.05000000074505806f;
                nx = FIELD(entity, 0x88, float); ny = FIELD(entity, 0x8C, float); nz = FIELD(entity, 0x90, float);
                vector10->x = x; vector10->y = y; vector10->z = z;
                distance = ((x * nx + y * ny) + z * nz) - projection;
                vector20->z = z - distance * nz; vector20->x = x - distance * nx; vector20->y = y - distance * ny;
                if (emit != 0) func_00215100(vector20, VECTOR(entity, 0x88), vector40, emit, 0, 0.3499999940395355f, 0.0f, 0.0f, 0.0f);
                emit = FIELD(entity, 0x3EC, u32);
                if (emit != 0) {
                    matrix = FRAME(&scratch, 0x50);
                    matrix->element[8] = 0.0f; matrix->element[9] = 1.0f; matrix->element[10] = 0.0f; matrix->element[11] = 0.0f;
                    matrix->element[0] = 0.0f; matrix->element[1] = 0.0f; matrix->element[2] = 1.0f; matrix->element[3] = 0.0f;
                    matrix->element[4] = 1.0f; matrix->element[5] = 0.0f; matrix->element[6] = 0.0f; matrix->element[7] = 0.0f;
                    matrix->element[12] = 0.0f; matrix->element[13] = 0.0f; matrix->element[14] = 0.0f; matrix->element[15] = 1.0f;
                    matrix->element[12] = vector20->x; matrix->element[13] = vector20->y; matrix->element[14] = vector20->z;
                    func_002393F8(func_00236A10(matrix, emit, 0, 0, 0, 0));
                }
            }
        }
    }
    if (FIELD(entity, 0x2D0, void *) != 0) {
        const GeorgeRotationMatrix *source = func_00192748(entity, 0x84);
        func_002A2200(FRAME(&scratch, 0x10), source, FRAME(entity, 0xF0));
        func_001F1410(FIELD(entity, 0x2D0, void *), FRAME(&scratch, 0x10), elapsed);
    }
    if (FIELD(entity, 0x2D4, void *) != 0) {
        const GeorgeRotationMatrix *source = func_00192748(entity, 0x85);
        func_002A2200(FRAME(&scratch, 0x10), source, FRAME(entity, 0xF0));
        func_001F1410(FIELD(entity, 0x2D4, void *), FRAME(&scratch, 0x10), elapsed);
    }
    func_00171FB8(entity, FRAME(entity, 0xF0));
    func_00191000(entity, elapsed);
    if (FIELD(entity, 0x1B0, void *) != 0) {
        if (FIELD(entity, 0x1B4, void *) != 0) {
            const GeorgeRotationMatrix *source = func_00192748(entity, 0x86);
            float length;
            FIELD(entity, 0x1C4, const GeorgeRotationMatrix *) = source;
            func_002A2200(FRAME(entity, 0x130), source, FRAME(entity, 0xF0));
            length = george_ee_square_root((vector0->x * vector0->x + vector0->y * vector0->y) + vector0->z * vector0->z);
            command = (u32)func_0018FCF0(entity, 2ULL);
            func_0026FE30(FIELD(entity, 0x1B4, void *), FRAME(entity, 0x130), command, elapsed, length);
            if (FIELD(entity, 0x2A0, void *) != 0) {
                source = func_00192818(entity, 0x29);
                func_002A2200(FRAME(&scratch, 0x10), source, FRAME(entity, 0x130));
                object = FIELD(entity, 0x2A0, void *);
                func_002A1C08(ADDRESS(object, 0x10), FRAME(&scratch, 0x10));
                FIELD(object, 0xA0, u32) |= 0x100U;
            }
        }
        object = FIELD(entity, 0x1B0, void *);
        if (object != 0 && FIELD(object, 0x43C, void *) != 0) {
            func_00276FF0(FIELD(object, 0x43C, void *), vector10);
            goto final_position;
        }
    }
    if (func_00191A78(entity) != 0) {
        func_00191B00(entity, FRAME(&scratch, 0x20));
        func_00191AB8(entity, FRAME(&scratch, 0x90));
        vector10->x = FIELD(&scratch, 0xC0, float); vector10->y = FIELD(&scratch, 0xC4, float); vector10->z = FIELD(&scratch, 0xC8, float);
    } else {
        vector10->x = FIELD(entity, 0x40, float); vector10->y = FIELD(entity, 0x44, float); vector10->z = FIELD(entity, 0x48, float);
    }
final_position:
    func_0021BF28(FIELD(entity, 0x384, void *), vector10, elapsed);
    {
        GeorgeGoalEntityData *data = entity->field18;
        bits = FIELD(entity, 0x190, GeorgeActorBits64);
        if (FIELD(data, 0x1D8, u32) == 0x45A78000U) {
            if ((bits & 0x02000000ULL) != 0) {
                float rate = FIELD(data, 0x38, float) * elapsed;
                float target, current, difference, angle;
                s32 negative;
                GeorgeActorBits64 magnitude, wrapped;
                if ((bits & 0x80000000000ULL) != 0) {
                    magnitude = func_00374848(FIELD(entity, 0x64, float) - FIELD(entity, 0x7C, float));
                    negative = func_00373250(magnitude, 0ULL);
                    target = FIELD(entity, 0x64, float);
                } else {
                    magnitude = func_00374848(FIELD(entity, 0x58, float) - FIELD(entity, 0x7C, float));
                    negative = func_00373250(magnitude, 0ULL);
                    target = FIELD(entity, 0x58, float);
                }
                current = FIELD(entity, 0x7C, float);
                if (negative < 0) magnitude = func_00372CC0(0ULL, magnitude);
                command = (u32)func_00373250(magnitude, 0x400921FB60000000ULL);
                difference = target - current;
                if ((s32)command > 0) {
                    magnitude = func_00374848(difference);
                    if (func_00373250(magnitude, 0ULL) < 0) magnitude = func_00372CC0(0ULL, magnitude);
                    wrapped = func_00372CC0(magnitude, 0x401921FB60000000ULL);
                    if (difference < 0.0f) wrapped = func_00372D28(wrapped, 0xBFF0000000000000ULL);
                    difference = func_003734F8(wrapped);
                }
                magnitude = func_00374848(difference);
                if (func_00373250(magnitude, 0ULL) < 0) magnitude = func_00372CC0(0ULL, magnitude);
                wrapped = func_00374848(rate);
                if (func_00373250(magnitude, wrapped) < 0) {
                    magnitude = func_00374848(difference);
                    if (func_00373250(magnitude, 0ULL) < 0) magnitude = func_00372CC0(0ULL, magnitude);
                    rate = func_003734F8(magnitude);
                }
                angle = difference <= 0.0f ? current - rate : current + rate;
                FIELD(entity, 0x7C, float) = angle;
                if (3.1415927410125732f < angle) angle = angle - 6.2831854820251465f;
                else if (!(-3.1415927410125732f <= angle)) angle = angle + 6.2831854820251465f;
                FIELD(entity, 0x7C, float) = angle;
            } else if ((bits & 0x80000000000ULL) != 0) FIELD(entity, 0x7C, float) = FIELD(entity, 0x64, float);
            else FIELD(entity, 0x7C, float) = FIELD(entity, 0x58, float);
        } else {
            float current = FIELD(entity, 0x78, float);
            if ((bits & 0x02000000ULL) != 0) {
                float difference = FIELD(entity, 0x74, float) - current;
                value = elapsed * 5.0f;
                FIELD(entity, 0x78, float) = current + difference * value;
            } else {
                value = elapsed * 5.0f;
                FIELD(entity, 0x78, float) = current - current * value;
            }
        }
    }
    func_001703F0(entity);
}
