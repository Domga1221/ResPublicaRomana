#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>

enum SceneCameraMovement {
	SCENE_CAMERA_MOVEMENT_FORWARD,
	SCENE_CAMERA_MOVEMENT_BACKWARD,
	SCENE_CAMERA_MOVEMENT_LEFT, 
	SCENE_CAMERA_MOVEMENT_RIGHT, 
	SCENE_CAMERA_MOVEMENT_UP,
	SCENE_CAMERA_MOVEMENT_DOWN
};


// constants 
const f32 SCENE_CAMERA_CONSTANT_YAW = -90.0f;
const f32 SCENE_CAMERA_CONSTANT_PITCH = 0.0f;
const f32 SCENE_CAMERA_CONSTANT_SPEED = 2.5f;
const f32 SCENE_CAMERA_CONSTANT_SENSITIVITY = 0.1f;
const f32 SCENE_CAMERA_CONSTANT_ZOOM = 45.0f;
const f32 SCENE_CAMERA_CONSTANT_START_VIEWPORT_WIDTH = 1280;
const f32 SCENE_CAMERA_CONSTANT_START_VIEWPORT_HEIGHT = 720;
const f32 SCENE_CAMERA_CONSTANT_NEARCLIP = 0.1f;
const f32 SCENE_CAMERA_CONSTANT_FARCLIP = 100.0f;


struct SceneCamera {
    glm::vec3 position;
    glm::vec3 front; glm::vec3 up; glm::vec3 right;
    glm::vec3 worldUp;
    //  rotation
    f32 yaw;
    f32 pitch;
    // controls 
    f32 movementSpeed;
    f32 mouseSensitivity;
    f32 zoom;
    // framebuffer / viewport 
    f32 viewportWidth; f32 viewportHeight;
    f32 aspectRatio;
    f32 nearClip; f32 farClip;
};

void SceneCamera_Create(SceneCamera* sceneCamera, 
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
    float yaw = SCENE_CAMERA_CONSTANT_YAW, float pitch = SCENE_CAMERA_CONSTANT_PITCH);

glm::mat4 SceneCamera_GetViewMatrix(SceneCamera* sceneCamera);
glm::mat4 SceneCamera_GetProjectionMatrix(SceneCamera* sceneCamera);
glm::mat4 SceneCamera_GetProjectionMatrixRH(SceneCamera* sceneCamera);

void SceneCamera_ProcessKeyboard(SceneCamera* sceneCamera, SceneCameraMovement direction, f32 deltaTime);
void SceneCamera_ProcessMouseMovement(SceneCamera* sceneCamera, f32 xOffset, f32 yOffset, b8 constrainPitch = true);
void SceneCamera_ProcessMouseScroll(SceneCamera* sceneCamera, float yOffset);

void SceneCamera_SetViewportSize(SceneCamera* sceneCamera, u32 width, u32 height);