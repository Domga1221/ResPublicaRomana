#pragma once 

#include "defines.hpp"

#include "Memory/List.hpp"

#include "BufferElement.hpp"

struct BufferLayout {
    List<BufferElement> elements;
    u32 stride;
};

void BufferLayout_Create(BufferLayout* bufferLayout);

void BufferLayout_CalculateOffsetAndStride(BufferLayout* bufferLayout);

void BufferLayout_AddElement(BufferLayout* bufferLayout, BufferElement element);

#ifdef DEBUG
#include <string>
void BufferLayout_Print(BufferLayout* bufferLayout);
#endif 