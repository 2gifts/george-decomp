#include "george/path_curves.h"

extern GeorgePathCurveCallback func_002C24B8(u32);
extern GeorgePathCurveCallback func_002C24D8(u32);
extern GeorgePathCurveCallback func_002C24F8(u32);
extern GeorgePathProjectionCallback func_002C2518(u32);
extern float func_0037B238(float, float);
extern u32 func_0029DD60(const GeorgeMathVec4 *, const GeorgePathRay *);
extern float func_0029CF28(const GeorgeMathVec3 *, const GeorgeMathVec3 *,
                          const GeorgeMathVec3 *, float *);

#define ADDRESS(object, offset) ((u8 *)((u32)(object) + (u32)(offset)))
#define FIELD(object, offset, type) (*(type *)ADDRESS(object, offset))
#define COUNT(object) FIELD(object, 4, u16)
#define STRIDE(object) FIELD(object, 1, u8)
#define KIND(object) FIELD(object, 0, u8)
#define FLAGS(object) FIELD(object, 2, u8)
#if __GNUC__ >= 3
#define PATH_INLINE static __inline__ __attribute__((always_inline))
#else
#define PATH_INLINE static __inline__
#endif

PATH_INLINE void *row(void *data, u32 index, u32 stride)
{
    return ADDRESS(data, ((index * stride) << 2) + 8U);
}

PATH_INLINE float endpoint_time(u32 kind, const void *point)
{
    if (kind == 16) return (float)FIELD(point, 7, u8) * 160.0f;
    return FIELD(point, 0, float);
}

u32 func_00295EB8(void *data, u32 index, float *position, float *fraction,
                 void **first, void **second)
{
    float time = *position;
    u32 count = COUNT(data), stride = STRIDE(data), kind;
    float end = endpoint_time(KIND(data), row(data, count - 1U, stride));
    float start = endpoint_time(KIND(data), ADDRESS(data, 8));
    float period = end - start;
    float base = endpoint_time(KIND(data), ADDRESS(data, 8));
    float left_time, right_time;
    void *left, *right;
    u32 done = 0;

    if (count == 1U) {
        left = ADDRESS(data, 8);
        *first = left;
        *second = left;
        *fraction = 0.0f;
    } else if (count == 2U) {
        *first = ADDRESS(data, 8);
        *second = row(data, 1, STRIDE(data));
        left_time = endpoint_time(KIND(data), *first);
        kind = KIND(data);
        right_time = endpoint_time(kind, *second);
        if (time < endpoint_time(kind, ADDRESS(data, 8))) {
            if (FLAGS(data) & 3U) {
                time = func_0037B238((time - base) + period, period) + base;
            } else time = left_time;
        } else if (endpoint_time(kind, row(data, (u32)COUNT(data) - 1U,
                                           STRIDE(data))) <= time) {
            if (FLAGS(data) & 12U) {
                time = func_0037B238(time - base, period) + base;
            } else time = right_time;
        }
        *fraction = (time - left_time) / (right_time - left_time);
    } else if (count != 0U) {
        kind = KIND(data);
        if (time < endpoint_time(kind, ADDRESS(data, 8))) {
            if (FLAGS(data) & 3U) {
                time = func_0037B238((time - base) + period, period) + base;
            } else {
                left = ADDRESS(data, 8);
                index = 0;
                *first = left;
                *second = left;
                *fraction = 0.0f;
                done = 1;
            }
        } else if (endpoint_time(kind, row(data, count - 1U, stride)) <= time) {
            if (FLAGS(data) & 12U) {
                time = func_0037B238((time - base) + period, period) + base;
            } else {
                *first = row(data, count - 1U, stride);
                *second = row(data, (u32)COUNT(data) - 1U, STRIDE(data));
                *fraction = 1.0f;
                index = (u32)COUNT(data) - 1U;
                done = 1;
            }
        }
        if (!done) {
            count = COUNT(data);
            stride = STRIDE(data);
            kind = KIND(data);
            if (!(index < count - 1U)) index = count - 2U;
            left = row(data, index, stride);
            right = row(data, index + 1U, stride);
            if (endpoint_time(kind, left) <= time) {
                while (endpoint_time(kind, right) < time) {
                    if (!(index < count - 1U)) break;
                    ++index;
                    left = ADDRESS(left, stride << 2);
                    right = ADDRESS(right, stride << 2);
                }
            } else {
                while (time < endpoint_time(kind, left)) {
                    if (index == 0U) break;
                    --index;
                    left = ADDRESS(left, 0U - (stride << 2));
                    right = ADDRESS(right, 0U - (stride << 2));
                }
            }
            *first = left;
            *second = right;
            left_time = endpoint_time(KIND(data), *first);
            right_time = endpoint_time(KIND(data), *second);
            *fraction = (time - left_time) / (right_time - left_time);
        }
    }
    if (FLAGS(data) & 32U) *position = time;
    return index;
}

