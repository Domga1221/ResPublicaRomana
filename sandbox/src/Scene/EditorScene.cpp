#include "EditorScene.hpp"

#include <Scene/Components.hpp>

#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>

#include <Renderer/PBR/ImageBasedLighting.hpp>
ImageBasedLighting ibl;
#include <Renderer/Primitives.hpp>
#include <glad/glad.h> // TODO: remove

#include <filesystem>

#include "ShaderPool.hpp"

#include "Renderer/Renderpasses/PostProcessing/Bloom.hpp"
#include "Renderer/Renderpasses/PostProcessing/ColorCorrect.hpp"
Bloom bloom;

void EditorScene_Initialze() {
    ShaderPool_Initialize();
    std::string hdrPath = std::string("Assets/HDR/newport_loft.hdr");
    ImageBasedLighting_Initialize(&ibl, hdrPath.c_str());
    Bloom_Initialize(&bloom);
    ColorCorrect_Initialize();
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
    Shader* editorShader = ShaderPool_GetEditorShader();
    glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
    glm::mat4 projection = SceneCamera_GetProjectinoMatrix(sceneCamera);

    auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        //RPR_INFO("(%d, %d, %d)", transformComponent.position.x, transformComponent.position.y, transformComponent.position.z);

        glm::mat4 model = transformComponent.GetTransform();

        Shader_Bind(editorShader);
        Shader_SetMat4(editorShader, "model", model);
        Shader_SetMat4(editorShader, "view", view);
        Shader_SetMat4(editorShader, "projection", projection);
        RenderCommand_ActiveTexture(0);
        Texture* albedo = materialComponent.material.textures.data[0];
        if(albedo != nullptr)
            Texture_Bind(albedo);
        Shader_SetInt(editorShader, "texture_0", 0);

        Shader_SetInt(editorShader, "colorCorrect", 0);

        Mesh_Bind(&meshComponent.mesh);
        RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
    }
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    RenderCommand_ActiveTexture(0);
    Shader_Bind(editorShader);
    Shader_SetMat4(editorShader, "model", model);
    Shader_SetMat4(editorShader, "view", view);
    Shader_SetMat4(editorShader, "projection", projection);
    Shader_SetInt(editorShader, "texture_0", 0);
    RenderCommand_BindTexture2D(ibl.hdrTexture);
    Shader_SetMat4(editorShader, "model", model);
    Primitives_RenderQuad();
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f));
    Shader_SetMat4(editorShader, "model", model);
    Primitives_RenderCube();

    // ibl
    ImageBasedLighting_RenderSkybox(&ibl, view, projection, true);
}

void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, Framebuffer* framebuffer) {
    RenderCommand_ActiveTexture(0);
    
    // TODO: renderpasses 
    bool colorCorrect = true;
    
    
    glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
    glm::mat4 projection = SceneCamera_GetProjectinoMatrix(sceneCamera);

    auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        Shader* shader = materialComponent.material.shader;
        if(shader == ShaderPool_GetEditorShader()) {
            glm::mat4 model = transformComponent.GetTransform();

            Shader_Bind(shader);
            Shader_SetMat4(shader, "model", model);
            Shader_SetMat4(shader, "view", view);
            Shader_SetMat4(shader, "projection", projection);
            RenderCommand_ActiveTexture(0);
            Texture* albedo = materialComponent.material.textures.data[0];
            if(albedo != nullptr)
                Texture_Bind(albedo);
            Shader_SetInt(shader, "texture_0", 0);

            Shader_SetInt(shader, "colorCorrect", colorCorrect);

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }
        if(shader == ShaderPool_GetPBRShader()) {
            Shader_Bind(shader);
            glm::mat4 model = transformComponent.GetTransform();
            Shader_SetMat4(shader, "model", model);
            Shader_SetMat4(shader, "view", view);
            Shader_SetMat4(shader, "projection", projection);
            Shader_SetVec3(shader, "camPos", sceneCamera->position);
            Material_BindTextures(&materialComponent.material);

            Shader_SetInt(shader, "albedoMap", 0);
            Shader_SetInt(shader, "normalMap", 1);
            Shader_SetInt(shader, "metallicMap", 2);
            Shader_SetInt(shader, "roughnessMap", 3);
            Shader_SetInt(shader, "aoMap", 4);

            RenderCommand_ActiveTexture(5);
            RenderCommand_BindCubemap(ibl.irradianceMap);
            Shader_SetInt(shader, "irradianceMap", 5);
            // Specular IBL 
            RenderCommand_ActiveTexture(6);
            RenderCommand_BindCubemap(ibl.prefilterMap);
            Shader_SetInt(shader, "prefilterMap", 6);
            RenderCommand_ActiveTexture(7);
            RenderCommand_BindTexture2D(ibl.brdfLUTTexture);
            Shader_SetInt(shader, "brdfLUT", 7);

            // TODO: shadowmap
            Shader_SetBool(shader, "applyShadow", false);
            // TODO: SSAO
            Shader_SetBool(shader, "applySSAO", 0);
            // TODO: lights

            // color correct
            Shader_SetInt(shader, "colorCorrect", colorCorrect); // TODO: do properly when loading textures or something

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }
    }

    
    ImageBasedLighting_RenderSkybox(&ibl, view, projection, !colorCorrect);


    // bloom 
    //Bloom_Render(&bloom, framebuffer);
    if(colorCorrect)
        ColorCorrect_Render(framebuffer);
}


void EditorScene_OnViewportResize(u32 width, u32 height) {
    Bloom_OnResize(&bloom, width, height);
}