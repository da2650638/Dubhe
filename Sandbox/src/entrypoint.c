#include <core/logger.h>
#include <core/asserts.h>
#include <core/application.h>
#include <core/dmemory.h>
#include <entrypoint.h>

#include "app.h"

b8 create_app(app* out_app)
{
    out_app->app_config.name = "Dubhe-Sandbox";
    out_app->app_config.start_pos_x = 100;
    out_app->app_config.start_pos_y = 100;
    out_app->app_config.start_width = 1280;
    out_app->app_config.start_height = 720;

    out_app->initialize = app_initialize;
    out_app->update = app_update;
    out_app->render = app_render;
    out_app->on_resize = app_on_resize;

    out_app->state = dallocate(sizeof(app_state), MEMORY_TAG_GAME);

    return TRUE;
}