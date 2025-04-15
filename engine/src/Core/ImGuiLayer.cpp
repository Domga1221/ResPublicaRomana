#include "ImGuiLayer.hpp"

#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Core/Log.hpp"


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
}

void ImGuiLayer_OnDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer_OnUpdate() {

}

void ImGuiLayer_OnImGuiRender() {
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

void ImGuiLayer_Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer_End() {
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