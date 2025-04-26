#include "IndexBuffer.hpp"

#include "Memory/Memory.hpp"

#include <glad/glad.h>

IndexBuffer* IndexBuffer_Create(u32* indices, u32 count) {
    IndexBuffer* indexBuffer = (IndexBuffer*) MEMORY_Allocate(sizeof(IndexBuffer), MEMORY_TAG_RENDERER);
    indexBuffer->indexCount = count;
    
    glCreateBuffers(1, &indexBuffer->ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->indexCount * sizeof(u32), indices, GL_STATIC_DRAW);

    return indexBuffer;
}

void IndexBuffer_Destroy(IndexBuffer* indexBuffer) {
    glDeleteBuffers(1, &indexBuffer->ID);

    MEMORY_Free(indexBuffer, sizeof(IndexBuffer), MEMORY_TAG_RENDERER);
}

void IndexBuffer_Bind(IndexBuffer* indexBuffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->ID);
}   

void IndexBuffer_Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}