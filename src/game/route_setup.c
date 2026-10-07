#include "george/route_setup.h"

#define AT(type, base, offset) ((type *)((u32)(base) + (u32)(offset)))
#define FIELD(type, base, offset) (*AT(type, base, offset))

void func_0020BEA8(void *owner, void *state, u32 kind, u32 first, u32 second,
                   const GeorgeMathVec3 *point, const GeorgeMathVec3 *target)
{
    GeorgeMathVec3 *position = AT(GeorgeMathVec3, owner, 0xF1C);
    GeorgeMathVec3 *destination = AT(GeorgeMathVec3, owner, 0xF28);
    u32 loaded_first, loaded_second;
    FIELD(void *, owner, 0xECC) = state;
    FIELD(u16, owner, 0xF08) = 1;
    FIELD(u16, owner, 0xEEC) = 1;
    FIELD(u8, owner, 0xED1) = (u8)kind;
    FIELD(u32, owner, 0xED8) = first;
    FIELD(u32, owner, 0xEDC) = second;
    FIELD(float, owner, 0xF18) = 1000000.0f;
    FIELD(u8, owner, 0xED0) = 0xFF;
    FIELD(u8, owner, 0xED2) = 0;
    FIELD(u32, owner, 0xEE4) = 0xFFFFFFFFU;
    FIELD(u32, owner, 0xEE8) = 0xFFFFFFFFU;
    FIELD(u16, owner, 0xEEE) = 0;
    FIELD(u16, owner, 0xEF0) = 0;
    FIELD(u16, owner, 0xEF2) = 0;
    FIELD(float, owner, 0xEF4) = 1000000.0f;
    FIELD(float, owner, 0xEF8) = 1000000.0f;
    FIELD(float, owner, 0xEFC) = 1000000.0f;
    FIELD(u32, owner, 0xF00) = 0xFFFFFFFFU;
    FIELD(u32, owner, 0xF04) = 0xFFFFFFFFU;
    FIELD(u16, owner, 0xF0A) = 0;
    FIELD(u16, owner, 0xF0C) = 0;
    FIELD(u16, owner, 0xF0E) = 0;
    FIELD(float, owner, 0xF10) = 1000000.0f;
    FIELD(float, owner, 0xF14) = 1000000.0f;
    FIELD(u32, owner, 0xEE0) = 0;
    FIELD(u8, owner, 0xED5) = 0;
    FIELD(u8, owner, 0xED4) = 0;
    FIELD(u8, owner, 0xED3) = 0;
    /* Scalar copies expose earlier stores to overlapping input vectors. */
    position->x = point->x;
    position->y = point->y;
    position->z = point->z;
    destination->x = target->x;
    destination->y = target->y;
    destination->z = target->z;
    FIELD(u8, owner, 0) = 0;
    loaded_first = FIELD(u32, owner, 0xED8);
    if (loaded_first == 0xFFFFFFFFU) {
        FIELD(u8, owner, 0xED2) = 6;
        loaded_first = FIELD(u32, owner, 0xED8);
        loaded_second = FIELD(u32, owner, 0xEDC);
    } else {
        loaded_second = FIELD(u32, owner, 0xEDC);
        if (loaded_second == 0xFFFFFFFFU) {
            FIELD(u8, owner, 0xED2) = 6;
            loaded_first = FIELD(u32, owner, 0xED8);
            loaded_second = FIELD(u32, owner, 0xEDC);
        } else loaded_first = FIELD(u32, owner, 0xED8);
    }
    if (loaded_first == loaded_second) FIELD(u8, owner, 0xED2) = 8;
}

void func_0020D080(void *owner, void *state, u32 kind, u32 first, u32 second,
                   const GeorgeMathVec3 *point, const GeorgeMathVec3 *target)
{
    GeorgeMathVec3 *position = AT(GeorgeMathVec3, owner, 0xCF4);
    GeorgeMathVec3 *destination = AT(GeorgeMathVec3, owner, 0xD00);
    u32 loaded_first, loaded_second;
    FIELD(void *, owner, 0xCB0) = state;
    FIELD(u8, owner, 0xCB5) = (u8)kind;
    FIELD(u32, owner, 0xCBC) = first;
    FIELD(u32, owner, 0xCC0) = second;
    position->x = point->x;
    position->y = point->y;
    position->z = point->z;
    destination->x = target->x;
    destination->y = target->y;
    destination->z = target->z;
    FIELD(u8, owner, 0xCB6) = 0;
    FIELD(u32, owner, 0xCC4) = 0xFFFFFFFFU;
    FIELD(float, owner, 0xCD8) = 1000000.0f;
    FIELD(u32, owner, 0xCC8) = 0xFFFFFFFFU;
    FIELD(u8, owner, 0xCCC) = 1;
    FIELD(u8, owner, 0xCCD) = 0;
    FIELD(u8, owner, 0xCCE) = 0;
    FIELD(u8, owner, 0xCCF) = 0;
    FIELD(float, owner, 0xCD0) = 1000000.0f;
    FIELD(float, owner, 0xCD4) = 1000000.0f;
    FIELD(u32, owner, 0xCDC) = 0xFFFFFFFFU;
    FIELD(u8, owner, 0xCE4) = 1;
    FIELD(u32, owner, 0xCE0) = 0xFFFFFFFFU;
    FIELD(u8, owner, 0xCE5) = 0;
    FIELD(u8, owner, 0xCE6) = 0;
    FIELD(u8, owner, 0xCE7) = 0;
    FIELD(float, owner, 0xCE8) = 1000000.0f;
    FIELD(float, owner, 0xCEC) = 1000000.0f;
    FIELD(float, owner, 0xCF0) = 1000000.0f;
    FIELD(u8, owner, 0xCB4) = 0xFF;
    /* The first tag is captured before the following flag stores. */
    loaded_first = FIELD(u32, owner, 0xCBC);
    FIELD(u8, owner, 0xCB9) = 0;
    FIELD(u8, owner, 0xCB8) = 0;
    FIELD(u8, owner, 0xCB7) = 0;
    FIELD(u8, owner, 0xCBA) = 0;
    FIELD(u8, owner, 0xCBB) = 0;
    FIELD(u8, owner, 0) = 0;
    if (loaded_first == 0xFFFFFFFFU) {
        FIELD(u8, owner, 0xCB6) = 6;
        loaded_first = FIELD(u32, owner, 0xCBC);
        loaded_second = FIELD(u32, owner, 0xCC0);
    } else {
        loaded_second = FIELD(u32, owner, 0xCC0);
        if (loaded_second == 0xFFFFFFFFU) {
            FIELD(u8, owner, 0xCB6) = 6;
            loaded_first = FIELD(u32, owner, 0xCBC);
            loaded_second = FIELD(u32, owner, 0xCC0);
        } else loaded_first = FIELD(u32, owner, 0xCBC);
    }
    if (loaded_first == loaded_second) FIELD(u8, owner, 0xCB6) = 8;
}

#undef FIELD
#undef AT
