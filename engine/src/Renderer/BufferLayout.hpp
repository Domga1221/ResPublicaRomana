#pragma once 

#include "defines.hpp"

#include "Memory/Vector.hpp"

#include "BufferElement.hpp"

struct BufferLayout {
    Vector<BufferElement> elements;
    u32 stride;
};

void BufferLayout_CalculateOffsetAndStride(BufferLayout* bufferLayout);

void BufferLayout_AddElement(BufferLayout* bufferLayout, BufferElement element);

#ifdef DEBUG
#include <string>
void BufferLayout_Print(BufferLayout* bufferLayout);
#endif 