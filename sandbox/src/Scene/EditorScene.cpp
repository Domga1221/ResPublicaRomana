#include "EditorScene.hpp"

#include <Scene/Components.hpp>

#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>

#include <filesystem>

static Shader editorShader;
void EditorScene_Initialze() {
    std::string currentPath = std::filesystem::current_path().string();

    std::string vertPath = currentPath + "/Assets/Shaders/triangle3D.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/triangle3D.frag";
    Shader_Create(&editorShader, vertPath, fragPath);
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
    auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&> tuple = group.get<TransformComponent, MeshComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);

        //RPR_INFO("(%d, %d, %d)", transformComponent.position.x, transformComponent.position.y, transformComponent.position.z);

        glm::mat4 model = transformComponent.GetTransform();
        glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
        glm::mat4 projection = SceneCamera_GetProjectinoMatrix(sceneCamera);

        Shader_Bind(&editorShader);
        Shader_SetMat4(&editorShader, "model", model);
        Shader_SetMat4(&editorShader, "view", view);
        Shader_SetMat4(&editorShader, "projection", projection);

        Mesh_Bind(&meshComponent.mesh);
        RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
    }
}