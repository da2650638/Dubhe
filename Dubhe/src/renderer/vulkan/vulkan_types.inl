#pragma once

#include "defines.h"
#include "core/asserts.h"

#include <vulkan/vulkan.h>

// 存储vulkan交换链支持信息的数据结构
typedef struct vulkan_swapchain_support_info{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_physical_device_requriements{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    // darray
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
}vulkan_physical_device_requriements;

typedef struct vulkan_physical_device_queue_family_info{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
}vulkan_physical_device_queue_family_info;

typedef struct vulkan_device {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;
    // NOTE: where is compute_queue_index
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;

    VkFormat depth_format;
}vulkan_device;

typedef struct vulkan_image{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} vulkan_image;

typedef enum vulkan_renderpass_state{
    READY,
    RECORDING,
    IN_RENDERPASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_renderpass_state;

typedef struct vulkan_renderpass{
    VkRenderPass handle;
    f32 x,y,w,h;
    f32 r,g,b,a;
    f32 depth;
    u32 stencil;
    vulkan_renderpass_state state;
}vulkan_renderpass;

typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
    vulkan_image depth_attachment;
} vulkan_swapchain;

typedef enum vulkan_command_buffer_state{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDERPASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

typedef struct vulkan_command_buffer{
    VkCommandBuffer handle;

    vulkan_command_buffer_state state;
} vulkan_command_buffer;

// TODO: this structure grow rapidly through out the development
typedef struct vulkan_context{
    u32 framebuffer_width;
    u32 framebuffer_height;    
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    vulkan_device device;

    vulkan_swapchain swapchain;
    u32 image_index;
    u32 current_frame;

    vulkan_renderpass main_renderpass;

    b8 recreating_swapchain;

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
} vulkan_context;

#define VK_CHECK(expr) \
    DASSERT( (expr) == VK_SUCCESS );