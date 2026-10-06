#include "george/geometry_classify.h"
#include "geometry_classify_template.h"
#include "geometry_classify_frame_template.h"

/* The established 64-bit soft arithmetic representation is an integer bit
 * pattern, passed in one EE register. Host tests substitute these calls. */
typedef unsigned long long GeometryBits64;
typedef char geometry_bits64_size[(sizeof(GeometryBits64) == 8) ? 1 : -1];
extern GeometryBits64 func_00374848(float value);
extern s32 func_00373250(GeometryBits64 left, GeometryBits64 right);
extern GeometryBits64 func_00372CC0(GeometryBits64 left, GeometryBits64 right);
extern GeometryBits64 func_00372C68(GeometryBits64 left, GeometryBits64 right);
extern float func_003734F8(GeometryBits64 value);

GEORGE_DEFINE_FACE_BOX_CLASSIFY(func_002A3CA0, 5, 3)
GEORGE_DEFINE_FACE_BOX_CLASSIFY(func_002A4520, 6, 4)

GEORGE_DEFINE_FACE_FRAME_CLASSIFY(func_002A4678, 5, 3)
GEORGE_DEFINE_FACE_SPHERE_CLASSIFY(func_002A4760, 5, 3)

#if defined(__GNUC__)
#define CLASSIFY_INLINE static __inline__ __attribute__((always_inline))
#else
#define CLASSIFY_INLINE static
#endif
CLASSIFY_INLINE GeometryBits64 positive_bits(float value)
{
    GeometryBits64 result = func_00374848(value);
    if (func_00373250(result, 0) < 0) result = func_00372CC0(0, result);
    return result;
}

u32 func_0029E098(const GeorgeGeometryFrame *frame, const GeorgeGeometryFace *face)
{
    float x = face->normal.x, y = face->normal.y, z = face->normal.z;
    float first = ((x * frame->axis[0].x + y * frame->axis[0].y) +
                   z * frame->axis[0].z) * frame->axis[0].w;
    float signed_distance = ((x * frame->position.x + y * frame->position.y) +
                             z * frame->position.z) - face->distance;
    float second = ((x * frame->axis[1].x + y * frame->axis[1].y) +
                    z * frame->axis[1].z) * frame->axis[1].w;
    float third = ((x * frame->axis[2].x + y * frame->axis[2].y) +
                   z * frame->axis[2].z) * frame->axis[2].w;
    GeometryBits64 sum, component, absolute_distance, extent;
    float radius;
    sum = positive_bits(first);
    component = positive_bits(second);
    sum = func_00372C68(sum, component);
    component = positive_bits(third);
    sum = func_00372C68(sum, component);
    radius = func_003734F8(sum);
    absolute_distance = positive_bits(signed_distance);
    extent = func_00374848(radius);
    if (func_00373250(absolute_distance, extent) <= 0) return 0;
    return signed_distance < 0.0f ? 1 : 2;
}

#undef CLASSIFY_INLINE
