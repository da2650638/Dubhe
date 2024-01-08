#include "renderer_backend.h"

// TODO: 后续更新的时候可能还需要加上其他平台的backend,包括opengl、dirextx
// TODO: as the program grows, additional platform backends may be required,including opengl and directx
#include "renderer/vulkan/vulkan_backend.h"

#include "core/logger.h"

b8 renderer_backend_create(renderer_backend_type type, struct platform_state* plat_state, renderer_backend* out_renderer_backend)
{
    out_renderer_backend->plat_state = plat_state;
    out_renderer_backend->frame_number = 0;
    switch(type)
    {
        case RENDERER_BACKEND_TYPE_VULKAN:
        {
            out_renderer_backend->initialize = vulkan_renderer_backend_initialize;
            out_renderer_backend->shutdown = vulkan_renderer_backend_shutdown;
            out_renderer_backend->resized = vulkan_renderer_backend_resized;
            out_renderer_backend->begin_frame = vulkan_renderer_backend_begin_frame;
            out_renderer_backend->end_frame = vulkan_renderer_backend_end_frame;
            break;
        }
        case RENDERER_BACKEND_TYPE_OPENGL:
        {
            break;
        }
        case RENDERER_BACKEND_TYPE_DIRECTX:
        {
            break;
        }
        default:
        {
            DFATAL("Unknown RENDERER_BACKEND_TYPE");
            return false;
        }
    }

    return true;
}

void renderer_backend_destroy(renderer_backend* backend)
{
    backend->initialize = 0;
    backend->shutdown = 0;
    backend->resized = 0;
    backend->begin_frame = 0;
    backend->end_frame = 0;
}