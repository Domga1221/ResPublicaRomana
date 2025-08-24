#include "ParticleSystem.hpp"

#include "Platform/Filesystem.hpp"

#include "ParticleUtility.hpp"

#include "Random.hpp"

#include <glm/gtc/constants.hpp>


void ParticleSystem_Create(ParticleSystem* particleSystem) {
    particleSystem->particleVAO = VertexArray_Create();
    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { "aPosition", ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aTexCoords", ShaderDataType::Float2 });
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);
    
    VertexBuffer* VBO = VertexBuffer_Create((f32*)particle_vertices, sizeof(particle_vertices));
    VertexBuffer_SetLayout(VBO, &bufferLayout);
    VertexArray_AddVertexBuffer(particleSystem->particleVAO, VBO);

    IndexBuffer* indexBuffer = IndexBuffer_Create((u32*)particle_indices, sizeof(particle_indices));
    VertexArray_SetIndexBuffer(particleSystem->particleVAO, indexBuffer);

    std::string currentPath = Filesystem_GetCWD();
    std::string v = currentPath + "/Assets/Shaders/Particle/Particle.vert";
    std::string f = currentPath + "/Assets/Shaders/Particle/Particle.frag";
    Shader_Create(&particleSystem->particleShader, v.c_str(), f.c_str());

    std::string texturePath = "/Assets/Textures/white_1x1.png";
    particleSystem->sprite = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
    Texture_Create(particleSystem->sprite, texturePath.c_str());

    particleSystem->particlePool.resize(1000);
}

void ParticleSystem_Destroy(ParticleSystem* particleSystem) {
    VertexArray_Destroy(particleSystem->particleVAO);
    Texture_Destroy(particleSystem->sprite);
    MEMORY_Free(particleSystem->sprite, sizeof(Texture), MEMORY_TAG_RENDERER);
    // TODO: Shader destruction
}

void ParticleSystem_Update(ParticleSystem* particleSystem, f32 deltaTime) {
    for(Particle& particle : particleSystem->particlePool) {
        if(!particle.active)
            continue;
        if(particle.lifeRemaining <= 0.0f) {
            particle.active = false;
            continue;
        }

        particle.lifeRemaining -= deltaTime;
        particle.position += particle.velocity * deltaTime;
        // TODO: rotation 
    }
}

void ParticleSystem_Render(glm::mat4* translation, glm::mat4* view, glm::mat4* projection, b8 colorCorrect) {

}

void ParticleSystem_Emit(ParticleSystem* particleSystem, const ParticleProps* particleProps) {
    // TODO: This is overkill 
    Particle& particle = particleSystem->particlePool[particleSystem->poolIndex];
    particle.active = true;
    particle.position = particleProps->position;
    particle.rotation = Random_Float() * 2.0f * glm::pi<float>();
    particle.rotation = 0.0f;

    // velocity
    particle.velocity = particleProps->velocity;
    particle.velocity.x += particleProps->velocityVariation.x * (Random_Float() - 0.5f);
    particle.velocity.y += particleProps->velocityVariation.y * (Random_Float() - 0.5f);
    particle.velocity.z += particleProps->velocityVariation.z * (Random_Float() - 0.5f);

    // Color 
    particle.colorBegin = particleProps->colorBegin;
    particle.colorEnd = particleProps->colorEnd;

    particle.lifeTime = particleProps->lifeTime;
    particle.lifeRemaining = particleProps->lifeTime;
    particle.sizeBegin = particleProps->sizeBegin + particleProps->sizeVariation * (Random_Float() - 0.5f);
    particle.sizeEnd = particleProps->sizeEnd;

    particle.billboard = particleProps->billboard;

    particleSystem->poolIndex = (particleSystem->poolIndex - 1) % particleSystem->particlePool.size();
}