#include "PBRRenderpass.hpp"

#include "Scene/Components.hpp"

#include "Renderer/RenderCommand.hpp"

void PBRRenderpass_Initialize(Renderpass* renderpass) {

}

void PBRRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    RenderCommand_BindTexture2D(0);
    RenderCommand_ActiveTexture(0);
    Framebuffer_Bind(renderProperties->framebuffer);

    Shader* pbrShader = renderProperties->pbrShader;
    glm::mat4* view = renderProperties->view;
    glm::mat4* projectionRH = renderProperties->projection;
    glm::vec3* cameraPosition = renderProperties->cameraPosition;
    ImageBasedLighting* ibl = renderProperties->ibl;
    Shader* editorShader = renderProperties->editorShader;


    auto lights = renderProperties->registry->view<TransformComponent, LightComponent>();
    auto group = renderProperties->registry->group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        Shader* shader = materialComponent.material.shader;
        if(shader == pbrShader) {
            Shader_Bind(shader);
            glm::mat4 model = transformComponent.GetTransform();
            Shader_SetMat4(shader, "model", model);
            Shader_SetMat4(shader, "view", *view);
            Shader_SetMat4(shader, "projection", *projectionRH);
            Shader_SetVec3(shader, "camPos", *cameraPosition);
            Material_BindTextures(&materialComponent.material);

            Shader_SetInt(shader, "albedoMap", 0);
            Shader_SetInt(shader, "normalMap", 1);
            Shader_SetInt(shader, "metallicMap", 2);
            Shader_SetInt(shader, "roughnessMap", 3);
            Shader_SetInt(shader, "aoMap", 4);

            RenderCommand_ActiveTexture(5);
            RenderCommand_BindCubemap(ibl->irradianceMap);
            Shader_SetInt(shader, "irradianceMap", 5);
            // Specular IBL 
            RenderCommand_ActiveTexture(6);
            RenderCommand_BindCubemap(ibl->prefilterMap);
            Shader_SetInt(shader, "prefilterMap", 6);
            RenderCommand_ActiveTexture(7);
            RenderCommand_BindTexture2D(ibl->brdfLUTTexture);
            Shader_SetInt(shader, "brdfLUT", 7);

            // TODO: shadowmap
            if(renderProperties->shadowmap != nullptr) {
                Shader_SetVec3(shader, "directionalLightPos", renderProperties->shadowmap->directionalLightPosition);
                Shader_SetMat4(shader, "lightSpaceMatrix", renderProperties->shadowmap->lightSpaceMatrix);
                RenderCommand_ActiveTexture(8);
                RenderCommand_BindTexture2D(renderProperties->shadowmap->depthMap);
                Shader_SetInt(shader, "shadowMap", 8);
                Shader_SetBool(shader, "applyShadow", true);
            }
            else { 
                Shader_SetBool(shader, "applyShadow", false);
            }

            // TODO: SSAO
            if(renderProperties->ssaoEnabled) {
                Shader_SetInt(shader, "applySSAO", 1);
                RenderCommand_ActiveTexture(9);
                RenderCommand_BindTexture2D(renderProperties->ssaoBlurColorBuffer);
                //RenderCommand_BindTexture2D(gBuffer.ssaoBlurColorBuffer);
                Shader_SetInt(shader, "SSAOBlurTexture", 9);
                Shader_SetVec3(shader, "u_resolution", 
                    glm::vec3(renderProperties->viewportSize.x, renderProperties->viewportSize.y, 0.0));

            } else { 
                Shader_SetBool(shader, "applySSAO", 0);
            }
            // TODO: lights

            // color correct
            Shader_SetInt(shader, "colorCorrect", !renderProperties->colorCorrectEnabled); // TODO: do properly when loading textures or something

            
            if(lights.size_hint() > 4) 
                RPR_WARN("EditorScene_OnUpdateRuntime: Rendering using more than 4 lights, only 4 lights supported!");
            Shader_SetInt(shader, "lightCount", lights.size_hint());
            if(lights.size_hint() >= 1) {
                int i = 0;
                lights.each([shader, &i](auto& transformComponent, auto& lightComponent) {
                    glm::vec3& lightPos = transformComponent.position;
                    std::string lightPosString = "lightPositions[" + std::to_string(i) + "]";
                    std::string lightColorsString = "lightColors[" + std::to_string(i) + "]";
                    Shader_SetVec3(shader, lightPosString.c_str(), lightPos);
                    Shader_SetVec3(shader, lightColorsString.c_str(), 
                        PointLight_GetIntensifiedColor(&lightComponent.pointLight));
                    i++;
                });
            }
            

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }

        // Actually editor renderpass 
        if(shader == editorShader) {
            glm::mat4 model = transformComponent.GetTransform();

            Shader_Bind(editorShader);
            Shader_SetMat4(editorShader, "model", model);
            Shader_SetMat4(editorShader, "view", *view);
            Shader_SetMat4(editorShader, "projection", *projectionRH);
            RenderCommand_ActiveTexture(0);
            Texture* albedo = materialComponent.material.textures.data[0];
            if(albedo != nullptr)
                Texture_Bind(albedo);
            Shader_SetInt(editorShader, "texture_0", 0);

            Shader_SetInt(editorShader, "colorCorrect", 0);

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }
    }
}

void PBRRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height) {

}

void PBRRenderpass_Shutdown(Renderpass* renderpass) {

}