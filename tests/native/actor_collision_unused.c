/* Link closure for unused functions in genuine separate translation units.
 * Every unused engine hook fails immediately if reached. No helper used by
 * the selected collision paths is substituted here or receives an award. */
#include <stdio.h>
#include <stdlib.h>
#define UNUSED(name) void name(void) { fputs("unexpected unused engine hook: " #name "\n",stderr); abort(); }
UNUSED(func_002AEC28)
UNUSED(func_002AEE40)
UNUSED(func_002AF100)
UNUSED(func_0029B940)
UNUSED(func_0029C168)
UNUSED(func_0029C090)
UNUSED(func_001868F8)
UNUSED(func_003936A0)
UNUSED(func_00177E48)
UNUSED(func_00173818)
UNUSED(func_00174500)
UNUSED(func_001915D8)
UNUSED(func_001DC468)
UNUSED(func_001DFEB0)
UNUSED(func_00397178)
UNUSED(func_00211BA8)
UNUSED(func_00239FD8)
UNUSED(func_00238BA0)
UNUSED(func_001779F8)
UNUSED(func_002D02A8)
UNUSED(func_001B67C0)
UNUSED(func_0018FCF0)
UNUSED(func_001BAAD8)
UNUSED(func_002CD130)
UNUSED(func_00192368)
UNUSED(func_001CB180)
UNUSED(func_001F6148)
UNUSED(func_00120B68)
unsigned char D_0043A2B0[128],D_004371B8[128],D_00436CE8[128],D_00436E48[128],D_00436DC0[128];
unsigned char D_004368A0[128],D_00436818[128],D_0045C640[128],D_004375A0[128],D_00437390[128];
#undef UNUSED
