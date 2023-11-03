#pragma once

#include "defines.h"
#include "core/asserts.h"

#include <vulkan/vulkan.h>

// TODO: this structure grow rapidly through out the development
typedef struct vulkan_context{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vulkan_context;

#define VK_CHECK(expr) \
    DASSERT( (expr) == VK_SUCCESS );