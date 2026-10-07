#include "george/spatial_queries.h"
#include "george/vector_transform.h"

#define AT(type, object, offset) ((type *)((u32)(object) + (u32)(offset)))
#define FIELD(type, object, offset) (*AT(type, object, offset))
#if __GNUC__ >= 3
#define SPATIAL_INLINE static __inline__ __attribute__((always_inline))
#else
#define SPATIAL_INLINE static __inline__
#endif

static __inline__ GeorgeGoalRoad *nested_road(void *header)
{
    GeorgeGoalRoad *road;
    if (header == NULL) return NULL;
    road = FIELD(GeorgeGoalRoad *, header, 0x2C);
    if (road == NULL || FIELD(u32, road, 0) == 0) return NULL;
    return road;
}

GeorgeGoalRoad *func_001CC960(const GeorgeMathVec3 *point)
{
    u32 index = 0;
    void *header = NULL;
    if (FIELD(u8, D_003F8C28, 0) != 0) {
        do {
            void *provider = FIELD(void *, D_003F8C28, 8U + index * 4U);
            if ((FIELD(u8, provider, 2) & 4U) != 0) {
                void *candidate = FIELD(void *, provider, 0x10);
                if (func_002A00A0(AT(GeorgeBounds, candidate, 4), point) != 0) {
                    header = candidate;
                    break;
                }
            }
            ++index;
        } while (index < FIELD(u8, D_003F8C28, 0));
    }
    return nested_road(header);
}

GeorgeGoalRoad *func_001CCA28(u32 word)
{
    GeorgeGoalMapOwner *manager = D_003F8C28;
    u32 count = FIELD(u8, manager, 0), index;
    void *header = NULL;
    void **providers = AT(void *, manager, 8);
    word &= 0x7FFF0000U;
    for (index = 0; index < count; ++index, ++providers) {
        void *provider = *providers;
        if ((FIELD(u8, provider, 2) & 4U) != 0) {
            void *candidate = FIELD(void *, provider, 0x10);
            if (FIELD(u32, candidate, 0) == word) {
                header = candidate;
                break;
            }
        }
    }
    return nested_road(header);
}

/* Both original bodies contain this same complete value/control sequence.
 * The transformed count is fresh after each real call and then captured for
 * the edge loop. Skipping a tiny edge retains the preceding side value. */
SPATIAL_INLINE s32 polygon_contains(GeorgeGoalRoadGeometry *road,
                                      GeorgeGoalRoadEntry *record,
                                      const GeorgeMathVec3 *point,
                                      volatile float *scratch)
{
    void *polygon = AT(u8, road->field40, FIELD(u32, record, 0x44));
    const GeorgeMathVec3 *vertex = AT(GeorgeMathVec3, polygon, 8);
    s32 count = 0, index;
    float side = 0.0f;
    if (FIELD(u8, polygon, 4) != 0) {
        do {
            func_002A1C60(record, vertex,
                         (GeorgeMathVec3 *)(scratch + (u32)count * 3U));
            ++count;
            ++vertex;
        } while (count < FIELD(u8, polygon, 4));
    }
    for (index = 0; index < count; ++index) {
        s32 next = (index + 1) % count;
        const volatile float *current = scratch + (u32)index * 3U;
        const volatile float *following = scratch + (u32)next * 3U;
        float z = current[2], x = current[0];
        float dz = following[2] - z;
        float negative_dx = x - following[0];
        if (-0.10000000149011612f < dz && dz < 0.10000000149011612f &&
            -0.10000000149011612f < negative_dx && negative_dx < 0.10000000149011612f)
            continue;
        /* Reload current components, as in the original edge expression. */
        {
            float point_x = point->x, point_z = point->z;
            float current_z = current[2], current_x = current[0];
            float z_product = (point_z - current_z) * negative_dx;
            float x_product = (point_x - current_x) * dz;
            side = x_product + z_product;
        }
        if (0.0f < side) break;
    }
    if (0.0f < side) return 0;
    {
        /* Intentionally indeterminate when no transform wrote scratch[1]. */
        float first_y = scratch[1], point_y = point->y;
        if (!(point_y < first_y + 2.0f)) return 0;
        if (!(first_y - 2.0f < point_y)) return 0;
    }
    return 1;
}

GeorgeGoalRouteResult *func_001CF340(void *context)
{
    const GeorgeMathVec3 *point = (const GeorgeMathVec3 *)context;
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CC960(point);
    volatile float scratch[16];
    u32 index = 0;
    if (road == NULL) return NULL;
    if (FIELD(u16, road, 4) != 0) {
        do {
            GeorgeGoalRoadEntry *record = AT(GeorgeGoalRoadEntry, road->field3C,
                                             index * 0x60U);
            if (polygon_contains(road, record, point, scratch) != 0)
                return (GeorgeGoalRouteResult *)record;
            ++index;
        } while (index < FIELD(u16, road, 4));
    }
    return NULL;
}

s32 func_001CF588(void *context, u32 word)
{
    u32 index = word & 0xFFFFU;
    GeorgeGoalRoadGeometry *road = (GeorgeGoalRoadGeometry *)func_001CCA28(word);
    GeorgeGoalRoadEntry *record;
    volatile float scratch[16];
    if (road == NULL) return 0;
    record = AT(GeorgeGoalRoadEntry, road->field3C, index * 0x60U);
    if (record == NULL) return 0;
    return polygon_contains(road, record, (const GeorgeMathVec3 *)context, scratch);
}

#undef FIELD
#undef AT
#undef SPATIAL_INLINE
