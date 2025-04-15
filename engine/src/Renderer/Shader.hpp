#pragma once 

#include "defines.hpp"

#include <string>

#include <glm/glm.hpp>

struct Shader {
    u32 ID;
};

void Shader_Create(Shader* shader, const u8* vertexPath, const u8* fragmentPath);
void Shader_Create(Shader* shader, std::string vertexPath, std::string fragmentPath);

void Shader_Bind(Shader* shader);
void Shader_Unbind();


void Shader_SetBool(Shader* shader, const u8* name, b8 value);
void Shader_SetBool(u32 location, b8 value);

void Shader_SetInt(Shader* shader, const u8* name, i32 value);
void Shader_SetInt(u32 location, i32 value);

void Shader_SetFloat(Shader* shader, const u8* name, f32 value);
void Shader_SetFloat(u32 location, f32 value);

void Shader_SetMat3(Shader* shader, const u8* name, const glm::mat3& mat);
void Shader_SetMat3(u32 location, const glm::mat3& mat);

void Shader_SetMat4(Shader* shader, const u8* name, const glm::mat4& mat);
void Shader_SetMat4(u32 location, const glm::mat4& mat);

void Shader_SetVec3(Shader* shader, const u8* name, const glm::vec3& vec);
void Shader_SetVec3(u32 location, const glm::vec3 vec);

void Shader_SetVec4(Shader* shader, const u8* name, const glm::vec4& vec);
void Shader_SetVec4(u32 location, const glm::vec4& vec);
