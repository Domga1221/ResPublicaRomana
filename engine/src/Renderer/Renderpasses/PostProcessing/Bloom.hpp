#pragma once 

#include "defines.hpp"

#include <Renderer/Framebuffer.hpp>
#include <Renderer/Shader.hpp>

typedef struct Bloom {
    Framebuffer framebuffer;
    Shader brightPixelShader;

    Framebuffer pingPongFramebuffers[2];
    Shader blurShader;
    Framebuffer combineFramebuffer;
    Shader combineShader;
} Bloom;

void RPR_API Bloom_Initialize(Bloom* bloom);
void RPR_API Bloom_Shutdown();

void RPR_API Bloom_Render(Bloom* bloom, Framebuffer* framebuffer); // TODO: Renderpass
void RPR_API Bloom_RenderToTexture(Bloom* bloom, const u32 hdrTexture);

void RPR_API Bloom_OnResize(Bloom* bloom, u32 width, u32 height);