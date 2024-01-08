#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/dmemory.h"

static renderer_backend* backend = 0;
typedef struct renderer_system_state {
    renderer_backend backend;
} renderer_system_state;

static renderer_system_state* state_ptr;

// 使用者调用这些函数
b8 renderer_system_initialize(u64* memory_requirement, void* new_state, const char* application_name)
{
    // new state
    *memory_requirement = sizeof(renderer_system_state);
    if (new_state == 0) {
        return true;
    }
    state_ptr = new_state;

    dzero_memory(state_ptr, sizeof(renderer_system_state));

    // TODO: make this creation configurable
    if(!renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, &state_ptr->backend))
    {
        DFATAL("create renderer_backend instance failed. renderer type: %s", renderer_backend_type_to_string(RENDERER_BACKEND_TYPE_VULKAN));
        return false;
    }
    state_ptr->backend.frame_number = 0;

    // new state
    if(!state_ptr->backend.initialize(&state_ptr->backend, application_name))
    {
        DFATAL("renderer_backend instance %p initialize failed.", &state_ptr->backend);
        return false;
    }

    return true;
}

void renderer_system_shutdown(void* state)
{
    // new state
    if (state_ptr) 
    {
        state_ptr->backend.shutdown(&state_ptr->backend);
    }
    state_ptr = 0;
}

void renderer_on_resize(u16 width, u16 height)
{
    if(state_ptr)
    {
        state_ptr->backend.resized(backend, width, height);
    }
    else
    {
        DWARN("renderer backend does not exists to accept resize: %i %i", width, height);
    }
}

b8 renderer_begin_frame(f32 delta_time)
{
    if (!state_ptr) 
    {
        return false;
    }
    return state_ptr->backend.begin_frame(&state_ptr->backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time)
{
    if (!state_ptr) 
    {
        return false;
    }
    b8 result = state_ptr->backend.end_frame(&state_ptr->backend, delta_time);
    state_ptr->backend.frame_number++;
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
            return false;
        }
    }

    return true;
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