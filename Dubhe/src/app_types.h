#pragma once

#include "core/application.h"

/**
 * Represents the basic game state in a app.
 * Called for creation by the application
*/
typedef struct app {
    // The application configuration
    application_config app_config;

    // Function pointer to app's initialize function
    b8 (*initialize)(struct app* app_instance);

    // Function pointer to app's update function
    b8 (*update)(struct app* app_instance, f32 delta_time);

    // Function pointer to app's render function
    b8 (*render)(struct app* app_instance, f32 delta_time);

    // Function pointer to app's handler of the resize event
    void (*on_resize)(struct app* app_instance, u32 width, u32 height);

    // State
    void* state;
}app;