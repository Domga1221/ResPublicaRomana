#pragma once 

#include "defines.hpp"

#include "Memory/Vector.hpp"

#include "VertexBuffer.hpp"

struct VertexArray {
    u32 ID;
    Vector<VertexBuffer*> vertexBuffers;    
    // TODO: IndexBuffer
};

void VertexArray_Create(VertexArray* vertexArray);
void VertexArray_Destroy(VertexArray* vertexArray);

void VertexArray_AddVertexBuffer(VertexArray* vertexArray, VertexBuffer* vertexBuffer);

void VertexArray_Bind(VertexArray* vertexArray);
void VertexArray_Unbind();