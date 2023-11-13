#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "core/asserts.h"
#include "core/dmemory.h"

#include "vulkan_device.h"

void internal_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* out_vulkan_swapchain);
void internal_destroy(vulkan_context* context, vulkan_swapchain* vulkan_swapchain);

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