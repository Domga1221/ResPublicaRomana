#pragma once 

#include "defines.hpp"

#include "BufferLayout.hpp"

struct VertexBuffer {
    u32 ID;
    BufferLayout bufferLayout;
};

void VertexBuffer_Create(VertexBuffer* vertexBuffer, f32* vertices, u32 vertexCount);
void VertexBuffer_Destroy(VertexBuffer* vertexBuffer);

void VertexBuffer_SetLayout(VertexBuffer* vertexBuffer, BufferLayout* bufferLayout);

void VertexBuffer_Bind(VertexBuffer* vertexBuffer);
void VertexBuffer_Unbind();
