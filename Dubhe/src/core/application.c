#include "application.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "app_types.h"
#include "core/dmemory.h"
#include "core/event.h"
#include "core/input.h"
#include "core/clock.h"

#include "memory/linear_allocator.h"

#include "renderer/renderer_frontend.h"

/*
 * 
 */
typedef struct application_state{
    app* app_instance;
    b8 is_running;
    b8 is_suspended;
    i16 width;
    i16 height;
    clock clock;
    f64 last_time;
    linear_allocator systems_allocator;

    u64 event_system_memory_requirement;
    void* event_system_state;

    u64 memory_system_memory_requirement;
    void* memory_system_state;

    u64 logging_system_memory_requirement;
    void* logging_system_state;

    u64 input_system_memory_requirement;
    void* input_system_state;

    u64 platform_system_memory_requirement;
    void* platform_system_state;

    u64 renderer_system_memory_requirement;
    void* renderer_system_state;
}application_state;

static application_state *app_state;

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data);
b8 application_on_key(u16 code, void* sender, void* listener, event_context data);
b8 application_on_resize(u16 code, void* sender, void* listener, event_context data);

b8 application_create(app* app_instance)
{
    if(app_instance->application_state)
    {
        DERROR("application_create called more than once");
        return false;
    }

    app_instance->application_state = dallocate(sizeof(application_state), MEMORY_TAG_APPLICATION);
    app_state = (application_state*)app_instance->application_state;
    app_state->app_instance = app_instance;
    app_state->is_running = false;
    app_state->is_suspended = false;

    u64 system_allocator_total_size = 64 * 1024 * 1024; // 64MB
    linear_allocator_create(system_allocator_total_size, 0, &app_state->systems_allocator);

    // Initialize subsystems

    // Initialize event subsystem
    event_system_initialize(&app_state->event_system_memory_requirement, 0);
    app_state->event_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->event_system_memory_requirement);
    event_system_initialize(&app_state->event_system_memory_requirement, app_state->event_system_state);

    // Initialize memory subsystem
    memory_system_initialize(&app_state->memory_system_memory_requirement, 0);
    app_state->memory_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->memory_system_memory_requirement);
    memory_system_initialize(&app_state->memory_system_memory_requirement, app_state->memory_system_state);

    // Initialize log subsystem
    initialize_logging(&app_state->logging_system_memory_requirement, 0);
    app_state->logging_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->logging_system_memory_requirement);
    if(!initialize_logging(&app_state->logging_system_memory_requirement, app_state->logging_system_state))
    {
        DERROR("Failed to initialize logging system; shutting down.");
        return false;
    }

    // Initialize input subsystem
    input_system_initialize(&app_state->input_system_memory_requirement, 0);
    app_state->input_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->input_system_memory_requirement);
    input_system_initialize(&app_state->input_system_memory_requirement, app_state->input_system_state);

    // Register the specific event
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_evnet);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_register(EVENT_CODE_RESIZED, 0, application_on_resize);

    // Platform
    platform_system_startup(&app_state->platform_system_memory_requirement, 0, 0, 0, 0, 0, 0);
    app_state->platform_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->platform_system_memory_requirement);
    if(!platform_system_startup(
        &app_state->platform_system_memory_requirement,
        app_state->platform_system_state,
        app_instance->app_config.name, 
        app_instance->app_config.start_pos_x, 
        app_instance->app_config.start_pos_y, 
        app_instance->app_config.start_width, 
        app_instance->app_config.start_height))
    {
        return false;
    }

    // 平台初始化之后，app初始化之前初始化renderer
    renderer_system_initialize(&app_state->renderer_system_memory_requirement, 0, 0);
    app_state->renderer_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->renderer_system_memory_requirement);
    if(!renderer_system_initialize(&app_state->renderer_system_memory_requirement, app_state->renderer_system_state, app_instance->app_config.name))
    {
        DFATAL("Failed to initialize renderer. Aborting application.");
        return false;
    }

    // Initialize the app
    if (!app_state->app_instance->initialize(app_state->app_instance))
    {
        DFATAL("App failed to initialize.");
        return false;
    }
    // NOTE: ???
    app_state->app_instance->on_resize(app_state->app_instance, app_state->width, app_state->height);

    // TODO: this function gonna grow quite a lot as the application does.

    return true;
}

