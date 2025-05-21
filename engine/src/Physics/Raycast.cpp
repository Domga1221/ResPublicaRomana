#include "Raycast.hpp"

#include "Core/Log.hpp"

b8 Physics_RayPlaneIntersection(Ray ray, Plane plane, glm::vec3* intersectionPoint) {
    float denom = glm::dot(plane.normal, ray.direction);

    if(glm::abs(denom) > 0.0001f) {
        float t = glm::dot((plane.origin - ray.origin), plane.normal) / denom;
        if(t >= 0) {
            glm::vec3 hit = ray.origin + ray.direction * t;
            RPR_WARN("Physics_RayPlaneIntersection HIT: %f, %f, %f", hit.x, hit.y, hit.z);
            
            *intersectionPoint = hit;
            return true;
        }
    }

    return false;
}