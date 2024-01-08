/**
 * @file dmath.h
 * @author dongze (da2650638@163.com)
 * @brief This header file contains definitions of mathematical constants and 
 *        utility macros for common mathematical operations. It includes constants 
 *        like PI, conversion multipliers between degrees and radians, square root values, 
 *        and definitions for large and small numerical values. These utilities are 
 *        essential for various mathematical computations and are designed for 
 *        high performance and accuracy in graphics and simulation calculations.
 * @version 0.1
 * @date 2023-12-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "math_types.h"
#include "core/dmemory.h"


#define D_PI 3.14159265358979323846f
#define D_PI_2 2.0f * D_PI
#define D_HALF_PI 0.5f * D_PI
#define D_QUARTER_PI 0.25f * D_PI
#define D_ONE_OVER_PI 1.0f / D_PI
#define D_ONE_OVER_TWO_PI 1.0f / D_PI_2
#define D_SQRT_TWO 1.41421356237309504880f
#define D_SQRT_THREE 1.73205080756887729352f
#define D_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define D_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define D_DEG2RAD_MULTIPLIER D_PI / 180.0f
#define D_RAD2DEG_MULTIPLIER 180.0f / D_PI

// The multiplier to convert seconds to milliseconds.
#define D_SEC_TO_MS_MULTIPLIER 1000.0f

// The multiplier to convert milliseconds to seconds.
#define D_MS_TO_SEC_MULTIPLIER 0.001f

// A huge number that should be larger than any valid number used.
#define D_INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define D_FLOAT_EPSILON 1.192092896e-07f

// ------------------------------------------
// General math functions
// ------------------------------------------
DAPI f32 dsin(f32 x);
DAPI f32 dcos(f32 x);
DAPI f32 dtan(f32 x);
DAPI f32 dacos(f32 x);
DAPI f32 dsqrt(f32 x);
DAPI f32 dabs(f32 x);

/**
 * Indicates if the value is a power of 2. 0 is considered _not_ a power of 2.
 * @param value The value to be interpreted.
 * @returns true if a power of 2, otherwise false.
 */
DINLINE b8 is_power_of_2(u64 value) 
{
    return (value != 0) && ((value & (value - 1)) == 0);
}

DAPI i32 drandom();
DAPI i32 drandom_in_range(i32 min, i32 max);

DAPI f32 fdrandom();
DAPI f32 fdrandom_in_range(f32 min, f32 max);

// ------------------------------------------
// Vector 2
// ------------------------------------------

/**
 * @brief Creates and returns a new 2-element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @return A new 2-element vector.
 */
DINLINE vec2 vec2_create(f32 x, f32 y)
{
    vec2 out_vector;
    out_vector.x = x;
    out_vector.y = y;
    return out_vector;
}

/**
 * @brief Creates and returns a 2-component vector with all components set to 0.0f.
 */
