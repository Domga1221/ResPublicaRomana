#pragma once 

#include "Layer.hpp"

void ImGuiLayer_Initialize(Layer* layer);
void ImGuiLayer_Shutdown(Layer* layer);

void ImGuiLayer_OnAttach();
void ImGuiLayer_OnDetach();
void ImGuiLayer_OnUpdate();
void ImGuiLayer_OnImGuiRender();

void ImGuiLayer_Begin();
void ImGuiLayer_End();