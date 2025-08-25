#include "EditorRenderpass.hpp"

#include "Scene/Components.hpp"

#include "Renderer/RenderCommand.hpp"
#include "Renderer/Shader.hpp"

void EditorRenderpass_Initialize(Renderpass* renderpass) {

}

void EditorRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    Shader* editorShader = renderProperties->editorShader;

    glm::mat4* view = renderProperties->view;
    glm::mat4* projectionRH = renderProperties->projection;
    

    auto group = renderProperties->registry->group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);
            
        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

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

void EditorRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height) {

}

void EditorRenderpass_Shutdown(Renderpass* renderpass) {

}