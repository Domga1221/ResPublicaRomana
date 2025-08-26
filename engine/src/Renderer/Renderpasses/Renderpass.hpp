#pragma once 

#include "defines.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/PBR/ImageBasedLighting.hpp"
#include "Renderer/Light/Shadowmap.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

enum RenderpassType {
    RENDERPASS_NONE,

    RENDERPASS_SHADOWMAP,

    RENDERPASS_EDITOR,
    RENDERPASS_PBR,

    RENDERPASS_SKYBOX,
    RENDERPASS_PARTICLESYSTEM,

    RENDERPASS_COLOR_CORRECT,
    RENDERPASS_BLOOM,
    RENDERPASS_SSAO, 
};


typedef struct RenderProperties {
    f32 deltaTime;
    Framebuffer* framebuffer;
    glm::mat4* view;
    glm::mat4* projection;
    glm::vec3* cameraPosition;

    glm::vec2 viewportSize; // TODO: could be in renderpass itself as to not copy around too much

    // TODO: passing these for now
    entt::registry* registry;
    Shader* pbrShader;
    Shader* editorShader;
    ImageBasedLighting* ibl;
    Shadowmap* shadowmap;
    b8 ssaoEnabled;
    u32 ssaoBlurColorBuffer;
    b8 colorCorrectEnabled;
} RenderProperties;


typedef struct Renderpass { // TODO: maybe should just use objects 
    void (*Initialize)(Renderpass* renderpass);
    void (*Render)(Renderpass* renderpass, RenderProperties* renderProperties);
    void (*Resize)(Renderpass* renderpass, u32 width, u32 height);
    void (*Shutdown)(Renderpass* renderpass);

    b8 enabled;
    RenderpassType type;
    void* internalData;
} Renderpass;

RPR_API void Renderpass_Create(Renderpass* renderpass, RenderpassType type);
RPR_API void Renderpass_Destroy(Renderpass* renderpass);