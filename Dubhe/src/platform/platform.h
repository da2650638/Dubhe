#pragma once

#include "defines.h"


b8 platform_system_startup(
    u64* memory_requirement,
    void* new_state,
    /*platform_state* state,*/
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

void platform_system_shutdown(void* new_state);

b8 platform_pump_message();

void* platform_allocate(u64 size, b8 aligned);
void platform_free(void* block, b8 aligned);
void* platform_zero_memory(void* block, u64 size);
void* platform_copy_memory(void* dest, const void* src, u64 size);
void* platform_set_memory(void* dest, i32 value, u64 size);

void platform_console_write(const char* message, u8 color);
void platform_console_write_error(const char* message, u8 color);

f64 platform_get_absolute_time();

// TODO: 
void platform_sleep(u64 ms);