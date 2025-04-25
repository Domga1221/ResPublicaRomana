#pragma once 

#include "defines.hpp"

#include "Scene.hpp"

#include "Core/Log.hpp"

#include "Memory/List.hpp"

struct RPR_API GameObject {
    entt::entity handle;
    Scene* scene;
    
    GameObject* parent;
    List<GameObject*> children;

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        if(HasComponent<T>()) {
            RPR_ERROR("GameObject with id: %d already has component!", handle);
        }
        T& component = scene->registry.emplace<T>(handle, std::forward<Args>(args)...);
        return component;
    }

    template<typename T> 
    T& GetComponent() {
        if(!HasComponent<T>()) {
            RPR_ERROR("GameObject with id: %d does not have component!", handle);
        }   // TODO: Crashes when entity doesn't have the compnent, but can't return null
        return scene->registry.get<T>(handle);
    }

    // TODO: TryGetComponent

    template<typename T>
    b8 HasComponent() {
        return scene->registry.all_of<T>(handle);
    }

    template<typename T>
    void RemoveComponent() {
        if(!HasComponent<T>()) {
            RPR_ERROR("GameObject with id: %d does not have component!", handle);
            return;
        }
        scene->registry.remove<T>(handle);
    }
};