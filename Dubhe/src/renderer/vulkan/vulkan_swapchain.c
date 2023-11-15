#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "core/asserts.h"
#include "core/dmemory.h"

#include "vulkan_device.h"
#include "vulkan_image.h"

void internal_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_vulkan_swapchain);
void internal_destroy(vulkan_context* context, vulkan_swapchain* swapchain);

void vulkan_swapchain_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_vulkan_swapchain)
{
    internal_create(context, width, height, out_vulkan_swapchain);
}

void vulkan_swapchain_recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_vulkan_swapchain)
{
    internal_destroy(context, out_vulkan_swapchain);
    internal_create(context, width, height, out_vulkan_swapchain);
}

void vulkan_swapchain_destroy(vulkan_context* context, vulkan_swapchain* vulkan_swapchain)
{
    internal_destroy(context, vulkan_swapchain);
}

b8 vulkan_swapchain_acquire_next_image_index(vulkan_context* context, 
                                                vulkan_swapchain* swapchain, 
                                                u64 timeout_ns, 
                                                VkSemaphore image_available_semaphore, 
                                                VkFence fence, 
                                                u32* out_image_index)
{
    VkResult result = vkAcquireNextImageKHR(context->device.logical_device, swapchain->handle, timeout_ns, image_available_semaphore, fence, out_image_index);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // 它指示交换链swapchain不再与surface兼容，或者surface已经更改，因此不能再用于渲染。简单来说，它通常意味着交换链需要重新创建。
        // Trigger the swapchain recreation, but then boot out of the render loop.
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return FALSE;
    }
    /*
     * 当你得到 VK_SUBOPTIMAL_KHR 时，这意味着交换链仍可以继续工作，但它的表面属性与物理设备的当前需求不完全匹配。
     * 例如：窗口尺寸变化：最常见的原因是窗口大小已更改，例如，用户调整了窗口的大小。虽然交换链可能仍然可以在新大小的窗口上呈现图像，但这可能不是最高效的方式。
     * 表面细节更改：其他原因可能包括表面的格式、颜色空间等属性发生了变化。
     * VK_SUBOPTIMAL_KHR 与 VK_ERROR_OUT_OF_DATE_KHR 的主要区别在于，前者表示交换链仍然可以使用，但可能不是最佳状态。而后者则表示交换链不能继续使用，必须重新创建。
     */
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        DFATAL("Failed to acquire swapchain image!");
        return FALSE;
    }

    return TRUE;
}

// FIXME: param named graphics_queue not used.
void vulkan_swapchain_present(vulkan_context* context, 
                              vulkan_swapchain* swapchain, 
                              VkQueue graphics_queue, 
                              VkQueue present_queue, 
                              VkSemaphore render_complete_semaphore, 
                              u32 present_image_index)
{
    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    }
    else if(result != VK_SUCCESS)
    {
        DFATAL("Failed to present swapchain image");
    }
}

