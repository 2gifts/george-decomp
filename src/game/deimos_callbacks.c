#include "george/deimos.h"

extern GeorgeDeimosValue *D_00474F48;
extern GeorgeDeimosValue D_00474748[];
extern const char D_00448CE8[];
extern const char D_00448D60[];
extern void func_002CC938(const char *format, ...);
/* Identified as newlib fmodf by its own exception name and complete wrapper. */
extern float func_0037B238(float dividend, float divisor);

/* DVDist: table arguments, x/y/z hash lookups, and an EE scalar square root. */
void func_002CF298(s32 unused, s32 destination)
{
    GeorgeDeimosHashTable *table;
    GeorgeDeimosValue *x_value;
    GeorgeDeimosValue *y_value;
    GeorgeDeimosValue *z_value;
    float first_x, first_y, first_z;
    float dx, dy, dz;
    float distance;
    (void)unused;

    if (D_00474F48[0].tag != 4 || D_00474F48[1].tag != 4) {
        func_002CC938(D_00448CE8);
    }
    table = D_00474F48[0].payload.pointer;
    x_value = func_002CD990(table, 0x8CDC1683u);
    y_value = func_002CD990(table, 0xFBDB2615u);
    z_value = func_002CD990(table, 0x62D277AFu);
    /* The original also performs this lookup although its return is unused. */
    func_002CD990(table, 0x00814509u);
    first_x = x_value->payload.scalar;
    first_y = y_value->payload.scalar;
    first_z = z_value->payload.scalar;

    table = D_00474F48[1].payload.pointer;
    x_value = func_002CD990(table, 0x8CDC1683u);
    y_value = func_002CD990(table, 0xFBDB2615u);
    z_value = func_002CD990(table, 0x62D277AFu);
    func_002CD990(table, 0x00814509u);
    dx = x_value->payload.scalar - first_x;
    dy = y_value->payload.scalar - first_y;
    dz = z_value->payload.scalar - first_z;
    distance = __builtin_sqrtf((dx * dx + dy * dy) + dz * dz);
    if (destination != -1) {
        D_00474748[destination].payload.scalar = distance;
        D_00474748[destination].tag = 2;
        D_00474748[destination].subtype = 0;
    }
}

/* DFMod: if the logger returns after a type error, calculation still follows. */
void func_002D05E0(s32 unused, s32 destination)
{
    float result;
    (void)unused;
    if (D_00474F48[0].tag != 2 || D_00474F48[1].tag != 2) {
        func_002CC938(D_00448D60);
    }
    result = func_0037B238(D_00474F48[0].payload.scalar, D_00474F48[1].payload.scalar);
    if (destination != -1) {
        D_00474748[destination].payload.scalar = result;
        D_00474748[destination].tag = 2;
        D_00474748[destination].subtype = 0;
    }
}
