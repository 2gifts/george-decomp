#include "george/engine_angles.h"
#include "george/ee_math.h"

float func_0029C230(float value)
{
    if (value < 0.0f)
        return 3.1415927410125732421875f - func_0029C230(-value);
    value = value * value;
    value = 1.0f / value;
    value = value - 1.0f;
    return func_0029C300(george_ee_square_root(value));
}

float func_0029C2A0(float value)
{
    float square;
    if (value < 0.0f)
        return -func_0029C2A0(-value);
    square = value * value;
    return func_0029C300(george_ee_reciprocal_square_root(value, 1.0f - square));
}

float func_0029C300(float value)
{
    float square = value * value;
    float previous, sum, power, denominator, negative_square;
    if (1.0f < square) {
        float reduced = func_0029C300(1.0f / value);
        if (0.0f < value)
            return 1.57079637050628662109375f - reduced;
        return -1.57079637050628662109375f - reduced;
    }
    if (0.17157287895679473876953125f < square) {
        float reduced = george_ee_square_root(square + 1.0f);
        reduced = reduced - 1.0f;
        reduced = reduced / value;
        reduced = func_0029C300(reduced);
        return reduced + reduced;
    }
    negative_square = -square;
    denominator = 1.0f;
    sum = 1.0f;
    power = negative_square;
    do {
        previous = sum;
        denominator = denominator + 2.0f;
        sum = previous + power / denominator;
        if (sum == previous)
            break;
        power = power * negative_square;
    } while (1);
    /* The converged comparison retains the preceding sum in f1. */
    return value * previous;
}
