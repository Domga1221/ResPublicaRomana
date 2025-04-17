#include "BufferLayout.hpp"

void BufferLayout_CalculateOffsetAndStride(BufferLayout* bufferLayout) {
    // TODO: Add assertion
    if(bufferLayout->elements.Size() == 0) {
        RPR_ERROR("BufferLayout_Create: Trying to create BufferLayout, but no elements have been added");
        return;
    }

    u32 offset = 0; 
    bufferLayout->stride = 0;
    for(int i = 0; i < bufferLayout->elements.Size(); i++) {
        BufferElement* bufferElement = &bufferLayout->elements[i];
        bufferElement->offset = offset;
        offset += bufferElement->size;
        bufferLayout->stride += bufferElement->size;
    }
}

void BufferLayout_AddElement(BufferLayout* bufferLayout, BufferElement element) {
    element.size = ShaderDataType_GetSize(element.shaderDataType);
    element.offset = 0;
    bufferLayout->elements.PushBack(element);  
}

#ifdef DEBUG
void BufferLayout_Print(BufferLayout* bufferLayout) {
    std::string s = "BufferLayout: \n\t\tstride: " + std::to_string(bufferLayout->stride);
    RPR_DEBUG("%s", s);
    for(int i = 0; i < bufferLayout->elements.Size(); i++) {
        BufferElement_Print(&bufferLayout->elements[i]);
    }
}
#endif 