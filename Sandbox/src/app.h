#pragma once

#include <defines.h>
#include <app_types.h>

typedef struct app_state{
    f32 delta_time;
}app_state;


b8 app_initialize(app* app_instance);

b8 app_update(app* app_instance, f32 delta_time);

b8 app_render(app* app_instance, f32 delta_time);

void app_on_resize(app* app_instance, u32 width, u32 height);