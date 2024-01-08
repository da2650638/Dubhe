#include "dmath.h"
#include <platform/platform.h>

#include <math.h>
#include <stdlib.h>

static b8 rand_seeded = false;

f32 dsin(f32 x)
{
    return sinf(x);
}

f32 dcos(f32 x)
{
    return cosf(x);
}

f32 dtan(f32 x)
{
    return tanf(x);
}

f32 dacos(f32 x)
{
    return acosf(x);
}

f32 dqrt(f32 x) 
{
    return sqrtf(x);
}

f32 dabs(f32 x) 
{
    return fabsf(x);
}

/**
 * @brief Generates a random integer.
 *
 * This function returns a random integer using the standard C library's `rand()`
 * function. It ensures that the random number generator is seeded exactly once
 * using the current time (obtained from `platform_get_absolute_time()`) before
 * generating the random number. This seeding is necessary to ensure different
 * random sequences in different runs of the program.
 *
 * @return i32 A random integer.
 */
i32 drandom()
{
    if(!rand_seeded)
    {
        srand((u32)platform_get_absolute_time());
        rand_seeded = true;
    }
    return rand();
}

/**
 * @brief Generates a random integer within a specified range.
 *
 * This function returns a random integer between `min` and `max` (inclusive).
 * Like `drandom()`, it seeds the random number generator on the first call.
 * The random number is generated using `rand()` and then scaled to the
 * specified range.
 *
 * @param min The minimum value in the range.
 * @param max The maximum value in the range.
 * @return i32 A random integer between `min` and `max`.
 */
i32 drandom_in_range(i32 min, i32 max)
{
    if(!rand_seeded)
    {
        srand((u32)platform_get_absolute_time());
        rand_seeded = true;
    }
    return rand() % (max - min + 1) + min;
}

/**
 * @brief Generates a random floating-point number between 0 and 1.
 *
 * This function returns a random floating-point number in the range [0, 1].
 * It uses `drandom()` to generate a random integer and then normalizes this
 * value to a float between 0 and 1 by dividing by `RAND_MAX`.
 *
 * @return f32 A random floating-point number between 0 and 1.
 */
f32 fdrandom()
{
    return (f32)drandom() / (f32)RAND_MAX;
}

/**
 * @brief Generates a random floating-point number within a specified range.
 *
 * This function returns a random floating-point number between `min` and `max`.
 * It first generates a random float between 0 and 1 using `fdrandom()`, then
 * scales this value to the specified range [min, max]. This is done by
 * multiplying the normalized random number with the range width (`max - min`)
 * and then adding the minimum value (`min`).
 *
 * @param min The minimum value in the range.
 * @param max The maximum value in the range.
 * @return f32 A random floating-point number between `min` and `max`.
 */
f32 fdrandom_in_range(f32 min, f32 max)
{
    return min + (fdrandom() * (max - min));
}

