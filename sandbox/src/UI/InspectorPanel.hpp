#pragma once 

#include "defines.hpp"

void InspectorPanel_Initialize();
void InspectorPanel_Shutdown();
void InspectorPanel_OnImGuiRender();

typedef struct GameObject GameObject;
void InspectorPanel_SetSelectedGameObject(GameObject* gameObject);