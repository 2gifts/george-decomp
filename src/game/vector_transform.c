#include "george/vector_transform.h"

#if __GNUC__ >= 3
#define TRANSFORM_INLINE static __inline__ __attribute__((always_inline))
#else
#define TRANSFORM_INLINE static __inline__
#endif

/* Reuse the published accumulator-expression shape: each single product
 * precedes its left-associated addition. The original uses VU broadcast
 * operations; ordinary scalar C remains reconstructed until whole-byte
 * comparison proves otherwise. */
TRANSFORM_INLINE float basis_component(const float *matrix, unsigned component,
                                       float x, float y, float z)
{
    float result = matrix[component] * x;
    float product = matrix[component + 4U] * y;
    result = result + product;
    product = matrix[component + 8U] * z;
    return result + product;
}

void func_002A1C60(const void *storage, const GeorgeMathVec3 *input,
                  GeorgeMathVec3 *output)
{
    const float *matrix = (const float *)storage;
    float z = input->z;
    float y = input->y;
    float x = input->x;
    float result_z = basis_component(matrix, 2U, x, y, z);
    float result_x = basis_component(matrix, 0U, x, y, z);
    float result_y = basis_component(matrix, 1U, x, y, z);
    result_z = result_z + matrix[14] * 1.0f;
    result_x = result_x + matrix[12] * 1.0f;
    result_y = result_y + matrix[13] * 1.0f;
    /* All source/result captures precede stores, even with shifted overlap.
     * The original captures fourth lanes too but never publishes them. */
    output->z = result_z;
    output->x = result_x;
    output->y = result_y;
}

void func_002A1D78(const void *storage, const GeorgeMathVec3 *input,
                  GeorgeMathVec3 *output)
{
    const float *matrix = (const float *)storage;
    float z = input->z;
    float y = input->y;
    float x = input->x;
    float result_z = basis_component(matrix, 2U, x, y, z);
    float result_x = basis_component(matrix, 0U, x, y, z);
    float result_y = basis_component(matrix, 1U, x, y, z);
    output->z = result_z;
    output->x = result_x;
    output->y = result_y;
}

#undef TRANSFORM_INLINE
