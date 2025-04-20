#pragma once 

#include "defines.hpp"

#include "ShaderDataType.hpp"

#include <string>

struct BufferElement {
    const char* name; // TODO: own string implementation 
    ShaderDataType shaderDataType;
    u32 size;
    u32 offset;
};

//#ifdef DEBUG
#include <string>
RPR_API void BufferElement_Print(BufferElement* element);
//#endif
