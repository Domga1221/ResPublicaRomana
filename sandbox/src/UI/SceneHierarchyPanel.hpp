#pragma once 

#include "defines.hpp"

#include <Scene/GameObject.hpp>

typedef struct Scene Scene;
void SceneHierarchyPanel_Initialize(Scene* scene);
void SceneHierarchyPanel_OnImGuiRender();

GameObject* SceneHierarchyPanel_GetSelectedGameObject(); // TODO: who should own selection?

typedef struct GameObject GameObject;
void SceneHierarchyPanel_SetSelectedGameObject(GameObject* gameObject); 