#pragma once 


#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

struct Skybox {
    VertexBuffer vertexBuffer;
    VertexArray vertexArray;
    u32 textureID;
    Shader skyboxShader;
};

RPR_API void Skybox_Create(Skybox* skybox);
RPR_API void Skybox_Destroy(Skybox*  skybox);
 
RPR_API void Skybox_Render(Skybox* skybox, glm::mat4* view, glm::mat4* projection);