#include "ContentBrowserPanel.hpp"

#include <imgui-docking/imgui.h>
#include <iostream>

#include <filesystem>

static std::filesystem::path currentDirectory = "Assets";
static const std::filesystem::path assetsPath = "Assets";

void ContentBrowserPanel_Initialize() {
    currentDirectory = assetsPath;
}


void ContentBrowserPanel_OnImGuiRender() {
    //std::cout << "ContentBrowserPanel_OnImGuiRender called \n";
    //std::cout << ImGui::GetCurrentContext() << "\n";
    ImGui::Begin("Content Browser");

    //static b32 test = false;
    //ImGui::Checkbox("Test", (bool*)&test);

    if(currentDirectory != std::filesystem::path(assetsPath)) {
        if(ImGui::Button("<-")) 
            currentDirectory = currentDirectory.parent_path();
    }

    ImGui::Text("test");

    // IMGUI_DEMO_MARKER("Layout/Basic Horizontal Layout/Manual wrapping");
    static f32 buttonSize = 140;
    ImGui::SliderFloat("Box size", &buttonSize, 40, 400, "%.0f");
    ImGuiStyle& style = ImGui::GetStyle();
    
    /*
    int buttonsCount = 20;
    float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
    ImVec2 button_sz = ImVec2(buttonSize, buttonSize);
    for(int n = 0; n < buttonsCount; n++) {
        ImGui::PushID(n);
        ImGui::Button("Box", button_sz);
        f32 last_button_x2 = ImGui::GetItemRectMax().x;
        // expected position if button was on same line
        f32 next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x;  
        if(n + 1 < buttonsCount && next_button_x2 < window_visible_x2) {
            ImGui::SameLine();
        }
        ImGui::PopID();
    }
    */

    float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
    ImVec2 button_sz = ImVec2(buttonSize, buttonSize);
    // list folders and files 
    int buttonsCount = 0;
    for([[maybe_unused]]const std::filesystem::directory_entry& a : std::filesystem::directory_iterator(currentDirectory))
        buttonsCount++;

    int n = 0;
    for(const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(currentDirectory)) {
        const std::filesystem::path& path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        ImGui::PushID(filenameString.c_str());
        ImGui::Button(filenameString.c_str(), button_sz);
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            if(directoryEntry.is_directory())  
                currentDirectory.append(path.filename().string());

        f32 last_button_x2 = ImGui::GetItemRectMax().x;
        // expected position if button was on same line
        f32 next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x;  
        if(n + 1 < buttonsCount && next_button_x2 < window_visible_x2) {
            ImGui::SameLine();
        }


        ImGui::PopID();

        n++;
    }



    static bool show = true;
    ImGui::ShowDemoWindow(&show);
    
    ImGui::Columns(4, nullptr);
    ImGui::Separator();

    ImGui::End();
}