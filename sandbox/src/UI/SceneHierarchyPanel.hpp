#pragma once 

#include "defines.hpp"

typedef struct Scene Scene;
void SceneHierarchyPanel_Initialize(Scene* scene);
void SceneHierarchyPanel_OnImGuiRender();

//typedef struct GameObject GameObject;
//void SceneHierarchyPanel_SetSelectedGameObject(GameObject* gameObject); // TODO: for in viewport selection