#ifndef GEORGE_MATH_HELPERS_H
#define GEORGE_MATH_HELPERS_H

#include "george/types.h"

typedef struct GeorgeMathVec3 { float x, y, z; } GeorgeMathVec3;
typedef struct GeorgeMathVec4 { float x, y, z, w; } GeorgeMathVec4;

/* Object prefixes use observed byte offsets. Guest pointers are explicit u32. */
typedef struct GeorgeMathDirectionsFC {
    u8 unknown00[0xE4];
    GeorgeMathVec3 fieldE4;
    GeorgeMathVec3 fieldF0;
} GeorgeMathDirectionsFC;

typedef struct GeorgeMathScaled16C {
    u8 unknown00[0x1C];
    float field1C;
    u8 unknown20[0xF4];
    GeorgeMathVec3 field114;
    u8 unknown120[0x40];
    GeorgeMathVec3 field160;
} GeorgeMathScaled16C;

typedef struct GeorgeMathAngularAC {
    u8 unknown00[0x48];
    float field48;
    u8 unknown4C[0x44];
    float field90;
    u8 unknown94[4];
    float field98;
    u8 unknown9C[0x0C];
    u32 fieldA8;
} GeorgeMathAngularAC;

typedef struct GeorgeMathColors40 {
    u8 unknown00[0x28];
    GeorgeMathVec3 field28;
    GeorgeMathVec3 field34;
} GeorgeMathColors40;

typedef struct GeorgeMathColors18 {
    GeorgeMathVec3 field00;
    GeorgeMathVec3 field0C;
} GeorgeMathColors18;

typedef struct GeorgeMathSign258 {
    u8 unknown00[0x0C];
    GeorgeMathVec3 field0C;
    u8 unknown18[0x28];
    GeorgeMathVec3 field40;
    u8 unknown4C[0x200];
    float field24C;
    u8 unknown250[4];
    u32 field254;
} GeorgeMathSign258;

typedef struct GeorgeMathSign268 {
    u8 unknown00[0x0C];
    GeorgeMathVec3 field0C;
    u8 unknown18[0x28];
    GeorgeMathVec3 field40;
    u8 unknown4C[0x208];
    float field254;
    u8 unknown258[0x0C];
    u32 field264;
} GeorgeMathSign268;

typedef struct GeorgeMathDifferenceA0 {
    u8 unknown00[0x4C];
    GeorgeMathVec3 field4C;
    u8 unknown58[0x3C];
    GeorgeMathVec3 field94;
} GeorgeMathDifferenceA0;

typedef struct GeorgeMathSum24 {
    u8 unknown00[0x0C];
    GeorgeMathVec3 field0C;
    GeorgeMathVec3 field18;
} GeorgeMathSum24;

typedef struct GeorgeMathSync10 {
    u8 unknown00[8];
    u32 field08;
    u32 field0C;
} GeorgeMathSync10;

typedef struct GeorgeMathSumSync64 {
    u8 unknown00[0x0C];
    GeorgeMathVec3 field0C;
    GeorgeMathVec3 field18;
    u8 unknown24[0x3C];
    u32 field60;
} GeorgeMathSumSync64;

typedef struct GeorgeMathPosition48 {
    u8 unknown00[0x44];
    float field44;
} GeorgeMathPosition48;

typedef struct GeorgeMathGravity78 {
    u8 unknown00[0x18];
    u32 field18;
    u8 unknown1C[0x54];
    float field70;
    float field74;
} GeorgeMathGravity78;

#define MATH_OFFSET(type, field, offset) \
    typedef char math_offset_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
