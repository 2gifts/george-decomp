#include "george/actor_construction.h"
#include "george/script_object.h"
#include "george/vector_math.h"
#include "george/ee_math.h"

extern const u8 D_0042BF90[], D_0042C018[], D_0042E7A8[];
extern void *func_0022C1E0(void);
extern void func_00316040(void *, void *);
extern void func_002AF100(void *);
extern void *func_002AEF08(u32);
extern void *func_002AEE60(u32);
extern void *func_002AAF98(u32);
extern void *func_0021B848(void *);
extern void *func_003936A0(void *, s32, u32);
extern void func_001767B8(GeorgeGoalEntity *);
extern void func_00176A20(GeorgeGoalEntity *, u32, u32);
extern void *func_001A1988(GeorgeGoalEntity *);
extern void *func_001A2278(GeorgeGoalEntity *, u32);
extern void *func_001A6158(GeorgeGoalEntity *, u32);
extern u32 func_00236A10(const GeorgeRotationMatrix *, u32, u32, u32, u32, u32);
extern void *func_00238BA0(void *, u32);
extern void func_0022D838(void *);
extern void func_0022D788(void *);
extern void func_00235CD8(void *, u32, u32);
extern void *func_00210078(u32, u32, u32);
extern void *func_00236CB8(const void *, void *, u32, u32, u32);
extern void func_002A2200(GeorgeRotationMatrix *, const GeorgeRotationMatrix *, const GeorgeRotationMatrix *);
extern void func_002A1C08(void *, const void *);
extern u32 func_00297640(u32);
extern u32 func_002A7418(u32);
extern s32 func_00397178(void);
extern void func_001910C0(GeorgeGoalEntity *, void *);
extern void func_00194790(GeorgeGoalEntity *, void *);
extern void func_00194A40(GeorgeGoalEntity *, void *);
extern void func_00194D58(GeorgeGoalEntity *, void *);
extern void func_00196418(GeorgeGoalEntity *, void *);
extern void func_001968D0(GeorgeGoalEntity *, void *);
extern void *func_00239E40(u32, void *, u32, GeorgeActorAttachmentCallback, void *, u32, u32);
extern void func_0017ADA8(void *, GeorgeGoalEntity *);
extern void func_00194028(void *, GeorgeGoalEntity *);
extern void *func_0022D4B0(const GeorgeMathVec3 *, void (*)(void *, GeorgeGoalEntity *),
                         void (*)(void *, GeorgeGoalEntity *), GeorgeGoalEntity *,
                         u32, u32, u32, float);
extern void func_0022D318(void *);
extern s32 func_001F6350(void *);
extern void func_001F6138(void *);
extern void func_002D0858(GeorgeScriptObject *);
extern void func_002A1F18(GeorgeRotationMatrix *, const GeorgeMathVec4 *, const GeorgeMathVec3 *);
extern void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);
extern s32 func_0022BB70(const GeorgeMathVec3 *, const GeorgeMathVec3 *, u32,
                        float *, GeorgeMathVec3 *, u32 *);
extern void *func_0022C800(u32);
extern s32 func_0029F080(const float *, const void *, float *);
extern float func_0029B940(float, float);
extern float func_0029C168(float);
extern float func_0029C090(float);
extern void func_002ADC80(u32, const void *, u32, float *, float);
extern void func_00307850(void *);
extern GeorgeActorBits64 func_00374848(float);
extern s32 func_00373250(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372CC0(GeorgeActorBits64, GeorgeActorBits64);
extern GeorgeActorBits64 func_00372D28(GeorgeActorBits64, GeorgeActorBits64);
extern float func_003734F8(GeorgeActorBits64);
extern void *D_003F8A68;

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define ADJUST(object, amount) ((void *)ADDRESS(object, (s32)(amount)))
#define FRAME(object, offset) ((GeorgeRotationMatrix *)ADDRESS(object, offset))
#define DATA(entity) FIELD(entity, 0x18, void *)

#if defined(__GNUC__) && __GNUC__ >= 3
#define ACTOR_INLINE static __inline__ __attribute__((always_inline))
#else
#define ACTOR_INLINE static __inline__
#endif

typedef struct ActorConstructionVirtualIndex {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, GeorgeGoalEntity *, signed char);
} ActorConstructionVirtualIndex;

typedef struct ActorConstructionVirtualVectorScalar {
    s16 adjustment;
    u16 unknown02;
    void (*invoke)(void *, const GeorgeMathVec4 *, float);
} ActorConstructionVirtualVectorScalar;

#define CONTEXT(control) FIELD(control, 0x1C, void *)
#define VECTOR(object, offset) ((GeorgeMathVec3 *)ADDRESS(object, offset))

ACTOR_INLINE GeorgeMathVec3 construction_read_vector(const void *object, u32 offset)
{
    GeorgeMathVec3 result;
    result.x = FIELD(object, offset, float);
    result.y = FIELD(object, offset + 4U, float);
    result.z = FIELD(object, offset + 8U, float);
    return result;
}

ACTOR_INLINE GeorgeMathVec3 construction_scale(GeorgeMathVec3 value, float amount)
{
    value.x = value.x * amount;
    value.y = value.y * amount;
    value.z = value.z * amount;
    return value;
}

ACTOR_INLINE GeorgeMathVec3 construction_cross(GeorgeMathVec3 first, GeorgeMathVec3 second)
{
    GeorgeMathVec3 result;
    result.x = first.y * second.z - first.z * second.y;
    result.y = first.z * second.x - first.x * second.z;
    result.z = first.x * second.y - first.y * second.x;
    return result;
}

ACTOR_INLINE float construction_dot(GeorgeMathVec3 first, GeorgeMathVec3 second)
{
    return (first.x * second.x + first.y * second.y) + first.z * second.z;
}

ACTOR_INLINE void construction_add_vector(void *object, u32 offset, GeorgeMathVec3 value)
{
    float x = FIELD(object, offset, float) + value.x;
    float y = FIELD(object, offset + 4U, float) + value.y;
    float z = FIELD(object, offset + 8U, float) + value.z;
    FIELD(object, offset, float) = x;
    FIELD(object, offset + 4U, float) = y;
    FIELD(object, offset + 8U, float) = z;
}

ACTOR_INLINE GeorgeMathVec3 construction_subtract(GeorgeMathVec3 first, GeorgeMathVec3 second)
{
    first.x = first.x - second.x;
    first.y = first.y - second.y;
    first.z = first.z - second.z;
    return first;
}

