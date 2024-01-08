#include "vulkan_backend.h"

#include "core/logger.h"
#include "core/asserts.h"
#include "core/dmemory.h"
#include "core/dstring.h"
#include "core/application.h"
#include "platform/platform.h"
#include "containers/darray.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_fence.h"
#include "vulkan_utils.h"

static vulkan_context context;
static u32 cached_framebuffer_width = 0;
static u32 cached_framebuffer_height = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state)
{
    context.find_memory_index = find_memory_index;

    context.allocator = 0;

    application_get_framebuffer_size(&cached_framebuffer_width, &cached_framebuffer_height);
    context.framebuffer_width = cached_framebuffer_width != 0 ? cached_framebuffer_width : 800;
    context.framebuffer_height = cached_framebuffer_height != 0 ? cached_framebuffer_height : 600;
    cached_framebuffer_width = cached_framebuffer_height = 0;

    // Setup VkApplicationInfo
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    app_info.pEngineName = "Dubhe";
    app_info.engineVersion = VK_MAKE_VERSION(1,0,0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;

    // Obtain a list of requried extensions
    const char** required_extensions = darray_create(const char*);
    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);   // Generic surface extension
    platform_get_required_extension_names(&required_extensions); 
#if defined(_DEBUG)
    darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Debug utilities
    DDEBUG("Required extensions:");
    u32 length = darray_length(required_extensions);
    for(u32 i = 0; i < length; i++)
    {
        DDEBUG("    %s", required_extensions[i]);
    }
#endif   
    create_info.enabledExtensionCount = darray_length(required_extensions);
    create_info.ppEnabledExtensionNames = required_extensions;

    // Validation layers.
    const char** required_validation_layer_names  = 0;
    u32 required_validation_layer_count = 0;
#if defined(_DEBUG)
    DINFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    required_validation_layer_names = darray_create(const char*);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = darray_length(required_validation_layer_names);

    // Obtain a list of available validation layers
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties* available_layer_properties = darray_reserve(VkLayerProperties, available_layer_count);
    // NOTE: 这么操作其实并没有把darray available_layer_properties的length属性改变。
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layer_properties));

    // Verify all layers are available
    for(u32 i = 0; i < required_validation_layer_count; i++)
    {
        DINFO("Serach for layer: %s......", required_validation_layer_names[i]);
        b8 found = false;
        for(u32 j = 0; j < available_layer_count; j++)
        {
            if(strings_equal(available_layer_properties[j].layerName, required_validation_layer_names[i]))
            {
                found = true;
                DINFO("Found.");
                break;
            }
        }

        if(!found)
        {
            DFATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return false;
        }
    }
    DINFO("All required layers are ready.");
#endif
    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;

    // NOTE: (1)Create vulkan instance
    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));

