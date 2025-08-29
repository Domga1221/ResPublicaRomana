#pragma once 

#include "defines.hpp"

#include "SceneCamera.hpp"
#include "Scene/Scene.hpp"
#include <Renderer/Framebuffer.hpp>

void EditorScene_Initialze();
void EditorScene_Shutdown(); // TODO: 

// TODO: Renderpacket/Renderproperties
void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, 
    Framebuffer* framebuffer, GameObject* selectedGameObject);
void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, 
    Framebuffer* framebuffer, b8 bloomEnabled, b8 ssaoEnabled, b8 colorCorrectEnabled, b8 playMode);

void EditorScene_OnViewportResize(u32 width, u32 height);