#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "app_types.h"

// Externally-defined function to create a app
extern b8 create_app(app* out_app);

/**
 * The main entrypoint of the application
 */
int main(void)
{
    // Request the app instance from the application
    app app_instance;
    if(!create_app(&app_instance))
    {
        DFATAL("Could not create app!");
        return -1;
    }

    //Ensure the function pointer exist
    if(!app_instance.initialize || !app_instance.update || !app_instance.render || !app_instance.on_resize)
    {
        DFATAL("The app's function pointers must be assigned!");
        return -2;
    }

    if(!application_create(&app_instance))
    {
        DFATAL("Failed to create Application instance.");
        return 0;
    }

    // Begin the app loop
    if(!application_run())
    {
        DINFO("Application did not shutdown gracefully.");
    }

    return 0;
}