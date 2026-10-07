#include "george/segment_intersection.h"

typedef unsigned long long IntersectionBits64;
typedef char intersection_bits64_size[(sizeof(IntersectionBits64) == 8) ? 1 : -1];
extern IntersectionBits64 func_00374848(float);
extern s32 func_00373250(IntersectionBits64, IntersectionBits64);
extern IntersectionBits64 func_00372CC0(IntersectionBits64, IntersectionBits64);

#if defined(__GNUC__) && __GNUC__ >= 3
#define INTERSECTION_INLINE static __inline__ __attribute__((always_inline))
#elif defined(__GNUC__)
#define INTERSECTION_INLINE static __inline__
#else
#define INTERSECTION_INLINE static
#endif

INTERSECTION_INLINE IntersectionBits64 intersection_absolute(float value)
{
    IntersectionBits64 bits = func_00374848(value);
    if (func_00373250(bits, 0) < 0) bits = func_00372CC0(0, bits);
    return bits;
}

INTERSECTION_INLINE void intersection_cross(GeorgeMathVec3 *output,
                                            const GeorgeMathVec3 *left,
                                            const GeorgeMathVec3 *right)
{
    output->x = left->y * right->z - left->z * right->y;
    output->y = left->z * right->x - left->x * right->z;
    output->z = left->x * right->y - left->y * right->x;
}

INTERSECTION_INLINE float intersection_dot(const GeorgeMathVec3 *left,
                                          const GeorgeMathVec3 *right)
{
    return (left->x * right->x + left->y * right->y) + left->z * right->z;
}

u32 func_0029EB98(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second,
                 const GeorgeMathVec3 *a, const GeorgeMathVec3 *b,
                 const GeorgeMathVec3 *c, GeorgeMathVec3 *output)
{
    GeorgeMathVec3 ab, ac, normal, point;
    GeorgeMathVec3 pa, pb, pc, bc, ca;
    GeorgeMathVec3 cross_a, cross_b, cross_c;
    float denominator, plane_offset, fraction;

    ab.x = b->x - a->x; ab.y = b->y - a->y; ab.z = b->z - a->z;
    ac.x = c->x - a->x; ac.y = c->y - a->y; ac.z = c->z - a->z;
    intersection_cross(&normal, &ab, &ac);
    func_002A3538(&normal);

    denominator = (normal.x * (second->x - first->x)
                 + normal.y * (second->y - first->y))
                 + normal.z * (second->z - first->z);
    plane_offset = (-normal.x * a->x - normal.y * a->y) - normal.z * a->z;
    if (func_00373250(intersection_absolute(denominator),
                     0x3EE4F8B580000000ULL) < 0) return 0;

    fraction = -(((plane_offset + normal.x * first->x)
                   + normal.y * first->y) + normal.z * first->z) / denominator;
    point.x = first->x + fraction * (second->x - first->x);
    point.y = first->y + fraction * (second->y - first->y);
    point.z = first->z + fraction * (second->z - first->z);
    if (fraction < 0.0f || 1.0f < fraction) return 0;

    if (output) {
        output->z = point.z;
        output->x = point.x;
        output->y = point.y;
    }

    /* Publication may have changed a vertex. Each following vector captures
     * the freshly loaded coordinates before its actual normalization call. */
    pa.x = point.x - a->x; pa.y = point.y - a->y; pa.z = point.z - a->z;
    func_002A3538(&pa);
    pb.x = point.x - b->x; pb.y = point.y - b->y; pb.z = point.z - b->z;
    func_002A3538(&pb);
    pc.x = point.x - c->x; pc.y = point.y - c->y; pc.z = point.z - c->z;
    func_002A3538(&pc);
    ab.x = b->x - a->x; ab.y = b->y - a->y; ab.z = b->z - a->z;
    func_002A3538(&ab);
    bc.x = c->x - b->x; bc.y = c->y - b->y; bc.z = c->z - b->z;
    func_002A3538(&bc);
    ca.x = a->x - c->x; ca.y = a->y - c->y; ca.z = a->z - c->z;
    func_002A3538(&ca);
    intersection_cross(&cross_a, &pa, &ab);
    intersection_cross(&cross_b, &pb, &bc);
    intersection_cross(&cross_c, &pc, &ca);
    if (!(0.0f <= intersection_dot(&cross_a, &cross_b))) return 0;
    if (!(0.0f <= intersection_dot(&cross_a, &cross_c))) return 0;
    return 0.0f <= intersection_dot(&cross_b, &cross_c);
}

u32 func_0029F080(const GeorgePathRay *ray, const GeorgeMathVec4 *plane,
                 float *fraction)
{
    float denominator = (ray->field0C.x * plane->x + ray->field0C.y * plane->y)
                       + ray->field0C.z * plane->z;
    float distance, extent, result;
    if (func_00373250(intersection_absolute(denominator),
                     0x3F50624DE0000000ULL) < 0) return 0;
    distance = (plane->w - ((ray->field00.x * plane->x + ray->field00.y * plane->y)
                             + ray->field00.z * plane->z)) / denominator;
    extent = ray->field18;
    result = distance / extent;
    *fraction = result;
    if (!(0.0f < distance)) return 0;
    return result < 1.00100004673004150390625f; /* binary32 3F8020C5 */
}

#undef INTERSECTION_INLINE
