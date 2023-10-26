#include "application.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "app_types.h"
#include "core/dmemory.h"
#include "core/event.h"

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
static application_state applicaton_state;

b8 application_create(app* app_instance)
{
    if(initialized)
    {
        DERROR("application_create called more than once");
        return FALSE;
    }

    applicaton_state.app_instance = app_instance;

    // Initialize subsystems
    // Initialize log subsystem
    initialize_logging();
    
    // TODO: remove those
    DFATAL("A test message: %.3f", 3.14);
    DERROR("A test message: %.3f", 3.14);
    DWARN("A test message: %.3f", 3.14);
    DINFO("A test message: %.3f", 3.14);
    DDEBUG("A test message: %.3f", 3.14);
    DTRACE("A test message: %.3f", 3.14);

    applicaton_state.is_running = TRUE;
    applicaton_state.is_suspended = FALSE;

    // Initialize event subsystem
    if(!event_initialize()) {
        DERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    if(!platform_startup(
        &applicaton_state.platform, 
        app_instance->app_config.name, 
        app_instance->app_config.start_pos_x, 
        app_instance->app_config.start_pos_y, 
        app_instance->app_config.start_width, 
        app_instance->app_config.start_height))
    {
        return FALSE;
    }

    // Initialize the app
    if (!applicaton_state.app_instance->initialize(applicaton_state.app_instance))
    {
        DFATAL("App failed to initialize.");
        return FALSE;
    }
    // NOTE: ???
    applicaton_state.app_instance->on_resize(applicaton_state.app_instance, applicaton_state.width, applicaton_state.height);

    // TODO: this function gonna grow quite a lot as the application does.

    initialized = TRUE;
    return TRUE;
}

b8 application_run()
{
    DINFO(get_memory_usage_str());

    // the application is going to continuously be in this function for the rest of its life until the user actually choose to quit  
    while(applicaton_state.is_running)
    {
        if(!platform_pump_message(&applicaton_state.platform))  // Just like GLFW glfwPollEvents(&window);
        {
            applicaton_state.is_running = FALSE;
        }

        if(!applicaton_state.is_suspended)
        {
            if(!applicaton_state.app_instance->update(applicaton_state.app_instance, (f32)0))
            {
                DFATAL("App update failed, shutting down...");
                applicaton_state.is_running = FALSE;
                break;
            }

            if (!applicaton_state.app_instance->render(applicaton_state.app_instance, (f32)0))
            {
                DFATAL("App render failed, shutting down...");
                applicaton_state.is_running = FALSE;
                break;
            }
        }
    }

    applicaton_state.is_running = FALSE;

    event_shutdown();

    platform_shutdown(&applicaton_state.platform);

    return TRUE;
}