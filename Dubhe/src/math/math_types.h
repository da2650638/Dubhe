#pragma once 

#include <defines.h>

typedef union vec2_u
{
    f32 elements[2];
    struct
    {
        union
        {
            f32 x,r,s,u;
        };
        union
        {
            f32 y,g,t,v;
        };
    };
} vec2;

typedef struct vec3_u 
{
    union 
    {
        // An array of x, y, z
        f32 elements[3];
        struct 
        {
            union 
            {
                // The first element.
                f32 x, r, s, u;
            };
            union 
            {
                // The second element.
                f32 y, g, t, v;
            };
            union 
            {
                // The third element.
                f32 z, b, p, w;
            };
        };
    };
} vec3;

typedef union vec4_u 
{
#if defined(DUSE_SIMD)
    alignas(16) __m128 data;
#endif 
    //alignas(16) f32 elements[4];
    f32 elements[4];
    union
    {
        struct
        {
            union 
            {
                f32 x, r, s;
            };
            union 
            {
                f32 y, g, t;
            };
            union 
            {
                f32 z, b, p;
            };
            union 
            {
                f32 w, a, q;
            };
        };
    };
} vec4;

typedef vec4 quat;

typedef union mat4_u
{
    f32 data[16];
} mat4;

typedef struct vertex_3d
{
    vec3 position;
} vertex_3d;