DINLINE vec2 vec2_zero()
{
    return (vec2){0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 2-component vector with all components set to 1.0f.
 */
DINLINE vec2 vec2_one() 
{
    return (vec2){1.0f, 1.0f};
}

/**
 * @brief Creates and returns a 2-component vector pointing up (0, 1).
 */
DINLINE vec2 vec2_up()
{
    return (vec2){0.0f, 1.0f};
}

/**
 * @brief Creates and returns a 2-component vector pointing down (0, -1).
 */
DINLINE vec2 vec2_down() 
{
    return (vec2){0.0f, -1.0f};
}

/**
 * @brief Creates and returns a 2-component vector pointing left (-1, 0).
 */
DINLINE vec2 vec2_left() 
{
    return (vec2){-1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 2-component vector pointing right (1, 0).
 */
DINLINE vec2 vec2_right() 
{
    return (vec2){1.0f, 0.0f};
}

/**
 * @brief Adds vector_1 to vector_0 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec2 vec2_add(vec2 vector_0, vec2 vector_1) {
    return (vec2){
        vector_0.x + vector_1.x,
        vector_0.y + vector_1.y};
}

/**
 * @brief Subtracts vector_1 from vector_0 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec2 vec2_sub(vec2 vector_0, vec2 vector_1) 
{
    return (vec2){
        vector_0.x - vector_1.x,
        vector_0.y - vector_1.y};
}

/**
 * @brief Multiplies vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec2 vec2_mul(vec2 vector_0, vec2 vector_1) 
{
    return (vec2){
        vector_0.x * vector_1.x,
        vector_0.y * vector_1.y};
}

/**
 * @brief Multiplies all elements of vector_0 by scalar and returns a copy of the result.
 * 
 * @param vector_0 The vector to be multiplied.
 * @param scalar The scalar value.
 * @return A copy of the resulting vector.
 */
DINLINE vec2 vec2_mul_scalar(vec2 vector_0, f32 scalar) 
{
    return (vec2){
        vector_0.x * scalar,
        vector_0.y * scalar};
}


/**
 * @brief Divides vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec2 vec2_div(vec2 vector_0, vec2 vector_1) 
{
    return (vec2){
        vector_0.x / vector_1.x,
        vector_0.y / vector_1.y};
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
 */
DINLINE f32 vec2_length_squared(vec2 vector) 
{
    return vector.x * vector.x + vector.y * vector.y;
}

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
 */
DINLINE f32 vec2_length(vec2 vector) 
{
    return dsqrt(vec2_length_squared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
DINLINE void vec2_normalize(vec2* vector) 
{
    const f32 length = vec2_length(*vector);
    vector->x /= length;
    vector->y /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector 
 */
DINLINE vec2 vec2_normalized(vec2 vector) 
{
    vec2_normalize(&vector);
    return vector;
}

DINLINE b8 vec2_compare(vec2 vector_0, vec2 vector_1, f32 tolerance)
{
    if(vector_0.x - vector_1.x > tolerance)
    {
        return false;
    }

    if(vector_0.y - vector_1.y > tolerance)
    {
        return false;
    }

    return true;
}

/**
 * @brief Returns the distance between vector_0 and vector_1.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The distance between vector_0 and vector_1.
 */
DINLINE f32 vec2_distance(vec2 vector_0, vec2 vector_1) 
{
    vec2 d = (vec2){
        vector_0.x - vector_1.x,
        vector_0.y - vector_1.y};
    return vec2_length(d);
}

// ------------------------------------------
// Vector 3
// ------------------------------------------

/**
 * @brief Creates and returns a new 3-element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 * @return A new 3-element vector.
 */
DINLINE vec3 vec3_create(f32 x, f32 y, f32 z) 
{
    return (vec3){x, y, z};
}

/**
 * @brief Returns a new vec3 containing the x, y and z components of the 
 * supplied vec4, essentially dropping the w component.
 * 
 * @param vector The 4-component vector to extract from.
 * @return A new vec3 
 */
DINLINE vec3 vec3_from_vec4(vec4 vector) 
{
    return (vec3){vector.x, vector.y, vector.z};
}

/**
 * @brief Returns a new vec4 using vector as the x, y and z components and w for w.
 * 
 * @param vector The 3-component vector.
 * @param w The w component.
 * @return A new vec4 
 */
DINLINE vec4 vec3_to_vec4(vec3 vector, f32 w) 
{
    return (vec4){vector.x, vector.y, vector.z, w};
}

/**
 * @brief Creates and returns a 3-component vector with all components set to 0.0f.
 */
DINLINE vec3 vec3_zero() 
{
    return (vec3){0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector with all components set to 1.0f.
 */
DINLINE vec3 vec3_one() 
{
    return (vec3){1.0f, 1.0f, 1.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing up (0, 1, 0).
 */
DINLINE vec3 vec3_up() 
{
    return (vec3){0.0f, 1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing down (0, -1, 0).
 */
DINLINE vec3 vec3_down() 
{
    return (vec3){0.0f, -1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing left (-1, 0, 0).
 */
DINLINE vec3 vec3_left() 
{
    return (vec3){-1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing right (1, 0, 0).
 */
DINLINE vec3 vec3_right() 
{
    return (vec3){1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing forward (0, 0, -1).
 */
DINLINE vec3 vec3_forward() 
{
    return (vec3){0.0f, 0.0f, -1.0f};
}

/**
 * @brief Creates and returns a 3-component vector pointing backward (0, 0, 1).
 */
DINLINE vec3 vec3_back() 
{
    return (vec3){0.0f, 0.0f, 1.0f};
}

/**
 * @brief Adds vector_1 to vector_0 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec3 vec3_add(vec3 vector_0, vec3 vector_1) 
{
    return (vec3){
        vector_0.x + vector_1.x,
        vector_0.y + vector_1.y,
        vector_0.z + vector_1.z};
}

/**
 * @brief Multiplies vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec3 vec3_mul(vec3 vector_0, vec3 vector_1) 
{
    return (vec3){
        vector_0.x * vector_1.x,
        vector_0.y * vector_1.y,
        vector_0.z * vector_1.z};
}

/**
 * @brief Multiplies all elements of vector_0 by scalar and returns a copy of the result.
 * 
 * @param vector_0 The vector to be multiplied.
 * @param scalar The scalar value.
 * @return A copy of the resulting vector.
 */
DINLINE vec3 vec3_mul_scalar(vec3 vector_0, f32 scalar) 
{
    return (vec3){
        vector_0.x * scalar,
        vector_0.y * scalar,
        vector_0.z * scalar};
}

/**
 * @brief Divides vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec3 vec3_div(vec3 vector_0, vec3 vector_1) 
{
    return (vec3){
        vector_0.x / vector_1.x,
        vector_0.y / vector_1.y,
        vector_0.z / vector_1.z};
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
 */
DINLINE f32 vec3_length_squared(vec3 vector) 
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}


/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
 */
DINLINE f32 vec3_length(vec3 vector) 
{
    return dsqrt(vec3_length_squared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
DINLINE void vec3_normalize(vec3* vector) 
{
    const f32 length = vec3_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector 
 */
DINLINE vec3 vec3_normalized(vec3 vector) 
{
    vec3_normalize(&vector);
    return vector;
}

/**
 * @brief Returns the dot product between the provided vectors. Typically used
 * to calculate the difference in direction.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The dot product. 
 */
DINLINE f32 vec3_dot(vec3 vector_0, vec3 vector_1) 
{
    f32 p = 0;
    p += vector_0.x * vector_1.x;
    p += vector_0.y * vector_1.y;
    p += vector_0.z * vector_1.z;
    return p;
}

/**
 * @brief Calculates and returns the cross product of the supplied vectors.
 * The cross product is a new vector which is orthoganal to both provided vectors.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The cross product. 
 */
DINLINE vec3 vec3_cross(vec3 vector_0, vec3 vector_1) 
{
    return (vec3){
        vector_0.y * vector_1.z - vector_0.z * vector_1.y,
        vector_0.z * vector_1.x - vector_0.x * vector_1.z,
        vector_0.x * vector_1.y - vector_0.y * vector_1.x};
}

/**
 * @brief Compares all elements of vector_0 and vector_1 and ensures the difference
 * is less than tolerance.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @param tolerance The difference tolerance. Typically K_FLOAT_EPSILON or similar.
 * @return true if within tolerance; otherwise false. 
 */
DINLINE const b8 vec3_compare(vec3 vector_0, vec3 vector_1, f32 tolerance) 
{
    if (dabs(vector_0.x - vector_1.x) > tolerance) 
    {
        return false;
    }

    if (dabs(vector_0.y - vector_1.y) > tolerance) 
    {
        return false;
    }

    if (dabs(vector_0.z - vector_1.z) > tolerance) 
    {
        return false;
    }

    return true;
}

/**
 * @brief Returns the distance between vector_0 and vector_1.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The distance between vector_0 and vector_1.
 */
DINLINE f32 vec3_distance(vec3 vector_0, vec3 vector_1) 
{
    vec3 d = (vec3){
        vector_0.x - vector_1.x,
        vector_0.y - vector_1.y,
        vector_0.z - vector_1.z};
    return vec3_length(d);
}

// ------------------------------------------
// Vector 4
// ------------------------------------------

/**
 * @brief Creates and returns a new 4-element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 * @param w The w value.
 * @return A new 4-element vector.
 */
DINLINE vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) 
{
    vec4 out_vector;
#if defined(KUSE_SIMD)
    out_vector.data = _mm_setr_ps(x, y, z, w);
#else
    out_vector.x = x;
    out_vector.y = y;
    out_vector.z = z;
    out_vector.w = w;
#endif
    return out_vector;
}


/**
 * @brief Returns a new vec3 containing the x, y and z components of the 
 * supplied vec4, essentially dropping the w component.
 * 
 * @param vector The 4-component vector to extract from.
 * @return A new vec3 
 */
DINLINE vec3 vec4_to_vec3(vec4 vector) 
{
    return (vec3){vector.x, vector.y, vector.z};
}

/**
 * @brief Returns a new vec4 using vector as the x, y and z components and w for w.
 * 
 * @param vector The 3-component vector.
 * @param w The w component.
 * @return A new vec4 
 */
DINLINE vec4 vec4_from_vec3(vec3 vector, f32 w) 
{
#if defined(KUSE_SIMD)
    vec4 out_vector;
    out_vector.data = _mm_setr_ps(x, y, z, w);
    return out_vector;
#else
    return (vec4){vector.x, vector.y, vector.z, w};
#endif
}

/**
 * @brief Creates and returns a 3-component vector with all components set to 0.0f.
 */
DINLINE vec4 vec4_zero() 
{
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3-component vector with all components set to 1.0f.
 */
DINLINE vec4 vec4_one() 
{
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f};
}

/**
 * @brief Adds vector_1 to vector_0 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec4 vec4_add(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
     for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] + vector_1.elements[i];
    }
    return result;
}

/**
 * @brief Subtracts vector_1 from vector_0 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec4 vec4_sub(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] - vector_1.elements[i];
    }
    return result;
}

/**
 * @brief Multiplies vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec4 vec4_mul(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] * vector_1.elements[i];
    }
    return result;
}

/**
 * @brief Divides vector_0 by vector_1 and returns a copy of the result.
 * 
 * @param vector_0 The first vector.
 * @param vector_1 The second vector.
 * @return The resulting vector. 
 */
DINLINE vec4 vec4_div(vec4 vector_0, vec4 vector_1) 
{
    vec4 result;
    for (u64 i = 0; i < 4; ++i) {
        result.elements[i] = vector_0.elements[i] / vector_1.elements[i];
    }
    return result;
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
 */
DINLINE f32 vec4_length_squared(vec4 vector) 
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
}

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
 */
DINLINE f32 vec4_length(vec4 vector) 
{
    return dsqrt(vec4_length_squared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
DINLINE void vec4_normalize(vec4* vector) 
{
    const f32 length = vec4_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
    vector->w /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector 
 */
DINLINE vec4 vec4_normalized(vec4 vector) 
{
    vec4_normalize(&vector);
    return vector;
}

/**
 * @brief 计算两个四维向量的点积
 * 
 * @param a0 a1 a2 a3 b0 b1 b2 b3 components of two vectors
 * @return f32 dot product 
 */
DINLINE f32 vec4_dot_f32(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3) 
{
    f32 p;
    p =
        a0 * b0 +
        a1 * b1 +
        a2 * b2 +
        a3 * b3;
    return p;
}

/**
 * @brief 计算两个四维向量的点积
 * 
 * @param vector_0 the first vector
 * @param vector_1 the second vector
 * @return f32 dot product of two vectors 
 */
DINLINE f32 vec4_dot(vec4 vector_0, vec4 vector_1)
{
    f32 p;
    p =
        vector_0.x * vector_1.x +
        vector_0.y * vector_1.y +
        vector_0.z * vector_1.z +
        vector_0.w * vector_1.w;
    return p;
}

/**
 * @brief Creates and returns an identity matrix:
 * 
 * {
 *   {1, 0, 0, 0},
 *   {0, 1, 0, 0},
 *   {0, 0, 1, 0},
 *   {0, 0, 0, 1}
 * }
 * 
 * @return A new identity matrix 
 */
DINLINE mat4 mat4_identity() {
    mat4 out_matrix;
    dzero_memory(out_matrix.data, sizeof(f32) * 16);
    out_matrix.data[0] = 1.0f;
    out_matrix.data[5] = 1.0f;
    out_matrix.data[10] = 1.0f;
    out_matrix.data[15] = 1.0f;
    return out_matrix;
}

/**
 * @brief Returns the result of multiplying matrix_0 and matrix_1.
 * 
 * @param matrix_0 The first matrix to be multiplied.
 * @param matrix_1 The second matrix to be multiplied.
 * @return The result of the matrix multiplication.
 */
DINLINE mat4 mat4_mul(mat4 matrix_0, mat4 matrix_1)
{
    mat4 out_matrix = mat4_identity();

    const f32* m1_ptr = matrix_0.data;
    const f32* m2_ptr = matrix_1.data;
    f32* dst_ptr = out_matrix.data;

    for (i32 i = 0; i < 4; ++i) {
        for (i32 j = 0; j < 4; ++j) {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }
    return out_matrix;
}

/**
 * @brief Creates and returns an orthographic projection matrix. Typically used to
 * render flat or 2D scenes.
 * 
 * @param left The left side of the view frustum.
 * @param right The right side of the view frustum.
 * @param bottom The bottom side of the view frustum.
 * @param top The top side of the view frustum.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new orthographic projection matrix. 
 */
DINLINE mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip) 
{
    mat4 out_matrix = mat4_identity();

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near_clip - far_clip);

    out_matrix.data[0] = -2.0f * lr;
    out_matrix.data[5] = -2.0f * bt;
    out_matrix.data[10] = 2.0f * nf;

    out_matrix.data[12] = (left + right) * lr;
    out_matrix.data[13] = (top + bottom) * bt;
    out_matrix.data[14] = (far_clip + near_clip) * nf;
    return out_matrix;
}

/**
 * @brief Creates and returns a perspective matrix. Typically used to render 3d scenes.
 * 
 * @param fov_radians The field of view in radians.
 * @param aspect_ratio The aspect ratio.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new perspective matrix. 
 */
DINLINE mat4 mat4_perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip) 
{
    f32 half_tan_fov = dtan(fov_radians * 0.5f);
    mat4 out_matrix;
    dzero_memory(out_matrix.data, sizeof(f32) * 16);
    out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_matrix.data[5] = 1.0f / half_tan_fov;
    out_matrix.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out_matrix.data[11] = -1.0f;
    out_matrix.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
    return out_matrix;
}

/**
 * @brief Creates and returns a look-at matrix, or a matrix looking 
 * at target from the perspective of position.
 * 
 * @param position The position of the matrix.
 * @param target The position to "look at".
 * @param up The up vector.
 * @return A matrix looking at target from the perspective of position. 
 */
DINLINE mat4 mat4_look_at(vec3 position, vec3 target, vec3 up) 
{
    mat4 out_matrix;
    vec3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis = vec3_normalized(z_axis);
    vec3 x_axis = vec3_normalized(vec3_cross(z_axis, up));
    vec3 y_axis = vec3_cross(x_axis, z_axis);

    out_matrix.data[0] = x_axis.x;
    out_matrix.data[1] = y_axis.x;
    out_matrix.data[2] = -z_axis.x;
    out_matrix.data[3] = 0;
    out_matrix.data[4] = x_axis.y;
    out_matrix.data[5] = y_axis.y;
    out_matrix.data[6] = -z_axis.y;
    out_matrix.data[7] = 0;
    out_matrix.data[8] = x_axis.z;
    out_matrix.data[9] = y_axis.z;
    out_matrix.data[10] = -z_axis.z;
    out_matrix.data[11] = 0;
    out_matrix.data[12] = -vec3_dot(x_axis, position);
    out_matrix.data[13] = -vec3_dot(y_axis, position);
    out_matrix.data[14] = vec3_dot(z_axis, position);
    out_matrix.data[15] = 1.0f;

    return out_matrix;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->colums)
 * 
 * @param matrix The matrix to be transposed.
 * @return A transposed copy of of the provided matrix.
 */
DINLINE mat4 mat4_transposed(mat4 matrix) 
{
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = matrix.data[0];
    out_matrix.data[1] = matrix.data[4];
    out_matrix.data[2] = matrix.data[8];
    out_matrix.data[3] = matrix.data[12];
    out_matrix.data[4] = matrix.data[1];
    out_matrix.data[5] = matrix.data[5];
    out_matrix.data[6] = matrix.data[9];
    out_matrix.data[7] = matrix.data[13];
    out_matrix.data[8] = matrix.data[2];
    out_matrix.data[9] = matrix.data[6];
    out_matrix.data[10] = matrix.data[10];
    out_matrix.data[11] = matrix.data[14];
    out_matrix.data[12] = matrix.data[3];
    out_matrix.data[13] = matrix.data[7];
    out_matrix.data[14] = matrix.data[11];
    out_matrix.data[15] = matrix.data[15];
    return out_matrix;
}

/**
 * @brief Creates and returns an inverse of the provided matrix.
 * 
 * @param matrix The matrix to be inverted.
 * @return A inverted copy of the provided matrix. 
 */
DINLINE mat4 mat4_inverse(mat4 matrix) 
{
    const f32* m = matrix.data;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    mat4 out_matrix;
    f32* o = out_matrix.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_matrix;
}

/**
 * @brief Creates a translation matrix.
 *
 * This function generates a 4x4 translation matrix that can be used
 * to translate objects in a 3D space. The translation is specified
 * by the provided `position` vector, which contains the X, Y, and Z
 * displacements. The resulting matrix can be applied to transform
 * the coordinates of objects or coordinate systems in 3D graphics.
 * The matrix is stored in column-major order.
 *
 * @param position A vec3 representing the translation in X, Y, and Z.
 * @return mat4 The 4x4 translation matrix.
 */
DINLINE mat4 mat4_translation(vec3 position) 
{
    mat4 out_matrix = mat4_identity();
    out_matrix.data[12] = position.x;
    out_matrix.data[13] = position.y;
    out_matrix.data[14] = position.z;
    return out_matrix;
}

/**
 * @brief Returns a scale matrix using the provided scale.
 * 
 * @param scale The 3-component scale.
 * @return A scale matrix.
 */
DINLINE mat4 mat4_scale(vec3 scale) 
{
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = scale.x;
    out_matrix.data[5] = scale.y;
    out_matrix.data[10] = scale.z;
    return out_matrix;
}

/**
 * @brief Creates a rotation matrix for a rotation around the X-axis.
 *
 * This function generates a 4x4 matrix representing a rotation
 * around the X-axis by a specified angle in radians. The rotation
 * follows the right-hand rule, so a positive angle results in a
 * counterclockwise rotation. The matrix can be used to rotate
 * objects or coordinate systems in 3D graphics. The matrix is
 * stored in column-major order.
 *
 * @param angle_radians The rotation angle in radians.
 * @return mat4 The 4x4 rotation matrix.
 */
DINLINE mat4 mat4_euler_x(f32 angle_radians)
{
    mat4 out_matrix = mat4_identity();
    f32 c = dcos(angle_radians);
    f32 s = dsin(angle_radians);

    out_matrix.data[5] = c;
    out_matrix.data[6] = s;
    out_matrix.data[9] = -s;
    out_matrix.data[10] = c;
    return out_matrix;
}

/**
 * @brief Creates a rotation matrix for a rotation around the Y-axis.
 *
 * This function generates a 4x4 matrix representing a rotation
 * around the Y-axis by a specified angle in radians. The rotation
 * follows the right-hand rule, so a positive angle results in a
 * counterclockwise rotation when looking along the direction of
 * the Y-axis. The matrix can be used to rotate objects or
 * coordinate systems in 3D graphics. The matrix is stored in
 * column-major order.
 *
 * @param angle_radians The rotation angle in radians.
 * @return mat4 The 4x4 rotation matrix.
 */
DINLINE mat4 mat4_euler_y(f32 angle_radians) 
{
    mat4 out_matrix = mat4_identity();
    f32 c = dcos(angle_radians);
    f32 s = dsin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[2] = -s;
    out_matrix.data[8] = s;
    out_matrix.data[10] = c;
    return out_matrix;
}

/**
 * @brief Creates a rotation matrix for a rotation around the Z-axis.
 *
 * This function generates a 4x4 matrix representing a rotation
 * around the Z-axis by a specified angle in radians. The rotation
 * follows the right-hand rule, so a positive angle results in a
 * counterclockwise rotation when looking along the direction of
 * the Z-axis. The matrix can be used to rotate objects or
 * coordinate systems in 3D graphics. The matrix is stored in
 * column-major order.
 *
 * The resulting rotation matrix will specifically rotate a point
 * or object around the Z-axis without affecting its position along
 * the X and Y axes.
 *
 * @param angle_radians The rotation angle around the Z-axis in radians.
 * @return mat4 The 4x4 rotation matrix.
 */
DINLINE mat4 mat4_euler_z(f32 angle_radians) {
    mat4 out_matrix = mat4_identity();

    f32 c = dcos(angle_radians);
    f32 s = dsin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[1] = s;
    out_matrix.data[4] = -s;
    out_matrix.data[5] = c;
    return out_matrix;
}

/**
 * @brief Creates a rotation matrix for a rotation around the X, Y, and Z axes.
 *
 * This function generates a 4x4 matrix representing a combined rotation
 * around the X, Y, and Z axes by specified angles in radians. The rotations
 * are applied in the order of X, Y, and then Z axes. This is useful for
 * rotating objects or coordinate systems in 3D graphics. The resulting
 * matrix is stored in column-major order and combines the effects of
 * individual X, Y, and Z axis rotations.
 *
 * @param x_radians The rotation angle around the X-axis in radians.
 * @param y_radians The rotation angle around the Y-axis in radians.
 * @param z_radians The rotation angle around the Z-axis in radians.
 * @return mat4 The 4x4 combined rotation matrix.
 */
DINLINE mat4 mat4_euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians) 
{
    mat4 rx = mat4_euler_x(x_radians);
    mat4 ry = mat4_euler_y(y_radians);
    mat4 rz = mat4_euler_z(z_radians);
    mat4 out_matrix = mat4_mul(rx, ry);
    out_matrix = mat4_mul(out_matrix, rz);
    return out_matrix;
}

/**
 * @brief Returns a forward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_forward(mat4 matrix) 
{
    vec3 forward;
    forward.x = -matrix.data[2];
    forward.y = -matrix.data[6];
    forward.z = -matrix.data[10];
    vec3_normalize(&forward);
    return forward;
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_backward(mat4 matrix) 
{
    vec3 backward;
    backward.x = matrix.data[2];
    backward.y = matrix.data[6];
    backward.z = matrix.data[10];
    vec3_normalize(&backward);
    return backward;
}

/**
 * @brief Returns a upward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_up(mat4 matrix) 
{
    vec3 up;
    up.x = matrix.data[1];
    up.y = matrix.data[5];
    up.z = matrix.data[9];
    vec3_normalize(&up);
    return up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_down(mat4 matrix) 
{
    vec3 down;
    down.x = -matrix.data[1];
    down.y = -matrix.data[5];
    down.z = -matrix.data[9];
    vec3_normalize(&down);
    return down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_left(mat4 matrix) 
{
    vec3 right;
    right.x = -matrix.data[0];
    right.y = -matrix.data[4];
    right.z = -matrix.data[8];
    vec3_normalize(&right);
    return right;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
DINLINE vec3 mat4_right(mat4 matrix) 
{
    vec3 left;
    left.x = matrix.data[0];
    left.y = matrix.data[4];
    left.z = matrix.data[8];
    vec3_normalize(&left);
    return left;
}

// ------------------------------------------
// Quaternion
// ------------------------------------------

/**
 * @brief Creates an identity quaternion representing no rotation.
 *
 * This function generates a quaternion that represents an identity rotation,
 * meaning it applies no rotation to an object. The identity quaternion is
 * useful as a default rotation state and is represented as (0, 0, 0, 1),
 * where the first three components are the imaginary part (x, y, z),
 * and the last component is the real part (w).
 *
 * @return quat The identity quaternion.
 */
DINLINE quat quat_identity() {
    return (quat){0, 0, 0, 1.0f};
}

/**
 * @brief Calculates the norm (or magnitude) of a quaternion.
 *
 * This function computes the norm of a quaternion, defined as the square root
 * of the sum of the squares of its components. The norm represents the length
 * of the quaternion in four-dimensional space and is important for various
 * quaternion operations, such as normalization and ensuring numerical stability
 * in calculations.
 *
 * @param q The quaternion whose norm is to be calculated.
 * @return f32 The norm of the quaternion.
 */
DINLINE f32 quat_normal(quat q) {
    return dsqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w);
}

/**
 * @brief Normalizes a quaternion to unit length.
 *
 * This function normalizes a quaternion, scaling it to unit length without
 * altering its orientation. Normalizing a quaternion is important when
 * its magnitude has deviated from 1 due to numerical operations, as
 * quaternion-based rotations typically require the quaternion to be of
 * unit length. The function first computes the norm of the quaternion,
 * then divides each component by this norm.
 *
 * @param q The quaternion to be normalized.
 * @return quat The normalized quaternion.
 */
DINLINE quat quat_normalize(quat q) {
    f32 normal = quat_normal(q);
    return (quat){
        q.x / normal,
        q.y / normal,
        q.z / normal,
        q.w / normal};
}

/**
 * @brief Computes the conjugate of a quaternion.
 *
 * The conjugate of a quaternion is obtained by negating its imaginary
 * (vector) part while keeping the real (scalar) part unchanged. If the
 * original quaternion represents a rotation, its conjugate represents
 * the inverse of that rotation. Conjugating a quaternion is useful in
 * many quaternion operations, including computing quaternion inverses
 * and transforming vectors between coordinate systems.
 *
 * @param q The quaternion to be conjugated.
 * @return quat The conjugated quaternion.
 */
DINLINE quat quat_conjugate(quat q) {
    return (quat){
        -q.x,
        -q.y,
        -q.z,
        q.w};
}

/**
 * @brief Computes the inverse of a quaternion.
 *
 * The inverse of a quaternion is used to perform the opposite rotation
 * represented by the quaternion. For a unit quaternion (a quaternion of
 * unit length), the inverse is the same as its conjugate. This function
 * first computes the conjugate of the quaternion and then normalizes it,
 * which is necessary if the quaternion is not already a unit quaternion.
 * The inverse is particularly useful in rotating vectors back to their
 * original coordinate system or undoing a rotation.
 *
 * @param q The quaternion whose inverse is to be computed.
 * @return quat The inverse of the quaternion.
 */
DINLINE quat quat_inverse(quat q) {
    return quat_normalize(quat_conjugate(q));
}

/**
 * @brief Computes the product of two quaternions.
 *
 * This function multiplies two quaternions, representing the combination
 * of their respective rotations. Quaternion multiplication is not commutative,
 * meaning the order of multiplication matters. The resulting quaternion
 * represents first applying the rotation of `q_1` and then the rotation of `q_0`.
 * This operation is used in 3D graphics to combine rotations and apply
 * them to objects or coordinate systems.
 *
 * @param q_0 The first quaternion in the multiplication.
 * @param q_1 The second quaternion in the multiplication.
 * @return quat The product of the two quaternions.
 */
DINLINE quat quat_mul(quat q_0, quat q_1) {
    quat out_quaternion;

    out_quaternion.x = q_0.x * q_1.w +
                       q_0.y * q_1.z -
                       q_0.z * q_1.y +
                       q_0.w * q_1.x;

    out_quaternion.y = -q_0.x * q_1.z +
                       q_0.y * q_1.w +
                       q_0.z * q_1.x +
                       q_0.w * q_1.y;

    out_quaternion.z = q_0.x * q_1.y -
                       q_0.y * q_1.x +
                       q_0.z * q_1.w +
                       q_0.w * q_1.z;

    out_quaternion.w = -q_0.x * q_1.x -
                       q_0.y * q_1.y -
                       q_0.z * q_1.z +
                       q_0.w * q_1.w;

    return out_quaternion;
}

/**
 * @brief Computes the dot product of two quaternions.
 *
 * This function calculates the dot product of two quaternions. The dot product
 * is a scalar value representing the cosine of the angle between the two
 * quaternion rotations in 4-dimensional space. It is often used in interpolation,
 * such as SLERP (Spherical Linear Interpolation), and to determine the similarity
 * or orientation difference between two quaternions. A dot product close to 1
 * indicates very similar or parallel orientations, while a dot product close to -1
 * indicates opposite orientations.
 *
 * @param q_0 The first quaternion.
 * @param q_1 The second quaternion.
 * @return f32 The dot product of the two quaternions.
 */
DINLINE f32 quat_dot(quat q_0, quat q_1) {
    return q_0.x * q_1.x +
           q_0.y * q_1.y +
           q_0.z * q_1.z +
           q_0.w * q_1.w;
}

/**
 * @brief Converts a quaternion to a 4x4 rotation matrix.
 *
 * This function converts a quaternion into a corresponding 4x4 rotation matrix.
 * The quaternion is first normalized to ensure it represents a pure rotation.
 * The resulting matrix can be used for rotating objects in 3D space. This is
 * useful in graphics programming where quaternions are used for their numerical
 * stability and efficiency, but the actual transformations are applied using
 * matrix operations.
 *
 * The conversion formula used in this function follows the standard method for
 * converting a normalized quaternion to a rotation matrix.
 *
 * @param q The quaternion to convert.
 * @return mat4 The corresponding 4x4 rotation matrix.
 */
DINLINE mat4 quat_to_mat4(quat q) {
    mat4 out_matrix = mat4_identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    quat n = quat_normalize(q);

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}

/**
 * @brief Converts a quaternion and a center point to a 4x4 rotation matrix.
 *
 * This function converts a quaternion into a corresponding 4x4 rotation matrix,
 * considering a rotation around a specified center point. The quaternion
 * represents the rotation, while the center point defines the pivot point
 * of the rotation. This is useful for rotating objects around a specific
 * point in 3D space, rather than around the origin.
 *
 * @param q The quaternion representing the rotation.
 * @param center The center point around which the rotation is applied.
 * @return mat4 The 4x4 rotation matrix.
 */
DINLINE mat4 quat_to_rotation_matrix(quat q, vec3 center) {
    mat4 out_matrix;

    f32* o = out_matrix.data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    return out_matrix;
}

/**
 * @brief Creates a quaternion from an axis and an angle of rotation.
 *
 * This function generates a quaternion representing a rotation around a given
 * axis by a specified angle. The axis is a 3D vector, and the angle is given
 * in radians. The quaternion is formed by taking the sine and cosine of half
 * of the angle, then applying these to the axis components and the scalar part
 * of the quaternion. Optionally, the quaternion can be normalized before being
 * returned, which is often necessary for accurate rotational behavior in 3D
 * graphics and simulations.
 *
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @param normalize A boolean flag to normalize the quaternion.
 * @return quat The resulting quaternion representing the rotation.
 */
DINLINE quat quat_from_axis_angle(vec3 axis, f32 angle, b8 normalize) {
    const f32 half_angle = 0.5f * angle;
    f32 s = dsin(half_angle);
    f32 c = dcos(half_angle);

    quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};
    if (normalize) {
        return quat_normalize(q);
    }
    return q;
}

/**
 * @brief Performs spherical linear interpolation between two quaternions.
 *
 * This function smoothly interpolates between two quaternions based on a given
 * percentage, using the spherical linear interpolation method. SLERP ensures
 * constant angular velocity and shortest path rotation. It's particularly useful
 * for smooth rotational transitions in animations and simulations. The function
 * handles cases where the quaternions are nearly collinear by falling back to
 * linear interpolation to avoid numerical instability.
 *
 * @param q_0 The first quaternion.
 * @param q_1 The second quaternion.
 * @param percentage The interpolation factor between 0 and 1.
 * @return quat The interpolated quaternion.
 */
DINLINE quat quat_slerp(quat q_0, quat q_1, f32 percentage) {
    quat out_quaternion;
    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    quat v0 = quat_normalize(q_0);
    quat v1 = quat_normalize(q_1);

    // Compute the cosine of the angle between the two vectors.
    f32 dot = quat_dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out_quaternion = (quat){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)};

        return quat_normalize(out_quaternion);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = dacos(dot);          // theta_0 = angle between input vectors
    f32 theta = theta_0 * percentage;  // theta = angle between v0 and result
    f32 sin_theta = dsin(theta);       // compute this value only once
    f32 sin_theta_0 = dsin(theta_0);   // compute this value only once

    f32 s0 = dcos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (quat){
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)};
}

/**
 * @brief Converts provided degrees to radians.
 * 
 * @param degrees The degrees to be converted.
 * @return The amount in radians.
 */
DINLINE f32 deg_to_rad(f32 degrees) {
    return degrees * D_DEG2RAD_MULTIPLIER;
}

/**
 * @brief Converts provided radians to degrees.
 * 
 * @param radians The radians to be converted.
 * @return The amount in degrees.
 */
DINLINE f32 rad_to_deg(f32 radians) {
    return radians * D_RAD2DEG_MULTIPLIER;
}