ACTOR_INLINE GeorgeActorBits64 construction_soft_abs(GeorgeActorBits64 value)
{
    if (func_00373250(value, 0) < 0) value = func_00372CC0(0, value);
    return value;
}

void func_0016EE80(void *object, u32 mode)
{
    FIELD(object, 4, const u8 *) = D_0042C018;
    if ((mode & 1U) != 0) func_002AF100(object);
}

void func_0016EA40(void *object, u32 mode)
{
    void *manager, *record;
    FIELD(object, 4, const u8 *) = D_0042BF90;
    manager = func_0022C1E0();
    func_00316040(manager, FIELD(object, 0x278, void *));
    record = FIELD(object, 0x278, void *);
    if (FIELD(record, 4, u16) != 0) {
        u16 count = (u16)(FIELD(record, 6, u16) - 1U);
        FIELD(record, 6, u16) = count;
        if (count == 0 && record != 0) {
            const GeorgeGoalVirtualWord *pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(record, 0, const u8 *), 8);
            pair->invoke(ADJUST(record, pair->adjustment), 3);
        }
    }
    FIELD(object, 0x278, u32) = 0;
    func_0016EE80(object, mode);
}

GeorgeGoalEntity *func_0016EEB0(GeorgeGoalEntity *entity)
{
    func_002D06E8((GeorgeScriptObject *)entity);
    FIELD(entity, 0x4F0, u32) = 5;
    FIELD(entity, 4, const u8 *) = D_0042E7A8;
    FIELD(entity, 0x4F4, u32) = 0;
    FIELD(entity, 0x4F8, u32) = 0;
    FIELD(entity, 0x4F0, u32) = 5;
    FIELD(entity, 0x4F4, u32) = 0;
    FIELD(entity, 0x4F8, void *) = func_002AEF08(0x28);
    FIELD(entity, 8, u32) = 0;
    return entity;
}

