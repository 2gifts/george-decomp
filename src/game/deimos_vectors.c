#include "george/deimos_vectors.h"

extern GeorgeDeimosPoolNode *D_003FDBA0;

/* Only types 0 and 1 consume the payload word. Unknown types intentionally
 * leave that word to be interpreted as the next key in the original stream. */
GeorgeDeimosPoolNode *func_002CFFC0(const u32 *records)
{
    GeorgeDeimosPoolNode *table = func_002CD348(0);
    GeorgeDeimosValue value;
    while (*records != 0) {
        u32 key = *records++;
        signed char type = *(const signed char *)records++;
        if (type == 0) {
            value.tag = 1;
            value.subtype = 0;
            value.payload.bits = *records++;
            func_002CCB10(table, key, &value);
        } else if (type == 1) {
            value.tag = 2;
            value.subtype = 0;
            value.payload.scalar = *(const float *)records++;
            func_002CCB10(table, key, &value);
        }
    }
    return table;
}

/* Lookup all four values before loading their payloads. Output may alias
 * table storage; capture the three scalar payloads before any output store. */
void func_002D00A0(GeorgeDeimosHashTable *table, GeorgeMathVec3 *vector, float *angle)
{
    GeorgeDeimosValue *x_value = func_002CD990(table, 0x8CDC1683u);
    GeorgeDeimosValue *y_value = func_002CD990(table, 0xFBDB2615u);
    GeorgeDeimosValue *z_value = func_002CD990(table, 0x62D277AFu);
    GeorgeDeimosValue *angle_value = func_002CD990(table, 0x00814509u);
    float x = x_value->payload.scalar;
    float y = y_value->payload.scalar;
    float z = z_value->payload.scalar;
    vector->x = x;
    vector->z = z;
    vector->y = y;
    if (angle != 0) {
        *angle = angle_value == 0 ? 0.0f : angle_value->payload.scalar * 0.01745329424738884f;
    }
}

GeorgeDeimosPoolNode *func_002D0178(const GeorgeMathVec3 *vector, float angle)
{
    GeorgeDeimosPoolNode *table = func_002CD348(0);
    GeorgeDeimosValue value;
    value.tag = 2;
    value.subtype = 0;
    value.payload.scalar = vector->x;
    func_002CCB10(table, 0x8CDC1683u, &value);
    value.tag = 2;
    value.subtype = 0;
    value.payload.scalar = vector->y;
    func_002CCB10(table, 0xFBDB2615u, &value);
    value.tag = 2;
    value.subtype = 0;
    value.payload.scalar = vector->z;
    func_002CCB10(table, 0x62D277AFu, &value);
    value.tag = 2;
    value.subtype = 0;
    value.payload.scalar = angle;
    func_002CCB10(table, 0x00814509u, &value);
    return table;
}

void func_002CFE18(void)
{
    if (D_003FDBA0 != 0) {
        func_002CD130(D_003FDBA0);
        D_003FDBA0 = 0;
    }
}
