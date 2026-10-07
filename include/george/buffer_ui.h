#ifndef GEORGE_BUFFER_UI_H
#define GEORGE_BUFFER_UI_H

#include "george/compiler.h"
#include "george/buffer_manager.h"

/* Only byte offsets used by the selected routines are named. The opaque
 * 0x10..0x20F span does not declare text or completion capacities. */
typedef struct GeorgeBufferUi {
    GeorgeBufferManager *field00;
    float field04;
    u32 field08;
    s32 field0C;
    u8 unknown10[0x200];
    u32 field210;
    u8 field214;
} GeorgeBufferUi;

typedef char buffer_ui_field04[(offsetof(GeorgeBufferUi, field04) == 4) ? 1 : -1];
typedef char buffer_ui_field0C[(offsetof(GeorgeBufferUi, field0C) == 0xC) ? 1 : -1];
typedef char buffer_ui_field210[(offsetof(GeorgeBufferUi, field210) == 0x210) ? 1 : -1];
typedef char buffer_ui_field214[(offsetof(GeorgeBufferUi, field214) == 0x214) ? 1 : -1];

extern GeorgeBufferUi *D_003F9408;
extern u8 *D_004683AC;

void func_002163E8(void) GEORGE_SAVE128;
void func_002167D8(float delta) GEORGE_SAVE128;
void func_002169C0(void) GEORGE_SAVE128;

#endif
