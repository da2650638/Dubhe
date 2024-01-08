#include "app.h"
#include <core/logger.h>
#include <core/dmemory.h>
#include <core/input.h>

b8 app_initialize(app* app_instance)
{
    DDEBUG("app_initialize called");
    return true;
}

b8 app_update(app* app_instance, f32 delta_time)
{
    static u64 alloc_count = 0;
    u64 prev_alloc_count = alloc_count;
    alloc_count = get_memory_alloc_count();
    if (input_is_key_up('M') && input_was_key_down('M')) 
    {
        DDEBUG("Allocations: %llu (%llu this frame)", alloc_count, alloc_count - prev_alloc_count);
    } 
    return true;
}

b8 app_render(app* app_instance, f32 delta_time)
{
    return true;
}

void app_on_resize(app* app_instance, u32 width, u32 height)
{

}