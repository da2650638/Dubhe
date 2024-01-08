#include "test_manager.h"
#include <core/logger.h>

#include "memory/linear_allocator_tests.h"

int main()
{
    test_manager_init();

    // TODO: add test registration here.
    linear_allocator_register_tests();

    DDEBUG("Starting tests...");

    test_manager_run_tests();

    return 0;
}