void internal_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_vulkan_swapchain)
{
    VkExtent2D swapchain_extent = {width, height};
    // NOTE:定义了同时处于渲染流程的最大帧数，也就是最多有2帧在渲染
    // Frames in flight 指的是已经开始渲染但是尚未完全完成所有操作（如绘制，呈现）的帧
    out_vulkan_swapchain->max_frames_in_flight = 2;

    // Choose swap image format
    b8 found = FALSE;
    for(u32 i = 0; i < context->device.swapchain_support.format_count; i++)
    {
        VkSurfaceFormatKHR format = context->device.swapchain_support.formats[i];
        if(format.format == VK_FORMAT_B8G8R8A8_UNORM && 
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            out_vulkan_swapchain->image_format = format;
            found = TRUE;
            break;
        }
    }

    if(!found)
    {
        out_vulkan_swapchain->image_format = context->device.swapchain_support.formats[0];
    }

    // Choose present mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for(u32 i = 0; i < context->device.swapchain_support.present_mode_count; i++)
    {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    // Requery swapchain support
    // NOTE: why requery?
    vulkan_device_query_swapchain_support(context->device.physical_device, 
                                          context->surface, 
                                          &context->device.swapchain_support);
    
    
    // Swapchain extent
    /*
     * 这里检查 currentExtent.width 是否不等于 UINT32_MAX。
     * currentExtent 是 VkSurfaceCapabilitiesKHR 结构的一个字段，表示表面的当前外延（即大小）。
     * 如果 currentExtent.width 等于 UINT32_MAX，则表示 Vulkan 表面的大小是由窗口系统决定的，应用程序可以自由设置它。
     * 如果不是 UINT32_MAX，则表示表面大小已经被窗口系统固定，应用程序必须使用这个大小。   
     */
    if(context->device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX)
    {
        swapchain_extent = context->device.swapchain_support.capabilities.currentExtent;
    }

    // Clamp to the value allowed by the GPU.
    VkExtent2D min = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_support.capabilities.maxImageExtent;
    swapchain_extent.width = DCLAMP(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = DCLAMP(swapchain_extent.height, min.height, max.height);

    u32 image_count = context->device.swapchain_support.capabilities.minImageCount + 1;
    if(context->device.swapchain_support.capabilities.maxImageCount > 0 && image_count > context->device.swapchain_support.capabilities.maxImageCount)
    {
        image_count = context->device.swapchain_support.capabilities.maxImageCount;
    }

    // Swapchain create info
    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = out_vulkan_swapchain->image_format.format;
    swapchain_create_info.imageColorSpace = out_vulkan_swapchain->image_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    //这行代码设置 imageArrayLayers 字段的值为 1，表示每个图像只有一个层。在大多数常规应用中，交换链中的图像只需要一个层。这个设置常用于2D应用程序，如传统的游戏和桌面应用。
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Setup the queue family indices
    if(context->device.graphics_queue_index != context->device.present_queue_index)
    {
        u32 queueFamilyIndices[] = {(u32)context->device.graphics_queue_index,
                                    (u32)context->device.present_queue_index};
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = 0;
    }

    swapchain_create_info.preTransform = context->device.swapchain_support.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device,&swapchain_create_info,context->allocator,&out_vulkan_swapchain->handle));

    context->current_frame = 0;

    // Get images
    out_vulkan_swapchain->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, out_vulkan_swapchain->handle, &out_vulkan_swapchain->image_count, 0));
    if(!out_vulkan_swapchain->images)
    {
        out_vulkan_swapchain->images = dallocate(sizeof(VkImage) * out_vulkan_swapchain->image_count, MEMORY_TAG_RENDERER);
    }
    if(!out_vulkan_swapchain->views)
    {
        out_vulkan_swapchain->views = dallocate(sizeof(VkImage) * out_vulkan_swapchain->image_count, MEMORY_TAG_RENDERER);
    }
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, out_vulkan_swapchain->handle, &out_vulkan_swapchain->image_count, out_vulkan_swapchain->images));

    // Create one view for each image
    for(u32 i = 0; i < out_vulkan_swapchain->image_count; i++)
    {
        VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_create_info.image = out_vulkan_swapchain->images[i];
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_create_info.format = out_vulkan_swapchain->image_format.format;
        view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_create_info.subresourceRange.baseMipLevel = 0;
        view_create_info.subresourceRange.levelCount = 1;
        view_create_info.subresourceRange.baseArrayLayer = 0;
        view_create_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &out_vulkan_swapchain->views[i]));
    }

    // Depth resources
    if(!vulkan_detect_device_depth_format(&context->device))
    {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        DFATAL("Failed to find a supported format.");
    }

    // Create depth image and its view
    vulkan_image_create(context, 
                        VK_IMAGE_TYPE_2D,
                        swapchain_extent.width,
                        swapchain_extent.height,
                        context->device.depth_format,
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        TRUE,
                        VK_IMAGE_ASPECT_DEPTH_BIT,
                        &out_vulkan_swapchain->depth_attachment);
    
    DINFO("Swapchain created successfully.");
}

void internal_destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{
    vulkan_image_deatroy(context, &swapchain->depth_attachment);

    // Only destroy the views, not the images, since those are owned by the swapchain and are thus
    // destroyed when it is.
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        vkDestroyImageView(context->device.logical_device, swapchain->views[i], context->allocator);
    }

    vkDestroySwapchainKHR(context->device.logical_device, swapchain->handle, context->allocator);
}