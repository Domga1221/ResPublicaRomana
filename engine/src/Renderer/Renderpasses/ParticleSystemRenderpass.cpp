#include "ParticleSystemRenderpass.hpp"

#include "Renderer/RenderCommand.hpp"
#include "Renderer/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Components.hpp"

#include <entt/entt.hpp>

void ParticleSystemRenderpass_Initialize(Renderpass* renderpass) {

}

void ParticleSystemRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    f32 deltaTime = renderProperties->deltaTime;
    glm::mat4* view = renderProperties->view;
    glm::mat4* projectionRH = renderProperties->projection;

    RenderCommand_EnableBlend();
    RenderCommand_BlendEquation_Add();
    RenderCommand_BlendFunc_SrcAlpha_One(); 
    RenderCommand_DepthMask(false);
    auto particleSystemComponents = renderProperties->registry->view<ParticleSystemComponent, TransformComponent>();
    particleSystemComponents.each([deltaTime, view, projectionRH](ParticleSystemComponent& pc, TransformComponent& tc) {
        glm::mat4 model = tc.GetTransform();
        ParticleSystem_Emit(pc.particleSystem, &pc.particleProps);
        ParticleSystem_Update(pc.particleSystem, deltaTime);
        ParticleSystem_Render(pc.particleSystem, &model, view, projectionRH, false);
    });
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    RenderCommand_DepthMask(true);
    RenderCommand_DisableBlend();
}

void ParticleSystemRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height) {

}

void ParticleSystemRenderpass_Shutdown(Renderpass* renderpass) {

}