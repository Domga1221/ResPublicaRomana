#include "Uniform.hpp"

UniformDataType GLenumToREUniformDataType(GLenum e) {
	switch (e) {
		case GL_SAMPLER_2D: return UniformDataType::Sampler2D;
		case GL_FLOAT_MAT4: return UniformDataType::Mat4;
		case GL_INT: return UniformDataType::Int;
        default: return UniformDataType::None;
	}
}