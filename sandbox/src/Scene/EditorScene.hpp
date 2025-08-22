#pragma once 

#include "defines.hpp"

#include "SceneCamera.hpp"
#include "Scene/Scene.hpp"
#include <Renderer/Framebuffer.hpp>

void EditorScene_Initialze();
void EditorScene_Shutdown(); // TODO: 

// TODO: Renderpacket/Renderproperties
void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera);
void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, 
    Framebuffer* framebuffer);

void EditorScene_OnViewportResize(u32 width, u32 height);