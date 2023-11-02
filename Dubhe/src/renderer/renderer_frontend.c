#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/dmemory.h"

struct platform_state;

static renderer_backend* backend = 0;

// 使用者调用这些函数
b8 renderer_initialize(const char* application_name, struct platform_state* plat_state)
{
    backend = (renderer_backend*)dallocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);
    dzero_memory(backend, sizeof(renderer_backend));

    // TODO: make this creation configurable
    if(!renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend))
    {
        DFATAL("create renderer_backend instance failed. renderer type: %s", renderer_backend_type_to_string(RENDERER_BACKEND_TYPE_VULKAN));
        return FALSE;
    }

    // 中文注释：
    // 
    if(!backend->initialize(backend, application_name, plat_state))
    {
        DFATAL("renderer_backend instance %p initialize failed.", backend);
        return FALSE;
    }

    return TRUE;
}

void renderer_shutdown()
{
    backend->shutdown(backend);
    dfree(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

void renderer_resize(u16 width, u16 height)
{
    backend->resized(backend, width, height);
}

b8 renderer_begin_frame(f32 delta_time)
{
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time)
{
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_number++;
    return result;
}

b8 renderer_draw_frame(renderer_packet* packet)
{
    if(renderer_begin_frame(packet->delta_time))
    {
        b8 result = renderer_end_frame(packet->delta_time);

        if(!result)
        {
            DERROR("renderer_end_frame failed. Application shutting down...");
            return FALSE;
        }
    }

    return TRUE;
}

const char* renderer_backend_type_to_string(renderer_backend_type type)
{
    switch(type)
    {
        case RENDERER_BACKEND_TYPE_VULKAN:
        {
            return "RENDERER_BACKEND_TYPE_VULKAN";
        }
        case RENDERER_BACKEND_TYPE_OPENGL:
        {
            return "RENDERER_BACKEND_TYPE_OPENGL";
        }
        case RENDERER_BACKEND_TYPE_DIRECTX:
        {
            return "RENDERER_BACKEND_TYPE_DIRECTX";
        }
        default:
        {
            return "RENDERER_BACKEND_TYPE_UNKNOWN";
        }
    }

    return "RENDERER_BACKEND_TYPE_UNKNOWN";
}