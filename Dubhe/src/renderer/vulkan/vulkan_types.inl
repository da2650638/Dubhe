#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

// TODO: this structure grow rapidly through out the development
typedef struct vulkan_context{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
} vulkan_context;