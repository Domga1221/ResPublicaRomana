#include "Material.hpp"

#include <vector>

int sortUniform(const void* u1, const void* u2) {
    return ((Uniform*)u1)->location - ((Uniform*)u2)->location;
}

void Material_Create(Material* material, Shader* shader) {
    List_Create(&material->uniforms);
    List_Create(&material->textures);
    material->textureCount = 0;

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
        Uniform u; 
        String_Create(&u.name, name);
        u.uniformDataType = GLenumToREUniformDataType(type);
        u.location = location;
        List_PushBack(&material->uniforms, u);
        RPR_ERROR("string address: %u, string: %s", material->uniforms.data[i].name.sequence, material->uniforms.data[i].name.sequence);
    }

    // TODO: sorting
    qsort(material->uniforms.data, material->uniforms.size, sizeof(Uniform), sortUniform);

    RPR_INFO("--- PRINTING UNIFORMS ---");
    for(u32 i = 0; i < material->uniforms.size; i++) {
        Uniform* uniform = &material->uniforms.data[i];
        RPR_INFO("Name: %s, DataType: %d, Location: %d",
                uniform->name.sequence, uniform->uniformDataType, uniform->location);
    }

    // TODO: special textures
    std::vector<std::string> textureDebugString;
    for(u32 i = 0; i < material->uniforms.size; i++) {
        Uniform* uniform = &material->uniforms.data[i];
        if(uniform->uniformDataType == UniformDataType::Sampler2D) {
            material->textureCount++;
            textureDebugString.push_back(uniform->name.sequence);
        }
    }


    RPR_INFO("--- PRINTING TEXTURES ---");
    RPR_INFO("Texture Count: %d", material->textureCount);
    // TODO: List resizing
    for(u32 i = 0; i < material->textureCount; i++) {
        List_PushBack(&material->textures, 0);  // TODO: currently pushing nullpointer
    }

    for(int i = 0; i < material->textures.size; i++) {
        RPR_INFO("texture: %s, \tindex: %d, \tpointerVal: %p",
                textureDebugString[i], i, material->textures.data[i]);
    }

    material->shader = shader;
}

RPR_API void Material_Destroy(Material* material) {
    material->shader = nullptr;
    material->textureCount = 0;
    for(u32 i = 0; i < material->textures.size; i++) {
        Texture_Destroy(material->textures.data[i]);
    }
    for(u32 i = 0; i < material->uniforms.size; i++) {
        RPR_ERROR("String address: %u, string: %s", material->uniforms.data[i].name.sequence, material->uniforms.data[i].name.sequence);
        String_Destroy(&material->uniforms.data[i].name);
        material->uniforms.data[i].uniformDataType = UniformDataType::None;
        material->uniforms.data[i].location = -99;
    }
    List_Destroy(&material->textures);
    List_Destroy(&material->uniforms);
}

void Material_SendToShader(Material* material, Uniform* uniform, void* data) {
    switch (uniform->uniformDataType) {
        case UniformDataType::None:
            RPR_WARN("Trying to send UniformDataType::None to shader!");
            break;
        case UniformDataType::Bool: 
            Shader_SetBool(uniform->location, *(bool*)data);
            break;
        case UniformDataType::Int:
            //std::cout << *(int*)data;
            Shader_SetInt(uniform->location, *(int*)data);
            break;
        case UniformDataType::Float:
            Shader_SetFloat(uniform->location, *(float*)data);
            break;
        case UniformDataType::Float3:
            Shader_SetVec3(uniform->location, *(glm::vec3*)data);
            break;
        case UniformDataType::Mat3: 
            Shader_SetMat3(uniform->location, *(glm::mat3*)data);
            break;
        case UniformDataType::Mat4:
            Shader_SetMat4(uniform->location, *((glm::mat4*)data));
            break;
        default:
            RPR_WARN("Trying to send UniformDataType to shader that is not supported!");
            break;
    }
}
