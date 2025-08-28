#pragma once 

#include "defines.hpp"

#include <Renderer/Framebuffer.hpp>
#include "Renderer/Shader.hpp"

// TODO: includes
#include <glm/glm.hpp>
#include <vector>
#include <entt/entt.hpp>


// TODO: check what's actually needed in struct and what to expose to client 
typedef struct GBuffer {
    u32 gBuffer; // framebuffer
    u32 gPosition; // color attachment for position
    u32 gNormal; // color attachment for normal
    u32 gAlbedo; // color attachment for albedo/specular
    u32 rboDepth;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    u32 noiseTexture;
    u32 ssaoFBO;
    u32 ssaoColorBuffer;
    
    u32 ssaoBlurFBO;
    u32 ssaoBlurColorBuffer;

    glm::vec2 viewportSize = glm::vec2(1600, 900);

    Shader gBufferShader;
    Shader SSAOShader;
    Shader SSAOBlurShader;
} GBuffer;


RPR_API void GBuffer_Initialize(GBuffer* gBuffer);
RPR_API void GBuffer_Shutdown(GBuffer* gBuffer);
RPR_API void GBuffer_OnResize(GBuffer* gBuffer, u32 width, u32 height);
RPR_API void GBuffer_Render(GBuffer* gBuffer, glm::mat4* view, 
    glm::mat4* projectionRH, entt::registry* registry, Shader* pbrShader);
