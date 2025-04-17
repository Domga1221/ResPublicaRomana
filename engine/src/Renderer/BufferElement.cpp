#include "BufferElement.hpp"

void BufferElement_Print(BufferElement* element) {
    std::string name = std::string(element->name);
    const char* dataTypeString = ShaderDataType_ToString(element->shaderDataType);
    RPR_DEBUG("BufferElement: name = %s, shaderDataType = %s, size = %d, offset = %d", 
        name, dataTypeString, element->size, element->offset);
}