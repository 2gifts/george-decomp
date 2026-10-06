#ifndef GEORGE_PATH_SAMPLING_H
#define GEORGE_PATH_SAMPLING_H

#include "george/matrix_rigid.h"

/* Observed path prefix only. These names do not establish original classes. */
typedef struct GeorgePathSampling {
    u8 reserved00[0x0C];
    void *field0C;
    u8 reserved10[0x24];
    void *field34;
    u16 field38;
    u8 reserved3A[0x0A];
    float field44, field48;
} GeorgePathSampling;

typedef struct GeorgePathRecordHeader {
    u8 field00, field01;
    u8 reserved02[2];
    u16 field04;
    u8 reserved06[2];
} GeorgePathRecordHeader;

#define PATH_OFFSET(type, field, offset) \
    typedef char path_##type##_##field[(offsetof(type, field) == (offset)) ? 1 : -1]
PATH_OFFSET(GeorgePathSampling, field0C, 0x0C);
PATH_OFFSET(GeorgePathSampling, field34, 0x34);
PATH_OFFSET(GeorgePathSampling, field38, 0x38);
PATH_OFFSET(GeorgePathSampling, field44, 0x44);
PATH_OFFSET(GeorgePathSampling, field48, 0x48);
PATH_OFFSET(GeorgePathRecordHeader, field04, 4);
typedef char path_sampling_size[(sizeof(GeorgePathSampling) == 0x4C) ? 1 : -1];
typedef char path_record_header_size[(sizeof(GeorgePathRecordHeader) == 8) ? 1 : -1];
#undef PATH_OFFSET

/* Void pointer entry signatures agree with their already published callers. */
void func_00135D10(void *, GeorgeMathVec3 *, const GeorgeMathVec3 *) GEORGE_SAVE128;
float func_00135D88(void *, const GeorgeMathVec3 *) GEORGE_SAVE128;
void func_00135E88(void *, GeorgeMathVec3 *, float) GEORGE_SAVE128;
u32 func_00135F90(void *, const GeorgeMathVec3 *, float *, float *) GEORGE_SAVE128;
void func_00136020(void *, float *, u16 *, GeorgeMathVec3 *) GEORGE_SAVE128;

#endif
