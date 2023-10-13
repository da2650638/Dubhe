#pragma once

#include "defines.h"

// Disable assertions by commenting out the below line.
#define DASSERTIONS_ENABLED

#ifdef DASSERTIONS_ENABLED
    #ifdef _MSC_VER
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        #define debugBreak() __builtin_trap()
    #endif

    /*
    expression - A string representation of the code which was running that cause the assertion failure.
    message    -
    file       - Name of the code file.
    line       - actual line number of the code file.
    */
    DAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

    #define DASSERT(expr)                                                      \
        {                                                                      \
            if((expr)) {                                                       \
            } else {                                                           \
                report_assertion_failure(#expr, "", __FILE__, __LINE__);       \
                debugBreak();                                                  \
            }                                                                  \
        }                                                                      

    #define DASSERT_MSG(expr, message)                                         \
        {                                                                      \
            if((expr)) {                                                       \
            } else {                                                           \
                report_assertion_failure(#expr, message, __FILE__, __LINE__);  \
                debugBreak();                                                  \
            }                                                                  \
        }                                                                      

    #ifdef _DEBUG
        #define DASSERT_DEBUG(expr)                                                \
            {                                                                      \
                if((expr)) {                                                       \
                } else {                                                           \
                    report_assertion_failure(#expr, "", __FILE__, __LINE__);       \
                    debugBreak();                                                  \
                }                                                                  \
            }
    #else
        #define DASSERT_DEBUG(expr)
    #endif

#else
    #define DASSERT(expr)
    #define DASSERT_MSG(expr)
    #define DASSERT_DEBUG(expr)
#endif