void func_0016EF18(GeorgeGoalEntity *entity, const GeorgeMathVec3 *position,
                  GeorgeGoalEntityData *data, u32 word3, u32 word4, u32 word5,
                  u32 word6, void *vehicle, s32 index, u32 word9, void *pointer10,
                  u32 word11, u32 word12, u32 word13, u32 word14, float angle)
{
    GeorgeRotationMatrix frame;
    GeorgeActorBits64 bits;
    void *collection, *component, *record, *captured_data, *physical;
    u32 key, flags, n;
    s32 i;

#define ZERO(offset) FIELD(entity, offset, u32) = 0
    FIELD(entity, 0x3B4, u32) = word12;
    FIELD(entity, 8, u32) = 1;
    FIELD(entity, 0x3B8, u32) = word13;
    FIELD(entity, 0x18, GeorgeGoalEntityData *) = data;
    ZERO(0x410);
    FIELD(entity, 0xC, s32) = -1;
    FIELD(entity, 0x10, s32) = -1;
    FIELD(entity, 0x14, s32) = -1;
    FIELD(entity, 0x40, float) = position->x;
    FIELD(entity, 0x44, float) = position->y;
    FIELD(entity, 0x48, float) = position->z;
    ZERO(0x4C); ZERO(0x54); ZERO(0x50);
    FIELD(entity, 0x7C, float) = angle;
    FIELD(entity, 0x58, float) = angle;
    FIELD(entity, 0x5C, float) = angle;
    ZERO(0x60); ZERO(0x68); ZERO(0x70); ZERO(0x6C);
    ZERO(0x74); ZERO(0x78);
    FIELD(entity, 0x80, float) = FIELD(entity, 0x44, float);
    {
        float y = FIELD(entity, 0x44, float);
        ZERO(0x88);
        FIELD(entity, 0x84, float) = y;
    }
    FIELD(entity, 0x8C, float) = 1.0f;
    ZERO(0x90); ZERO(0x94); ZERO(0x9C); ZERO(0x98);
    ZERO(0xA0); ZERO(0xC); ZERO(0x1BC); ZERO(0x1C0);
    func_001767B8(entity);
    {
        float height = FIELD(DATA(entity), 0xCC, float);
        FIELD(entity, 0xC, s32) = -1;
        FIELD(entity, 0x428, float) = height;
    }
    FIELD(entity, 0x190, GeorgeActorBits64) = 0x02400844ULL;
    if (pointer10 != 0) func_0018FD30(entity, 0x400ULL);
    else func_0018FD40(entity, 0x400ULL);
    ZERO(0x198); ZERO(0x19C); ZERO(0x1A0); ZERO(0x1A4); ZERO(0x1B8);
    ZERO(0x1C8); ZERO(0x1CC); ZERO(0x1D0); ZERO(0x1D4); ZERO(0x1D8); ZERO(0x1DC);
    ZERO(0x1F8); ZERO(0x1FC); ZERO(0x200); ZERO(0x204); ZERO(0x208);
    ZERO(0x20C); ZERO(0x210); ZERO(0x214); ZERO(0x218); ZERO(0x21C);
    ZERO(0x220); ZERO(0x224); ZERO(0x228); ZERO(0x22C); ZERO(0x230);
    ZERO(0x234); ZERO(0x238); ZERO(0x264); ZERO(0x23C); ZERO(0x240); ZERO(0x288);
    FIELD(entity, 0x1F4, u32) = 1;
    func_003936A0(ADDRESS(entity, 0x244), 0, 0x20);
    func_003936A0(ADDRESS(entity, 0x268), 0, 0x20);
    collection = func_002AAF98(4);
    ZERO(0x28C); ZERO(0x290); ZERO(0x29C); ZERO(0x2A0); ZERO(0x2A4);
    ZERO(0x2D0); ZERO(0x2D4); ZERO(0x2D8); ZERO(0x2E0); ZERO(0x2E4);
    ZERO(0x2E8); ZERO(0x2DC); ZERO(0x35C); ZERO(0x360); ZERO(0x364);
    ZERO(0x368); ZERO(0x374); ZERO(0x378); ZERO(0x37C); ZERO(0x380);
    FIELD(entity, 0x2C0, u16) = 0;
    FIELD(entity, 0x2C2, u16) = 0;
    FIELD(entity, 0x36C, float) = 1.0f;
    FIELD(entity, 0x294, void *) = collection;
    component = func_0021B848(func_002AEE60(0x114));
    bits = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 35) & ~(1ULL << 30);
    FIELD(entity, 0x384, void *) = component;
    FIELD(entity, 0x3BC, u32) = 0x7F9000CF;
    FIELD(entity, 0x3C0, u32) = 0x728F0147;
    FIELD(entity, 0x190, GeorgeActorBits64) = bits;
    ZERO(0x388); ZERO(0x38C); ZERO(0x390); ZERO(0x394); ZERO(0x398);
    ZERO(0x39C); ZERO(0x3A0); ZERO(0x3A4); ZERO(0x3A8); ZERO(0x3AC); ZERO(0x3B0);
    ZERO(0x3C4); ZERO(0x3CC); ZERO(0x3DC); ZERO(0x3E0); ZERO(0x3E4);
    ZERO(0x3E8); ZERO(0x3EC); ZERO(0x400); ZERO(0x2F0); ZERO(0x2F4);
    ZERO(0x404); ZERO(0x438); ZERO(0x410); ZERO(0x300); ZERO(0x304);
    ZERO(0x2FC); ZERO(0x314); ZERO(0x2F8);
    for (i = 2; i >= 0; --i) FIELD(entity, 0x308U + (u32)i * 4U, u32) = 0;
    ZERO(0x318); ZERO(0x31C); ZERO(0x358); ZERO(0x408); ZERO(0x40C);
    ZERO(0x43C); ZERO(0x444); ZERO(0x440); ZERO(0x448); ZERO(0x450); ZERO(0x44C);
    captured_data = DATA(entity);
    FIELD(entity, 0xA34, u32) = word6;
    FIELD(entity, 0x9F4, float) = 2.0f;
    ZERO(0x454); ZERO(0x4E0); ZERO(0x4E4); ZERO(0x458); ZERO(0x45C);
    ZERO(0x460); ZERO(0x464); ZERO(0x328); ZERO(0x350); ZERO(0x354);
    ZERO(0x484); ZERO(0x24); ZERO(0x8B8); ZERO(0x34); ZERO(0x38); ZERO(0x424); ZERO(0x9EC);
    FIELD(entity, 0x334, float) = FIELD(captured_data, 0x370, float);
    ZERO(0x298);
    key = FIELD(captured_data, 0xF0, u32);
    if (key != 0) func_00191DC8(entity, key);
    bits = FIELD(entity, 0x190, GeorgeActorBits64) & ~(1ULL << 39);
    ZERO(0x4E8); ZERO(0x4EC);
    FIELD(entity, 0x190, GeorgeActorBits64) = bits;
    ZERO(0x4FC);
    if (word11 != 0) {
        FIELD(entity, 0x190, GeorgeActorBits64) = bits | 0x10000000ULL;
        FIELD(entity, 0x19C, u32) = 0x98197A65;
    }
    captured_data = DATA(entity);
    key = FIELD(captured_data, 0x1D8, u32);
    if (key == 0x218568E4U) FIELD(entity, 0x20, void *) = func_001A1988(entity);
    else if (key == 0x35F655E9U) FIELD(entity, 0x20, void *) = func_001A2278(entity, FIELD(captured_data, 0x1DC, u32));
    else if (key == 0x45A78000U) FIELD(entity, 0x20, void *) = func_001A6158(entity, word14);
    FIELD(entity, 0x1A8, u32) = word4;
    ZERO(0x1B0);
    FIELD(entity, 0x1AC, u32) = word3;
    func_00176A20(entity, word3, word5);
    ZERO(0x1B4); ZERO(0x1C);
    record = FIELD(DATA(entity), 0x374, void *);
    if (record != 0) func_00195358(entity, record);
    component = (void *)func_00236A10(FRAME(entity, 0xB0), 0x935FADFC, 1, 0, 0, 0);
    FIELD(entity, 0x1C8, void *) = component;
    record = func_00238BA0(component, 0x1A6B0F5D);
    FIELD(record, 0x34, GeorgeGoalEntity *) = entity;
    captured_data = DATA(entity);
    flags = FIELD(captured_data, 0xA8, u32) | 1U;
    if (FIELD(captured_data, 0xAC, u32) != 0) flags |= 0x100U;
    {
        void *command = FIELD(record, 0x3C, void *);
        flags = FIELD(record, 0x28, u32) | flags;
        FIELD(record, 0x28, u32) = flags;
        if (command != 0) {
            func_0022D838(command);
            FIELD(record, 0x2C, u32) = FIELD(record, 0x28, u32);
            func_0022D788(FIELD(record, 0x3C, void *));
        } else FIELD(record, 0x2C, u32) = flags;
    }
    record = func_00238BA0(FIELD(entity, 0x1C8, void *), 0xBA44E6F8);
    FIELD(record, 0x18, float) = FIELD(DATA(entity), 0x128, float);
    component = (void *)func_00236A10(FRAME(entity, 0xB0), 0x6966F026, 1, 0, 0, 0);
    FIELD(entity, 0x1CC, void *) = component;
    record = func_00238BA0(component, 0x1A6B0F5D);
    {
        void *command = FIELD(record, 0x3C, void *);
        flags = FIELD(record, 0x28, u32) | 0x40010080U;
        FIELD(record, 0x34, GeorgeGoalEntity *) = entity;
        FIELD(record, 0x28, u32) = flags;
        if (command != 0) {
            func_0022D838(command);
            FIELD(record, 0x2C, u32) = FIELD(record, 0x28, u32);
            func_0022D788(FIELD(record, 0x3C, void *));
        } else FIELD(record, 0x2C, u32) = flags;
    }

#define ATTACH_KEY(data_offset, identifier, entity_offset, notify) do { \
    if (FIELD(DATA(entity), data_offset, u32) != 0) { \
        const GeorgeRotationMatrix *source = func_00192748(entity, identifier); \
        func_002A2200(&frame, source, FRAME(entity, 0xB0)); \
        component = (void *)func_00236A10(&frame, FIELD(DATA(entity), data_offset, u32), 1, 0, 0, 0); \
        FIELD(entity, entity_offset, void *) = component; \
        if (notify) func_00235CD8(component, 0x9F79558F, 0); \
    } \
} while (0)
    ATTACH_KEY(0xD4, 0x23, 0x214, 1);
    ATTACH_KEY(0xD8, 0x83, 0x218, 0);
    ATTACH_KEY(0xDC, 0x81, 0x21C, 0);
    ATTACH_KEY(0xE0, 0x82, 0x220, 0);
