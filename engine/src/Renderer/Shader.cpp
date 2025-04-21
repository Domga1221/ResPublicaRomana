#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Core/Log.hpp"


void Shader_Create(Shader* shader, const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vertexShaderFile, fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vertexShaderFile.open((const char*)vertexPath);
        fragmentShaderFile.open((const char*)fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        //std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
        RPR_ERROR("Shader_Read: FILE NOT SUCCESFULLY READ\n %s", e.what());
    }

    const char* vertexShaderCode = vertexCode.c_str();
    const char* fragmentShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        RPR_ERROR("Shader_Read: VERTEX COMPILATION FAILED\n %s", infoLog);
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        RPR_ERROR("Shader_Read: FRAGMENT COMPILATION FAILED\n %s", infoLog);
    }

    // shader Program
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertex);
    glAttachShader(shader->ID, fragment);
    glLinkProgram(shader->ID);
    // print linking errors if any
    glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
        RPR_ERROR("Shader_Create: PROGRAM LINKING FAILED\n {0}", infoLog);
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader_Create(Shader* shader, std::string vertexPath, std::string fragmentPath) {
    Shader_Create(shader, (const char*)vertexPath.c_str(), (const char*)fragmentPath.c_str());
}

void Shader_Bind(Shader* shader) {
    glUseProgram(shader->ID);
}

void Shader_Unbind() {
    glUseProgram(0);
}


void Shader_SetBool(Shader* shader, const char* name, b8 value) {
    glUniform1i(glGetUniformLocation(shader->ID, (const char*)name), (int)value);
}

void Shader_SetBool(u32 location, b8 value) {
    glUniform1i(location, (int)value);
}

void Shader_SetInt(Shader* shader, const char* name, i32 value) {
    glUniform1i(glGetUniformLocation(shader->ID, (const char*)name), (int) value);
}

void Shader_SetInt(u32 location, i32 value) {
    glUniform1i(location, value);
}

void Shader_SetFloat(Shader* shader, const char* name, f32 value) {
    glUniform1f(glGetUniformLocation(shader->ID, (const char*)name), value);
}   

void Shader_SetFloat(u32 location, f32 value) {
    glUniform1f(location, value);
}

void Shader_SetMat3(Shader* shader, const char* name, const glm::mat3& mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader->ID, (const char*)name), 1, GL_FALSE, &mat[0][0]);
}

void Shader_SetMat3(u32 location, const glm::mat3& mat) {
    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader_SetMat4(Shader* shader, const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, (const char*)name), 1, GL_FALSE, &mat[0][0]);
}

void Shader_SetMat4(u32 location, const glm::mat4& mat) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader_SetVec3(Shader* shader, const char* name, const glm::vec3& vec) {
    glUniform3fv(glGetUniformLocation(shader->ID, (const char*)name), 1, &vec[0]);
}

void Shader_SetVec3(u32 location, const glm::vec3 vec) {
    glUniform3fv(location, 1, &vec[0]);
}

void Shader_SetVec4(Shader* shader, const char* name, const glm::vec4& vec) {
    glUniform4fv(glGetUniformLocation(shader->ID, (const char*)name), 1, &vec[0]);
}

void Shader_SetVec4(u32 location, const glm::vec4& vec) {
    glUniform4fv(location, 1, &vec[0]);
}