MATH_OFFSET(GeorgeMathDirectionsFC, fieldE4, 0xE4);
MATH_OFFSET(GeorgeMathDirectionsFC, fieldF0, 0xF0);
MATH_OFFSET(GeorgeMathScaled16C, field1C, 0x1C);
MATH_OFFSET(GeorgeMathScaled16C, field114, 0x114);
MATH_OFFSET(GeorgeMathScaled16C, field160, 0x160);
MATH_OFFSET(GeorgeMathAngularAC, field48, 0x48);
MATH_OFFSET(GeorgeMathAngularAC, field90, 0x90);
MATH_OFFSET(GeorgeMathAngularAC, field98, 0x98);
MATH_OFFSET(GeorgeMathAngularAC, fieldA8, 0xA8);
MATH_OFFSET(GeorgeMathColors40, field28, 0x28);
MATH_OFFSET(GeorgeMathColors40, field34, 0x34);
MATH_OFFSET(GeorgeMathColors18, field0C, 0x0C);
MATH_OFFSET(GeorgeMathSign258, field0C, 0x0C);
MATH_OFFSET(GeorgeMathSign258, field40, 0x40);
MATH_OFFSET(GeorgeMathSign258, field24C, 0x24C);
MATH_OFFSET(GeorgeMathSign258, field254, 0x254);
MATH_OFFSET(GeorgeMathSign268, field0C, 0x0C);
MATH_OFFSET(GeorgeMathSign268, field40, 0x40);
MATH_OFFSET(GeorgeMathSign268, field254, 0x254);
MATH_OFFSET(GeorgeMathSign268, field264, 0x264);
MATH_OFFSET(GeorgeMathDifferenceA0, field4C, 0x4C);
MATH_OFFSET(GeorgeMathDifferenceA0, field94, 0x94);
MATH_OFFSET(GeorgeMathSum24, field0C, 0x0C);
MATH_OFFSET(GeorgeMathSum24, field18, 0x18);
MATH_OFFSET(GeorgeMathSync10, field08, 8);
MATH_OFFSET(GeorgeMathSync10, field0C, 12);
MATH_OFFSET(GeorgeMathSumSync64, field18, 0x18);
MATH_OFFSET(GeorgeMathSumSync64, field60, 0x60);
MATH_OFFSET(GeorgeMathPosition48, field44, 0x44);
MATH_OFFSET(GeorgeMathGravity78, field18, 0x18);
MATH_OFFSET(GeorgeMathGravity78, field70, 0x70);
MATH_OFFSET(GeorgeMathGravity78, field74, 0x74);
#undef MATH_OFFSET

float func_00119EE8(const GeorgeMathDirectionsFC *object, s32 direction, const GeorgeMathVec3 *vector);
u32 func_0011A568(const GeorgeMathVec3 *left, const GeorgeMathVec3 *right, float tolerance);
void func_00132BC8(GeorgeMathScaled16C *object);
u32 func_001348F8(void *unused, const GeorgeMathVec3 *vector);
float func_00141590(const GeorgeMathAngularAC *object);
void func_00162650(GeorgeMathColors40 *object);
void func_00166250(GeorgeMathVec3 *object);
void func_00166C00(GeorgeMathColors18 *object);
u32 func_0016ED28(const GeorgeMathSign258 *object);
void func_0018F7A8(const GeorgeMathDifferenceA0 *object, GeorgeMathVec3 *output);
u32 func_0019CBD0(const GeorgeMathSign268 *object);
u32 func_0019D740(const GeorgeMathSign268 *object);
void func_001A1450(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector);
void func_001A1488(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector);
void func_001A14F0(const GeorgeMathSum24 *object, GeorgeMathVec3 *output);
void func_001A21C0(GeorgeMathGravity78 *object, float step);
void func_001A5C40(GeorgeMathSum24 *object, const GeorgeMathVec3 *vector);
void func_001A5C78(GeorgeMathSumSync64 *object, const GeorgeMathVec3 *vector);
void func_001A5CF8(const GeorgeMathSum24 *object, GeorgeMathVec3 *output);
void func_001CDEB0(GeorgeMathVec3 *position, const GeorgeMathVec4 *plane);

#endif
