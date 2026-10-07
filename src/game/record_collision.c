#include "george/record_collision.h"
#include "george/segment_distance.h"

/* Actual VU/MMI engine entries. The finite native contracts are test models;
 * they are not new production implementations of these helpers. */
extern void func_002A1C60(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);
extern void func_002A1D78(const void *, const GeorgeMathVec3 *, GeorgeMathVec3 *);

s32 func_00270A00(GeorgeRecordCollision *object,
                 const GeorgeRotationMatrix *frame,
                 const GeorgeMathVec3 *first, const GeorgeMathVec3 *second,
                 float *fraction, u32 *word, GeorgeMathVec3 *normal)
{
    GeorgeMathVec3 delta, local_first, local_delta, record_first, record_delta;
    GeorgeRotationMatrix inverse;
    GeorgeCollisionRecords *records;
    u32 index;

    if (object->field398 == 0) return 0;
    delta.x = second->x - first->x;
    delta.y = second->y - first->y;
    delta.z = second->z - first->z;
    /* The incoming second GPR argument remains live until this call. */
    func_002A1098(&inverse, frame);
    func_002A1C60(&inverse, first, &local_first);
    func_002A1D78(&inverse, &delta, &local_delta);
    records = object->field398;
    if (records->field00 == 0) return 0;

    index = 0;
    do {
        GeorgeCollisionRecord *record = &records->field04[index];
        u32 selector = object->field0C;
        s32 mapped;
        if (selector == 3) selector = 2;
        mapped = object->field388[selector][record->field00];
        if (mapped >= 0) {
            GeorgeRotationMatrix *basis = &object->field3D8[selector][mapped];
            float scale = record->field10;
            float first_fraction, second_fraction, distance, radius;
            float first_x, first_y, first_z, second_x, second_y, second_z;
            record_first.x = basis->element[12] + scale * basis->element[8];
            record_first.y = basis->element[13] + scale * basis->element[9];
            record_first.z = basis->element[14] + scale * basis->element[10];
            func_002A35C0(&record_delta,
                         (const GeorgeMathVec3 *)&basis->element[8], record->field08);
            distance = func_0029D5D0(&local_first, &local_delta,
                                    &record_first, &record_delta,
                                    &first_fraction, &second_fraction);
            radius = record->field0C;
            if (distance <= radius * radius) {
                /* All six coordinates are captured before the first output.
                 * The word is deliberately loaded after publishing fraction. */
                first_z = first->z + first_fraction * delta.z;
                first_x = first->x + first_fraction * delta.x;
                first_y = first->y + first_fraction * delta.y;
                second_x = record_first.x + second_fraction * record_delta.x;
                second_y = record_first.y + second_fraction * record_delta.y;
                second_z = record_first.z + second_fraction * record_delta.z;
                *fraction = first_fraction;
                *word = record->field04;
                normal->x = first_x - second_x;
                normal->y = first_y - second_y;
                normal->z = first_z - second_z;
                func_002A3538(normal);
                return 1;
            }
        }
        records = object->field398;
        ++index;
    } while (index < records->field00);
    return 0;
}

void func_00270C90(GeorgeRecordCollision *object, GeorgeMathVec3 *output, u32 word)
{
    u32 index;
    GeorgeCollisionRecords *records;
    output->x = 0.0f;
    output->y = 0.0f;
    output->z = 0.0f;
    records = object->field398;
    if (records == 0 || records->field00 == 0) return;
    index = 0;
    do {
        GeorgeCollisionRecord *record;
        records = object->field398;
        record = &records->field04[index];
        if (record->field04 == word) {
            u32 selector = object->field0C;
            s32 mapped;
            GeorgeRotationMatrix *basis;
            float position, x, y, z;
            if (selector == 3) selector = 2;
            mapped = object->field388[selector][record->field00];
            if (mapped < 0) return;
            basis = &object->field3D8[selector][mapped];
            position = record->field10 + record->field08 * 0.5f;
            z = basis->element[14] + position * basis->element[10];
            x = basis->element[12] + position * basis->element[8];
            y = basis->element[13] + position * basis->element[9];
            output->z = z;
            output->x = x;
            output->y = y;
            return;
        }
        ++index;
    } while (index < records->field00);
}
