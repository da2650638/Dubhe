#include <core/logger.h>
#include <core/asserts.h>
#include <platform/platform.h>

int main(void)
{
    DFATAL("A test message: %.3f", 3.14);
    DERROR("A test message: %.3f", 3.14);
    DWARN("A test message: %.3f", 3.14);
    DINFO("A test message: %.3f", 3.14);
    DDEBUG("A test message: %.3f", 3.14);
    DTRACE("A test message: %.3f", 3.14);

    platform_state state;
    if(platform_startup(&state, "Dubhe", 100, 100, 1280, 720))
    {
        while(TRUE)
        {
            platform_pump_message(&state);  // Just like GLFW glfwPollEvents(&window);
        }
    }
    platform_shutdown(&state);
    return 0;
}