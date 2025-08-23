#pragma once 

#include "defines.hpp"

#include "Renderer/Shader.hpp"

#include <glm/glm.hpp>

typedef struct Shadowmap {
    glm::vec3 directionalLightPosition;
    u32 depthMapFBO;
    u32 depthMap;
    float nearPlane;
    float farPlane;

    u32 SHADOW_WIDTH;
    u32 SHADOW_HEIGHT;

    Shader simpleDepthShader;   // TODO: initialize shaders once, when initializing renderer, 
    Shader debugDepthQuadShader; // instead of creating them every time when creating a shadowmap, etc. 

    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
} Shadowmap;

RPR_API void Shadowmap_Create(Shadowmap* shadowmap);
RPR_API void Shadowmap_Destroy(Shadowmap* shadowmap); // TODO: 
