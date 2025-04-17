#include "VertexArray.hpp"

#include "ShaderDataType.hpp"

void VertexArray_Create(VertexArray* vertexArray) {
    glCreateVertexArrays(1, &vertexArray->ID);
    //vertexArray->vertexBuffers = Vector<VertexBuffer*>();
}

void VertexArray_Destroy(VertexArray* vertexArray) {
    for(u32 i = 0; i < vertexArray->vertexBuffers.Size(); i++) {
        // TODO: delete
    }
    glDeleteVertexArrays(1, &vertexArray->ID);
}

void VertexArray_AddVertexBuffer(VertexArray* vertexArray, VertexBuffer* vertexBuffer) {
    // TODO: assert
    if(vertexBuffer->bufferLayout.elements.Size() <= 0)   
        RPR_ERROR("VertexArray_AddVertexBuffer: provided vertexBuffer has no layout");

    glBindVertexArray(vertexArray->ID);
    VertexBuffer_Bind(vertexBuffer);

    // following line needs copy constructor for Vector, since BufferLayout contains a Vector<BufferElement>
    // otherwise memory usage won't be increased, but will be decreased after end of this scope 
    // should have used a pointer to BufferLayout to avoid copy for performance reasons
    BufferLayout layout = vertexBuffer->bufferLayout; 
    for(u32 i = 0; i < layout.elements.Size(); i++) {
        BufferElement element = layout.elements[i];
        u32 componentCount = ShaderDataType_GetComponentCount(element.shaderDataType);
        GLenum glType = ShaderDataType_ToGLType(element.shaderDataType);
        GLboolean normalized = GL_FALSE;
        u32 stride = layout.stride;
        const void* offset = (const void*)(size_t)element.offset;

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, componentCount, glType, normalized, stride, offset);
    }
    vertexArray->vertexBuffers.PushBack(vertexBuffer);
}

void VertexArray_Bind(VertexArray* vertexArray) {
    glBindVertexArray(vertexArray->ID);
}

void VertexArray_Unbind() {
    glBindVertexArray(0);
}