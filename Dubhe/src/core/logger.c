#include "logger.h"
#include "asserts.h"
#include "platform/platform.h"

// TODO: temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct logger_system_state
{
    b8 initialized;
} logger_system_state;

static logger_system_state* state_ptr;

b8 initialize_logging(u64* memory_requirement, void* state)
{
    *memory_requirement = sizeof(logger_system_state);
    if(state == 0)
    {
        return true;
    }

    state_ptr = state;
    state_ptr->initialized = true;

    // TODO: create a log file
    return true;
}

void shutdown_logging(void* state)
{
    // TODO: cleanup logging/write queued entries
    state_ptr = 0;
}

void log_output(log_level level, const char* message, ...)
{
    const char* level_strings[6] = { "[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: " };
    b8 is_error = level < LOG_LEVEL_WARN;
    char out_message[32768];
    memset(out_message, 0, sizeof(out_message));

    // Format original message
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, sizeof(out_message), message, arg_ptr);
    va_end(arg_ptr);

    char final_out_message[32768];
    sprintf(final_out_message, "%s%s\n", level_strings[level], out_message);

    // platform specific
    if(is_error)
    {
        platform_console_write_error(final_out_message, level);
    }
    else
    {
        platform_console_write(final_out_message, level);
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line)
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: %s, in file: %s, line: %d\n", expression, message, file, line);
}