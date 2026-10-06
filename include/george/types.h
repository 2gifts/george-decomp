#ifndef GEORGE_TYPES_H
#define GEORGE_TYPES_H

#include <stddef.h>

/* Only the observed prefixes are modeled. These are not complete class types. */
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef char george_u8_size[(sizeof(u8) == 1) ? 1 : -1];
typedef char george_s16_size[(sizeof(s16) == 2) ? 1 : -1];
typedef char george_u16_size[(sizeof(u16) == 2) ? 1 : -1];
typedef char george_s32_size[(sizeof(s32) == 4) ? 1 : -1];
typedef char george_u32_size[(sizeof(u32) == 4) ? 1 : -1];
typedef char george_float_size[(sizeof(float) == 4) ? 1 : -1];

/* Numeric suffixes are byte offsets, not asserted gameplay meanings. */
typedef struct GeorgeFlags18 {
    u8 unknown00[0x18];
    u32 field18;
} GeorgeFlags18;

typedef struct GeorgeScalar44 {
    u8 unknown00[0x3C];
    u32 field3C;
    u8 unknown40[4];
    float field44;
} GeorgeScalar44;

typedef struct GeorgeRate48 {
    u8 unknown00[0x30];
    u32 field30;
    u8 unknown34[0x0C];
    s16 field40;
    u8 unknown42[2];
    u32 field44;
    float field48;
} GeorgeRate48;

typedef struct GeorgeRateInput {
    u16 field00;
    u8 unknown02[2];
    s32 field04;
    u32 field08;
} GeorgeRateInput;

typedef struct GeorgeRateOutput {
    s32 field00;
    u32 field04;
    u32 field08;
} GeorgeRateOutput;

typedef struct GeorgeTimer50 {
    u8 unknown00[0x28];
    u32 field28;
    u32 field2C;
    u32 field30;
    float field34;
    u8 unknown38[0x18];
    float field50;
} GeorgeTimer50;

typedef struct GeorgeBlend20 {
    u8 unknown00[0x18];
    u16 field18;
    u8 unknown1A[2];
    float field1C;
    float field20;
} GeorgeBlend20;

typedef struct GeorgeFade4C {
    u8 unknown00[0x18];
    u32 field18;
    float field1C;
    u8 unknown20[0x20];
    u32 field40;
    u32 field44;
    u32 field48;
    float field4C;
} GeorgeFade4C;

#define GEORGE_OFFSET_ASSERT(type, field, offset) \
    typedef char george_offset_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
GEORGE_OFFSET_ASSERT(GeorgeFlags18, field18, 0x18);
GEORGE_OFFSET_ASSERT(GeorgeScalar44, field3C, 0x3C);
GEORGE_OFFSET_ASSERT(GeorgeScalar44, field44, 0x44);
GEORGE_OFFSET_ASSERT(GeorgeRate48, field30, 0x30);
GEORGE_OFFSET_ASSERT(GeorgeRate48, field40, 0x40);
GEORGE_OFFSET_ASSERT(GeorgeRate48, field44, 0x44);
GEORGE_OFFSET_ASSERT(GeorgeRate48, field48, 0x48);
GEORGE_OFFSET_ASSERT(GeorgeRateInput, field00, 0);
GEORGE_OFFSET_ASSERT(GeorgeRateInput, field04, 4);
GEORGE_OFFSET_ASSERT(GeorgeRateInput, field08, 8);
GEORGE_OFFSET_ASSERT(GeorgeRateOutput, field00, 0);
GEORGE_OFFSET_ASSERT(GeorgeRateOutput, field04, 4);
GEORGE_OFFSET_ASSERT(GeorgeRateOutput, field08, 8);
GEORGE_OFFSET_ASSERT(GeorgeTimer50, field28, 0x28);
GEORGE_OFFSET_ASSERT(GeorgeTimer50, field2C, 0x2C);
GEORGE_OFFSET_ASSERT(GeorgeTimer50, field30, 0x30);
GEORGE_OFFSET_ASSERT(GeorgeTimer50, field34, 0x34);
GEORGE_OFFSET_ASSERT(GeorgeTimer50, field50, 0x50);
GEORGE_OFFSET_ASSERT(GeorgeBlend20, field18, 0x18);
GEORGE_OFFSET_ASSERT(GeorgeBlend20, field1C, 0x1C);
GEORGE_OFFSET_ASSERT(GeorgeBlend20, field20, 0x20);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field18, 0x18);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field1C, 0x1C);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field40, 0x40);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field44, 0x44);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field48, 0x48);
GEORGE_OFFSET_ASSERT(GeorgeFade4C, field4C, 0x4C);
#undef GEORGE_OFFSET_ASSERT

void func_001100D0(GeorgeFlags18 *object);
void func_001100E0(GeorgeFlags18 *object);
u8 *func_001102F8(GeorgeFlags18 *object, u32 *output);
float func_00111FA0(const GeorgeScalar44 *object);
const GeorgeRateInput *func_001134E8(GeorgeRate48 *object, const GeorgeRateInput *input);
GeorgeRateOutput *func_00113518(const GeorgeRate48 *object, GeorgeRateOutput *output);
float func_0011C078(const GeorgeTimer50 *object);
void func_0011C108(GeorgeTimer50 *object);
void func_0011C120(GeorgeTimer50 *object);
void func_0011C160(GeorgeTimer50 *object, float step);
const u32 *func_0011F0E8(GeorgeBlend20 *object, const u32 *input);
void func_0011FE00(GeorgeFade4C *object, u32 value);
void func_0011FFB0(GeorgeFade4C *object, float step);

#endif
