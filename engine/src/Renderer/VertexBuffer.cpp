#include "VertexBuffer.hpp"

VertexBuffer* VertexBuffer_Create(f32* vertices, u32 vertexCount) {
    VertexBuffer* vertexBuffer = (VertexBuffer*)MEMORY_Allocate(sizeof(VertexBuffer), MEMORY_TAG_RENDERER);
    BufferLayout_Create(&vertexBuffer->bufferLayout);

    glCreateBuffers(1, &vertexBuffer->ID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->ID);
    glBufferData(GL_ARRAY_BUFFER, vertexCount, vertices, GL_STATIC_DRAW);
    VertexBuffer_Unbind();

    return vertexBuffer;
}

void VertexBuffer_Destroy(VertexBuffer* vertexBuffer) {
    glDeleteBuffers(1, &vertexBuffer->ID);

    List_Destroy(&vertexBuffer->bufferLayout.elements);

    MEMORY_Free(vertexBuffer, sizeof(VertexBuffer), MEMORY_TAG_RENDERER);
}

void VertexBuffer_SetLayout(VertexBuffer* vertexBuffer, BufferLayout* bufferLayout) {
    RPR_DEBUG("Copying bufferlayout in vertexBuffer_SetLayout");
    // Requires assignment operator overload since VertexBuffer* already exists and no new object is created
    // vertexBuffer->bufferLayout is assigned the contents of another existing object
    // https://www.geeksforgeeks.org/copy-constructor-vs-assignment-operator-in-c/
    vertexBuffer->bufferLayout = *bufferLayout;
}

void VertexBuffer_Bind(VertexBuffer* vertexBuffer) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->ID);
}

void VertexBuffer_Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}