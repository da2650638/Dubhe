#pragma once

#include "defines.h"

DAPI u64 string_length(const char* str);

DAPI char* string_duplicate(const char* str);

// Case-sensitive string comparison. True if the same, otherwise false.
DAPI b8 strings_equal(const char* str0, const char* str1);