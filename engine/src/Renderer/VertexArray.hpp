#pragma once 

#include "defines.hpp"

#include "Memory/List.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

struct VertexArray {
    u32 ID;
    List<VertexBuffer*> vertexBuffers;    
    IndexBuffer* indexBuffer;
};

RPR_API void VertexArray_Create(VertexArray* vertexArray);
RPR_API void VertexArray_Destroy(VertexArray* vertexArray);

RPR_API void VertexArray_AddVertexBuffer(VertexArray* vertexArray, VertexBuffer* vertexBuffer);
RPR_API void VertexArray_SetIndexBuffer(VertexArray* vertexArray, IndexBuffer* indexBuffer);

RPR_API void VertexArray_Bind(VertexArray* vertexArray);
RPR_API void VertexArray_Unbind();