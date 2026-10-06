#ifndef GEORGE_RENDER_RECORDS_H
#define GEORGE_RENDER_RECORDS_H

#include "george/types.h"
#include "george/compiler.h"

/* Allocation size is 48 bytes. Only these scalar offsets are established;
 * reserved2C remains untouched by the recovered initializer. */
typedef struct GeorgeRenderRecord {
    u32 field00;
    float x,y,z;
    float scale_x,scale_y,scale_z;
    u32 color;
    u32 argument20,argument24;
    u32 field28,reserved2C;
} GeorgeRenderRecord;
typedef char render_record_size[(sizeof(GeorgeRenderRecord)==0x30)?1:-1];
typedef char render_record_arguments[(offsetof(GeorgeRenderRecord,argument20)==0x20)?1:-1];
typedef char render_record_tail[(offsetof(GeorgeRenderRecord,reserved2C)==0x2C)?1:-1];

GeorgeRenderRecord *func_002B8A68(u32 argument20,u32 argument24,float x,float y,float z) GEORGE_SAVE128;
void func_002B8AF0(GeorgeRenderRecord *,u32 argument20,u32 argument24,float x,float y,float z);

#endif
