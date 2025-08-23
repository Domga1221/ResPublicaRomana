#include "PointLight.hpp"

glm::vec3 PointLight_GetIntensifiedColor(PointLight* pointLight) {
    return pointLight->color * pointLight->intensity;
}