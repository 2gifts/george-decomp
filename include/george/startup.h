#ifndef GEORGE_STARTUP_H
#define GEORGE_STARTUP_H

#include "george/compiler.h"
#include "george/types.h"

/* Reviewed prefixes only; names and complete source classes remain unknown. */
typedef struct GeorgeStartupNamed {
    char name00[0x80];
    const void *field80;
} GeorgeStartupNamed;

typedef struct GeorgeStartupBase {
    u8 unknown00[4];
    const void *field04;
} GeorgeStartupBase;

typedef struct GeorgeStartupName7 {
    u32 field00;
    s16 field04;
    signed char field06;
} GeorgeStartupName7;

typedef int (*GeorgeStartupCompare)(const void *left, const void *right);

/* Each command argument has a 16-bit tag followed by a 32-bit payload at +4. */
typedef struct GeorgeStartupArgument {
    s16 field00;
    s16 field02;
    union {
        u32 word;
        float scalar;
        u32 address;
    } field04;
} GeorgeStartupArgument;

typedef struct GeorgeStartupVector2234 {
    u8 unknown00[0x222C];
    float field222C;
    float field2230;
    float field2234;
} GeorgeStartupVector2234;

typedef char george_startup_arg_size[(sizeof(GeorgeStartupArgument) == 8) ? 1 : -1];
typedef char george_startup_arg_payload[(offsetof(GeorgeStartupArgument, field04) == 4) ? 1 : -1];
typedef char george_startup_named_table[(offsetof(GeorgeStartupNamed, field80) == 0x80) ? 1 : -1];
typedef char george_startup_vector_x[(offsetof(GeorgeStartupVector2234, field222C) == 0x222C) ? 1 : -1];
typedef char george_startup_vector_y[(offsetof(GeorgeStartupVector2234, field2230) == 0x2230) ? 1 : -1];
typedef char george_startup_vector_z[(offsetof(GeorgeStartupVector2234, field2234) == 0x2234) ? 1 : -1];

void **func_00100C30(void **begin, void **end, void *const *key, GeorgeStartupCompare compare) GEORGE_SAVE128;
void func_00100CD0(GeorgeStartupBase *object, u32 flags) GEORGE_SAVE128;
void func_00100D50(GeorgeStartupNamed *object) GEORGE_SAVE128;
void func_00100D78(GeorgeStartupNamed *object) GEORGE_SAVE128;
s32 func_00100DE0(const void *left, const void *right) GEORGE_SAVE128;
void func_00100E30(void) GEORGE_SAVE128;
GeorgeStartupNamed *func_00100EA8(const char *name, u32 *index) GEORGE_SAVE128;
GeorgeStartupNamed *func_00100F78(GeorgeStartupNamed *object) GEORGE_SAVE128;
void func_00100FD8(GeorgeStartupNamed *object, u32 flags) GEORGE_SAVE128;
void func_00101070(GeorgeStartupNamed *object, const char *name) GEORGE_SAVE128;
void func_00101818(void *unused, s32 index) GEORGE_SAVE128;
void func_001018D0(void *unused, s32 index) GEORGE_SAVE128;

#endif
