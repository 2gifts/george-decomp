#include "george/startup.h"

/*
 * Address names and observed offsets are preserved. These routines operate on
 * a named-object registry and tagged command arguments used near startup.
 * Exact linked-byte verification, not an object-file resemblance, establishes
 * a match. See config/functions/startup_services.json for individual status.
 */

extern void *D_003F21AC;
extern u32 D_003F2204;
extern u32 D_003F2208;
extern GeorgeStartupNamed *D_00457D60[];
extern const u8 D_004200E0[];
extern const u8 D_004200F8[];
extern const GeorgeStartupName7 D_00420120;
extern const u8 D_00420128[];
extern GeorgeStartupArgument *D_00474F48;
extern GeorgeStartupArgument D_00474748[];
extern const char D_004202F8[];
extern const char D_00420330[];
extern float D_FLT_003F2C38[3];

extern void func_002BB530(void *object, u32 flags);
extern void func_002AF100(void *object);
extern const char *func_001010B8(const GeorgeStartupNamed *object);
extern s32 func_00393A28(const char *left, const char *right);
extern char *func_00393B74(char *destination, const char *source);
extern char *func_003984D8(char *text);
extern char *func_00398628(const char *text, const char *search);
extern void func_00396788(void *base, u32 count, u32 width, GeorgeStartupCompare compare);
extern GeorgeStartupVector2234 *func_002CDF90(void);
extern void func_002CC938(const char *message);

/* 0x00100C30: upper-bound search over 4-byte entries with an indirect comparator. */
void **func_00100C30(void **begin, void **end, void *const *key, GeorgeStartupCompare compare)
{
    s32 count = (s32)(end - begin);

    while (count > 0) {
        s32 half = count >> 1;
        void **middle = begin + half;
        if (compare(*key, *middle) != 0) {
            count = half;
        } else {
            begin = middle + 1;
            count = count - half - 1;
        }
    }
    return begin;
}

/* 0x00100CD0: base cleanup, global release, and optional deletion. */
void func_00100CD0(GeorgeStartupBase *object, u32 flags)
{
    object->field04 = D_004200E0;
    if (D_003F21AC != 0) {
        func_002BB530(D_003F21AC, 3);
    }
    D_003F2204 = 1;
    D_003F21AC = 0;
    object->field04 = D_004200F8;
    if ((flags & 1) != 0) {
        func_002AF100(object);
    }
}

/* 0x00100D50: append a named object; original has no capacity check. */
void func_00100D50(GeorgeStartupNamed *object)
{
    D_00457D60[D_003F2208++] = object;
}

/* 0x00100D78: unordered erase, replacing the found entry with the last entry. */
void func_00100D78(GeorgeStartupNamed *object)
{
    u32 index;
    for (index = 0; index < D_003F2208; ++index) {
        if (D_00457D60[index] == object) {
            --D_003F2208;
            if (index != D_003F2208) {
                D_00457D60[index] = D_00457D60[D_003F2208];
            }
            return;
        }
    }
}

/* 0x00100DE0: comparator dereferences entries and negates the string comparison. */
s32 func_00100DE0(const void *left, const void *right)
{
    const GeorgeStartupNamed *first = *(const GeorgeStartupNamed *const *)left;
    const char *second_name = func_001010B8(*(const GeorgeStartupNamed *const *)right);
    const char *first_name = func_001010B8(first);
    /* unsigned negation preserves the original 32-bit negu even at INT_MIN. */
    return (s32)(0u - (u32)func_00393A28(second_name, first_name));
}

/* 0x00100E30: sort registered pointers with the observed name comparator. */
void func_00100E30(void)
{
    func_00396788(D_00457D60, D_003F2208, 4, func_00100DE0);
}

/* 0x00100EA8: lowercase both strings and return the first substring match. */
GeorgeStartupNamed *func_00100EA8(const char *name, u32 *index)
{
    char search[0x200];
    char candidate[0x200];
    u32 current;

    func_00393B74(search, name);
    func_003984D8(search);
    for (current = 0; current < D_003F2208; ++current) {
        func_00393B74(candidate, func_001010B8(D_00457D60[current]));
        func_003984D8(candidate);
        if (func_00398628(candidate, search) != 0) {
            if (index != 0) {
                *index = current;
            }
            return D_00457D60[current];
        }
    }
    return 0;
}

/* 0x00100F78: construct the name prefix and register the object. */
GeorgeStartupNamed *func_00100F78(GeorgeStartupNamed *object)
{
    object->field80 = D_00420128;
    func_00101070(object, 0);
    D_00457D60[D_003F2208++] = object;
    return object;
}

/* 0x00100FD8: reset the observed table pointer, unregister, optionally delete. */
void func_00100FD8(GeorgeStartupNamed *object, u32 flags)
{
    u32 index;

    object->field80 = D_00420128;
    for (index = 0; index < D_003F2208; ++index) {
        if (D_00457D60[index] == object) {
            --D_003F2208;
            if (index != D_003F2208) {
                D_00457D60[index] = D_00457D60[D_003F2208];
            }
            break;
        }
    }
    if ((flags & 1) != 0) {
        func_002AF100(object);
    }
}

/* 0x00101070: copy a supplied name or the seven bytes of "noname\0". */
void func_00101070(GeorgeStartupNamed *object, const char *name)
{
    if (name != 0) {
        func_00393B74(object->name00, name);
    } else {
        /* The original uses three width-specific accesses, not a byte loop. */
        GeorgeStartupName7 *prefix = (GeorgeStartupName7 *)object->name00;
        prefix->field00 = D_00420120.field00;
        prefix->field04 = D_00420120.field04;
        prefix->field06 = D_00420120.field06;
    }
}

/* 0x00101818: require three tag-2 scalars, update a vector, consume an index. */
void func_00101818(void *unused, s32 index)
{
    (void)unused;
    if (D_00474F48[0].field00 == 2 &&
        D_00474F48[1].field00 == 2 &&
        D_00474F48[2].field00 == 2) {
        GeorgeStartupVector2234 *object = func_002CDF90();
        object->field222C = D_00474F48[0].field04.scalar;
        object->field2230 = D_00474F48[1].field04.scalar;
        object->field2234 = D_00474F48[2].field04.scalar;
    } else {
        func_002CC938(D_004202F8);
    }
    if (index != -1) {
        D_00474748[index].field02 = 0;
        D_00474748[index].field00 = 0;
    }
}

/* 0x001018D0: normalize three tag-2 scalars by 255 and consume an index. */
void func_001018D0(void *unused, s32 index)
{
    (void)unused;
    if (D_00474F48[0].field00 == 2 &&
        D_00474F48[1].field00 == 2 &&
        D_00474F48[2].field00 == 2) {
        /* Rounded single value has original bits 0x3B808081. */
        const float scale = 1.0f / 255.0f;
        float third = D_00474F48[2].field04.scalar;
        float first = D_00474F48[0].field04.scalar;
        float second = D_00474F48[1].field04.scalar;
        third *= scale;
        first *= scale;
        second *= scale;
        D_FLT_003F2C38[2] = third;
        D_FLT_003F2C38[0] = first;
        D_FLT_003F2C38[1] = second;
    } else {
        func_002CC938(D_00420330);
    }
    if (index != -1) {
        D_00474748[index].field02 = 0;
        D_00474748[index].field00 = 0;
    }
}
