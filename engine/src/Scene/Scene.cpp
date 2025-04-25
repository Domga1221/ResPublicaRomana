#include "Scene.hpp"

#include "GameObject.hpp"

#include "Components.hpp"

RPR_API void GameObject_Create(Scene* scene, GameObject* gameObject) {
    gameObject->handle = scene->registry.create();
    gameObject->scene = scene;
    gameObject->AddComponent<TransformComponent>();
    TagComponent& tagComponent = gameObject->AddComponent<TagComponent>();
    tagComponent.tag = "New Entity";
}

RPR_API void GameObject_Destroy(Scene* scene, GameObject* gameObject) {
    scene->registry.destroy(gameObject->handle);
    gameObject->scene = nullptr;
    gameObject->handle = entt::null;
}
