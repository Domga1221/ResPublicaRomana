#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>

typedef struct PointLight {
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
} PointLight;

RPR_API glm::vec3 PointLight_GetIntensifiedColor(PointLight* pointLight);