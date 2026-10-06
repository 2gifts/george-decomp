#include "george/curve_query.h"
#include "george/ee_math.h"

float func_0029CF28(const GeorgeMathVec3 *first,
                    const GeorgeMathVec3 *second,
                    const GeorgeMathVec3 *point, float *fraction)
{
    float dx = second->x - first->x;
    float dy = second->y - first->y;
    float dz = second->z - first->z;
    float x = point->x - first->x;
    float y = point->y - first->y;
    float z = point->z - first->z;
    float along = (dx * x + dy * y) + dz * z;
    float position;

    if (0.0f < along) {
        float square = (dx * dx + dy * dy) + dz * dz;
        if (along < square) {
            position = along / square;
            x = x - position * dx;
            y = y - position * dy;
            z = z - position * dz;
        } else {
            position = 1.0f;
            x = x - dx;
            y = y - dy;
            z = z - dz;
        }
    } else {
        position = 0.0f;
    }
    if (fraction != 0)
        *fraction = position;
    return (x * x + y * y) + z * z;
}

u32 func_0029DD60(const GeorgeMathVec4 *query, const GeorgePathRay *ray)
{
    float x = query->x - ray->field00.x;
    float y = query->y - ray->field00.y;
    float z = query->z - ray->field00.z;
    float radius_square = query->w * query->w;
    float distance_square = (x * x + y * y) + z * z;
    float along, perpendicular_square, limit, entry;

    if (distance_square < radius_square)
        return 1;
    along = (x * ray->field0C.x + y * ray->field0C.y)
            + z * ray->field0C.z;
    if (along < 0.0f)
        return 0;
    perpendicular_square = distance_square - along * along;
    if (radius_square < perpendicular_square)
        return 0;
    limit = ray->field18;
    entry = along - george_ee_square_root(radius_square - perpendicular_square);
    return entry < limit;
}
