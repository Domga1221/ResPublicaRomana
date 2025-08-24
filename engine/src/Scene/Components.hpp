#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

#include <Platform/Filesystem.hpp>

#include <Renderer/Light/PointLight.hpp>
#include <Renderer/Light/Shadowmap.hpp>
#include <Renderer/ParticleSystem/ParticleSystem.hpp>

struct TagComponent {
    std::string tag; // TODO: String implementation 
    const char* c_str() {
        return tag.c_str();
    }
};

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetTransform() const {
        glm::mat4 rotate = 
              glm::rotate(glm::mat4(1.0f), rotation.x, {1, 0, 0}) 
            * glm::rotate(glm::mat4(1.0f), rotation.y, {0, 1, 0}) 
            * glm::rotate(glm::mat4(1.0f), rotation.z, {0, 0, 1});
        
        return glm::translate(glm::mat4(1.0f), position) * rotate * glm::scale(glm::mat4(1.0f), scale);
    }
};

struct MeshComponent {
    Mesh mesh;
    MeshComponent() = default;
    MeshComponent(std::string& relativePath) {
        std::string path = std::string(relativePath);
        Mesh_Create(&mesh, path);
    }
    MeshComponent(MeshComponent&& other) {
        mesh = other.mesh;
    }
    ~MeshComponent() {
        RPR_CLIENT_ERROR("%u", this);
        Mesh_Destroy(&mesh);
    }
};

struct MaterialComponent {
    Material material;
    MaterialComponent() = default;
    MaterialComponent(Shader* shader) {
        Material_Create(&material, shader);
        const char* defaultTexturePath = "Assets/Textures/bricks10_diffuse_1k.jpg";
        //std::string fullPath = Filesystem_GetCWD();
        //fullPath += defaultTexturePath;
        Texture* materialTexture = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
        Texture_Create(materialTexture, defaultTexturePath);
        material.textures.data[0] = materialTexture;
    }

    // https://github.com/skypjack/entt/issues/456
    // Should have used C
    // TODO: is this just a clang thing? need to test on MSVC
    MaterialComponent(MaterialComponent&& other) {
        material = other.material;
    }

    ~MaterialComponent() {
        RPR_CLIENT_ERROR("%u", this);
        if(material.shader != nullptr)
            Material_Destroy(&material);
    }
};

struct LightComponent {
    PointLight pointLight;
    Shadowmap* shadowmap = nullptr;
    LightComponent() = default;
    LightComponent(LightComponent&& other) {
        pointLight = other.pointLight;
        shadowmap = other.shadowmap;
    }
    ~LightComponent() {
        if(shadowmap != nullptr) 
        DestroyShadowmap();
    }
    void CreateShadowmap() {
        shadowmap = (Shadowmap*) MEMORY_Allocate(sizeof(Shadowmap), MEMORY_TAG_RENDERER);
        Shadowmap_Create(shadowmap);
    }
    void DestroyShadowmap() {
        Shadowmap_Destroy(shadowmap); // TODO:
        MEMORY_Free(shadowmap, sizeof(Shadowmap), MEMORY_TAG_RENDERER);
        shadowmap = nullptr;
    }
};

struct ParticleSystemComponent {
    ParticleProps particleProps;
    ParticleSystem* particleSystem = nullptr;

    ParticleSystemComponent() {
	    particleProps.colorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
        particleProps.colorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
        particleProps.sizeBegin = 0.5f, particleProps.sizeVariation = 0.3f, particleProps.sizeEnd = 0.0f;
        particleProps.lifeTime = 1.0f;
        particleProps.velocity = { 0.0f, 0.0f, 0.0f };
        particleProps.velocityVariation = { 3.0f, 1.0f, 0.0f };
        particleProps.position = { 0.0f, 0.0f, 0.0f };

        particleSystem = (ParticleSystem*)MEMORY_Allocate(sizeof(ParticleSystem), MEMORY_TAG_RENDERER);
        ParticleSystem_Create(particleSystem);
    }
    ~ParticleSystemComponent() {
        // TODO:
    }

    // TODO: Move constructor
};