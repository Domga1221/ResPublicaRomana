#include "Scene.hpp"

#include "GameObject.hpp"

#include "Components.hpp"

#include <Core/ClientLog.hpp>


RPR_API GameObject* Scene_Create(Scene* scene) { 
    GameObject* root = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    root->handle = scene->registry.create();
    root->scene = scene;
    TagComponent& tagComponent = root->AddComponent<TagComponent>();
    tagComponent.tag = "Root";
    List_Create(&root->children);
    
    scene->root = root;
    return root;
}

RPR_API void Scene_Destroy(Scene* scene) {
    RPR_CLIENT_INFO("Scene_Destroy");
    for(u32 i = 0; i < scene->root->children.size; i++) {
        GameObject* g = scene->root->children.data[i];
        RPR_CLIENT_INFO("Destroying GameObject: %u", g->handle);
        GameObject_Destroy(scene, g);
    }
    List_Destroy(&scene->root->children);
    MEMORY_Free(scene->root, sizeof(GameObject), MEMORY_TAG_ENTITY);
    scene->registry.clear();
    scene->root = nullptr;
}

RPR_API GameObject* GameObject_Create(Scene* scene) {
    GameObject* gameObject = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    gameObject->handle = scene->registry.create();
    gameObject->scene = scene;
    gameObject->AddComponent<TransformComponent>();
    TagComponent& tagComponent = gameObject->AddComponent<TagComponent>();
    tagComponent.tag = "New Entity";
    List_Create(&gameObject->children);
    List_PushBack(&scene->root->children, gameObject);
    return gameObject;
}

RPR_API GameObject* GameObject_Create(Scene* scene, const char* tag) {
    GameObject* gameObject = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    gameObject->handle = scene->registry.create();
    gameObject->scene = scene;
    gameObject->AddComponent<TransformComponent>();
    TagComponent& tagComponent = gameObject->AddComponent<TagComponent>();
    tagComponent.tag = tag;
    List_Create(&gameObject->children);
    List_PushBack(&scene->root->children, gameObject);
    return gameObject;
}

RPR_API void GameObject_Destroy(Scene* scene, GameObject* gameObject) {
    scene->registry.destroy(gameObject->handle);
    RPR_CLIENT_WARN("address of scene: %u", scene);

    List_Remove(&scene->root->children, gameObject);
    List_Destroy(&gameObject->children);
    gameObject->scene = nullptr;
    gameObject->handle = entt::null;
    
    MEMORY_Free(gameObject, sizeof(GameObject), MEMORY_TAG_ENTITY);
}
