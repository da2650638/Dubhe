#include "vulkan_device.h"

#include "core/dmemory.h"
#include "containers/darray.h"
#include "core/dstring.h"
#include "core/logger.h"

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_support_info)
{
    // Get surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));

    // Get surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info->format_count,
        0));
    if(out_support_info->format_count != 0)
    {
        if(!out_support_info->formats)
        {
            out_support_info->formats = (VkSurfaceFormatKHR*)dallocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    // Get surface present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info->present_mode_count,
        0));
    if(out_support_info->present_mode_count != 0)
    {
        if(!out_support_info->present_modes)
        {
            out_support_info->present_modes = (VkPresentModeKHR*)dallocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

b8 physical_device_meets_requirements(VkPhysicalDevice physical_device, 
    VkSurfaceKHR surface, 
    const VkPhysicalDeviceProperties* properties, 
    const VkPhysicalDeviceFeatures* features, 
    const vulkan_physical_device_requriements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_support_info* out_swapchain_support)
{   
    //
    out_queue_info->graphics_family_index = -1;
    out_queue_info->present_family_index = -1;
    out_queue_info->compute_family_index = -1;
    out_queue_info->transfer_family_index = -1;

    // Need discrete gpu?
    if(requirements->discrete_gpu)
    {
        if(properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            DINFO("Device is not a discrete GPU, and one discrete gpu is required. Skipping.");
            return FALSE;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
        &queue_family_count,
        0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
        &queue_family_count,
        queue_families);
    // Look at each queue and see what queues it supports.
    DINFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for(u32 i = 0; i < queue_family_count; i++)
    {
        u8 current_transfer_score = 0;
        // Graphics queue?
        if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
        }

        // Compute queue?
        if(queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
        }

        // Tranfer queue?
        if(queue_families[i].queueFlags * VK_QUEUE_TRANSFER_BIT)
        {

            if(current_transfer_score <= min_transfer_score)
            {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
            }
        }

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &supports_present));
        if(supports_present == VK_SUCCESS)
        {
            out_queue_info->present_family_index = i;
        }
    }

    DINFO("       %d |       %d |       %d |        %d | %s", 
    out_queue_info->graphics_family_index != -1, 
    out_queue_info->present_family_index != -1, 
    out_queue_info->compute_family_index != -1, 
    out_queue_info->transfer_family_index != -1,
    properties->deviceName);

    if((!requirements->graphics || out_queue_info->graphics_family_index != -1) &&
       (!requirements->present || out_queue_info->present_family_index != -1) &&
       (!requirements->compute || out_queue_info->compute_family_index != -1) &&
       (!requirements->transfer || out_queue_info->transfer_family_index != -1))
    {
        // 如果找到了我们所需要的队列
        DINFO("Device meets queue requirements.");
        DTRACE("Graphics Family Index: %i", out_queue_info->graphics_family_index);
        DTRACE("Present Family Index:  %i", out_queue_info->present_family_index);
        DTRACE("Transfer Family Index: %i", out_queue_info->transfer_family_index);
        DTRACE("Compute Family Index:  %i", out_queue_info->compute_family_index);

        // Query swapchain support.
        vulkan_device_query_swapchain_support(
            physical_device, 
            surface, 
            out_swapchain_support);
        
        if(out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1)
        {
            if(out_swapchain_support->formats)
            {
                dfree(out_swapchain_support->formats, out_swapchain_support->format_count * sizeof(VkSurfaceFormatKHR), MEMORY_TAG_RENDERER);
            }
            if(out_swapchain_support->present_modes)
            {
                dfree(out_swapchain_support->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count, MEMORY_TAG_RENDERER);
            }
            DINFO("Required swapchain support not present, skipping device.");
            return FALSE;
        }

        // Device extensions.
        if(requirements->device_extension_names)
        {
            u32 available_extension_count = 0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device,
                0,
                &available_extension_count,
                0));
            if(available_extension_count != 0)
            {
                available_extensions = (VkExtensionProperties*)dallocate(sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device,
                0,
                &available_extension_count,
                available_extensions));

                u32 required_extension_count = darray_length(requirements->device_extension_names);
                for(u32 i = 0; i < required_extension_count; ++i)
                {
                    b8 found = FALSE;
                    for(u32 j = 0; j < available_extension_count; ++j)
                    {
                        if(strings_equal(requirements->device_extension_names[i], available_extensions[j].extensionName))
                        {
                            found = TRUE;
                            break;
                        }
                    }

                    if(!found)
                    {
                        dfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                        return FALSE;
                    }
                }
                dfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
            }
        }

        // Check sampler anisotropy
        if(requirements->sampler_anisotropy && !features->samplerAnisotropy)
        {
            DINFO("Device does not support samplerAnisotropy, skipping.");
            return FALSE;
        }

        // Device meets all requirements.
        return TRUE;
    }

    return FALSE;
}

b8 seletc_physical_device(vulkan_context* context)
{
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));
    if(physical_device_count == 0)
    {
        DERROR("No devices which support vulkan found.");
        return FALSE;
    }
    VkPhysicalDevice physical_devices[physical_device_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));
    for(u32 i =0; i < physical_device_count; ++i)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // TODO: make the requirement configurable.
        vulkan_physical_device_requriements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        // NOTE: Enable this if compute will be required.
        // requirements.compute = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(physical_devices[i], context->surface, &properties, &features, &requirements, &queue_info, &context->device.swapchain_support);
        // Only if this deivce meets requirements
        if(result)
        {
            DINFO("Selected device: %s", properties.deviceName);
            switch(properties.deviceType)
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                {
                    DINFO("GPU type is Unknown.");
                    break;
                }
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                {
                    DINFO("GPU type is Intergrated.");
                    break;
                }
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                {
                    DINFO("GPU type is Discrete.");
                    break;
                }
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                {
                    DINFO("GPU type is Virtual.");
                    break;
                }
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                {
                    DINFO("GPU type is CPU.");
                    break;
                }
            }

            DINFO("GPU Driver version: %d.%d.%d",
            VK_VERSION_MAJOR(properties.driverVersion),
            VK_VERSION_MINOR(properties.driverVersion),
            VK_VERSION_PATCH(properties.driverVersion));

            DINFO("Vulkan API version: %d.%d.%d",
            VK_VERSION_MAJOR(properties.apiVersion),
            VK_VERSION_MINOR(properties.apiVersion),
            VK_VERSION_PATCH(properties.apiVersion));

            // Memory info
            for(u32 j = 0; j < memory.memoryHeapCount; ++j)
            {
                f32 memory_size_gib = (((f32)memory.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f);
                if(memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    DINFO("Local GPU memory: %.2f GiB", memory_size_gib);
                }
                else
                {
                    DINFO("Shared GPU memory: %.2f GiB", memory_size_gib);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index = queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;
            context->device.properties = properties;
            context->device.features = features;
            context->device.memory = memory;
            break;
        }
    }

    // Ensure a device was selected.
    if(!context->device.physical_device)
    {
        DERROR("No physical devices were found which meet the requirements.");
        return FALSE;
    }

    DINFO("Physical device selected.");
    return TRUE;
}

b8 vulkan_device_create(vulkan_context* context)
{   
    // NOTE: physical devices do not need to be created, only selected, so we do not need destroy physical device which we selected.
    if(!seletc_physical_device(context))
    {
        DERROR("No physical device selected.");
        return FALSE;
    }
    return TRUE;
}

void vulkan_device_destroy(vulkan_context* context)
{

}