#ifndef GEORGE_RECORD_COLLISION_H
#define GEORGE_RECORD_COLLISION_H

#include "george/matrix_rigid.h"
#include "george/vector_math.h"

/* Observed prefixes only; numeric fields do not establish original classes. */
typedef struct GeorgeCollisionRecord {
    u32 field00, field04;
    float field08, field0C, field10;
} GeorgeCollisionRecord;
typedef struct GeorgeCollisionRecords {
    u32 field00;
    GeorgeCollisionRecord field04[1];
} GeorgeCollisionRecords;
typedef struct GeorgeRecordCollision {
    u8 reserved00[0x0C];
    u32 field0C;
    u8 reserved10[0x378];
    s16 *field388[4];
    GeorgeCollisionRecords *field398;
    u8 reserved39C[0x3C];
    GeorgeRotationMatrix *field3D8[4];
} GeorgeRecordCollision;

#define RECORD_OFFSET(type, field, value) \
    typedef char record_##type##_##field[(offsetof(type, field) == (value)) ? 1 : -1]
RECORD_OFFSET(GeorgeRecordCollision, field0C, 0x0C);
RECORD_OFFSET(GeorgeRecordCollision, field388, 0x388);
RECORD_OFFSET(GeorgeRecordCollision, field398, 0x398);
RECORD_OFFSET(GeorgeRecordCollision, field3D8, 0x3D8);
RECORD_OFFSET(GeorgeCollisionRecords, field04, 4);
typedef char collision_record_size[(sizeof(GeorgeCollisionRecord) == 20) ? 1 : -1];
#undef RECORD_OFFSET

s32 func_00270A00(GeorgeRecordCollision *, const GeorgeRotationMatrix *,
                 const GeorgeMathVec3 *, const GeorgeMathVec3 *, float *,
                 u32 *, GeorgeMathVec3 *) GEORGE_SAVE128;
void func_00270C90(GeorgeRecordCollision *, GeorgeMathVec3 *, u32);

#endif
