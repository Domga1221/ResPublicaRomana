#include "Renderpass.hpp"

#include "Core/Log.hpp"

#include "PostProcessing/Bloom.hpp"
#include "PostProcessing/ColorCorrect.hpp"
#include "PostProcessing/SSAO.hpp"
#include "EditorRenderpass.hpp"
#include "PBRRenderpass.hpp"
#include "ShadowmapRenderpass.hpp"

void Renderpass_Create(Renderpass* renderpass, RenderpassType type) {
    if(type == RENDERPASS_BLOOM) {
        renderpass->Initialize = Bloom_Initialize;
        renderpass->Render = Bloom_Render;
        renderpass->Resize = Bloom_Resize;
        renderpass->Shutdown = Bloom_Shutdown;
    }

    if(type == RENDERPASS_SSAO) {
        renderpass->Initialize = SSAO_Initialize;
        renderpass->Render = SSAO_Render;
        renderpass->Resize = SSAO_Resize;
        renderpass->Shutdown = SSAO_Shutdown;
    }

    if(type == RENDERPASS_COLOR_CORRECT) {
        renderpass->Initialize = ColorCorrect_Initialize;
        renderpass->Render = ColorCorrect_Render;
        renderpass->Resize = ColorCorrect_Resize;
        renderpass->Shutdown = ColorCorrect_Shutdown;
    }

    if(type == RENDERPASS_EDITOR) {
        renderpass->Initialize = EditorRenderpass_Initialize;
        renderpass->Render = EditorRenderpass_Render;
        renderpass->Resize = EditorRenderpass_Resize;
        renderpass->Shutdown = EditorRenderpass_Shutdown;
    }

    if(type == RENDERPASS_PBR) {
        renderpass->Initialize = PBRRenderpass_Initialize;
        renderpass->Render = PBRRenderpass_Render;
        renderpass->Resize = PBRRenderpass_Resize;
        renderpass->Shutdown = PBRRenderpass_Shutdown;
    }

    if(type == RENDERPASS_SHADOWMAP) {
        renderpass->Initialize = ShadowmapRenderpass_Initialize;
        renderpass->Render = ShadowmapRenderpass_Render;
        renderpass->Resize = ShadowmapRenderpass_Resize;
        renderpass->Shutdown = ShadowmapRenderpass_Shutdown;
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