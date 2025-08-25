#include "SkyboxRenderpass.hpp"

#include "Renderer/PBR/ImageBasedLighting.hpp"

void SkyboxRenderpass_Initialize(Renderpass* renderpass) {

}

void SkyboxRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    ImageBasedLighting* ibl = renderProperties->ibl;
    glm::mat4* view = renderProperties->view;
    glm::mat4* projectionRH = renderProperties->projection;
    b8 colorCorrect = !renderProperties->colorCorrectEnabled;
    ImageBasedLighting_RenderSkybox(ibl, *view, *projectionRH, colorCorrect);
}

void SkyboxRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height) {

}

void SkyboxRenderpass_Shutdown(Renderpass* renderpass) {

}