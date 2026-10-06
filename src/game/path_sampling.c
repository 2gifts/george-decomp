#include "george/path_sampling.h"

/* Complete supporting originals establish these lower curve contracts. They
 * remain external engine calls, with no reconstruction or matching award here.
 * 297178's f0 return is ignored by this caller. */
extern float func_00297178(void *, u16 *, float *, GeorgeMathVec3 *);
extern u32 func_002966F0(void *, const GeorgeMathVec3 *, u16 *, s32,
                        float *, float *);
extern void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define FRAME(object) ((GeorgeRotationMatrix *)ADDRESS(object, 0x50))

void func_00135D10(void *object, GeorgeMathVec3 *output,
                   const GeorgeMathVec3 *reference)
{
    GeorgePathSampling *path = (GeorgePathSampling *)object;
    float distance = 0.0f;
    func_00135F90(path, reference, &path->field48, &distance);
    if (path->field44 == -1.0f) path->field44 = path->field48;
    func_00136020(path, &path->field44, &path->field38, output);
}

float func_00135D88(void *object, const GeorgeMathVec3 *reference)
{
    GeorgePathSampling *path = (GeorgePathSampling *)object;
    GeorgePathRecordHeader *data;
    float distance = 0.0f, end, current, start;
    u32 offset;
    func_00135F90(path, reference, &path->field48, &distance);
    if (path->field44 == -1.0f) path->field44 = path->field48;
    data = (GeorgePathRecordHeader *)path->field34;
    if (data->field00 == 0x10) {
        offset = (((u32)data->field04 - 1U) * (u32)data->field01) << 2;
        end = (float)FIELD(data, offset + 0x0FU, u8) * 160.0f;
    } else {
        offset = (((u32)data->field04 - 1U) * (u32)data->field01) << 2;
        end = FIELD(data, offset + 8U, float);
    }
    current = path->field44;
    if (data->field00 == 0x10) start = (float)FIELD(data, 0x0F, u8) * 160.0f;
    else start = FIELD(data, 8, float);
    return current / (end - start);
}

void func_00135E88(void *object, GeorgeMathVec3 *output, float fraction)
{
    GeorgePathSampling *path = (GeorgePathSampling *)object;
    GeorgePathRecordHeader *data;
    float end, start, position;
    u32 offset;
    if (path->field44 == -1.0f) {
        path->field44 = fraction;
        path->field48 = fraction;
    }
    data = (GeorgePathRecordHeader *)path->field34;
    if (data->field00 == 0x10) {
        offset = (((u32)data->field04 - 1U) * (u32)data->field01) << 2;
        end = (float)FIELD(data, offset + 0x0FU, u8) * 160.0f;
    } else {
        offset = (((u32)data->field04 - 1U) * (u32)data->field01) << 2;
        end = FIELD(data, offset + 8U, float);
    }
    if (data->field00 == 0x10) start = (float)FIELD(data, 0x0F, u8) * 160.0f;
    else start = FIELD(data, 8, float);
    position = fraction * (end - start);
    func_00136020(path, &position, &path->field38, output);
}

u32 func_00135F90(void *object, const GeorgeMathVec3 *reference,
                  float *position, float *distance)
{
    GeorgePathSampling *path = (GeorgePathSampling *)object;
    GeorgeRotationMatrix inverse;
    /* Both local vectors reserve the original sixteen-byte stack region.
     * Their fourth words are not read by these selected bodies. */
    GeorgeMathVec4 local;
    func_002A1098(&inverse, FRAME(path->field0C));
    func_002A1C60(&inverse, reference, (GeorgeMathVec3 *)&local);
    return func_002966F0(path->field34, (const GeorgeMathVec3 *)&local,
                         0, 0, position, distance);
}

void func_00136020(void *object, float *position, u16 *index,
                   GeorgeMathVec3 *output)
{
    GeorgePathSampling *path = (GeorgePathSampling *)object;
    GeorgeMathVec4 local;
    func_00297178(path->field34, index, position, (GeorgeMathVec3 *)&local);
    func_002A1C60(FRAME(path->field0C), (const GeorgeMathVec3 *)&local, output);
}

#undef FRAME
#undef FIELD
#undef ADDRESS
