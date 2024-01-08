#include "application.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "app_types.h"
#include "core/dmemory.h"
#include "core/event.h"
#include "core/input.h"
#include "core/clock.h"
#include "renderer/renderer_frontend.h"

/*
 * 
 */
typedef struct application_state{
    app* app_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state platform_state;
    i16 width;
    i16 height;
    clock clock;
    f64 last_time;
}application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data);
b8 application_on_key(u16 code, void* sender, void* listener, event_context data);
b8 application_on_resize(u16 code, void* sender, void* listener, event_context data);

b8 application_create(app* app_instance)
{
    if(initialized)
    {
        DERROR("application_create called more than once");
        return FALSE;
    }

    app_state.app_instance = app_instance;

    // Initialize subsystems
    // Initialize log subsystem
    initialize_logging();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    // Initialize input subsystem
    input_initialize();
    // Initialize event subsystem
    if(!event_initialize()) {
        DERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }
    // Register the specific event
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_evnet);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_register(EVENT_CODE_RESIZED, 0, application_on_resize);

    if(!platform_startup(
        &app_state.platform_state, 
        app_instance->app_config.name, 
        app_instance->app_config.start_pos_x, 
        app_instance->app_config.start_pos_y, 
        app_instance->app_config.start_width, 
        app_instance->app_config.start_height))
    {
        return FALSE;
    }

    // 平台初始化之后，app初始化之前初始化renderer
    if(!renderer_initialize(app_instance->app_config.name, &app_state.platform_state))
    {
        DFATAL("Failed to initialize renderer. Aborting application.");
        return FALSE;
    }

    // Initialize the app
    if (!app_state.app_instance->initialize(app_state.app_instance))
    {
        DFATAL("App failed to initialize.");
        return FALSE;
    }
    // NOTE: ???
    app_state.app_instance->on_resize(app_state.app_instance, app_state.width, app_state.height);

    // TODO: this function gonna grow quite a lot as the application does.

    initialized = TRUE;
    return TRUE;
}

b8 application_run()
{
    DINFO(get_memory_usage_str());
    clock_start(&app_state.clock);
    clock_update(&app_state.clock);
    app_state.last_time = app_state.clock.elapsed;
    f64 running_time = 0.0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f / 60.0;

    // the application is going to continuously be in this function for the rest of its life until the user actually choose to quit  
    while(app_state.is_running)
    {
        if(!platform_pump_message(&app_state.platform_state))  // Just like GLFW glfwPollEvents(&window);
        {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended)
        {
            clock_update(&app_state.clock);
            f64 current_time = app_state.clock.elapsed;
            f64 delta_time = current_time - app_state.last_time;
            f64 frame_start_time = platform_get_absolute_time();

            if(!app_state.app_instance->update(app_state.app_instance, (f32)delta_time))
            {
                DFATAL("App update failed, shutting down...");
                app_state.is_running = FALSE;
                break;
            }

            if (!app_state.app_instance->render(app_state.app_instance, (f32)delta_time))
            {
                DFATAL("App render failed, shutting down...");
                app_state.is_running = FALSE;
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
                b8 limit_frames = FALSE;
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
            app_state.last_time = current_time;
        }
    }

    app_state.is_running = FALSE;

    // Unregister the event
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_evnet);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    input_shutdown();

    renderer_shutdown();

    platform_shutdown(&app_state.platform_state);

    return TRUE;
}

void application_get_framebuffer_size(u32* width, u32* height) 
{
    *width = app_state.width;
    *height = app_state.height;
}

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data)
{
    switch(code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            DINFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
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
                return TRUE;
            }
            case KEY_A:
            {
                DDEBUG("Explicit - A key pressed!");
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
            case KEY_B:
            {
                // Example on checking for a key
                DDEBUG("Explicit - B key released!");
                break;
            }
            default:
            {
                DDEBUG("'%c' key released in window.", key_code);
            }
        }
    }
    return FALSE;   
}

b8 application_on_resize(u16 code, void* sender, void* listener, event_context data)
{
    if(code == EVENT_CODE_RESIZED)
    {
        u16 width = data.data.u16[0];
        u16 height = data.data.u16[1];

        if(width != app_state.width || height != app_state.height)
        {
            app_state.width = width;
            app_state.height = height;

            DDEBUG("Window resize: %i, %i", width, height);

            if(width == 0 || height == 0)
            {
                DINFO("Window minimized. Suspending application.");
                app_state.is_suspended = TRUE;
                return TRUE;
            }
            else
            {
                if(app_state.is_suspended)
                {
                    DINFO("Window restored, resuming application.");
                    app_state.is_suspended = FALSE;
                }
                app_state.app_instance->on_resize(app_state.app_instance, width, height);
                renderer_on_resize(width, height);
            }
        }
    }

    // Event purposely not handled to allow other listener to get this.
    return FALSE;
}