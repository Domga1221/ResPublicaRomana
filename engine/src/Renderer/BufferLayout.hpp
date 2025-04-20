#pragma once 

#include "defines.hpp"

#include "Memory/List.hpp"

#include "BufferElement.hpp"

struct BufferLayout {
    List<BufferElement> elements;
    u32 stride;
};

RPR_API void BufferLayout_Create(BufferLayout* bufferLayout);

RPR_API void BufferLayout_CalculateOffsetAndStride(BufferLayout* bufferLayout);

RPR_API void BufferLayout_AddElement(BufferLayout* bufferLayout, BufferElement element);

//#ifdef DEBUG
#include <string>
RPR_API void BufferLayout_Print(BufferLayout* bufferLayout);
//#endif 