#include "application.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "app_types.h"
#include "core/dmemory.h"

/*
 * 
 */
typedef struct application_state{
    app* app_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
}application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(app* app_instance)
{
    if(initialized)
    {
        DERROR("application_create called more than once");
        return FALSE;
    }

    app_state.app_instance = app_instance;

    // Initialize subsystem
    // Initialize log subsystem
    initialize_logging();
    
    // TODO: remove those
    DFATAL("A test message: %.3f", 3.14);
    DERROR("A test message: %.3f", 3.14);
    DWARN("A test message: %.3f", 3.14);
    DINFO("A test message: %.3f", 3.14);
    DDEBUG("A test message: %.3f", 3.14);
    DTRACE("A test message: %.3f", 3.14);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!platform_startup(
        &app_state.platform, 
        app_instance->app_config.name, 
        app_instance->app_config.start_pos_x, 
        app_instance->app_config.start_pos_y, 
        app_instance->app_config.start_width, 
        app_instance->app_config.start_height))
    {
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

    // the application is going to continuously be in this function for the rest of its life until the user actually choose to quit  
    while(app_state.is_running)
    {
        if(!platform_pump_message(&app_state.platform))  // Just like GLFW glfwPollEvents(&window);
        {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended)
        {
            if(!app_state.app_instance->update(app_state.app_instance, (f32)0))
            {
                DFATAL("App update failed, shutting down...");
                app_state.is_running = FALSE;
                break;
            }

            if (!app_state.app_instance->render(app_state.app_instance, (f32)0))
            {
                DFATAL("App render failed, shutting down...");
                app_state.is_running = FALSE;
                break;
            }
        }
    }

    app_state.is_running = FALSE;

    platform_shutdown(&app_state.platform);

    return TRUE;
}