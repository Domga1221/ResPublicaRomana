#pragma once 

#include <Scene/GameObject.hpp>

#include <imgui-docking/imgui.h>
#include <imgui-docking/imgui_internal.h>

template <typename T, typename UIFunction>
void GUI_DrawComponent(const char* name, GameObject* gameObject, UIFunction uiFunction) {
    const ImGuiTreeNodeFlags treeNodeFlags = 
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
       | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if(T* component = gameObject->TryGetComponent<T>()) {
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name);
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if(ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) 
            ImGui::OpenPopup("ComponentSettings");
        
        bool removeComponent = false;
        if(ImGui::BeginPopup("ComponentSettings")) {
            if(ImGui::MenuItem("Remove Component"))
                removeComponent = true;
            ImGui::EndPopup();
        }

        if(open) {
            uiFunction(component);
            ImGui::TreePop();
        }
        if(removeComponent)
            gameObject->RemoveComponent<T>();
    }
}