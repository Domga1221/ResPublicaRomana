#include "VertexArray.hpp"

#include "ShaderDataType.hpp"


VertexArray* VertexArray_Create() {
    VertexArray* vertexArray = (VertexArray*)MEMORY_Allocate(sizeof(VertexArray), MEMORY_TAG_RENDERER);
    List_Create(&vertexArray->vertexBuffers);

    glCreateVertexArrays(1, &vertexArray->ID);

    return vertexArray;
}

void VertexArray_Destroy(VertexArray* vertexArray) {
    for(u32 i = 0; i < vertexArray->vertexBuffers.size; i++) {
        VertexBuffer* vertexBuffer = vertexArray->vertexBuffers.data[i];
        VertexBuffer_Destroy(vertexBuffer);
    }
    glDeleteVertexArrays(1, &vertexArray->ID);
    if(vertexArray->indexBuffer != nullptr) 
        IndexBuffer_Destroy(vertexArray->indexBuffer);
    
    List_Destroy(&vertexArray->vertexBuffers);
}

void VertexArray_AddVertexBuffer(VertexArray* vertexArray, VertexBuffer* vertexBuffer) {
    // TODO: assert
    if(vertexBuffer->bufferLayout.elements.size <= 0)   
        RPR_ERROR("VertexArray_AddVertexBuffer: provided vertexBuffer has no layout");

    glBindVertexArray(vertexArray->ID);
    VertexBuffer_Bind(vertexBuffer);

    // following line needs copy constructor for Vector, since BufferLayout contains a Vector<BufferElement>
    // otherwise memory usage won't be increased, but will be decreased after end of this scope 
    // should have used a pointer to BufferLayout to avoid copy for performance reasons
    BufferLayout layout = vertexBuffer->bufferLayout; 
    for(u32 i = 0; i < layout.elements.size; i++) {
        BufferElement element = layout.elements.data[i];
        u32 componentCount = ShaderDataType_GetComponentCount(element.shaderDataType);
        GLenum glType = ShaderDataType_ToGLType(element.shaderDataType);
        GLboolean normalized = GL_FALSE;
        u32 stride = layout.stride;
        const void* offset = (const void*)(size_t)element.offset;

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, componentCount, glType, normalized, stride, offset);
    }
    List_PushBack(&vertexArray->vertexBuffers, vertexBuffer);
}

void VertexArray_SetIndexBuffer(VertexArray* vertexArray, IndexBuffer* indexBuffer) {
    glBindVertexArray(vertexArray->ID);
    IndexBuffer_Bind(indexBuffer);

    vertexArray->indexBuffer = indexBuffer;
}

void VertexArray_Bind(VertexArray* vertexArray) {
    glBindVertexArray(vertexArray->ID);
}

void VertexArray_Unbind() {
    glBindVertexArray(0);
}