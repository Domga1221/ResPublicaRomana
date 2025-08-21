#include "EditorScene.hpp"

#include <Scene/Components.hpp>

#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>

#include <Renderer/PBR/ImageBasedLighting.hpp>
ImageBasedLighting ibl;
#include <Renderer/Primitives.hpp>
#include <glad/glad.h> // TODO: remove

#include <filesystem>

static Shader editorShader;
void EditorScene_Initialze() {
    std::string currentPath = std::filesystem::current_path().string();

    std::string vertPath = currentPath + "/Assets/Shaders/Editor/EditorShader.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/Editor/EditorShader.frag";
    Shader_Create(&editorShader, vertPath, fragPath);

    std::string hdrPath = std::string("Assets/HDR/newport_loft.hdr");
    ImageBasedLighting_Initialize(&ibl, hdrPath.c_str());
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
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

        Shader_Bind(&editorShader);
        Shader_SetMat4(&editorShader, "model", model);
        Shader_SetMat4(&editorShader, "view", view);
        Shader_SetMat4(&editorShader, "projection", projection);
        RenderCommand_ActiveTexture(0);
        Texture* albedo = materialComponent.material.textures.data[0];
        if(albedo != nullptr)
            Texture_Bind(albedo);
        Shader_SetInt(&editorShader, "texture_0", 0);

        Mesh_Bind(&meshComponent.mesh);
        RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
    }
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    Shader_Bind(&editorShader);
    RenderCommand_BindTexture2D(ibl.hdrTexture);
    Shader_SetMat4(&editorShader, "model", model);
    Primitives_RenderQuad();
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f));
    Shader_SetMat4(&editorShader, "model", model);
    Primitives_RenderCube();

    // ibl
    ImageBasedLighting_RenderSkybox(&ibl, view, projection, true);
}