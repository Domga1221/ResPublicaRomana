#pragma once 

#include "defines.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

enum class ShaderDataType {
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

u32 ShaderDataType_GetSize(ShaderDataType type);

u32 ShaderDataType_GetComponentCount(ShaderDataType type);

GLenum ShaderDataType_ToGLType(ShaderDataType type);

#ifdef DEBUG 
const char* ShaderDataType_ToString(ShaderDataType type);
#endif 