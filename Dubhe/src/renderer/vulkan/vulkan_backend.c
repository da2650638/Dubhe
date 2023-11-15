#include "vulkan_backend.h"

#include "core/logger.h"
#include "core/asserts.h"
#include "core/dmemory.h"
#include "core/dstring.h"
#include "platform/platform.h"
#include "containers/darray.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

static vulkan_context context;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state)
{
    context.find_memory_index = find_memory_index;

    context.allocator = 0;

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
        b8 found = FALSE;
        for(u32 j = 0; j < available_layer_count; j++)
        {
            if(strings_equal(available_layer_properties[j].layerName, required_validation_layer_names[i]))
            {
                found = TRUE;
                DINFO("Found.");
                break;
            }
        }

        if(!found)
        {
            DFATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return FALSE;
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
        return FALSE;
    }
    DDEBUG("Vulkan surface created.");


    // NOTE: (4)Create device
    DDEBUG("Creating vulkan device...");
    if (!vulkan_device_create(&context)) {
        DERROR("Failed to create device!");
        return FALSE;
    }
    DDEBUG("Vulkan device created...");

    // NOTE: (5)Create swapchain
    DDEBUG("Creating vulkan swapchain...");
    vulkan_swapchain_create(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);
    DDEBUG("Vulkan swapchain created...");

    DINFO("Vulkan renderer backend initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend)
{
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

}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time)
{
    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time)
{
    return TRUE;
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