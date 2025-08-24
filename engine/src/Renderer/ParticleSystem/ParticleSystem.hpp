#pragma once 

#include "defines.hpp"

#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

#include <vector>

typedef struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 colorBegin, colorEnd;
    f32 rotation = 0.0f;
    f32 sizeBegin, sizeEnd;

    f32 lifeTime = 1.0f;
    f32 lifeRemaining = 0.0f;

    b8 billboard = false;

    b8 active = false;
} Particle;


typedef struct ParticleProps {
    glm::vec3 position;
    glm::vec3 velocity, velocityVariation;
    glm::vec4 colorBegin, colorEnd;
    f32 sizeBegin, sizeEnd, sizeVariation;
    f32 lifeTime = 1.0f;
    b8 billboard = false;
} ParticleProps;


typedef struct ParticleSystem {
    std::vector<Particle> particlePool;
    u32 poolIndex = 999;

    // Rendering
    VertexArray* particleVAO;
    Shader particleShader; // TODO: Add to Shaderpool
    Texture* sprite;
} ParticleSystem;

RPR_API void ParticleSystem_Create(ParticleSystem* particleSystem);
RPR_API void ParticleSystem_Destroy(ParticleSystem* particleSystem); // TODO: 

RPR_API void ParticleSystem_Update(ParticleSystem* particleSystem, f32 deltaTime);
RPR_API void ParticleSystem_Render(ParticleSystem* particleSystem, 
    glm::mat4* translation, glm::mat4* view, glm::mat4* projection, b8 colorCorrect);

RPR_API void ParticleSystem_Emit(ParticleSystem* particleSystem, const ParticleProps* particleProps);
