#pragma once 

#include "defines.hpp"

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Shader.hpp"

#include "Renderer/Renderpasses/Renderpass.hpp"

typedef struct Bloom {
    Framebuffer framebuffer;
    Shader brightPixelShader;

    Framebuffer pingPongFramebuffers[2];
    Shader blurShader;
    Framebuffer combineFramebuffer;
    Shader combineShader;
} Bloom;

void RPR_API Bloom_Initialize(Renderpass* bloomRenderpass);
void RPR_API Bloom_Render(Renderpass* bloomRenderpass, RenderProperties* renderProperties); // TODO: Renderpass
void RPR_API Bloom_Resize(Renderpass* bloomRenderpass, u32 width, u32 height);
void RPR_API Bloom_Shutdown(Renderpass* bloomRenderpass);


void RPR_API Bloom_RenderToTexture(Bloom* bloom, const u32 hdrTexture); // TODO: should only be internal 