b8 application_run()
{
    DINFO(get_memory_usage_str());
    app_state->is_running = true;
    clock_start(&app_state->clock);
    clock_update(&app_state->clock);
    app_state->last_time = app_state->clock.elapsed;
    f64 running_time = 0.0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f / 60.0;

    // the application is going to continuously be in this function for the rest of its life until the user actually choose to quit  
    while(app_state->is_running)
    {
        if(!platform_pump_message())  // Just like GLFW glfwPollEvents(&window);
        {
            app_state->is_running = false;
        }

        if(!app_state->is_suspended)
        {
            clock_update(&app_state->clock);
            f64 current_time = app_state->clock.elapsed;
            f64 delta_time = current_time - app_state->last_time;
            f64 frame_start_time = platform_get_absolute_time();

            if(!app_state->app_instance->update(app_state->app_instance, (f32)delta_time))
            {
                DFATAL("App update failed, shutting down...");
                app_state->is_running = false;
                break;
            }

            if (!app_state->app_instance->render(app_state->app_instance, (f32)delta_time))
            {
                DFATAL("App render failed, shutting down...");
                app_state->is_running = false;
                break;
            }

            // TODO: refactor packet creation
            renderer_packet packet;
            packet.delta_time = delta_time;
            renderer_draw_frame(&packet);

            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 frame_remaining_time = target_frame_seconds - frame_elapsed_time;
            if(frame_remaining_time > 0.0)
            {
                u64 remaining_ms = (u64)frame_remaining_time * 1000;
                // If there is time left, give it back to the OS.
                b8 limit_frames = false;
                if(remaining_ms > 0 && limit_frames)
                {
                    platform_sleep(remaining_ms);
                }

                frame_count++;
            }

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            input_update(delta_time);

            // Update last time
            app_state->last_time = current_time;
        }
    }

    app_state->is_running = false;

    // Unregister the event
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_evnet);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_unregister(EVENT_CODE_RESIZED, 0, application_on_resize);
    input_system_shutdown();

    renderer_system_shutdown(app_state->renderer_system_state);

    platform_system_shutdown(app_state->platform_system_state);

    memory_system_shutdown(app_state->memory_system_state);

    event_system_shutdown();

    return true;
}

void application_get_framebuffer_size(u32* width, u32* height) 
{
    *width = app_state->width;
    *height = app_state->height;
}

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data)
{
    switch(code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            DINFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
            app_state->is_running = false;
            return true;
        }
    }

    return false;
}

b8 application_on_key(u16 code, void* sender, void* listener, event_context data)
{
    u16 key_code = data.data.u16[0];
    if(code == EVENT_CODE_KEY_PRESSED)
    {
        switch(key_code)
        {
            case KEY_ESCAPE:
            {
                event_context data = {};
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                return true;
            }
            case KEY_LALT:
            {
                DDEBUG("LALT key pressed in window.");
                break;
            }
            case KEY_RALT:
            {
                DDEBUG("RALT key pressed in window.");
                break;
            }
            case KEY_LSHIFT:
            {
                DDEBUG("LSHIT key pressed in window.");
                break;
            }
            case KEY_RSHIFT:
            {
                DDEBUG("RSHIFT key pressed in window.");
                break;
            }
            case KEY_LCONTROL:
            {
                DDEBUG("LCONTROL key pressed in window.");
                break;
            }
            case KEY_RCONTROL:
            {
                DDEBUG("RCONTROL key pressed in window.");
                break;
            }
            default:
            {
                DDEBUG("'%c' key pressed in window.", key_code);
            }
        }
    }
    else if(code == EVENT_CODE_KEY_RELEASED)
    {
        u16 key_code = data.data.u16[0];
        switch (key_code)
        {
            case KEY_LALT:
            {
                DDEBUG("LALT key released in window.");
                break;
            }
            case KEY_RALT:
            {
                DDEBUG("RALT key released in window.");
                break;
            }
            case KEY_LSHIFT:
            {
                DDEBUG("LSHIT key released in window.");
                break;
            }
            case KEY_RSHIFT:
            {
                DDEBUG("RSHIFT key released in window.");
                break;
            }
            case KEY_LCONTROL:
            {
                DDEBUG("LCONTROL key released in window.");
                break;
            }
            case KEY_RCONTROL:
            {
                DDEBUG("RCONTROL key released in window.");
                break;
            }
            default:
            {
                DDEBUG("'%c' key released in window.", key_code);
            }
        }
    }
    return false;   
}

b8 application_on_resize(u16 code, void* sender, void* listener, event_context data)
{
    if(code == EVENT_CODE_RESIZED)
    {
        u16 width = data.data.u16[0];
        u16 height = data.data.u16[1];

        if(width != app_state->width || height != app_state->height)
        {
            app_state->width = width;
            app_state->height = height;

            DDEBUG("Window resize: %i, %i", width, height);

            if(width == 0 || height == 0)
            {
                DINFO("Window minimized. Suspending application.");
                app_state->is_suspended = true;
                return true;
            }
            else
            {
                if(app_state->is_suspended)
                {
                    DINFO("Window restored, resuming application.");
                    app_state->is_suspended = false;
                }
                app_state->app_instance->on_resize(app_state->app_instance, width, height);
                renderer_on_resize(width, height);
            }
        }
    }

    // Event purposely not handled to allow other listener to get this.
    return false;
}