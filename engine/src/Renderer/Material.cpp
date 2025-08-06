#include "Material.hpp"

#include <vector>

int sortUniform(const void* u1, const void* u2) {
    return ((Uniform*)u1)->location - ((Uniform*)u2)->location;
}

void Material_Create(Material* material, Shader* shader) {
    List_Create(&material->uniforms);
    List_Create(&material->textures);

    RPR_WARN("READING UNIFORMS IN SHADER");
    GLint count;
    GLint size;
    GLenum type;
    const GLsizei bufSize = 16;
    GLchar name[bufSize];
    GLsizei length;
    glGetProgramiv(shader->ID, GL_ACTIVE_UNIFORMS, &count);
    RPR_WARN("Active Uniforms: %d", count); 

    for(GLint i = 0; i < count; i++) {
        MEMORY_Set(name, 0, bufSize);
        glGetActiveUniform(shader->ID, (GLuint)i, bufSize, &length, & size, &type, name);
        int location = glGetUniformLocation(shader->ID, name);
        //RE_CORE_INFO("Uniform: {0}, Type: {1}, Name: {2}, Location: {3}", i, type, name, location);
        u32 strLen = strlen(name); 
        Uniform u; 
        u.name = (i8*)MEMORY_Allocate(strLen + 1, MEMORY_TAG_STRING);
        MEMORY_Copy(u.name, name, strLen); 
        u.name[strLen] = '\0';
        u.uniformDataType = GLenumToREUniformDataType(type);
        u.location = location;
        List_PushBack(&material->uniforms, u);
    }

    // TODO: sorting
    qsort(material->uniforms.data, material->uniforms.size, sizeof(Uniform), sortUniform);

    RPR_INFO("--- PRINTING UNIFORMS ---");
    for(u32 i = 0; i < material->uniforms.size; i++) {
        Uniform* uniform = &material->uniforms.data[i];
        RPR_INFO("Name: %s, DataType: %d, Location: %d",
                uniform->name, uniform->uniformDataType, uniform->location);
    }

    // TODO: special textures
    std::vector<std::string> textureDebugString;
    for(u32 i = 0; i < material->uniforms.size; i++) {
        Uniform* uniform = &material->uniforms.data[i];
        if(uniform->uniformDataType == UniformDataType::Sampler2D) {
            material->textureCount++;
            textureDebugString.push_back(((char*)uniform->name));
        }
    }


    RPR_INFO("--- PRINTING TEXTURES ---");
    RPR_INFO("Texture Count: %d", material->textureCount);
    // TODO: List resizing
    for(u32 i = 0; i < material->textureCount; i++) {
        List_PushBack(&material->textures, 0);
    }

    for(int i = 0; i < material->textures.size; i++) {
        RPR_INFO("texture: %s, \tindex: %d, \tpointerVal: %p",
                textureDebugString[i], i, material->textures.data[i]);
    }
}

