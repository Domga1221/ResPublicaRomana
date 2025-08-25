#include "SSAO.hpp"

#include "GBuffer.hpp"

RPR_API void SSAO_Initialize(Renderpass* renderpass) {
    // TODO: doing it like this for now, kinda reminds of subpasses
    renderpass->internalData = MEMORY_Allocate(sizeof(GBuffer), MEMORY_TAG_RENDERER);
    GBuffer* gBuffer = (GBuffer*)renderpass->internalData;
    //MEMORY_Set(&gBuffer, 0, sizeof(GBuffer));
    GBuffer_Initialize(gBuffer);
}

RPR_API void SSAO_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    GBuffer* gBuffer = (GBuffer*)renderpass->internalData;
    GBuffer_Render(gBuffer, 
        renderProperties->view, renderProperties->projection, 
        renderProperties->registry,
        renderProperties->pbrShader);
    renderProperties->ssaoBlurColorBuffer = gBuffer->ssaoBlurColorBuffer;
}

RPR_API void SSAO_Resize(Renderpass* renderpass, u32 width, u32 height) {
    GBuffer* gBuffer = (GBuffer*)renderpass->internalData;
    GBuffer_OnResize(gBuffer, width, height);
}

RPR_API void SSAO_Shutdown(Renderpass* renderpass) {
    // TODO:
}