#undef ATTACH_KEY
#define ATTACH_OBJECT(data_offset, identifier, entity_offset, cached) do { \
    if (FIELD(DATA(entity), data_offset, u32) != 0) { \
        void *object; \
        const GeorgeRotationMatrix *source = func_00192748(entity, identifier); \
        func_002A2200(&frame, source, FRAME(entity, 0xB0)); \
        object = func_00210078(FIELD(DATA(entity), data_offset, u32), 0, 0); \
        if (object != 0) { \
            const GeorgeGoalVirtualWord *pair; \
            component = func_00236CB8(&frame, object, 1, 0, 0); \
            FIELD(entity, entity_offset, void *) = component; \
            func_002A1C08(ADDRESS(component, 0x10), &frame); \
            FIELD(component, 0xA0, u32) |= 0x100U; \
            pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0x20, const u8 *), 0x10); \
            pair->invoke(ADJUST(object, pair->adjustment), 0); \
        } \
        FIELD(entity, cached, u32) = FIELD(DATA(entity), data_offset, u32); \
    } \
} while (0)
    ATTACH_OBJECT(0xE4, 0x61, 0x224, 0x464);
    ATTACH_OBJECT(0xE8, 0x62, 0x228, 0x460);
#undef ATTACH_OBJECT
    key = FIELD(DATA(entity), 0xF8, u32);
    if (key != 0) func_00239E40(key, FRAME(entity, 0xF0), 1, func_001968D0, entity, 0, 0);
    key = FIELD(DATA(entity), 0xFC, u32);
    if (key != 0) func_00239E40(key, FRAME(entity, 0xF0), 1, func_001910C0, entity, 0, 0);

/* The two original loops reload actor +18 after every lookup and effect call. */
#define ATTACH_LIST(count_offset, data_offset, entity_offset, callback) do { \
    u32 index_value = 0, data_stride = 0; \
    void *context = ADDRESS(entity, entity_offset); \
    if (FIELD(DATA(entity), count_offset, s32) > 0) { \
        FIELD(context, 4, u32) = 0; \
        for (;;) { \
            FIELD(context, 0, GeorgeGoalEntity *) = entity; \
            ++index_value; \
            FIELD(context, 8, u32) = func_00297640(FIELD(DATA(entity), data_offset + 8U + data_stride, u32)); \
            FIELD(context, 12, u32) = func_002A7418(FIELD(DATA(entity), data_offset + 4U + data_stride, u32)); \
            key = FIELD(DATA(entity), data_offset + data_stride, u32); \
            func_00239E40(key, FRAME(entity, 0xF0), 1, callback, context, 0, 0); \
            context = ADDRESS(context, 16); \
            data_stride += 12U; \
            if (!((s32)index_value < FIELD(DATA(entity), count_offset, s32))) break; \
            FIELD(context, 4, u32) = index_value; \
        } \
    } \
} while (0)
    ATTACH_LIST(0x288, 0x28C, 0x5D0, func_00194A40);
    ATTACH_LIST(0x2EC, 0x2F0, 0x65C, func_00194790);
#undef ATTACH_LIST
    captured_data = DATA(entity);
    if (FIELD(captured_data, 0x280, s32) != 0) {
        s32 random_value = func_00397178();
        s32 remainder = random_value % FIELD(DATA(entity), 0x280, s32);
        key = FIELD(captured_data, 0x284U + (u32)remainder * 4U, u32);
        func_00239E40(key, FRAME(entity, 0xB0), 1, func_00194D58, entity, 0, 0);
    }
    key = FIELD(DATA(entity), 0x114, u32);
    if (key != 0) func_00239E40(key, FRAME(entity, 0xB0), 1, func_00196418, entity, 0, 0);
    captured_data = DATA(entity);
    if (FIELD(captured_data, 0x140, u32) != 0) {
        void *object;
        u32 identifier = func_002A7418(FIELD(captured_data, 0x144, u32));
        const GeorgeRotationMatrix *source = func_00192748(entity, identifier);
        func_002A2200(&frame, source, FRAME(entity, 0xF0));
        object = func_00210078(FIELD(DATA(entity), 0x140, u32), 0, 0);
        if (object != 0) {
            const GeorgeGoalVirtualWord *pair;
            component = func_00236CB8(&frame, object, 0, 0, 0);
            FIELD(entity, 0x23C, void *) = component;
            func_002A1C08(ADDRESS(component, 0x10), &frame);
            FIELD(component, 0xA0, u32) |= 0x100U;
            pair = (const GeorgeGoalVirtualWord *)ADDRESS(FIELD(object, 0x20, const u8 *), 0x10);
            pair->invoke(ADJUST(object, pair->adjustment), 0);
        }
    }
    physical = func_0022D4B0((const GeorgeMathVec3 *)ADDRESS(entity, 0x40),
                           func_0017ADA8, func_00194028, entity, 0, 0, 0, 0.0f);
    FIELD(entity, 0x1D0, void *) = physical;
    FIELD(physical, 0x2C, u32) = FIELD(DATA(entity), 0x408, u32) != 0 ? 0xF800400AU : 0xF800000AU;
    physical = FIELD(entity, 0x1D0, void *);
    {
        void *command = FIELD(physical, 0x10, void *);
        if (command != 0) func_0022D838(command);
        for (n = 0; n < 3U; ++n) FIELD(physical, 0x30U + n * 8U, u32) = FIELD(physical, 0x2CU + n * 8U, u32);
        if (command != 0) func_0022D788(FIELD(physical, 0x10, void *));
    }
    func_0022D318(FIELD(entity, 0x1D0, void *));
    if (word9 != 0) func_0018FD30(entity, 0x4000ULL);
    else func_0018FD40(entity, 0x4000ULL);
    func_00175AF0(entity, vehicle == 0 && word9 != 0);
    if (vehicle != 0) {
        void *fresh_vehicle;
        const ActorConstructionVirtualIndex *pair;
        if (index < 0) index = func_001F6350(vehicle);
        FIELD(entity, 0x730, void *) = vehicle;
        func_001F6138(vehicle);
        fresh_vehicle = FIELD(entity, 0x730, void *);
        pair = (const ActorConstructionVirtualIndex *)ADDRESS(FIELD(fresh_vehicle, 4, const u8 *), 0x1A8);
        pair->invoke(ADJUST(fresh_vehicle, pair->adjustment), entity, (signed char)index);
        FIELD(entity, 0x738, s32) = index;
        FIELD(entity, 0x73C, u32) = 1;
        FIELD(entity, 0x734, s32) = index;
        func_00190E00(entity, 14);
        func_00183C58(entity);
    } else {
        func_00190E00(entity, FIELD(DATA(entity), 0x278, u32) != 0);
    }
    func_002D0858((GeorgeScriptObject *)entity);
#undef ZERO
}

