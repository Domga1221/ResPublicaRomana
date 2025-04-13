#include "EditorLayer.hpp"

#include <Core/Log.hpp>

#include <iostream>

void EditorLayer_OnAttach() {

}

void EditorLayer_OnDetach() {

}

void EditorLayer_OnUpdate() {
    std::string test = "Hello from EditorLayer";
    std::cout << "Hello from EditorLayer\n";
}

void EditorLayer_OnImGuiRender() {

}