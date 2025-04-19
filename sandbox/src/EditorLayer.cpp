#include "EditorLayer.hpp"

#include <Core/Log.hpp>

#include <iostream>
#include <glm/glm.hpp>

#include "UI/ContentBrowserPanel.hpp"
#include <imgui-docking/imgui.h>

glm::vec2 viewportSize = glm::vec2(1280, 720);

void EditorLayer_OnAttach() {
    std::string test = "Hello from EditorLayer";
    std::cout << "Hello from EditorLayer\n";

    ContentBrowserPanel_Initialize();
}

void EditorLayer_OnDetach() {

}

void EditorLayer_OnUpdate() {
    
}

void EditorLayer_OnImGuiRender(ImGuiContext* context) {
    ImGui::SetCurrentContext(context); // ImGui global does not persist across dll boundaries 

    return;
    
    static bool dockSpace = true;
    static bool opt_fullscreen_persistent = true;
    bool opt_fullscreen = opt_fullscreen_persistent;
    // to avoid two dockable targets 
    // use ImGuiDockNodeFlags_None to make the parent window not dockable into
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // Fullscreen setup 
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if(opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize 
            | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Dockspace", &dockSpace, window_flags);
    ImGui::PopStyleVar();

    if(opt_fullscreen)  
        ImGui::PopStyleVar(2);

    // Dockspace 
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            //  ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
    
            if (ImGui::MenuItem("New", "Ctrl+N")) std::cout << "New\n";
            if (ImGui::MenuItem("Open...", "Ctrl+O")) std::cout << "Open\n";
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) std::cout << "Save As\n";
    
            if (ImGui::MenuItem("Exit")) std::cout << "Exit\n";
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    ContentBrowserPanel_OnImGuiRender();

    // viewport 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    //ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    //ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    //ImVec2 viewportOffset = ImGui::GetWindowPos();
    
    u32 textureID = 0;
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)textureID, ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::PopStyleVar();

    ImGui::End(); 
    
}