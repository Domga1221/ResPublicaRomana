#include "Scene.hpp"

#include "GameObject.hpp"

#include "Components.hpp"

RPR_API GameObject* GameObject_Create(Scene* scene) {
    GameObject* gameObject = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_RENDERER);
    gameObject->handle = scene->registry.create();
    gameObject->scene = scene;
    gameObject->AddComponent<TransformComponent>();
    TagComponent& tagComponent = gameObject->AddComponent<TagComponent>();
    tagComponent.tag = "New Entity";
    return gameObject;
}

RPR_API void GameObject_Destroy(Scene* scene, GameObject* gameObject) {
    scene->registry.destroy(gameObject->handle);
    gameObject->scene = nullptr;
    gameObject->handle = entt::null;
}
