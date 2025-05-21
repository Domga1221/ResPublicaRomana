#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Plane {
    glm::vec3 origin;
    glm::vec3 normal;
};

RPR_API b8 Physics_RayPlaneIntersection(Ray ray, Plane plane, glm::vec3* intersectionPoint);