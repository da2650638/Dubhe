#include "application.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "app_types.h"
#include "core/dmemory.h"
#include "core/event.h"
#include "core/input.h"

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
    f64 last_time;
}application_state;

static b8 initialized = FALSE;
static application_state applicaton_state;

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data);
b8 application_on_key(u16 code, void* sender, void* listener, event_context data);

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

    applicaton_state.is_running = TRUE;
    applicaton_state.is_suspended = FALSE;

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

    if(!platform_startup(
        &applicaton_state.platform_state, 
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
        if(!platform_pump_message(&applicaton_state.platform_state))  // Just like GLFW glfwPollEvents(&window);
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

    // Unregister the event
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_evnet);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    input_shutdown();

    platform_shutdown(&applicaton_state.platform_state);

    return TRUE;
}

b8 application_on_evnet(u16 code, void* sender, void* listener, event_context data)
{
    switch(code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            DINFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
            applicaton_state.is_running = FALSE;
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