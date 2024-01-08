#include "platform/platform.h"

// Windows platform layer.
#if DPLATFORM_WINDOWS

#include <core/logger.h>
#include "core/input.h"
#include "core/event.h"

#include "containers/darray.h"

#include <windows.h>
#include <windowsx.h> // param input extraction
#include <stdlib.h>

// For surface creation
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "renderer/vulkan/vulkan_types.inl"

typedef struct platform_state
{
    HINSTANCE h_instance;   // handler of the instance of the windows application
    HWND hwnd;              // handler of the windows window
    VkSurfaceKHR surface;
    f64 clock_frequency;
    LARGE_INTEGER start_time;
}platform_state;

static f64 clock_frequency;

static platform_state* state_ptr;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_system_startup(
    u64* memory_requirement,
    void* new_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height)
{
    //new state
    *memory_requirement = sizeof(platform_state);
    if (new_state == 0) {
        return true;
    }
    state_ptr = new_state;
    state_ptr->h_instance = GetModuleHandleA(0);

    // Setup and register window class
    HICON icon = LoadIcon(state_ptr->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;                          // Get double-clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state_ptr->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);       
    wc.hbrBackground = NULL;                        // Transparent
    wc.lpszClassName = "Dubhe_window_class";

    if(!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // In this case, the border rectangle is negative.
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the OS border.
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style, 
                                "Dubhe_window_class", 
                                application_name,
                                window_style, 
                                window_x, 
                                window_y, 
                                window_width, 
                                window_height, 
                                0, 
                                0, 
                                state_ptr->h_instance, 
                                0);
    
    if (handle == 0) 
    {
        MessageBoxA(NULL, "Window Creation failed.", "Error", MB_ICONEXCLAMATION | MB_OK);
        DFATAL("Wdinwo Createion Failed!");
        return false;
    }
    else
    {
        // internal_state->hwnd = handle;
        // new state
        state_ptr->hwnd = handle;
    }

    //Show the window
    b32 should_activate = 1;    // TODO: if the window should not accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW/* SW_MINMIZE/SW_SHOWMAXIMIZED */ : SW_SHOWNOACTIVATE; 
    ShowWindow(state_ptr->hwnd, show_window_command_flags);

    // Clock setup
    // get clock frequency and setup the starttime
    // new state
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    state_ptr->clock_frequency = 1.0 / (f64)frequency.QuadPart;
    clock_frequency = state_ptr->clock_frequency;
    QueryPerformanceCounter(&state_ptr->start_time);

    return true;
}

void platform_system_shutdown(void* new_state)
{
    // new state
    if (state_ptr && state_ptr->hwnd) {
        DestroyWindow(state_ptr->hwnd);
        state_ptr->hwnd = 0;
    }
}

b8 platform_pump_message()
{
    if (state_ptr) 
    {
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }

    return true;
}

void* platform_allocate(u64 size, b8 aligned)
{
    // TODO: very temporary
    return malloc(size);
}

void platform_free(void* block, b8 aligned)
{
    // TODO: very temporary
    free(block);
}

void* platform_zero_memory(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* src, u64 size)
{
    return memcpy(dest, src, size);
}

void* platform_set_memory(void* dest, i32 value, u64 size)
{
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color)
{
    // TODO: 弄明白这段代码的意思
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {BACKGROUND_RED, FOREGROUND_RED, FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_INTENSITY};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);        //往开发环境控制台输出字符串
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);  //往Windows控制台输出字符串
}

void platform_console_write_error(const char* message, u8 color)
{
    // TODO: 弄明白这段代码的意思
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {BACKGROUND_RED, FOREGROUND_RED, FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_INTENSITY};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);        //往开发环境控制台输出字符串
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);  //往Windows控制台输出字符串
}

f64 platform_get_absolute_time()
{
    if(state_ptr)
    {
        LARGE_INTEGER now_time;
        QueryPerformanceCounter(&now_time);
        // time in seconds
        return (f64)now_time.QuadPart * state_ptr->clock_frequency;
    }
    else
    {
        LARGE_INTEGER now_time;
        QueryPerformanceCounter(&now_time);
        // time in seconds
        return (f64)now_time.QuadPart * clock_frequency;
    }
}

void platform_sleep(u64 ms)
{
    Sleep(ms);
}

void platform_get_required_extension_names(const char*** names_darray)
{
    darray_push(*names_darray, &"VK_KHR_win32_surface");
}

b8 platform_create_vulkan_surface(struct vulkan_context* context)
{
    VkWin32SurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    create_info.hinstance = state_ptr->h_instance;
    create_info.hwnd = state_ptr->hwnd;

    VkResult result = vkCreateWin32SurfaceKHR(context->instance,
    &create_info,
    context->allocator,
    &state_ptr->surface);

    if(result != VK_SUCCESS)
    {
        DFATAL("Vulkan surface creation failed.");
        return false;
    }

    context->surface = state_ptr->surface;
    return true;
}

// void platform_destroy_vulkan_surface(struct platform_state* plat_state, struct vulkan_context* context)
// {
//     windows_app_internal_state* state = (windows_app_internal_state*)plat_state->internal_state;
//     VkSurfaceKHR surface = state->surface;

//     DINFO("Destroy vulkan surface");
//     vkDestroySurfaceKHR(context->instance, surface, context->allocator);
// }

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1;
        }
        case WM_CLOSE:
        {
            // Fire an event for the application to quit.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
            return true;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        {
            // Get updated size.
            RECT r;
            GetClientRect(hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;
            event_context data = {};
            data.data.u16[0] = (u16)width;
            data.data.u16[1] = (u16)height;
            event_fire(EVENT_CODE_RESIZED, 0, data);
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            // // key pressed/released
            b8 pressed = (msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN);
            key_code key = (u16)w_param;

            if(w_param == VK_MENU)
            {
                if(GetKeyState(VK_RMENU) & 0x8000)
                {
                    key = KEY_RALT;
                }
                else if(GetKeyState(VK_LMENU) & 0x8000)
                {
                    key = KEY_LALT;
                }
            }
            else if(w_param == VK_SHIFT)
            {
                if(GetKeyState(VK_RSHIFT) & 0x8000)
                {
                    key = KEY_RSHIFT;
                }
                else if(GetKeyState(VK_LSHIFT) & 0x8000)
                {
                    key = KEY_LSHIFT;
                }
            }
            else if(w_param == VK_CONTROL)
            {
                if(GetKeyState(VK_RCONTROL) & 0x8000)
                {
                    key = KEY_RCONTROL;
                }
                else if(GetKeyState(VK_LCONTROL) & 0x8000)
                {
                    key = KEY_LCONTROL;
                }
            }

            input_process_key(key, pressed);
            break;
        }
        case WM_MOUSEMOVE:
        {
            // // Mouse move
            // i32 x_position = GET_X_LPARAM(l_param);
            // i32 y_position = GET_Y_LPARAM(l_param);
            // // TODO: input processing
            break;
        }
        case WM_MOUSEWHEEL:
        {
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if(z_delta != 0)
            // {
            //     // Flatten the input to an OS-indenpent (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: input processing
            // }
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            // b8 pressed = (msg == WM_LBUTTONDOWN) || (msg == WM_MBUTTONDOWN) || (msg == WM_RBUTTONDOWN);
            // // TODO: input processing
            break;
        }
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif  // DPLATFORM_WINDOWS
