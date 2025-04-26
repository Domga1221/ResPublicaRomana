#pragma once 

#include "defines.hpp"

struct IndexBuffer {
    u32 ID;
    u32 indexCount;
};

IndexBuffer* IndexBuffer_Create(u32* indices, u32 count);
void IndexBuffer_Destroy(IndexBuffer* indexBuffer);

void IndexBuffer_Bind(IndexBuffer* indexBuffer);
void IndexBuffer_Unbind();