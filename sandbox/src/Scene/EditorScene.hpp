#pragma once 

#include "defines.hpp"

#include "SceneCamera.hpp"
#include "Scene/Scene.hpp"

void EditorScene_Initialze();

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera);