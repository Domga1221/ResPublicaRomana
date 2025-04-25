#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>

#include <string>

struct TagComponent {
    std::string tag; // TODO: String implementation 
    const char* c_str() {
        return tag.c_str();
    }
};

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};