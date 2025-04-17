#pragma once 

#include "defines.hpp"

#include "ShaderDataType.hpp"

#include <string>

struct BufferElement {
    std::string name; // TODO: own string implementation 
    ShaderDataType shaderDataType;
    u32 size;
    u32 offset;
};

#ifdef DEBUG
#include "Core/Log.hpp"
#include <string>
void BufferElement_Print(BufferElement* element);
#endif
