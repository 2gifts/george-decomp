#ifndef GEORGE_GEOMETRY_CLASSIFY_TEMPLATE_H
#define GEORGE_GEOMETRY_CLASSIFY_TEMPLATE_H

/* Both entire original 344-byte bodies differ only in the count and last
 * plane eligible to set bit two. No engine helper call is introduced. */
#define GEORGE_DEFINE_FACE_BOX_CLASSIFY(name, count, last) \
u32 name(const GeorgeGeometryFace *faces, const GeorgeBounds *bounds) \
{ \
    u32 flags = 0, index; \
    for (index = 0; index < (count); ++index, ++faces) { \
        GeorgeMathVec3 first, second; \
        float nx, ny, nz, distance, value; \
        if (0.0f <= faces->normal.x) { \
            second.x = bounds->upper.x; first.x = bounds->lower.x; \
        } else { first.x = bounds->upper.x; second.x = bounds->lower.x; } \
        if (0.0f <= faces->normal.y) { \
            first.y = bounds->lower.y; second.y = bounds->upper.y; \
        } else { first.y = bounds->upper.y; second.y = bounds->lower.y; } \
        if (0.0f <= faces->normal.z) { \
            first.z = bounds->lower.z; second.z = bounds->upper.z; \
        } else { first.z = bounds->upper.z; second.z = bounds->lower.z; } \
        nx = faces->normal.x; ny = faces->normal.y; \
        nz = faces->normal.z; distance = faces->distance; \
        value = (first.x * nx + first.y * ny) + first.z * nz; \
        if (distance < value) return 0; \
        value = (second.x * nx + second.y * ny) + second.z * nz; \
        if (value < distance) flags |= 1; \
        else if (index <= (last)) flags |= 2; \
    } \
    return (flags & 2) ? 2 : 1; \
}

#endif
