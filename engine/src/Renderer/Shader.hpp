#pragma once 

#include "defines.hpp"

#include <string>

#include <glm/glm.hpp>

struct Shader {
    u32 ID;
};

RPR_API void Shader_Create(Shader* shader, const u8* vertexPath, const u8* fragmentPath);
RPR_API void Shader_Create(Shader* shader, std::string vertexPath, std::string fragmentPath);

RPR_API void Shader_Destroy(Shader* shader);

RPR_API void Shader_Bind(Shader* shader);
RPR_API void Shader_Unbind();


RPR_API void Shader_SetBool(Shader* shader, const char* name, b8 value);
RPR_API void Shader_SetBool(u32 location, b8 value);

RPR_API void Shader_SetInt(Shader* shader, const char* name, i32 value);
RPR_API void Shader_SetInt(u32 location, i32 value);

RPR_API void Shader_SetFloat(Shader* shader, const char* name, f32 value);
RPR_API void Shader_SetFloat(u32 location, f32 value);

RPR_API void Shader_SetMat3(Shader* shader, const char* name, const glm::mat3& mat);
RPR_API void Shader_SetMat3(u32 location, const glm::mat3& mat);

RPR_API void Shader_SetMat4(Shader* shader, const char* name, const glm::mat4& mat);
RPR_API void Shader_SetMat4(u32 location, const glm::mat4& mat);

RPR_API void Shader_SetVec3(Shader* shader, const char* name, const glm::vec3& vec);
RPR_API void Shader_SetVec3(u32 location, const glm::vec3 vec);

RPR_API void Shader_SetVec4(Shader* shader, const char* name, const glm::vec4& vec);
RPR_API void Shader_SetVec4(u32 location, const glm::vec4& vec);
