#pragma once 

#include "defines.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

#include <string>

enum class UniformDataType {
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool,
	Sampler2D
};

/*
unsigned int UniformDataTypeSize(UniformDataType type) {
	switch (type) {
        case UniformDataType::Float:    return 4;
        case UniformDataType::Float2:   return 4 * 2;
        case UniformDataType::Float3:   return 4 * 3;
        case UniformDataType::Float4:   return 4 * 4;
        case UniformDataType::Mat3:     return 4 * 3 * 3;
        case UniformDataType::Mat4:     return 4 * 4 * 4;
        case UniformDataType::Int:      return 4;
        case UniformDataType::Int2:     return 4 * 2;
        case UniformDataType::Int3:     return 4 * 3;
        case UniformDataType::Int4:     return 4 * 4;
        case UniformDataType::Bool:     return 1;
        default: 
            return -1;
	}

	RPR_ERROR("UniformDataTypeSize UNKOWN TYPE {0}", (int)type);
	return 0;
}
*/


UniformDataType GLenumToREUniformDataType(GLenum e) {
	switch (e) {
		case GL_SAMPLER_2D: return UniformDataType::Sampler2D;
		case GL_FLOAT_MAT4: return UniformDataType::Mat4;
		case GL_INT: return UniformDataType::Int;
        default: return UniformDataType::None;
	}
}


// TODO: Think about combining this with ShaderDataType
typedef struct Uniform {
    i8* name;
    UniformDataType uniformDataType;
    // glGetUniformLocation returns -1 if uniform is inside block, or if name is not found as active
    i32 location = -99;
} Uniform;

