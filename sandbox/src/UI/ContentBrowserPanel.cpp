#include "ContentBrowserPanel.hpp"

#include <imgui-docking/imgui.h>
#include <iostream>

void ContentBrowserPanel_Initialize() {

}

void ContentBrowserPanel_OnImGuiRender() {
    //std::cout << "ContentBrowserPanel_OnImGuiRender called \n";
    //std::cout << ImGui::GetCurrentContext() << "\n";
    ImGui::Begin("Content Browser");

    //static b32 test = false;
    //ImGui::Checkbox("Test", (bool*)&test);
    ImGui::Text("test");

    ImGui::End();
}