#include <core/logger.h>
#include <core/asserts.h>

int main(void)
{
    DFATAL("A test message: %.3f", 3.14);
    DERROR("A test message: %.3f", 3.14);
    DWARN("A test message: %.3f", 3.14);
    DINFO("A test message: %.3f", 3.14);
    DDEBUG("A test message: %.3f", 3.14);
    DTRACE("A test message: %.3f", 3.14);
    return 0;
}