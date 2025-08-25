#pragma once 

#include "defines.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Shader.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

enum RenderpassType {
    RENDERPASS_NONE,

    RENDERPASS_EDITOR,
    RENDERPASS_PBR,

    RENDERPASS_COLOR_CORRECT,
    RENDERPASS_BLOOM,
    RENDERPASS_SSAO, 
};


typedef struct RenderProperties {
    f32 deltaTime;
    Framebuffer* framebuffer;
    glm::mat4* view;
    glm::mat4* projection;

    // TODO: passing these for now
    entt::registry* registry;
    Shader* pbrShader;
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