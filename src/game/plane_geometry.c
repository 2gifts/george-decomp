#include "george/plane_geometry.h"
#include "george/ee_math.h"

/* Both complete original entries capture their six differences before the
 * first output store, use the last-minus-middle edge, and write Y/Z/X. */
#define WRITE_NORMAL(output, first, middle, last) do { \
    float ax = (middle)->x - (first)->x; \
    float ay = (middle)->y - (first)->y; \
    float az = (middle)->z - (first)->z; \
    float bx = (last)->x - (middle)->x; \
    float by = (last)->y - (middle)->y; \
    float bz = (last)->z - (middle)->z; \
    float y = az * bx - ax * bz; \
    float z = ax * by - ay * bx; \
    float x = ay * bz - az * by; \
    (output)->y = y; \
    (output)->z = z; \
    (output)->x = x; \
    func_002A3538((GeorgeMathVec3 *)(output)); \
} while (0)

GeorgeMathVec4 *func_0029C6C0(GeorgeMathVec4 *output,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *middle,
    const GeorgeMathVec3 *last)
{
    WRITE_NORMAL(output, first, middle, last);
    /* First is read again after normalization; output may overlap it. */
    output->w = -((output->x * first->x + output->y * first->y)
                  + output->z * first->z);
    return output;
}

GeorgeMathVec3 *func_0029C7B0(GeorgeMathVec3 *output,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *middle,
    const GeorgeMathVec3 *last)
{
    WRITE_NORMAL(output, first, middle, last);
    return output;
}

#undef WRITE_NORMAL

s32 func_0029CA28(const GeorgeMathVec3 *query,
    const GeorgeMathVec3 *first, const GeorgeMathVec3 *second, float radius)
{
    GeorgeMathVec3 direction;
    float x, y, z, length, projection;
    direction.x = second->x - first->x;
    direction.y = second->y - first->y;
    direction.z = second->z - first->z;
    func_002A3538(&direction);
    x = query->x - first->x;
    y = query->y - first->y;
    z = query->z - first->z;
    length = george_ee_square_root((x * x + y * y) + z * z);
    projection = (x * direction.x + y * direction.y) + z * direction.z;
    return length - radius <= projection;
}

s32 func_0029CB20(const GeorgeMathVec3 *triangle,
    const GeorgeMathVec3 *query)
{
    GeorgeMathVec3 normal;
    float ax, ay, az, bx, by, bz, cx, cy, cz;
    float abx, aby, abz, bcx, bcy, bcz, cax, cay, caz;
    float x0, y0, z0, x1, y1, z1, x2, y2, z2;
    float dot;
    func_0029C7B0(&normal, triangle, triangle + 1, triangle + 2);
    ax = triangle[0].x - query->x;
    ay = triangle[0].y - query->y;
    az = triangle[0].z - query->z;
    bx = triangle[1].x - query->x;
    by = triangle[1].y - query->y;
    bz = triangle[1].z - query->z;
    cx = triangle[2].x - query->x;
    cy = triangle[2].y - query->y;
    cz = triangle[2].z - query->z;
    abx = triangle[1].x - triangle[0].x;
    aby = triangle[1].y - triangle[0].y;
    abz = triangle[1].z - triangle[0].z;
    bcx = triangle[2].x - triangle[1].x;
    bcy = triangle[2].y - triangle[1].y;
    bcz = triangle[2].z - triangle[1].z;
    cax = triangle[0].x - triangle[2].x;
    cay = triangle[0].y - triangle[2].y;
    caz = triangle[0].z - triangle[2].z;
    x0 = ay * abz - az * aby;
    y0 = az * abx - ax * abz;
    z0 = ax * aby - ay * abx;
    x1 = by * bcz - bz * bcy;
    y1 = bz * bcx - bx * bcz;
    z1 = bx * bcy - by * bcx;
    x2 = cy * caz - cz * cay;
    y2 = cz * cax - cx * caz;
    z2 = cx * cay - cy * cax;
    dot = (normal.x * x0 + normal.y * y0) + normal.z * z0;
    if (!(0.0f < dot)) return 0;
    dot = (normal.x * x1 + normal.y * y1) + normal.z * z1;
    if (!(0.0f < dot)) return 0;
    dot = (normal.x * x2 + normal.y * y2) + normal.z * z2;
    return 0.0f < dot;
}

float func_0029D048(const GeorgeMathVec3 *origin,
    const GeorgeMathVec3 *direction, const GeorgeMathVec3 *query,
    float *parameter)
{
    float ox = origin->x, oy = origin->y, oz = origin->z;
    float dx = direction->x, dy = direction->y, dz = direction->z;
    float qx = query->x, qy = query->y, qz = query->z;
    float x = qx - ox, y = qy - oy, z = qz - oz;
    float along = (dx * x + dy * y) + dz * z;
    x = (ox + along * dx) - qx;
    y = (oy + along * dy) - qy;
    z = (oz + along * dz) - qz;
    if (parameter != 0) *parameter = along;
    return (x * x + y * y) + z * z;
}

void func_0029E448(GeorgeBounds *bounds, const GeorgeMathVec3 *point)
{
    float x = bounds->lower.x < point->x ? bounds->lower.x : point->x;
    float y = bounds->lower.y < point->y ? bounds->lower.y : point->y;
    float z = bounds->lower.z < point->z ? bounds->lower.z : point->z;
    bounds->lower.x = x;
    bounds->lower.y = y;
    bounds->lower.z = z;
    /* Read the point again after the lower stores, including shifted aliases. */
    x = point->x < bounds->upper.x ? bounds->upper.x : point->x;
    y = point->y < bounds->upper.y ? bounds->upper.y : point->y;
    z = point->z < bounds->upper.z ? bounds->upper.z : point->z;
    bounds->upper.x = x;
    bounds->upper.z = z;
    bounds->upper.y = y;
}

s32 func_0029E720(const GeorgeBounds *bounds, const GeorgeMathVec4 *sphere)
{
    float distance = 0.0f, difference;
    if (sphere->x < bounds->lower.x) {
        difference = sphere->x - bounds->lower.x;
        distance = difference * difference;
    } else if (bounds->upper.x < sphere->x) {
        difference = sphere->x - bounds->upper.x;
        distance = difference * difference;
    }
    if (sphere->y < bounds->lower.y) {
        difference = sphere->y - bounds->lower.y;
        distance += difference * difference;
    } else if (bounds->upper.y < sphere->y) {
        difference = sphere->y - bounds->upper.y;
        distance += difference * difference;
    }
    if (sphere->z < bounds->lower.z) {
        difference = sphere->z - bounds->lower.z;
        distance += difference * difference;
    } else if (bounds->upper.z < sphere->z) {
        difference = sphere->z - bounds->upper.z;
        distance += difference * difference;
    }
    return !(sphere->w * sphere->w < distance);
}
