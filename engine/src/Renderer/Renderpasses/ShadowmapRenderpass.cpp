#include "ShadowmapRenderpass.hpp"

#include "Scene/Components.hpp"
#include "Renderer/RenderCommand.hpp"

RPR_API void ShadowmapRenderpass_Initialize(Renderpass* renderpass) {

}

RPR_API void ShadowmapRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    auto lights = renderProperties->registry->view<TransformComponent, LightComponent>();
    auto group = renderProperties->registry->group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);

    Shadowmap* shadowmap = nullptr;
    glm::vec3 lightPosition;
    // TODO: check why i can't iterate with for(auto entity : lights)
    // TODO: don't need to iterate over all lights,
    lights.each([&shadowmap, &lightPosition](TransformComponent& transformComponent, LightComponent& lightComponent) {
        if(lightComponent.shadowmap != nullptr) {
            shadowmap = lightComponent.shadowmap;
            lightPosition = transformComponent.position;
        }
    });
    if(shadowmap != nullptr) {
        RenderCommand_CullFrontFace();
        // render to depth map 
        shadowmap->directionalLightPosition = lightPosition;
        shadowmap->lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shadowmap->lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowmap->nearPlane, shadowmap->farPlane);
        shadowmap->lightSpaceMatrix = shadowmap->lightProjection * shadowmap->lightView;
            
        // render scene from lights point of view
        Shader_Bind(&shadowmap->simpleDepthShader);
        Shader_SetMat4(&shadowmap->simpleDepthShader, "lightSpaceMatrix", shadowmap->lightSpaceMatrix);
        

        RenderCommand_SetViewportSize(shadowmap->SHADOW_WIDTH, shadowmap->SHADOW_HEIGHT);
        RenderCommand_BindFramebuffer(shadowmap->depthMapFBO);
        RenderCommand_Clear(false, true);

        for(entt::entity entity : group) {
            std::tuple<TransformComponent&, MeshComponent&> tuple =
                group.get<TransformComponent, MeshComponent>(entity);

            TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
            MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);

            glm::mat4 model = transformComponent.GetTransform();
            Shader_SetMat4(&shadowmap->simpleDepthShader, "model", model);
            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }    
        RenderCommand_CullBackFace();    
    }


    // TODO: maybe own function UpdateRenderProperties after Render is called for all Renderpasses
    renderProperties->shadowmap = shadowmap;
}

RPR_API void ShadowmapRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height) {

}

RPR_API void ShadowmapRenderpass_Shutdown(Renderpass* renderpass) {

}

