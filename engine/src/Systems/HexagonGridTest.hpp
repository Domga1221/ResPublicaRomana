#pragma once 

#include "defines.hpp"

#include "Core/Log.hpp"
#include "Core/Input.hpp"
#include "Scene/SceneCamera.hpp"
#include "Physics/Raycast.hpp"
#include "Systems/Hex.hpp"

#include <glm/glm.hpp>

// NOTE: moved here from EditorLayer as backup, if needed
RPR_API void Hexagon_Raycast(glm::vec2 viewportMousePosition, glm::vec2 viewportSize, SceneCamera& sceneCamera) {
    static b8 pressed = false;
    if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_1) && pressed == false) {
        
        RPR_WARN("%f, %f", viewportMousePosition.x, viewportMousePosition.y);
        pressed = true;

        // to ndc
        // x[-1, 1] y[-1, 1] z[-1, 1]
        float x = (2.0f * viewportMousePosition.x) / viewportSize.x - 1.0f;
        float y = (2.0f * viewportMousePosition.y) / viewportSize.y - 1.0f;
        float z = 1.0f;
        RPR_WARN("%f, %f, %f", x, y, z);

        // homogenous clip coordinates
        glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);

        // Camera coordinates, Eye / view space 
        glm::vec4 ray_eye = glm::inverse(SceneCamera_GetProjectionMatrixRH(&sceneCamera)) * ray_clip;
        ray_eye.z = 1.0f;
        ray_eye.w = 0.0f;

        // World coordinates
        glm::vec3 ray_world = glm::inverse(SceneCamera_GetViewMatrix(&sceneCamera)) * ray_eye;
        ray_world = glm::normalize(ray_world);

        RPR_WARN("%f, %f, %f", ray_world.x, ray_world.y, ray_world.z);


        // ray plane intersect
        glm::vec3 plane_normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 plane_offset = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 ray_origin = sceneCamera.position;
        glm::vec3 ray_direction = ray_world;

        Ray ray;
        ray.origin = ray_origin;
        ray.direction = ray_direction;
        Plane plane;
        plane.origin = plane_offset;
        plane.normal = plane_normal;

        glm::vec3 hitPoint;
        bool hit = Physics_RayPlaneIntersection(ray, plane, &hitPoint);
        if(hit) {
            RPR_WARN("HIT: %f, %f, %f", hitPoint.x, hitPoint.y, hitPoint.z);
            glm::vec3 axialCoordinates = Hex_PointToHex(glm::vec2(hitPoint.x, hitPoint.z), 1.0f);
            glm::ivec3 intCubeCoordinates = Hex_FractionalCubeToIntCube(axialCoordinates);
            RPR_WARN("HEX: %d, %d, %d", intCubeCoordinates.x, intCubeCoordinates.y, intCubeCoordinates.z);
        }
        

        
        float denom = glm::dot(plane_normal, ray_direction);
        if(glm::abs(denom) > 0.0001f) {
            float t = glm::dot((plane_offset - ray_origin), plane_normal) / denom;
            if(t >= 0) {
                glm::vec3 hit = ray_origin + ray_direction * t;
                RPR_WARN("HIT: %f, %f, %f", hit.x, hit.y, hit.z);
                glm::vec3 axialCoordinates = Hex_PointToHex(glm::vec2(hit.x, hit.z), 1.0f);
                glm::ivec3 intCubeCoordinates = Hex_FractionalCubeToIntCube(axialCoordinates);
                RPR_WARN("HEX: %d, %d, %d", intCubeCoordinates.x, intCubeCoordinates.y, intCubeCoordinates.z);
            }
        }
        
    }
}