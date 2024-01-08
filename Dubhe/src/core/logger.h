#pragma once

#include "defines.h"

/*
 We are going incresing the capabilities of the logger as we progress through the series.
*/

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logging for release builds
#ifdef DRELEASE
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

/**
 * @brief Initialize logging system. Call twice: once with state = 0 to get required memory size.
 * then a second time passing allocated memory to state.
 * 
 * @param memory_requirement 
 * @param state 0 if just request memory requirement, other allocated block of memory.
 * @return b8 ture on success, otherwise false.
 */
b8 initialize_logging(u64* memory_requirement, void* state);
void shutdown_logging(void* state);

DAPI void log_output(log_level level, const char* message, ...);

#define DFATAL(message, ...) log_output(LOG_LEVEL_FATAL, (message), ##__VA_ARGS__);

#ifndef DERROR
#define DERROR(message, ...) log_output(LOG_LEVEL_ERROR, (message), ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define DWARN(message, ...) log_output(LOG_LEVEL_WARN, (message), ##__VA_ARGS__)
#else
#define DWARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define DINFO(message, ...) log_output(LOG_LEVEL_INFO, (message), ##__VA_ARGS__)
#else
#define DINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define DDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, (message), ##__VA_ARGS__)
#else
#define DDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define DTRACE(message, ...) log_output(LOG_LEVEL_TRACE, (message), ##__VA_ARGS__)
#else
#define DTRACE(message, ...)
#endif