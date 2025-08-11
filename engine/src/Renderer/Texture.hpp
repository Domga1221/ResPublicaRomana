#pragma once 

#include "defines.hpp"

#include "Memory/String.hpp"

struct Texture {
    String path; // TODO: string implementation 
    bool loaded;

    u32 width; 
    u32 height;
    u32 numberOfChannels;
    
    u32 ID;
};

void RPR_API Texture_Create(Texture* texture, const char* path);
void RPR_API Texture_Destroy(Texture* texture);

void RPR_API Texture_Bind(Texture* texture);
void RPR_API Texture_Unbind();
