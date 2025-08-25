#include "Renderpass.hpp"

#include "Core/Log.hpp"

#include "PostProcessing/Bloom.hpp"

void Renderpass_Create(Renderpass* renderpass, RenderpassType type) {
    if(type == RENDERPASS_BLOOM) {
        renderpass->Initialize = Bloom_Initialize;
        renderpass->Render = Bloom_Render;
        renderpass->Resize = Bloom_OnResize;
        renderpass->Shutdown = Bloom_Shutdown;
    }

    if(type == RENDERPASS_SSAO) {

    }

    if(type == RENDERPASS_COLOR_CORRECT) {

    }
    
    if(type == RENDERPASS_NONE) {
        RPR_ERROR("Renderpass_Create: type RENDERPASS_NONE requested");
        return;
    }


    renderpass->enabled = true;
}

void Renderpass_Destroy(Renderpass* renderpass) {
    renderpass->Initialize = 0;
    renderpass->Render = 0;
    renderpass->Resize = 0;
    renderpass->Shutdown = 0;
    renderpass->type = RENDERPASS_NONE;
}