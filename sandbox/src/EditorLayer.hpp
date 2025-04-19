#pragma once 

#include <defines.hpp>

#include <Core/Layer.hpp>

void EditorLayer_OnAttach();
void EditorLayer_OnDetach();
void EditorLayer_OnUpdate();
// TODO: OnEvent

void EditorLayer_OnImGuiRender(ImGuiContext* context);