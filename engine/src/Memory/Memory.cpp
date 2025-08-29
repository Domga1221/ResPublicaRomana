#include "Memory.hpp"

#include "Core/Log.hpp"

static u64 totalBytesAllocated;
static u64 taggedAllocations[MEMORY_TAG_LAST];

static const char* memoryTagStrings[MEMORY_TAG_LAST] = {
    "UNKNOWN ",
    "ARRAY   ",
    "VECTOR  ",
    "LIST    ",
    "STRING  ",

    "LAYER   ",

    "RENDERER",
    "TEXTURE ",
    "ENTITY  "
};

RPR_API void MEMORY_Initialize() {
    totalBytesAllocated = 0;
    MEMORY_Zero(&taggedAllocations, sizeof(taggedAllocations));
}

RPR_API void MEMORY_Shutdown() {
    RPR_DEBUG("MEMORY_Shutdown");
}

RPR_API void* MEMORY_Allocate(u64 size, Memory_Tag tag) {
    if(tag == MEMORY_TAG_UNKNOWN) 
        RPR_WARN("MEMORY_Allocate called using MEMORY_TAG_UNKNOWN");
    
    totalBytesAllocated += size;
    taggedAllocations[tag] += size;

    void* block = malloc(size);
    if(block == 0)
        RPR_CRITICAL("MEMORY_Allocate, malloc failed");

    return block;
}

RPR_API void MEMORY_Free(void* block, u64 size,  Memory_Tag tag) {
    if(tag == MEMORY_TAG_UNKNOWN)
        RPR_WARN("MEMORY_Free called using MEMORY_TAG_UNKOWN");
    
    totalBytesAllocated -= size;
    taggedAllocations[tag] -= size;

    free(block);
}

RPR_API void* MEMORY_Zero(void* block, u64 size) {
    return memset(block, 0, size);
}

RPR_API void* MEMORY_Copy(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

RPR_API void* MEMORY_Set(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

RPR_API void* MEMORY_Move(void* dest, const void* source, u64 size) {
    return memmove(dest, source, size);
}


RPR_API const char* MEMORY_GetMemoryUsage_C_String() {
    std::string memString;
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    memString = "system memory use (tagged):\n";
    for(u32 i = 0; i < MEMORY_TAG_LAST; i++) {
        std::string unit = "XB";
        float amount = 1.0f;
        if(taggedAllocations[i] >= gib) {
            unit[0] = 'G';
            amount = taggedAllocations[i] / (float)gib;
        } else if(taggedAllocations[i] >= mib) {
            unit[0] = 'M';
            amount = taggedAllocations[i] / (float)mib;
        } else if(taggedAllocations[i] >= kib) {
            unit[0] = 'K';
            amount = taggedAllocations[i] / (float)kib;
        } else {
            unit = 'B';
            amount = (float)taggedAllocations[i];
        }
        memString = memString + " " + memoryTagStrings[i] + ": " + std::to_string(amount) + unit + "\n";
    }
    
    char* memString_cStr = (char*)MEMORY_Allocate(memString.size() + 1, MEMORY_TAG_STRING);
    MEMORY_Copy(memString_cStr, memString.c_str(), memString.size());
    memString_cStr[memString.size()] = '\0';
    return memString_cStr;
}


std::string MEMORY_GetMemoryUsageString() {
    std::string memString;
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    memString = "system memory use (tagged):\n";
    for(u32 i = 0; i < MEMORY_TAG_LAST; i++) {
        std::string unit = "XiB";
        float amount = 1.0f;
        if(taggedAllocations[i] >= gib) {
            unit[0] = 'G';
            amount = taggedAllocations[i] / (float)gib;
        } else if(taggedAllocations[i] >= mib) {
            unit[0] = 'M';
            amount = taggedAllocations[i] / (float)mib;
        } else if(taggedAllocations[i] >= kib) {
            unit[0] = 'K';
            amount = taggedAllocations[i] / (float)kib;
        } else {
            unit = 'B';
            amount = (float)taggedAllocations[i];
        }
        memString = memString + " " + memoryTagStrings[i] + ": " + std::to_string(amount) + unit + "\n";
    }

    return memString;
}

RPR_API void MEMORY_IncreaseMemoryUsage(u64 size, Memory_Tag memoryTag) {
    //std::string s = "MEMORY_IncreaseMemoryUsage: increasing by: " + std::to_string(size) + "\t"
    //    + memoryTagStrings[memoryTag] + "\tcurrent bytes: " + std::to_string(taggedAllocations[memoryTag]);
    totalBytesAllocated += size;
    taggedAllocations[memoryTag] += size;

    //s.append("\tafter increase: " + std::to_string(taggedAllocations[memoryTag]));
    //RPR_INFO("%s", s);
}

RPR_API void MEMORY_DecreaseMemoryUsage(u64 size, Memory_Tag memoryTag) {
    //std::string s = "MEMORY_DecreaseMemoryUsage: decreasing by: " + std::to_string(size) + "\t"
    //    + memoryTagStrings[memoryTag] + "\tcurrent bytes: " + std::to_string(taggedAllocations[memoryTag]);
   
    totalBytesAllocated -= size;
    taggedAllocations[memoryTag] -= size;
   
    //s.append("\tafter decrease: " + std::to_string(taggedAllocations[memoryTag]));
    //RPR_WARN("%s", s);
}

