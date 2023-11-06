#pragma once

#include "vulkan_types.inl"

void vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vulkan_swapchain_support_info* out_support_info);

b8 physical_device_meets_requirements(VkPhysicalDevice physical_device, 
VkSurfaceKHR surface, 
const VkPhysicalDeviceProperties* properties, 
const VkPhysicalDeviceFeatures* features, 
const vulkan_physical_device_requriements* requirements,
vulkan_physical_device_queue_family_info* out_queue_info,
vulkan_swapchain_support_info* out_swapchain_support);

b8 seletc_physical_device(vulkan_context* context);

b8 vulkan_device_create(vulkan_context* context);

void vulkan_device_destroy(vulkan_context* context);