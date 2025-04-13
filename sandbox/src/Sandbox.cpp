#include <Core/Entrypoint.hpp>

#include <Core/Layer.hpp>
#include <Memory/Memory.hpp>

#include "EditorLayer.hpp"

Application* CreateApplication() {
    return new Application();
}

void AfterAppInitialization(Application* app) {
    Layer* layer = (Layer*)MEMORY_Allocate(sizeof(Layer), MEMORY_TAG_LAYER);
    layer->OnAttach = EditorLayer_OnAttach;
    layer->OnDetach = EditorLayer_OnDetach;
    layer->OnUpdate = EditorLayer_OnUpdate;
    layer->OnImGuiRender = EditorLayer_OnImGuiRender;

    Application_PushLayer(layer);
}