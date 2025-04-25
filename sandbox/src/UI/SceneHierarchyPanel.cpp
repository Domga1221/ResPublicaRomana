#include "SceneHierarchyPanel.hpp"

#include <Scene/Scene.hpp>
#include <Scene/GameObject.hpp>
#include <Scene/Components.hpp>

#include <imgui-docking/imgui.h>


static Scene* activeScene = 0;
static GameObject* selection;

void drawComponents(GameObject* gameObject);

void SceneHierarchyPanel_Initialize(Scene* scene) {
    activeScene = scene;
}

void SceneHierarchyPanel_OnImGuiRender() {
    ImGui::Begin("Scene Hierarchy");

    //RPR_INFO("--- SceneHierarchyPanel_OnImGuiRender --- ");
    if(activeScene->root == nullptr) {
        RPR_ERROR("SceneHierarchyPanel_OnImGuiRender root node is null");
        return;
    }
    //ImGui::Text("Root");
    

    ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;
    base_flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;
    base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    base_flags |= ImGuiTreeNodeFlags_SpanFullWidth;
    if(ImGui::TreeNodeEx("Root", base_flags)) {
        for(u32 i = 0; i < activeScene->root->children.size; i++) {
            GameObject* current = activeScene->root->children.data[i];
            TagComponent& tc = current->GetComponent<TagComponent>();
            //RPR_INFO("%s", tc.c_str());
            ImGui::TreeNodeEx((const void*)current->handle, base_flags, "%s", tc.c_str());
            if(ImGui::IsItemClicked())
                selection = current;

            ImGui::TreePop();
        }
        ImGui::TreePop();
    }


    if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        selection = nullptr;

        
        
    ImGui::End();
    

    ImGui::Begin("Inspector");
    if(selection != nullptr)
        drawComponents(selection);

    ImGui::End();
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
}