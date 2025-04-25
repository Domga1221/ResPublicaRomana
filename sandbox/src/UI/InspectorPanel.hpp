#pragma once 

#include "defines.hpp"

typedef struct Scene Scene;
void InspectorPanel_Initialize();
void InspectorPanel_OnImGuiRender();

typedef struct GameObject GameObject;
void InspectorPanel_SetSelectedGameObject(GameObject* gameObject);