#pragma once

#include "defines.h"

typedef struct linear_allocator
{
    u64 total_size; //bytes;
    u64 allocated;
    void* memory;
    b8 owns_memory;
} linear_allocator;

/**
 * @brief 创建线性分配器。
 *
 * 此函数用于初始化一个线性分配器。用户可以提供一块内存供分配器管理，或者让分配器自己分配内存。
 * 如果提供了内存，分配器将不会尝试释放这块内存；否则，分配器会负责内存的分配和释放。
 *
 * @param total_size 分配器可以管理的总内存大小（字节）。
 * @param memory 用于初始化分配器的内存块指针。如果为NULL，分配器将自行分配内存。
 * @param out_allocator 指向初始化后的分配器的指针。
 */
DAPI void linear_allocator_create(u64 total_size, void* memory, linear_allocator* out_allocator);

/**
 * @brief 销毁线性分配器。
 *
 * 此函数用于销毁一个线性分配器，释放其管理的内存（如果分配器负责分配这块内存的话）。
 * 函数将重置分配器结构的所有字段。
 *
 * @param allocator 指向要销毁的分配器的指针。
 */
DAPI void linear_allocator_destroy(linear_allocator* allocator);

/**
 * @brief 从线性分配器中分配内存。
 *
 * 此函数用于从线性分配器中分配指定大小的内存块。分配是连续进行的，如果剩余空间不足以满足请求，
 * 函数将返回NULL，并通过日志记录错误。
 *
 * @param allocator 指向分配器的指针。
 * @param size 请求分配的内存大小（字节）。
 * @return void* 指向分配的内存块的指针。如果分配失败，返回NULL。
 */
DAPI void* linear_allocator_allocate(linear_allocator* allocator, u64 size);

/**
 * @brief 释放线性分配器管理的所有内存。
 *
 * 此函数用于重置线性分配器，释放所有之前分配的内存块。它不会释放分配器管理的内存本身，
 * 而是将分配的偏移量重置为零，并清零内存内容。
 *
 * @param allocator 指向要释放内存的分配器的指针。
 */
DAPI void linear_allocator_free_all(linear_allocator* allocator);