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

void GUI_DrawVec3Control(const std::string& label, glm::vec3& values, 
        float resetValue = 0.0f, float columnWidth = 100.0f) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight - 3.0f };


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}