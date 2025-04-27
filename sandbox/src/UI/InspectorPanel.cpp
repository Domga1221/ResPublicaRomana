#include "InspectorPanel.hpp"

#include <Scene/GameObject.hpp>
#include <Scene/Components.hpp>

#include <imgui-docking/imgui.h>

#include "ImGuiPayload.hpp"

#include <filesystem>

static GameObject* selected;

void drawComponents(GameObject* gameObject);

void InspectorPanel_Initialize() {

}

void InspectorPanel_OnImGuiRender() {
    ImGui::Begin("Inspector");
    if(selected != nullptr)
        drawComponents(selected);
    
    ImGui::End();
}


void InspectorPanel_SetSelectedGameObject(GameObject* gameObject) {
    selected = gameObject;
}

void drawComponents(GameObject* gameObject) {
    if(gameObject->HasComponent<TagComponent>()) {
        TagComponent& tagComponent = gameObject->GetComponent<TagComponent>();
        char buffer[256];
        MEMORY_Zero(&buffer, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tagComponent.c_str());
        if(ImGui::InputText("Tag", buffer, sizeof(buffer))) {
            tagComponent.tag = std::string(buffer);
        }
    }


    if(TransformComponent* transformComponent = gameObject->TryGetComponent<TransformComponent>()) {
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        

        f32 columnWidth = 120.0f;
        // Position
        const std::string label1 = "position";
        glm::vec3* position = &transformComponent->position;
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label1.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##position", ImVec2(20.0f, 20.0f))) position->x = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Xp", &position->x, 0.1f);

            if(ImGui::Button("Y##position", ImVec2(20.0f, 20.0f))) position->y = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Yp", &position->y, 0.1f);

            if(ImGui::Button("Z##position", ImVec2(20.0f, 20.0f))) position->z = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Zp", &position->z, 0.1f);
        
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Rotation
        const std::string label2 = "rotation";
        glm::vec3 rotation = glm::degrees(transformComponent->rotation);
        ImGui::NextColumn();
            ImGui::Text("%s", label2.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##rotation", ImVec2(20.0f, 20.0f))) rotation.x = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Xr", &rotation.x, 0.1f);

            if(ImGui::Button("Y##rotation", ImVec2(20.0f, 20.0f))) rotation.y = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Yr", &rotation.y, 0.1f);

            if(ImGui::Button("Z##rotation", ImVec2(20.0f, 20.0f))) rotation.z = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Zr", &rotation.z, 0.1f);
        transformComponent->rotation = glm::radians(rotation);


        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Scale
        const std::string label3 = "scale";
        glm::vec3* scale = &transformComponent->scale;
        ImGui::NextColumn();
            ImGui::Text("%s", label3.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##scale", ImVec2(20.0f, 20.0f))) scale->x = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Xs", &scale->x, 0.1f);

            if(ImGui::Button("Y##scale", ImVec2(20.0f, 20.0f))) scale->y = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Ys", &scale->y, 0.1f);

            if(ImGui::Button("Z##scale", ImVec2(20.0f, 20.0f))) scale->z = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Zs", &scale->z, 0.1f);

        //ImGui::Text("%f", transformComponent->position.x);
        //ImGui::Text("%f", transformComponent->position.y);
        //ImGui::Text("%f", transformComponent->position.z);

        ImGui::Columns(1);
        ImGui::PopStyleVar();
    }

    if(MeshComponent* meshComponent = gameObject->TryGetComponent<MeshComponent>()) {
        ImGui::Text("Mesh");
        ImGui::Button(meshComponent->mesh.relativePath.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 20.0f));
        // TODO: drag and drop 
        if(ImGui::BeginDragDropTarget()) { // TODO: Think about relative and absolute paths
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_CONTENT_BROWSER_ITEM)) {
                const char* path = (const char*)payload->Data;
                std::filesystem::path current = std::filesystem::current_path();
                std::filesystem::path modelPath = std::filesystem::path(path);
                current /= modelPath;
                std::string s = current.string(); 
                Mesh_Destroy(&meshComponent->mesh);
                Mesh_Create(&meshComponent->mesh, s);
                if(!meshComponent->mesh.isLoaded) 
                    RPR_ERROR("Failed to load Mesh");
            }
        }
    }
}