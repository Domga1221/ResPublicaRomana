#include "Skybox.hpp"

#include <filesystem>

#include <stb/stb_image.h>

#include "RenderCommand.hpp"

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void Skybox_Create(Skybox* skybox) {
    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { "aPosition", ShaderDataType::Float3 });
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);

    VertexBuffer_Create(&skybox->vertexBuffer, skyboxVertices, sizeof(skyboxVertices));
    VertexBuffer_SetLayout(&skybox->vertexBuffer, &bufferLayout);

    VertexArray_Create(&skybox->vertexArray);
    VertexArray_AddVertexBuffer(&skybox->vertexArray, &skybox->vertexBuffer);

    std::string currentPath = std::filesystem::current_path().parent_path().parent_path().string();
    RPR_DEBUG("Skybox_Create: Loading cubemap from path: %s", currentPath.c_str());
    
    
    const char* faces[6] = {
        "px",
        "nx",
        "py",
        "ny",
        "pz",
        "nz"
    };
    
    /*
    const char* faces[6] = {
        "right",
        "left",
        "top",
        "bottom",
        "front",
        "back"
    };
    */

    glGenTextures(1, &skybox->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->textureID);

    stbi_set_flip_vertically_on_load(false);
    i32 width; i32 height; i32 numberOfChannels;
    for(u32 i = 0; i < 6; i++) {
        std::string filePath = currentPath + "/Assets/Textures/Standard-Cube-Map/" + faces[i] + ".png";
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &numberOfChannels, 0);
        if(!data) {
            RPR_ERROR("Failed to load cubemap face: %d, %s", i, faces[i]);
            stbi_image_free(data);
            return;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::string vertPath = currentPath + "/Assets/Shaders/skybox.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/skybox.frag";
    Shader_Create(&skybox->skyboxShader, vertPath.c_str(), fragPath.c_str());
}

void Skybox_Destroy(Skybox* skybox) {
    // TODO:
}

void Skybox_Render(Skybox* skybox, glm::mat4* view, glm::mat4* projection) {
    glm::mat4 view_upper3x3 = *view;
    view_upper3x3 = glm::mat4(glm::mat3(view_upper3x3));
    glDepthFunc(GL_LEQUAL);
    Shader_Bind(&skybox->skyboxShader);
    Shader_SetInt(&skybox->skyboxShader, "skybox", 0);
    Shader_SetMat4(&skybox->skyboxShader, "view", view_upper3x3);
    Shader_SetMat4(&skybox->skyboxShader, "projection", *projection);
    VertexArray_Bind(&skybox->vertexArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    VertexArray_Unbind();
    glDepthFunc(GL_LESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}