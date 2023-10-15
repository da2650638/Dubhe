#pragma once

#include "defines.h"

struct app; 

// Application configuration
typedef struct application_config{
    i16 start_pos_x;
    i16 start_pos_y;
    i16 start_width;
    i16 start_height;
    char* name;
}application_config;

DAPI b8 application_create(struct app* app_instance);

DAPI b8 application_run();