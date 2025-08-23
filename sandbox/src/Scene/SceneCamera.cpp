#include "SceneCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void updateCameraVectors(SceneCamera* sceneCamera);

void SceneCamera_Create(SceneCamera* sceneCamera, 
        glm::vec3 position, 
        glm::vec3 up, 
        float yaw, float pitch) {
    sceneCamera->position = position;
    sceneCamera->worldUp = up;
    sceneCamera->yaw = yaw;
    sceneCamera->pitch = pitch;

    sceneCamera->front = glm::vec3(0.0f, 0.0f, -1.0f);
    
    sceneCamera->movementSpeed = SCENE_CAMERA_CONSTANT_SPEED;
    sceneCamera->mouseSensitivity = SCENE_CAMERA_CONSTANT_SENSITIVITY;
    sceneCamera->zoom = SCENE_CAMERA_CONSTANT_ZOOM;
    

    sceneCamera->viewportWidth = SCENE_CAMERA_CONSTANT_START_VIEWPORT_WIDTH;
    sceneCamera->viewportHeight = SCENE_CAMERA_CONSTANT_START_VIEWPORT_HEIGHT;
    sceneCamera->zoom = SCENE_CAMERA_CONSTANT_ZOOM;
    sceneCamera->nearClip = SCENE_CAMERA_CONSTANT_NEARCLIP;
    sceneCamera->farClip = SCENE_CAMERA_CONSTANT_FARCLIP;
    sceneCamera->aspectRatio = sceneCamera->viewportWidth / sceneCamera->viewportHeight;

    updateCameraVectors(sceneCamera);
}

glm::mat4 SceneCamera_GetViewMatrix(SceneCamera* sceneCamera) {
    return glm::lookAt(sceneCamera->position, 
        sceneCamera->position + sceneCamera->front, sceneCamera->up);
}

glm::mat4 SceneCamera_GetProjectionMatrix(SceneCamera* sceneCamera) {
    return glm::perspectiveLH(glm::radians(sceneCamera->zoom), sceneCamera->aspectRatio, 
        sceneCamera->nearClip, sceneCamera->farClip);
}

glm::mat4 SceneCamera_GetProjectionMatrixRH(SceneCamera* sceneCamera) {
    return glm::perspectiveRH(glm::radians(sceneCamera->zoom), sceneCamera->aspectRatio, 
    sceneCamera->nearClip, sceneCamera->farClip);
}


void SceneCamera_ProcessKeyboard(SceneCamera* sceneCamera, SceneCameraMovement direction, f32 deltaTime) {
    f32 velocity = sceneCamera->movementSpeed * deltaTime;
    if(direction == SCENE_CAMERA_MOVEMENT_FORWARD) sceneCamera->position += sceneCamera->front * velocity;
    if(direction == SCENE_CAMERA_MOVEMENT_BACKWARD) sceneCamera->position -= sceneCamera->front * velocity;
    if(direction == SCENE_CAMERA_MOVEMENT_RIGHT) sceneCamera->position += sceneCamera->right * velocity;
    if(direction == SCENE_CAMERA_MOVEMENT_LEFT) sceneCamera->position -= sceneCamera->right * velocity;
    if(direction == SCENE_CAMERA_MOVEMENT_UP) sceneCamera->position += sceneCamera->up * velocity;
    if(direction == SCENE_CAMERA_MOVEMENT_DOWN) sceneCamera->position -= sceneCamera->up * velocity;    
}

void SceneCamera_ProcessMouseMovement(SceneCamera* sceneCamera, f32 xOffset, f32 yOffset, b8 constrainPitch) {
    xOffset *= sceneCamera->mouseSensitivity;
    yOffset *= sceneCamera->mouseSensitivity;

    sceneCamera->yaw -= xOffset;
    sceneCamera->pitch -= yOffset;

    if(constrainPitch) {
        if(sceneCamera->pitch > 89.0f)  sceneCamera->pitch = 89.0f;
        if(sceneCamera->pitch < -89.0f) sceneCamera->pitch = -89.0f;
    }

    updateCameraVectors(sceneCamera);
}

void SceneCamera_ProcessMouseScroll(SceneCamera* sceneCamera, float yOffset) {
    // TODO: 
}

void updateCameraVectors(SceneCamera* sceneCamera) {
    glm::vec3 front;
    front.x = cos(glm::radians(sceneCamera->yaw)) * cos(glm::radians(sceneCamera->pitch));
    front.y = sin(glm::radians(sceneCamera->pitch));
    front.z = sin(glm::radians(sceneCamera->yaw)) * cos(glm::radians(sceneCamera->pitch));
    sceneCamera->front = glm::normalize(front);
    sceneCamera->right = glm::normalize(glm::cross(sceneCamera->front, sceneCamera->worldUp));
    sceneCamera->up = glm::normalize(glm::cross(sceneCamera->right, sceneCamera->front));
}

void SceneCamera_SetViewportSize(SceneCamera* sceneCamera, u32 width, u32 height) {
    sceneCamera->viewportWidth = width;
    sceneCamera->viewportHeight = height;
    sceneCamera->aspectRatio = sceneCamera->viewportWidth / sceneCamera->viewportHeight;
}