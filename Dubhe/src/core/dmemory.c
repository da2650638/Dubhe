#include "dmemory.h"

#include "core/logger.h"
#include "platform/platform.h"

// TODO: Custom string lib
#include <string.h>
#include <stdio.h>

struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "APP        ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      "};

static struct memory_stats stats;

void initialize_memory()
{
    platform_zero_memory(&stats, sizeof(struct memory_stats));
}

void shutdown_memory()
{
    DINFO("shutdown_memory");
}

void* dallocate(u64 size, memory_tag tag)
{
    if(tag == MEMORY_TAG_UNKNOWN)
    {
        DWARN("dallocate called using MEMORY_TAG_UNKNOWN. Re-calss this allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: memory alignment
    void* block = platform_allocate(size, FALSE);
    return block;
}

void dfree(void* block, u64 size, memory_tag tag)
{
    if(tag == MEMORY_TAG_UNKNOWN)
    {
        DWARN("dallocate called using MEMORY_TAG_UNKNOWN. Re-calss this allocation.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // TODO: memory alignment
    platform_free(block, FALSE);
}

void* dzero_memory(void* block, u64 size)
{
    return platform_zero_memory(block, size);
}

void* dcopy_memory(void* dest, const void* src, u64 size)
{
    return platform_copy_memory(dest, src, size);
}

void* dset_memory(void* dest, i32 value, u64 size)
{
    return platform_set_memory(dest, value, size);
}

char* get_memory_usage_str()
{
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8192] = "System memory use(tagged):\n";
    u64 offset = strlen(buffer);
    for(u64 i = 0; i < MEMORY_TAG_MAX_TAGS; i++)
    {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if(stats.tagged_allocations[i] >= gib)
        {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gib;
        }
        else if(stats.tagged_allocations[i] >= mib)
        {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mib;
        }
        else if(stats.tagged_allocations[i] >= kib)
        {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kib;
        }
        else
        {
            unit[0] = 'B';
            unit[1] = '0';
            amount = (float)stats.tagged_allocations[i];
        }
        i32 length = snprintf(buffer + offset, 8192, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    // This is actually a leak mem, but don't worry about it.
    char* out_string = _strdup(buffer);
    return out_string;
}