void func_0016CF30(GeorgeActorControlObject *control, const GeorgeMathVec3 *input)
{
    void *data, *context, *config;
    GeorgeMathVec3 origin, weighted, velocity, negative_up, response, local_point;
    GeorgeMathVec3 scratch90, scratchA0, scratchB0, scratchC0, scratchD0, scratchE0;
    GeorgeMathVec4 negative_pose, linear_output, angular_output;
    float lateral[4], longitudinal[4], query_fraction, curve_output;
    float *curve_pair = (float *)&scratchE0;
    float projected_speed, amount, x, y, z;
    u32 query_word, index_value, offset;
    s32 fallback = -1, direction = -1, recent_direction = 1;
    GeorgeActorBits64 soft;

    FIELD(control, 0x20, u32) = 0;
    FIELD(control, 0x28, u32) = 0;
    FIELD(control, 0x24, u32) = 0;
    FIELD(control, 0x2C, u32) = 0;
    FIELD(control, 0x30, u32) = 0;
    FIELD(control, 0x34, u32) = 0;
    data = FIELD(control, 0x18, void *);
    origin = construction_read_vector(data, 0xE0);
    negative_pose.x = -FIELD(data, 0x120, float);
    negative_pose.y = -FIELD(data, 0x124, float);
    negative_pose.z = -FIELD(data, 0x128, float);
    negative_pose.w = FIELD(data, 0x12C, float);
    func_002A1F18(FRAME(control, 0x40), &negative_pose, &origin);
    context = CONTEXT(control);
    amount = FIELD(FIELD(context, 0x27C, void *), 4, float);
    weighted = construction_scale(construction_read_vector(control, 0x40), amount);
    amount = FIELD(FIELD(context, 0x27C, void *), 8, float);
    response = construction_scale(construction_read_vector(control, 0x50), amount);
    weighted.x = weighted.x + response.x;
    weighted.y = weighted.y + response.y;
    weighted.z = weighted.z + response.z;
    amount = FIELD(FIELD(context, 0x27C, void *), 12, float);
    response = construction_scale(construction_read_vector(control, 0x60), amount);
    weighted.x = weighted.x + response.x;
    weighted.y = weighted.y + response.y;
    weighted.z = weighted.z + response.z;
    func_002A1C08(ADDRESS(control, 0x80), ADDRESS(control, 0x40));
    construction_add_vector(control, 0xB0, weighted);
    {
        float angular_y = FIELD(data, 0x194, float);
        float negative_x = -FIELD(data, 0x190, float);
        float negative_z = -FIELD(data, 0x198, float);
        float basis_y, target;
        context = CONTEXT(control);
        velocity = construction_read_vector(data, 0x180);
        basis_y = FIELD(control, 0x64, float);
        target = FIELD(context, 0x268, float);
        projected_speed = construction_dot(construction_read_vector(context, 0xC),
                                           construction_read_vector(control, 0x60));
        if (target != 0.0f || FIELD(context, 0x26C, float) != 0.0f) {
            float basis40_y = FIELD(control, 0x44, float);
            float target_x = target * basis_y;
            float target_z = FIELD(context, 0x26C, float) * basis40_y;
            float coefficient = FIELD(FIELD(context, 0x27C, void *), 0, float) * 5.0f;
            float impulse_x = (target_x - negative_x) * coefficient;
            float impulse_z = (target_z - negative_z) * coefficient;
            float negative_impulse = -impulse_x;
            x = FIELD(control, 0x2C, float) + negative_impulse * FIELD(control, 0x40, float);
            y = FIELD(control, 0x30, float) + negative_impulse * basis40_y;
            z = FIELD(control, 0x34, float) + negative_impulse * FIELD(control, 0x48, float);
            FIELD(control, 0x2C, float) = x;
            FIELD(control, 0x34, float) = z;
            FIELD(control, 0x30, float) = y;
            negative_impulse = -impulse_z;
            x = FIELD(control, 0x2C, float) + negative_impulse * FIELD(control, 0x60, float);
            z = z + negative_impulse * FIELD(control, 0x68, float);
            y = y + negative_impulse * FIELD(control, 0x64, float);
            FIELD(control, 0x2C, float) = x;
            FIELD(control, 0x34, float) = z;
            FIELD(control, 0x30, float) = y;
            /* The original also computes angular_y * coefficient into scratch
             * +64; all three scratch values are overwritten before use. */
            (void)angular_y;
        }
    }
    negative_up.x = -FIELD(control, 0x50, float);
    negative_up.y = -FIELD(control, 0x54, float);
    negative_up.z = -FIELD(control, 0x58, float);
    for (index_value = 0; index_value < 4U; ++index_value) {
        u32 front = index_value < 2U;
        float distance;
        void *record;
        offset = index_value * 0x68U;
        context = CONTEXT(control);
        config = FIELD(context, 0x27C, void *);
        distance = FIELD(context, front ? 0x234 : 0x238, float) + FIELD(config, front ? 0x20 : 0x38, float);
        context = CONTEXT(control);
        func_002A1C60(ADDRESS(control, 0x40), VECTOR(context, offset + 0x90U), VECTOR(context, offset + 0x9CU));
        context = CONTEXT(control);
        record = ADDRESS(context, offset);
        scratch90 = construction_read_vector(record, 0x9C);
        local_point = construction_subtract(scratch90, construction_read_vector(data, 0x100));
        /* VOPMULA/VOPMSUB followed by VADD produce angular cross position
         * plus linear velocity. Their fourth lanes are never observed here. */
        response = construction_cross(construction_read_vector(data, 0x190), local_point);
        origin = construction_read_vector(data, 0x180);
        response.x = response.x + origin.x;
        response.y = response.y + origin.y;
        response.z = response.z + origin.z;
        FIELD(record, 0xA8, float) = response.x;
        FIELD(record, 0xB0, float) = response.z;
        FIELD(record, 0xAC, float) = response.y;
        context = CONTEXT(control);
        origin = construction_read_vector(context, offset + 0x9CU);
        response = construction_scale(negative_up, distance);
        scratch90.x = origin.x + response.x;
        scratch90.y = origin.y + response.y;
        scratch90.z = origin.z + response.z;
        if (func_0022BB70(VECTOR(context, offset + 0x9CU), &scratch90, 0x2D,
                         &query_fraction, VECTOR(context, offset + 0xCCU), &query_word) == 0) {
            FIELD(CONTEXT(control), offset + 0xB4U, u32) = 0;
            FIELD(CONTEXT(control), offset + 0xBCU, float) = distance;
            FIELD(CONTEXT(control), offset + 0xECU, u32) = 0;
            if (fallback == -1) {
                const GeorgeGoalVirtualPointer *pair;
                void *object;
                context = CONTEXT(control);
                pair = (const GeorgeGoalVirtualPointer *)ADDRESS(FIELD(context, 4, const u8 *), 0x10);
                object = pair->invoke(ADJUST(context, pair->adjustment));
                origin = construction_read_vector(object, 0x30);
                response = construction_scale(negative_up, 10.0f);
                scratchA0.x = origin.x + response.x;
                scratchA0.y = origin.y + response.y;
                scratchA0.z = origin.z + response.z;
                fallback = func_0022BB70(VECTOR(CONTEXT(control), offset + 0x9CU), &scratchA0, 0x2D, 0, 0, 0);
            }
            if (fallback == 0) {
                float ray[7];
                context = CONTEXT(control);
                origin = construction_read_vector(context, offset + 0x9CU);
                ray[0] = origin.x; ray[1] = origin.y; ray[2] = origin.z;
                ray[3] = negative_up.x; ray[4] = negative_up.y; ray[5] = negative_up.z; ray[6] = distance;
                if (func_0029F080(ray, ADDRESS(FIELD(context, 8, void *), 0x93C), &query_fraction) != 0) {
                    FIELD(CONTEXT(control), offset + 0xB4U, u32) = 1;
                    FIELD(CONTEXT(control), offset + 0xBCU, float) = query_fraction * distance;
                }
            }
        } else {
            void *object;
            FIELD(CONTEXT(control), offset + 0xB4U, u32) = 1;
            FIELD(CONTEXT(control), offset + 0xBCU, float) = query_fraction * distance;
            object = func_0022C800(query_word);
            if (object != 0) {
                u32 alternate;
                context = CONTEXT(control);
                alternate = FIELD(D_003F8A68, 0x68, u32) != 0 &&
                            FIELD(D_003F8A68, 0x60, u32) != 0 &&
                            FIELD(D_003F8A68, 0x80, float) == 0.0f;
                FIELD(context, offset + 0xECU, u32) = FIELD(FIELD(object, 0x18, void *), alternate ? 0x78 : 0x74, u32);
            } else FIELD(CONTEXT(control), offset + 0xECU, u32) = 0;
        }
        context = CONTEXT(control);
        FIELD(context, offset + 0xB8U, float) = FIELD(context, offset + 0xBCU, float) - FIELD(context, front ? 0x234 : 0x238, float);
        context = CONTEXT(control);
        distance = FIELD(context, offset + 0xBCU, float);
        origin = construction_read_vector(context, offset + 0x9CU);
        response = construction_scale(negative_up, distance);
        FIELD(context, offset + 0xC0U, float) = origin.x + response.x;
        FIELD(context, offset + 0xC8U, float) = origin.z + response.z;
        FIELD(context, offset + 0xC4U, float) = origin.y + response.y;
    }
    for (index_value = 0; index_value < 2U; ++index_value) {
        offset = index_value * 0xD0U;
        context = CONTEXT(control);
        amount = FIELD(context, offset + 0x124U, float) - FIELD(context, offset + 0xBCU, float);
        amount = amount * FIELD(FIELD(context, 0x27C, void *), index_value == 0 ? 0x30 : 0x48, float);
        FIELD(context, offset + 0xDCU, float) = amount;
        context = CONTEXT(control);
        FIELD(context, offset + 0x144U, float) = -FIELD(context, offset + 0xDCU, float);
    }
    {
        GeorgeMathVec3 basis = construction_read_vector(control, 0x60);
        float dot = construction_dot(velocity, basis);
        float signed_square = 0.0f <= dot ? dot * -dot : dot * dot;
        amount = signed_square * FIELD(FIELD(CONTEXT(control), 0x27C, void *), 0x2D8, float);
        construction_add_vector(control, 0x20, construction_scale(basis, amount));
        basis = construction_read_vector(control, 0x40);
        amount = -construction_dot(velocity, basis) * FIELD(FIELD(CONTEXT(control), 0x27C, void *), 0x2EC, float);
        construction_add_vector(control, 0x20, construction_scale(basis, amount));
        amount = -construction_dot(velocity, construction_read_vector(control, 0x60)) *
                 FIELD(FIELD(CONTEXT(control), 0x27C, void *), 0x2E8, float);
        construction_add_vector(control, 0x20, construction_scale(construction_read_vector(control, 0x50), amount));
    }
    for (index_value = 0; index_value < 4U; ++index_value) {
        offset = index_value * 0x68U;
        FIELD(CONTEXT(control), offset + 0xD8U, u32) = 0;
        context = CONTEXT(control);
        if (FIELD(context, offset + 0xB4U, u32) != 0) {
            u32 front = index_value < 2U;
            float rate, force, displacement;
            config = FIELD(context, 0x27C, void *);
            displacement = FIELD(config, front ? 0x20 : 0x38, float) - FIELD(context, offset + 0xB8U, float);
            context = CONTEXT(control);
            config = FIELD(context, 0x27C, void *);
            force = FIELD(config, front ? 0x24 : 0x3C, float) * displacement + FIELD(context, offset + 0xDCU, float);
            rate = construction_dot(construction_read_vector(context, offset + 0xA8U), negative_up);
            force = force + FIELD(config, front ? (rate > 0.0f ? 0x28 : 0x2C) : (rate > 0.0f ? 0x40 : 0x44), float) * rate;
            if (force < 0.0f) force = 0.0f;
            FIELD(CONTEXT(control), offset + 0xD8U, float) = force;
            response = construction_scale(negative_up, -force);
            context = CONTEXT(control);
            construction_add_vector(control, 0x20, response);
            local_point = construction_subtract(construction_read_vector(context, offset + 0x9CU), construction_read_vector(control, 0xB0));
            construction_add_vector(control, 0x2C, construction_cross(local_point, response));
        }
    }
    soft = construction_soft_abs(func_00374848(projected_speed));
    if (func_00373250(soft, 0x3FF3333340000000ULL) < 0) {
        if (0.0f <= FIELD(CONTEXT(control), 0x24C, float)) direction = 1;
    } else if (0.0f <= projected_speed) direction = 1;
    context = CONTEXT(control);
    if (direction != FIELD(context, 0x260, s32)) {
        FIELD(context, 0x260, s32) = direction;
        FIELD(CONTEXT(control), 0x264, u32) = 0;
    }
    context = CONTEXT(control);
    FIELD(context, 0x264, float) = FIELD(context, 0x264, float) + input->z;
    context = CONTEXT(control);
    if (0.25f < FIELD(context, 0x264, float)) recent_direction = 0;
    FIELD(context, 0x78, float) = FIELD(control, 0x40, float);
    FIELD(context, 0x7C, float) = FIELD(control, 0x44, float);
    FIELD(context, 0x80, float) = FIELD(control, 0x48, float);
    context = CONTEXT(control);
    FIELD(context, 0x84, float) = FIELD(control, 0x40, float);
    FIELD(context, 0x88, float) = FIELD(control, 0x44, float);
    FIELD(context, 0x8C, float) = FIELD(control, 0x48, float);
    context = CONTEXT(control);
    {
        u32 steered_offset = FIELD(FIELD(FIELD(context, 8, void *), 0x21C, void *), 0x60, u32) != 0 ? 0x84 : 0x78;
        float cosine = func_0029C168(FIELD(context, 0x250, float) * FIELD(context, 0x230, float));
        context = CONTEXT(control);
        FIELD(context, steered_offset, float) = FIELD(context, steered_offset, float) * cosine;
        y = FIELD(context, steered_offset + 4U, float) * cosine;
        z = FIELD(context, steered_offset + 8U, float) * cosine;
        FIELD(context, steered_offset + 4U, float) = y;
        FIELD(context, steered_offset + 8U, float) = z;
        context = CONTEXT(control);
        amount = func_0029C090(FIELD(context, 0x250, float) * FIELD(context, 0x230, float));
        context = CONTEXT(control);
        response = construction_scale(construction_read_vector(control, 0x60), amount);
        construction_add_vector(context, steered_offset, response);
    }
    context = CONTEXT(control);
    {
        GeorgeMathVec3 first = construction_read_vector(context, 0xC0);
        GeorgeMathVec3 second = construction_read_vector(context, 0x128);
        FIELD(context, 0x60, float) = first.x + second.x;
        FIELD(context, 0x68, float) = first.z + second.z;
        FIELD(context, 0x64, float) = first.y + second.y;
        context = CONTEXT(control);
        FIELD(context, 0x60, float) = FIELD(context, 0x60, float) * 0.5f;
        y = FIELD(context, 0x64, float) * 0.5f;
        z = FIELD(context, 0x68, float) * 0.5f;
        FIELD(context, 0x64, float) = y;
        FIELD(context, 0x68, float) = z;
        context = CONTEXT(control);
        first = construction_read_vector(context, 0x190);
        second = construction_read_vector(context, 0x1F8);
        FIELD(context, 0x6C, float) = first.x + second.x;
        FIELD(context, 0x74, float) = first.z + second.z;
        FIELD(context, 0x70, float) = first.y + second.y;
        context = CONTEXT(control);
        FIELD(context, 0x6C, float) = FIELD(context, 0x6C, float) * 0.5f;
        y = FIELD(context, 0x70, float) * 0.5f;
        z = FIELD(context, 0x74, float) * 0.5f;
        FIELD(context, 0x70, float) = y;
        FIELD(context, 0x74, float) = z;
    }

    for (index_value = 0; index_value < 4U; ++index_value) {
        u32 front = index_value < 2U;
        void *record;
        float normal_dot, transverse_speed;
        context = CONTEXT(control);
        longitudinal[index_value] = 0.0f;
        lateral[index_value] = 0.0f;
        record = ADDRESS(context, 0x90U + index_value * 0x68U);
        scratch90 = construction_read_vector(context, front ? 0x78 : 0x84);
        scratchB0 = construction_read_vector(record, 0x3C);
        normal_dot = construction_dot(scratch90, scratchB0);
        scratchA0 = construction_subtract(scratch90, construction_scale(scratchB0, normal_dot));
        func_002A3538(&scratchA0);
        scratchD0 = construction_cross(scratchA0, scratchB0);
        func_002A3538(&scratchD0);
        if (FIELD(record, 0x24, u32) == 0) {
            FIELD(record, 0x54, u32) = 0;
        } else {
            float angle, captured_coefficient;
            origin = construction_read_vector(record, 0x18);
            normal_dot = construction_dot(origin, scratchB0);
            scratchE0 = construction_subtract(origin, construction_scale(scratchB0, normal_dot));
            transverse_speed = construction_dot(scratchE0, scratchA0);
            soft = construction_soft_abs(func_00374848(construction_dot(scratchE0, scratchD0)));
            amount = func_003734F8(soft);
            if (amount < 5.0f) amount = 5.0f;
            angle = func_0029B940(transverse_speed, amount);
            captured_coefficient = FIELD(record, 0x48, float);
            FIELD(record, 0x54, float) = angle;
            /* The config is captured before converting the angle, matching
             * the load in that conversion call's delay slot. */
            config = FIELD(CONTEXT(control), 0x27C, void *);
            soft = construction_soft_abs(func_00374848(angle));
            amount = func_003734F8(soft);
            func_002ADC80(FIELD(config, 0x50, u32), ADDRESS(config, 0x148), 1, &curve_output, amount);
            amount = curve_output * (0.0f <= angle ? -captured_coefficient : captured_coefficient);
            if (FIELD(CONTEXT(control), 0x25C, u32) == 0) amount = amount * 0.5f;
            lateral[index_value] = amount;
        }
        transverse_speed = construction_dot(construction_read_vector(record, 0x18), scratchD0);
        context = CONTEXT(control);
        if (FIELD(context, 0x244, u32) != 0) {
            config = FIELD(context, 0x27C, void *);
            func_002ADC80(FIELD(config, 0x4C, u32), ADDRESS(config, 0x58), 2, curve_pair,
                         projected_speed * 3.6000001430511475f);
            context = CONTEXT(control);
            longitudinal[index_value] = (FIELD(context, 0x24C, float) * curve_pair[0]) * FIELD(FIELD(context, 0x27C, void *), 0, float);
        } else if (FIELD(record, 0x58, u32) != 0 || recent_direction != 0) {
            FIELD(record, 0x50, u32) = 0;
            if (FIELD(record, 0x24, u32) != 0) {
                GeorgeActorBits64 sign = 0x3FF0000000000000ULL;
                GeorgeActorBits64 coefficient, scale;
                if (FIELD(record, 0x50, float) <= transverse_speed) sign = 0xBFF0000000000000ULL;
                coefficient = func_00374848(FIELD(record, 0x48, float));
                scale = func_00374848(FIELD(FIELD(CONTEXT(control), 0x27C, void *), 0x2E0, float));
                soft = construction_soft_abs(func_00374848(transverse_speed));
                if (func_00373250(soft, 0) >= 0) {
                    soft = construction_soft_abs(func_00374848(transverse_speed));
                    if (func_00373250(soft, 0x3FF0000000000000ULL) > 0) soft = 0x3FF0000000000000ULL;
                    soft = func_00372D28(sign, soft);
                    soft = func_00372D28(soft, coefficient);
                    soft = func_00372D28(soft, scale);
                    longitudinal[index_value] = func_003734F8(soft);
                }
            }
        } else {
            float command, product;
            FIELD(record, 0x50, float) = transverse_speed / FIELD(context, front ? 0x234 : 0x238, float);
            context = CONTEXT(control);
            command = FIELD(context, 0x24C, float);
            product = command * projected_speed;
            if (product < 0.0f || FIELD(context, 0x270, float) <= projected_speed) {
                if (FIELD(record, 0x24, u32) != 0) {
                    GeorgeActorBits64 coefficient, scale;
                    soft = construction_soft_abs(func_00374848(command));
                    coefficient = func_00374848(FIELD(record, 0x48, float));
                    scale = func_00374848(FIELD(FIELD(CONTEXT(control), 0x27C, void *), 0x2DC, float));
                    if (0.0f <= projected_speed) coefficient = func_00372D28(coefficient, 0xBFF0000000000000ULL);
                    soft = func_00372D28(soft, coefficient);
                    soft = func_00372D28(soft, scale);
                    longitudinal[index_value] = func_003734F8(soft);
                }
            } else if (0.0f < product) {
                s32 mode = FIELD(context, 0x240, s32);
                if (mode == 2 || (mode == 0 && front) || (mode == 1 && !front)) {
                    config = FIELD(CONTEXT(control), 0x27C, void *);
                    func_002ADC80(FIELD(config, 0x4C, u32), ADDRESS(config, 0x58), 2, curve_pair,
                                 projected_speed * 3.6000001430511475f);
                    if (FIELD(record, 0x24, u32) != 0) {
                        amount = FIELD(CONTEXT(control), 0x24C, float) * FIELD(record, 0x48, float);
                        longitudinal[index_value] = amount * curve_pair[0];
                    }
                    if (FIELD(CONTEXT(control), 0x270, float) < projected_speed)
                        longitudinal[index_value] = george_ee_minimum(longitudinal[index_value], 0.0f);
                }
            } else {
                if (FIELD(record, 0x24, u32) != 0) {
                    float coefficient = FIELD(record, 0x48, float);
                    amount = FIELD(FIELD(context, 0x27C, void *), 0x2E4, float);
                    longitudinal[index_value] = coefficient * (0.0f <= projected_speed ? -amount : amount);
                }
            }
        }
        context = CONTEXT(control);
        if (FIELD(context, 0x254, u32) != 0) {
            config = FIELD(context, 0x27C, void *);
            func_002ADC80(FIELD(config, 0x54, u32), ADDRESS(config, 0x1E8), 2, curve_pair, FIELD(context, 0x258, float));
            lateral[index_value] = lateral[index_value] * curve_pair[front ? 0 : 1];
        }
        if (FIELD(CONTEXT(control), (index_value ^ 1U) * 0x68U + 0xB4U, u32) == 0) {
            longitudinal[index_value] = longitudinal[index_value] * (2.0f < projected_speed ? 0.75f : 0.20000000298023224f);
            lateral[index_value] = lateral[index_value] * 0.75f;
        }
    }
    for (index_value = 0; index_value < 4U; ++index_value) {
        GeorgeMathVec3 basis, normal;
        float captured_lateral = lateral[index_value];
        float captured_longitudinal = longitudinal[index_value];
        float dot;
        offset = index_value * 0x68U;
        FIELD(CONTEXT(control), offset + 0xF0U, u32) = 0;
        FIELD(CONTEXT(control), offset + 0xF4U, u32) = 0;
        context = CONTEXT(control);
        basis = construction_read_vector(context, index_value < 2U ? 0x78 : 0x84);
        normal = construction_read_vector(context, offset + 0xCCU);
        dot = construction_dot(basis, normal);
        scratchA0 = construction_subtract(basis, construction_scale(normal, dot));
        scratch90 = construction_cross(scratchA0, construction_read_vector(control, 0x50));
        func_002A3538(&scratchA0);
        func_002A3538(&scratch90);
        FIELD(CONTEXT(control), offset + 0xF0U, float) = captured_lateral * 0.00009999999747378752f;
        FIELD(CONTEXT(control), offset + 0xF4U, float) = captured_longitudinal * 0.00009999999747378752f;
        scratchB0 = construction_scale(scratchA0, captured_lateral);
        context = CONTEXT(control);
        construction_add_vector(control, 0x20, scratchB0);
        local_point = construction_subtract(construction_read_vector(context, offset + 0xC0U), construction_read_vector(control, 0xB0));
        scratchC0 = construction_cross(local_point, scratchB0);
        construction_add_vector(control, 0x2C, scratchC0);
        scratchB0 = construction_scale(scratch90, captured_longitudinal);
        if (FIELD(CONTEXT(control), 0x244, u32) != 0) {
            scratchB0.x = scratchB0.x + scratchB0.x;
            scratchB0.y = scratchB0.y + scratchB0.y;
            scratchB0.z = scratchB0.z + scratchB0.z;
        }
        context = CONTEXT(control);
        construction_add_vector(control, 0x20, scratchB0);
        local_point = construction_subtract(construction_read_vector(context, offset + 0xC0U), construction_read_vector(control, 0xB0));
        construction_add_vector(control, 0x2C, construction_cross(local_point, scratchB0));
    }
    linear_output.x = FIELD(control, 0x20, float);
    linear_output.y = FIELD(control, 0x24, float);
    linear_output.z = FIELD(control, 0x28, float);
    linear_output.w = 0.0f;
    amount = input->z;
    angular_output.x = FIELD(control, 0x2C, float);
    angular_output.y = FIELD(control, 0x30, float);
    angular_output.z = FIELD(control, 0x34, float);
    angular_output.w = 0.0f;
    func_00307850(data);
    {
        const ActorConstructionVirtualVectorScalar *pair = (const ActorConstructionVirtualVectorScalar *)ADDRESS(FIELD(data, 0xA0, const u8 *), 0xA8);
        pair->invoke(ADJUST(ADDRESS(data, 0xA0), pair->adjustment), &linear_output, amount);
    }
    amount = input->z;
    func_00307850(data);
    {
        const ActorConstructionVirtualVectorScalar *pair = (const ActorConstructionVirtualVectorScalar *)ADDRESS(FIELD(data, 0xA0, const u8 *), 0xB8);
        pair->invoke(ADJUST(ADDRESS(data, 0xA0), pair->adjustment), &angular_output, amount);
    }
}
