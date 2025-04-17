#include "ShaderDataType.hpp"

u32 ShaderDataType_GetSize(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        default: ;
    }

    RPR_ERROR("ShaderDataType_GetSize UNKOWN TYPE %d", (int)type);
    return 0;
}

u32 ShaderDataType_GetComponentCount(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:   return 1;
        case ShaderDataType::Float2:  return 2;
        case ShaderDataType::Float3:  return 3;
        case ShaderDataType::Float4:  return 4;
        case ShaderDataType::Mat3:    return 3 * 3;
        case ShaderDataType::Mat4:    return 4 * 4;
        case ShaderDataType::Int:     return 1;
        case ShaderDataType::Int2:    return 2;
        case ShaderDataType::Int3:    return 3;
        case ShaderDataType::Int4:    return 4;
        case ShaderDataType::Bool:    return 1;
        default: ;
    }

    RPR_ERROR("ShaderDataType_GetComponentCount: UNKOWN TYPE %d", (int)type);
    return 0;
}

GLenum ShaderDataType_ToGLType(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:    return GL_FLOAT;
        case ShaderDataType::Float2:   return GL_FLOAT;
        case ShaderDataType::Float3:   return GL_FLOAT;
        case ShaderDataType::Float4:   return GL_FLOAT;
        case ShaderDataType::Mat3:     return GL_FLOAT;
        case ShaderDataType::Mat4:     return GL_FLOAT;
        case ShaderDataType::Int:      return GL_INT;
        case ShaderDataType::Int2:     return GL_INT;
        case ShaderDataType::Int3:     return GL_INT;
        case ShaderDataType::Int4:     return GL_INT;
        case ShaderDataType::Bool:     return GL_BOOL;
        default: ;
    }

    RPR_ERROR("ShaderDataType_ToGLType UNKOWN TYPE %d", (int)type);
    return 0;
}

#ifdef DEBUG 
const char* ShaderDataType_ToString(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:   return "Float";
        case ShaderDataType::Float2:  return "Float2";
        case ShaderDataType::Float3:  return "Float3";
        case ShaderDataType::Float4:  return "Float4";
        case ShaderDataType::Mat3:    return "Mat3";
        case ShaderDataType::Mat4:    return "Mat4";
        case ShaderDataType::Int:     return "Int";
        case ShaderDataType::Int2:    return "Int2";
        case ShaderDataType::Int3:    return "Int3";
        case ShaderDataType::Int4:    return "Int4";
        case ShaderDataType::Bool:    return "Bool";
        default: ;
    }
    RPR_WARN("ShaderDataType_ToString: UNKNOWN TYPE %d", (int)type);
    return "UNKNOWN TYPE";
}
#endif 