#include "george/render_records.h"
#include "george/heap.h"

GeorgeRenderRecord *func_002B8A68(u32 argument20,u32 argument24,float x,float y,float z)
{
    GeorgeRenderRecord *record=(GeorgeRenderRecord *)func_002AEC28(0x30);
    if (record!=NULL) func_002B8AF0(record,argument20,argument24,x,y,z);
    return record;
}

void func_002B8AF0(GeorgeRenderRecord *record,u32 argument20,u32 argument24,
                 float x,float y,float z)
{
    record->x=x;
    record->y=y;
    record->z=z;
    record->argument20=argument20;
    record->argument24=argument24;
    record->x=x;
    record->field00=0;
    record->z=z;
    record->y=y;
    record->color=0x80808080U;
    record->scale_x=1.0f;
    record->scale_z=1.0f;
    record->scale_y=1.0f;
    record->field28=0;
}
