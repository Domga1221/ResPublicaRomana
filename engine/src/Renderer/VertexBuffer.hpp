#pragma once 

#include "defines.hpp"

#include "BufferLayout.hpp"

struct VertexBuffer {
    u32 ID;
    BufferLayout bufferLayout;
};

RPR_API void VertexBuffer_Create(VertexBuffer* vertexBuffer, f32* vertices, u32 vertexCount);
RPR_API void VertexBuffer_Destroy(VertexBuffer* vertexBuffer);

RPR_API void VertexBuffer_SetLayout(VertexBuffer* vertexBuffer, BufferLayout* bufferLayout);

RPR_API void VertexBuffer_Bind(VertexBuffer* vertexBuffer);
RPR_API void VertexBuffer_Unbind();
