#pragma once

#include "renderer_types.inl"

// TODO: unused type static_mesh_data
struct static_mesh_data;
struct platform_state;

const char* renderer_backend_type_to_string(renderer_backend_type type);

// 使用者调用这些函数
b8 renderer_initialize(const char* application_name, struct platform_state* plat_state);
void renderer_shutdown();

void renderer_on_resize(u16 width, u16 height);
b8 renderer_begin_frame(f32 delta_time);
b8 renderer_end_frame(f32 delta_time);
b8 renderer_draw_frame(renderer_packet* packet);