#if defined(_DEBUG)
    DDEBUG("Creating vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    // NOTE: (2)Create vulkan debugger if in debugg mode
    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    DDEBUG("Vulkan debugger created.");
#endif

    // NOTE: (3)Create surface
    // FIXME: destroy surface after vulkan_device_destroy
    DDEBUG("Creating vulkan surface...");
    if (!platform_create_vulkan_surface(plat_state, &context)) {
        DERROR("Failed to create platform surface!");
        return false;
    }
    DDEBUG("Vulkan surface created.");


    // NOTE: (4)Create device
    DDEBUG("Creating vulkan device...");
    if (!vulkan_device_create(&context)) {
        DERROR("Failed to create device!");
        return false;
    }
    DDEBUG("Vulkan device created...");

    // NOTE: (5)Create swapchain
    DDEBUG("Creating vulkan swapchain...");
    vulkan_swapchain_create(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);
    DDEBUG("Vulkan swapchain created...");

    // NOTE: (6)Create Renderpass
    DDEBUG("Creating vulan renderpass...");
    vulkan_renderpass_create(&context, &context.main_renderpass,
    0,0,context.framebuffer_width,context.framebuffer_height,
    0.0f, 0.0f, 0.2f, 1.0f,
    1.0f,0);
    DDEBUG("Vulkan renderpass created...");

    // NOTE:(7)Create framebuffers
    // TODO: destroy these framebuffer in somewhere.
    DDEBUG("Creating vulkan framebuffer...");
    context.swapchain.framebuffers = darray_reserve(vulkan_framebuffer, context.swapchain.image_count);
    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);
    DDEBUG("Vulkan framebuffer created...");

    // NOTE: (8)Create Command Buffers
    DDEBUG("Creating vulkan command buffers...");
    create_command_buffers(backend);
    DDEBUG("Vulkan command buffers created...");

    // NOTE: (9)Create sync objects
    DDEBUG("Creating vulkan sync objects...");
    context.image_available_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.queue_complete_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.in_flight_fences = darray_reserve(vulkan_fence, context.swapchain.max_frames_in_flight);
    for(u8 i = 0; i < context.swapchain.max_frames_in_flight; i++)
    {
        VkSemaphoreCreateInfo create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(context.device.logical_device, &create_info, context.allocator, &context.image_available_semaphores[i]);
        vkCreateSemaphore(context.device.logical_device, &create_info, context.allocator, &context.queue_complete_semaphores[i]);

        // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered until a frame is "rendered" before it.
        vulkan_fence_create(&context, true, &context.in_flight_fences[i]);
    }
    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
    // by this list.
    context.images_in_flight = darray_reserve(vulkan_fence, context.swapchain.image_count);
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        context.images_in_flight[i] = 0;
    }
    DDEBUG("Vulkan sync objects created...");

    DINFO("Vulkan renderer backend initialized successfully.");
    return true;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend)
{
    vkDeviceWaitIdle(context.device.logical_device);

    // NOTE: (9)Destroy Sync objects
    DDEBUG("Destroying vulkan sync objects");
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
        if (context.image_available_semaphores[i]) {
            vkDestroySemaphore(
                context.device.logical_device,
                context.image_available_semaphores[i],
                context.allocator);
            context.image_available_semaphores[i] = 0;
        }
        if (context.queue_complete_semaphores[i]) {
            vkDestroySemaphore(
                context.device.logical_device,
                context.queue_complete_semaphores[i],
                context.allocator);
            context.queue_complete_semaphores[i] = 0;
        }
        vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
    }
    darray_destroy(context.image_available_semaphores);
    context.image_available_semaphores = 0;
    darray_destroy(context.queue_complete_semaphores);
    context.queue_complete_semaphores = 0;
    darray_destroy(context.in_flight_fences);
    context.in_flight_fences = 0;
    darray_destroy(context.images_in_flight);
    context.images_in_flight = 0;

    // NOTE: (8)Destroy Command Buffers
    DDEBUG("Destroying command buffers...");
    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        if(context.graphics_command_buffers[i].handle)
        {
            vulkan_command_buffer_free(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);
            context.graphics_command_buffers[i].handle = 0;
        }
    }
    darray_destroy(context.graphics_command_buffers);
    context.graphics_command_buffers = 0;

    // NOTE: (7)Destroy the framebuffer
    DDEBUG("Destroying vulkan framebuffer...");
    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        vulkan_framebuffer_destroy(&context,&context.swapchain.framebuffers[i]);
    }
    darray_destroy(context.swapchain.framebuffers);

    // NOTE: (6)Destroy renderpass
    DDEBUG("Destroying vulkan renderpass...");
    vulkan_renderpass_destroy(&context, &context.main_renderpass);

    // NOTE: (5)Destroy swapchain and its depth image and view
    DDEBUG("Destroying vulkan swapchain...");
    vulkan_swapchain_destroy(&context, &context.swapchain);

    // Destroy in the opposite order of the creation
    // NOTE: (4)Destroying vulkan device
    DDEBUG("Destroying vulkan device...");
    vulkan_device_destroy(&context);

    // NOTE: (3)Destroying vulkan surface;
    DDEBUG("Destroying vulkan surface...");
    if(context.surface)
    {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

#if defined(_DEBUG)
    // NOTE: (2)Destroying vulkan debugger
    DINFO("Destroying vulkan debugger...");
    if(context.debug_messenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }
    DINFO("Done.");
#endif
    // NOTE: (1)Destroying vulkan instance
    DINFO("Destroying vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
    DINFO("Done.");
}

void vulkan_renderer_backend_resized(renderer_backend* backend, u16 width, u16 height)
{
    cached_framebuffer_width = width;
    cached_framebuffer_height = height;
    context.framebuffer_size_generation++;

    DINFO("Vulkan render backend resized: [width/height/generation: %i/%i/%llu]", width, height, context.framebuffer_size_generation);

}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time)
{
    vulkan_device* device = &context.device;

    // Check if recreating swapchain and boot out.
    if(context.recreating_swapchain)
    {
        VkResult result = vkDeviceWaitIdle(device->logical_device);
        if(!vulkan_result_is_success(result))
        {
            DERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed: '%s'", vulkan_result_string(result, true));
            return false;
        }
        DINFO("Recreating swapchain, booting...");
        return false;
    }

    // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    if(context.framebuffer_size_generation != context.framebuffer_size_last_generation)
    {
        VkResult result = vkDeviceWaitIdle(device->logical_device);
        if(!vulkan_result_is_success(result))
        {
            DERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '%s'", vulkan_result_string(result, true));
            return false;
        }

        // If the swapchain recreation failed because , for example, the window was minimized.
        // boot out before unsetting the flag
        if(!recreate_swapchain(backend))
        {
            return false;
        }

        DINFO("Resized, booting...");
        return false;
    }

    if(!vulkan_fence_wait(
        &context, 
        &context.in_flight_fences[context.current_frame], 
        (uint64_t)0xffffffffffffffff))
    {
        DWARN("In-flaght fence wait failure.");
        return false;
    }

    if(!vulkan_swapchain_acquire_next_image_index(
        &context, 
        &context.swapchain, 
        (uint64_t)0xffffffffffffffff, 
        context.image_available_semaphores[context.current_frame],
        0,
        &context.image_index))
    {
        return false;
    }

    vulkan_command_buffer* command_buffer = &context.graphics_command_buffers[context.image_index];
    vulkan_command_buffer_reset(command_buffer);
    vulkan_command_buffer_begin(command_buffer, false, false, false);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context.framebuffer_height;
    viewport.width = (f32)context.framebuffer_width;
    viewport.height = (f32)context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context.framebuffer_width;
    scissor.extent.height = context.framebuffer_height;

    vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);

    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    vulkan_renderpass_begin(command_buffer, &context.main_renderpass, context.swapchain.framebuffers[context.image_index].handle);

    return true;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time)
{
    vulkan_command_buffer* command_buffer = &context.graphics_command_buffers[context.image_index];

    vulkan_renderpass_end(command_buffer, &context.main_renderpass);

    vulkan_command_buffer_end(command_buffer);

    if(context.images_in_flight[context.image_index] != VK_NULL_HANDLE)
    {
        vulkan_fence_wait(&context, context.images_in_flight[context.image_index], (uint64_t)0xffffffffffffffff);
    }

    context.images_in_flight[context.image_index] = &context.in_flight_fences[context.current_frame];

    vulkan_fence_reset(&context, &context.in_flight_fences[context.current_frame]);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &context.queue_complete_semaphores[context.current_frame];

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &context.image_available_semaphores[context.current_frame];

    VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    VkResult result = vkQueueSubmit(context.device.graphics_queue, 1, &submit_info, context.in_flight_fences[context.current_frame].handle);

    if(result != VK_SUCCESS)
    {
        DERROR("vkQueueSubmit failed with result: %s", vulkan_result_string(result, true));
        return false;
    }

    vulkan_command_buffer_update_submitted(command_buffer);

    vulkan_swapchain_present(&context,
        &context.swapchain,
        context.device.graphics_queue,
        context.device.present_queue,
        context.queue_complete_semaphores[context.current_frame],
        context.image_index);

    return true;
}

