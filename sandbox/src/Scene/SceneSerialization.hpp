#pragma once 

#include "Scene/Scene.hpp"

void Scene_Serialize(Scene* scene, const std::string& filepath); // TODO: String
void Scene_Deserialize(Scene* scene, const std::string& filepath);