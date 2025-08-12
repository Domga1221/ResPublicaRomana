#pragma once 

#include "defines.hpp"

#include <entt/entt.hpp>

typedef struct GameObject GameObject;
struct Scene {
    GameObject* root;
    entt::registry registry;
};

RPR_API GameObject* GameObject_Create(Scene* scene);
RPR_API void GameObject_Destroy(Scene* scene, GameObject* entity);