VKAPI_ATTR VkBool32 vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData)
{
    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            DERROR(pCallbackData->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            DWARN(pCallbackData->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        {
            DINFO(pCallbackData->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        {
            DTRACE(pCallbackData->pMessage);
            break;
        }
        default:
        {
            break;
        }
    }

    return VK_FALSE;
}

// FIXME: 不懂这段代码是什么意思，查阅一下。
i32 find_memory_index(u32 type_filter, u32 property_flags)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device,&memory_properties);

    for(u32 i = 0; i < memory_properties.memoryTypeCount; i++)
    {
        if(type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
        {
            return i;
        }
    }

    DWARN("Unable to find suitable memory type!");
    return -1;
}

void create_command_buffers(renderer_backend* backend)
{
    if(!context.graphics_command_buffers)
    {
        context.graphics_command_buffers = darray_reserve(vulkan_command_buffer, context.swapchain.image_count);
        for(u32 i = 0; i < context.swapchain.image_count; i++)
        {
            dzero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        }
    }

    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        if(context.graphics_command_buffers[i].handle)
        {
            vulkan_command_buffer_free(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);
        }
        dzero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        vulkan_command_buffer_allocate(&context, context.device.graphics_command_pool, true, &context.graphics_command_buffers[i]);
    }
}

void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass)
{
    // NOTE: 这里边创建的framebuffer会在recreate_swapchain函数中被销毁，因此不会有内存泄漏问题。
    for(u32 i = 0; i < swapchain->image_count; i++)
    {
        // TODO: make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        VkImageView attachments[2] = {swapchain->views[i], swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context, 
            &context.main_renderpass, 
            context.framebuffer_width, 
            context.framebuffer_height, 
            attachment_count, 
            attachments, 
            &context.swapchain.framebuffers[i]);
    }
}

b8 recreate_swapchain(renderer_backend* backend)
{
    if(context.recreating_swapchain)
    {
        DDEBUG("recreate_swapchain called when already recreating. Booting.");
        return false;
    }

    if(context.framebuffer_width == 0 || context.framebuffer_height == 0)
    {
        DDEBUG("recreate_swapchain called when window is < 1 in a dimension. Booting.");
        return false;
    }

    context.recreating_swapchain = true;

    vkDeviceWaitIdle(context.device.logical_device);

    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        context.images_in_flight[i] = 0;
    }

    vulkan_device_query_swapchain_support(context.device.physical_device, context.surface, &context.device.swapchain_support);
    vulkan_detect_device_depth_format(&context.device);

    vulkan_swapchain_recreate(&context, cached_framebuffer_width, cached_framebuffer_height, &context.swapchain);

    context.framebuffer_width = cached_framebuffer_width;
    context.framebuffer_height = cached_framebuffer_height;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;

    context.framebuffer_size_last_generation = context.framebuffer_size_generation;

    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        vulkan_command_buffer_free(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);
    }

    for(u32 i = 0; i < context.swapchain.image_count; i++)
    {
        vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    context.main_renderpass.x = 0;
    context.main_renderpass.y = 0;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    create_command_buffers(backend);

    context.recreating_swapchain = false;

    return true;
}