#ifndef GEORGE_GEOMETRY_CLASSIFY_FRAME_TEMPLATE_H
#define GEORGE_GEOMETRY_CLASSIFY_FRAME_TEMPLATE_H

/* The complete five- and six-face bodies differ only in the face count and
 * final index eligible for crossing. Preserve the original strict gates. */
#define GEORGE_DEFINE_FACE_FRAME_CLASSIFY(name, count, last_crossing) \
u32 name(const GeorgeGeometryFace *faces, const GeorgeGeometryFrame *frame) \
{ \
    u32 flags = 0, index; \
    for (index = 0; index < count; ++index, ++faces) { \
        s32 result = (s32)func_0029E098(frame, faces); \
        if (result == 1) flags |= 1; \
        else if (result == 0) { if (index <= last_crossing) flags |= 2; } \
        else if (result == 2) return 0; \
    } \
    return (flags & 2) ? 2 : 1; \
}

#define GEORGE_DEFINE_FACE_SPHERE_CLASSIFY(name, count, last_crossing) \
u32 name(const GeorgeGeometryFace *faces, const GeorgeMathVec4 *sphere) \
{ \
    float radius = sphere->w; \
    float x = sphere->x, y = sphere->y, z = sphere->z; \
    u32 flags = 0, index; \
    for (index = 0; index < count; ++index, ++faces) { \
        float value = ((x * faces->normal.x + y * faces->normal.y) + \
                       z * faces->normal.z) - faces->distance; \
        if (radius < value) return 0; \
        if (-radius < value) { if (index <= last_crossing) flags |= 2; } \
        else flags |= 1; \
    } \
    return (flags & 2) ? 2 : 1; \
}

#endif
