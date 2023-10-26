#include "darray.h"

#include "core/logger.h"
#include "core/dmemory.h"
#include "core/asserts.h"

void* _darray_create(u64 capacity, u64 stride)
{
    u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 array_size = capacity * stride;
    u64* new_array = (u64*)dallocate(header_size + array_size, MEMORY_TAG_DARRAY);
    dset_memory(new_array, 0, header_size + array_size);
    new_array[DARRAY_CAPACITY] = capacity;
    new_array[DARRAY_LENGTH] = 0;
    new_array[DARRAY_STRIDE] = stride;
    return (void*)(new_array + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void* array)
{
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 total_size = header_size + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
    dfree(header, total_size, MEMORY_TAG_DARRAY);
}

u64 _darray_field_get(void* array, u64 field)
{
    DASSERT_MSG(field < DARRAY_FIELD_LENGTH, "darray filed index could not be larger than DARRAY_FIELD_LENGTH");
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    return header[field];
}

void _darray_field_set(void* array, u64 field, u64 value)
{
    DASSERT_MSG(field < DARRAY_FIELD_LENGTH, "darray filed index could not be larger than DARRAY_FIELD_LENGTH");
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    header[field] = value;
}

void* _darray_resize(void* array)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    void* temp = _darray_create(DARRAY_RESIZE_FACTOR * darray_capacity(array), stride);
    dcopy_memory(temp, array, stride * length);
    
    _darray_field_set(temp, DARRAY_LENGTH, length);
    _darray_destroy(array);
    return temp;
}

void* _darray_push(void* array, const void* value_ptr)
{
    u64 length = darray_length(array);
    u64 capacity = darray_capacity(array);
    u64 stride = darray_stride(array);
    if(length >= capacity)
    {
        array = _darray_resize(array);
    }
    u64 addr = (u64)array;
    addr += length * stride;
    dcopy_memory((void*)addr, value_ptr, stride);
    darray_length_set(array, length + 1);
    return array;
}

void* _darray_pop(void* array, void* dest)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    if(length == 0)
    {
        return array;
    }
    u64 addr = (u64)array;
    addr += (length - 1) * stride;
    dcopy_memory(dest, (void*)addr, stride);
    darray_length_set(array, length - 1);
    return array;
}

void* _darray_insert_at(void* array, u64 index, const void* value_ptr)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    u64 capacity = darray_capacity(array);
    DASSERT_MSG(index >= 0 && index <= length, "darray insert at index < 0 or index > length");
    if(length >= capacity)
    {
        array = _darray_resize(array);
    }
    u64 addr = (u64)array;
    addr += index * stride;
    dcopy_memory((void*)(addr + stride), (void*)addr, (length - index) * stride);
    dcopy_memory((void*)addr, value_ptr, stride);
    darray_length_set(array, length + 1);
    return array;
}

void* _darray_pop_at(void* array, u64 index, void* dest)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    DASSERT_MSG(index >= 0 && index <= length, "darray pop at index < 0 or index > length");
    u64 addr = (u64)array;
    addr += index * stride;
    dcopy_memory(dest, (void*)addr, stride);
    dcopy_memory((void*)addr, (void*)(addr + stride), (length - index - 1) * stride);
    darray_length_set(array, length - 1);
    return array;
}