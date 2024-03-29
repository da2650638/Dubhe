#pragma once

#include "defines.h"
#include "core/asserts.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(expr) \
    DASSERT( (expr) == VK_SUCCESS );

typedef struct vulkan_buffer 
{
    u64 total_size;
    VkBuffer handle;
    VkBufferUsageFlagBits usage;
    b8 is_locked;
    VkDeviceMemory memory;
    i32 memory_index;
    u32 memory_property_flags;
} vulkan_buffer;

// 存储vulkan交换链支持信息的数据结构
typedef struct vulkan_swapchain_support_info{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_physical_device_requirements{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    // darray
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
}vulkan_physical_device_requirements;

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

    VkCommandPool graphics_command_pool;

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

typedef struct vulkan_framebuffer{
    VkFramebuffer handle;
    u32 attachment_count;
    VkImageView* attachments;
    vulkan_renderpass* renderpass;
} vulkan_framebuffer;

typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
    vulkan_image depth_attachment;

    // darray. framebuffers used for on-screen rendering.
    vulkan_framebuffer* framebuffers;
} vulkan_swapchain;

typedef enum vulkan_command_buffer_state{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDERPASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED  // Initialized state
} vulkan_command_buffer_state;

typedef struct vulkan_command_buffer{
    VkCommandBuffer handle;

    vulkan_command_buffer_state state;
} vulkan_command_buffer;

typedef struct vulkan_fence{
    VkFence handle;
    b8 is_signaled;
} vulkan_fence;

typedef struct vulkan_shader_stage
{
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
}vulkan_shader_stage;

typedef struct vulkan_pipeline
{
    VkPipeline handle;
    VkPipelineLayout pipeline_layout;
}vulkan_pipeline;

#define OBJECT_SHADER_STAGE_COUNT 2 // vertex and fragment
typedef struct vulkan_object_shader
{
    vulkan_shader_stage stages[OBJECT_SHADER_STAGE_COUNT];

    vulkan_pipeline pipeline;

} vulkan_object_shader;

// TODO: this structure grow rapidly through out the development
typedef struct vulkan_context{
    u32 framebuffer_width;
    u32 framebuffer_height;  
    u64 framebuffer_size_generation;
    u64 framebuffer_size_last_generation;

    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
    vulkan_device device;

    vulkan_swapchain swapchain;
    vulkan_renderpass main_renderpass;

    vulkan_buffer object_vertex_buffer;
    vulkan_buffer object_index_buffer;
    
    // darray
    vulkan_command_buffer* graphics_command_buffers;

    // darray
    VkSemaphore* image_available_semaphores;

    // darray
    VkSemaphore* queue_complete_semaphores;

    u32 in_flight_fence_count;
    // darray
    vulkan_fence* in_flight_fences;
    // TODO: darray?
    vulkan_fence** images_in_flight;

    u32 image_index;
    u32 current_frame;

    b8 recreating_swapchain;

    vulkan_object_shader object_shader;

    u64 geometry_vertex_offset;
    u64 geometry_index_offset;

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
} vulkan_context;