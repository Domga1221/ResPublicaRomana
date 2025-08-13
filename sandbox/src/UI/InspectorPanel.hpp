#pragma once 

#include "defines.hpp"

typedef struct Scene Scene;
typedef struct Shader Shader; // TODO: remove 
void InspectorPanel_Initialize(Shader* shader);
void InspectorPanel_OnImGuiRender();

typedef struct GameObject GameObject;
void InspectorPanel_SetSelectedGameObject(GameObject* gameObject);