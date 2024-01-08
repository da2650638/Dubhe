#include "app.h"
#include <core/logger.h>

b8 app_initialize(app* app_instance)
{
    DDEBUG("app_initialize called");
    return TRUE;
}

b8 app_update(app* app_instance, f32 delta_time)
{
    return TRUE;
}

b8 app_render(app* app_instance, f32 delta_time)
{
    return TRUE;
}

void app_on_resize(app* app_instance, u32 width, u32 height)
{

}