u32 func_00296560(void *data, const GeorgeMathVec4 *query,
                 float *position, float *distance)
{
    u32 index = 0, next, stride;
    void *first, *second;
    GeorgeMathVec3 direction;
    GeorgePathRay ray;
    float length, fraction, left_time, right_time;
    if ((u32)COUNT(data) - 1U == 0U) return 0;
    stride = STRIDE(data);
    do {
        next = index + 1U;
        first = row(data, index, stride);
        second = row(data, next, stride);
        direction.y = FIELD(second, 8, float) - FIELD(first, 8, float);
        direction.z = FIELD(second, 12, float) - FIELD(first, 12, float);
        direction.x = FIELD(second, 4, float) - FIELD(first, 4, float);
        length = func_002A3538(&direction);
        ray.field00.x = FIELD(first, 4, float);
        ray.field00.y = FIELD(first, 8, float);
        ray.field00.z = FIELD(first, 12, float);
        ray.field0C = direction;
        ray.field18 = length;
        if (func_0029DD60(query, &ray)) {
            *distance = func_0029CF28((GeorgeMathVec3 *)ADDRESS(first, 4),
                                     (GeorgeMathVec3 *)ADDRESS(second, 4),
                                     (const GeorgeMathVec3 *)query, &fraction);
            left_time = FIELD(first, 0, float);
            right_time = FIELD(second, 0, float);
            *position = left_time + fraction * (right_time - left_time);
            return 1;
        }
        index = next;
        if (!(index < (u32)COUNT(data) - 1U)) break;
        stride = STRIDE(data);
    } while (1);
    return 0;
}

u32 func_002966F0(void *data, const GeorgeMathVec3 *reference, u16 *index,
                 s32 range, float *position, float *distance)
{
    GeorgePathProjectionCallback callback = func_002C2518(KIND(data));
    float candidate_distance, candidate_position, parameter;
    s32 begin = 0, end, current, next, best = 0;
    u32 found = 0, before = 0, after = 0, stride, kind;
    void *first, *second;
    float x, y, z, dx, dy, dz, ox, oy, oz, dot, square;
    *distance = 100000000.0f;
    *position = 0.0f;
    candidate_distance = 100000000.0f;
    candidate_position = *position;
    end = (s32)COUNT(data) - 1;
    if (index != 0) {
        begin = *index;
        if (range > 0) {
            s32 limit = end;
            end = (s32)((u32)begin + (u32)range);
            begin = (s32)((u32)begin - (u32)range);
            if ((u32)limit < (u32)end) end = limit;
            if (!(begin > -1)) begin = 0;
        }
    }
    current = begin;
    if (current < end) {
        stride = STRIDE(data);
        do {
            next = (s32)((u32)current + 1U);
            first = row(data, (u32)current, stride);
            second = row(data, (u32)next, stride);
            callback(reference, first, second, &candidate_distance, &candidate_position);
            if (candidate_distance < *distance) {
                parameter = candidate_position;
                best = current;
                *distance = candidate_distance;
                found = 1;
                *position = parameter;
                if (best == begin || next == end) {
                    kind = KIND(data);
                    stride = STRIDE(data);
                    if ((endpoint_time(kind, row(data, (u32)best, stride)) - 0.0001f < parameter &&
                         parameter < endpoint_time(kind, row(data, (u32)current, stride)) + 0.0001f) ||
                        (endpoint_time(kind, row(data, (u32)next, stride)) - 0.0001f < parameter &&
                         parameter < endpoint_time(kind, row(data, (u32)next, stride)) + 0.0001f)) {
                        first = row(data, (u32)current, stride);
                        second = row(data, (u32)next, stride);
                        x = reference->x;
                        y = reference->y;
                        z = reference->z;
                        oz = z - FIELD(first, 12, float);
                        oy = y - FIELD(first, 8, float);
                        dz = FIELD(second, 12, float) - FIELD(first, 12, float);
                        dx = FIELD(second, 4, float) - FIELD(first, 4, float);
                        dy = FIELD(second, 8, float) - FIELD(first, 8, float);
                        ox = x - FIELD(first, 4, float);
                        dot = (ox * dx + oy * dy) + oz * dz;
                        if (current == begin && dot < 0.0f) before = 1;
                        if (next == end) {
                            square = (dx * dx + dy * dy) + dz * dz;
                            if (square < dot) after = 1;
                        }
                    }
                }
            }
            current = next;
            if (!(current < end)) break;
            stride = STRIDE(data);
        } while (1);
    }
    if (best == begin && before) found = 0;
    else if ((s32)((u32)best + 1U) == end && after) found = 0;
    if (index != 0) *index = (u16)best;
    return found;
}

float func_00297178(void *data, u16 *index, float *position, GeorgeMathVec3 *output)
{
    GeorgePathCurveCallback callback = func_002C24B8(KIND(data));
    float fraction;
    void *first, *second;
    u32 selected = func_00295EB8(data, *index, position, &fraction, &first, &second);
    *index = (u16)selected;
    callback(first, second, output, fraction);
    return fraction;
}

/* The two complete 136-byte originals have the same argument and access
 * sequence; only the genuine callback-column selector differs. */
#define PATH_COLUMN_ENTRY(name, selector) \
float name(void *data, u16 *index, void *output, float time) \
{ \
    GeorgePathCurveCallback callback = selector(KIND(data)); \
    float fraction; \
    void *first, *second; \
    u32 selected = func_00295EB8(data, *index, &time, &fraction, &first, &second); \
    *index = (u16)selected; \
    callback(first, second, output, fraction); \
    return fraction; \
}
PATH_COLUMN_ENTRY(func_00297208, func_002C24D8)
PATH_COLUMN_ENTRY(func_00297290, func_002C24F8)
#undef PATH_COLUMN_ENTRY

float func_00297318(void *data, u16 *index, GeorgeMathVec3 *output, float time)
{
    float fraction;
    void *first, *second;
    u32 selected = func_00295EB8(data, *index, &time, &fraction, &first, &second);
    *index = (u16)selected;
    func_002A3390(output, (GeorgeMathVec3 *)ADDRESS(first, 0x28),
                  (GeorgeMathVec3 *)ADDRESS(second, 0x28), fraction);
    return fraction;
}

#undef FLAGS
#undef PATH_INLINE
#undef KIND
#undef STRIDE
#undef COUNT
#undef FIELD
#undef ADDRESS
