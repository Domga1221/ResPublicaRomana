#include "EditorScene.hpp"

#include <Scene/Components.hpp>

#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>

#include <filesystem>

static Shader editorShader;
void EditorScene_Initialze() {
    std::string currentPath = std::filesystem::current_path().string();

    std::string vertPath = currentPath + "/Assets/Shaders/Editor/EditorShader.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/Editor/EditorShader.frag";
    Shader_Create(&editorShader, vertPath, fragPath);
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
    auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        //RPR_INFO("(%d, %d, %d)", transformComponent.position.x, transformComponent.position.y, transformComponent.position.z);

        glm::mat4 model = transformComponent.GetTransform();
        glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
        glm::mat4 projection = SceneCamera_GetProjectinoMatrix(sceneCamera);

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
}