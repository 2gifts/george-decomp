#include "george/file_archive.h"
#include "george/string_registry.h"
#include "george/string_algorithms.h"

extern char *func_00398628(const char *text,const char *needle);
extern const char D_004473C0[];

void func_002B0998(GeorgeFileSlot *slot)
{
    if(D_003FD240==0)
        func_002B1198(slot->field04);
    D_003FD23C=D_003FD23C-1u;
    slot->field18=0;
    slot->field00=0;
    slot->field04=0;
    slot->field08=0;
    slot->field0C=0;
    slot->field14=0;
}

void *func_002B1BA8(void *context,const char *path)
{
    /* Observed scratch begins at SP+0; saved RA begins at SP+0x40.
     * It is uninitialized and provides no safe input/output capacity claim. */
    char scratch[0x40];
    char *marker;
    const char *text;
    u32 key;
    func_002ABAE8(path,scratch);
    marker=func_00398628(scratch,D_004473C0);
    text=marker!=0?marker+7:scratch;
    key=func_0029C648((const signed char *)text);
    return func_002A7C08(((GeorgeArchiveMapView *)context)->field50,key);
}
