#include "EditorLayer.hpp"

#include <Core/Log.hpp>

#include <iostream>

void EditorLayer_OnAttach() {
    std::string test = "Hello from EditorLayer";
    std::cout << "Hello from EditorLayer\n";
}

void EditorLayer_OnDetach() {

}

void EditorLayer_OnUpdate() {
    
}

void EditorLayer_OnImGuiRender() {

}