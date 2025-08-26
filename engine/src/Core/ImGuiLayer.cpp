#include "ImGuiLayer.hpp"

#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"

#include "imguizmo/ImGuizmo.h"

#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Core/Log.hpp"

void ImGuiTheme();


void ImGuiLayer_Initialize(Layer* layer) {
    layer->OnAttach = ImGuiLayer_OnAttach;
    layer->OnDetach = ImGuiLayer_OnDetach;
    layer->OnUpdate = ImGuiLayer_OnUpdate;
    layer->OnImGuiRender = ImGuiLayer_OnImGuiRender;
}

void ImGuiLayer_Shutdown(Layer* layer) {

}

void ImGuiLayer_OnAttach() {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // TODO: Font, style 

    GLFWwindow* window = Window_GetGLFWWindowPtr();
    if(!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        RPR_CRITICAL("Failed to initialize ImGui for OpenGL");
    }

    if(!ImGui_ImplOpenGL3_Init("#version 460")) {
        RPR_CRITICAL("Failed to initialize ImGui for OpenGl3");
    }

    ImGuiTheme();
}

void ImGuiLayer_OnDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer_OnUpdate(f32 deltaTime) {

}

void ImGuiLayer_OnImGuiRender(ImGuiContext* context) {
    ImGui::SetCurrentContext(context);
    //RPR_WARN("ImGuiLayer_OnImGuiRender called");
    //static bool show = true;
    //ImGui::ShowDemoWindow(&show);
    //ImGui::Begin("My name is window, ImGui window");
    //ImGui::Text("Hello there adventurer");
    //ImGui::End();
}

ImGuiContext* ImGuiLayer_Begin() {
    //RPR_WARN("ImGuiLayer_Begin called");
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGuiContext* context = ImGui::GetCurrentContext();
    return context;
}

void ImGuiLayer_End() {
    //RPR_WARN("ImGuiLayer_End called");
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1280, 720); // TODO: Make dynamic 
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if(io.ConfigFlags &  ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiTheme() {

    // Fonts
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontFromFileTTF("Assets/Fonts/CastoroTitling/CastoroTitling-Regular.ttf", 24.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/CastoroTitling/CastoroTitling-Regular.ttf", 18.0f);


    ImGuiStyle& style = ImGui::GetStyle();
    // Borders
    style.TabBarBorderSize = 0;
    style.WindowBorderSize = 0;
    style.ChildBorderSize = 0;
    style.PopupBorderSize = 0;
    style.FrameBorderSize = 0;
    // Tabs
    style.TabBorderSize = 0;
    style.TabBarBorderSize = 0;
    style.TabCloseButtonMinWidthSelected = -1;
    style.TabCloseButtonMinWidthUnselected = 0;
    style.TabRounding = 0;
    // Windows 
    style.WindowMenuButtonPosition = ImGuiDir_None; //Disable this to get back Arrow on Tabbar
    


    auto& colors = ImGui::GetStyle().Colors;
    
    colors[ImGuiCol_WindowBg] = ImVec4{ 15.0f / 255.0f, 17.0f / 255.0f, 16.0f / 255.0f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 125.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.3, 0.3f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 196.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f, 1.0f };

    // Tabs
    //colors[ImGuiCol_Tab] = ImVec4{ 125.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f };
    colors[ImGuiCol_Tab] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };      
    colors[ImGuiCol_TabActive] = ImVec4{ 196.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f, 1.0f };
    colors[ImGuiCol_TabSelectedOverline] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    
    // Menubar
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 177.0f / 255.0f, 160.0f / 255.0f, 140.0f / 255.0f, 1.0f };
    //colors[ImGuiCol_MenuBarBg] = ImVec4{ 222.0f / 255.0f, 135.0f / 255.0f, 53.0f / 255.0f, 1.0f };

    // Button
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };

    // Seperator 
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 125.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 196.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f, 1.0f };

    // TODO: 
    /*
    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
    */
}