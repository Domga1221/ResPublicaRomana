#pragma once 

#include "defines.hpp"

enum Memory_Tag {
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_VECTOR,
    MEMORY_TAG_STRING,

    MEMORY_TAG_LAYER,

    MEMORY_TAG_RENDERER,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_ENTITY,

    MEMORY_TAG_LAST
};

RPR_API void MEMORY_Initialize();
RPR_API void MEMORY_Shutdown();

RPR_API void* MEMORY_Allocate(u64 size, Memory_Tag tag);
RPR_API void MEMORY_Free(void* block, u64 size,  Memory_Tag tag);

RPR_API void* MEMORY_Zero(void* block, u64 size);
RPR_API void* MEMORY_Copy(void* dest, const void* source, u64 size);
RPR_API void* MEMORY_Set(void* dest, i32 value, u64 size);

RPR_API const char* MEMORY_GetMemoryUsageString(); // char* to be freed by caller 

RPR_API void MEMORY_IncreaseMemoryUsage(u64 size, Memory_Tag memoryTag);
RPR_API void MEMORY_DecreaseMemoryUsage(u64 size, Memory_Tag memoryTag);