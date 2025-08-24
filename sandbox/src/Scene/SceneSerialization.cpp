#include "SceneSerialization.hpp"

#include <Scene/GameObject.hpp>

#include <nlohmannjson/json.hpp>

#include <iostream>
#include <fstream>

#include <Core/Log.hpp>
#include <Scene/ShaderPool.hpp>

void serializeGameObject(nlohmann::ordered_json* gameObjectsArray, GameObject gameObject) {
    nlohmann::ordered_json entityObj = nlohmann::json::object();
    entityObj["Entity"] = 123456789; // TODO: UNIQUE IDENTIFIER

    RPR_CLIENT_INFO("Processing: %u", gameObject.handle);
    if (TagComponent* tc = gameObject.TryGetComponent<TagComponent>()) {
        RPR_CLIENT_INFO("in if");
        entityObj["TagComponent"] = { { "Tag", tc->tag } };
    }

    if(TransformComponent* tc = gameObject.TryGetComponent<TransformComponent>()) {
        entityObj["TransformComponent"] = nlohmann::json::object();
        entityObj["TransformComponent"]["position"] = { tc->position.x, tc->position.y, tc->position.z};
        entityObj["TransformComponent"]["rotation"] = { tc->rotation.x, tc->rotation.y, tc->rotation.z };
        entityObj["TransformComponent"]["scale"] = { tc->scale.x, tc->scale.y, tc->scale.z };
    }

    if (MeshComponent* mc = gameObject.TryGetComponent<MeshComponent>()) {
        entityObj["MeshComponent"] = nlohmann::json::object();
        entityObj["MeshComponent"]["Directory"] = mc->mesh.relativePath;
    }

    if (MaterialComponent* mc = gameObject.TryGetComponent<MaterialComponent>()) {
        // TODO: 
        
        entityObj["MaterialComponent"] = nlohmann::json::object();

        if (mc->material.shader == ShaderPool_GetPBRShader())
            entityObj["MaterialComponent"]["type"] = "PBR";
        else if (mc->material.shader == ShaderPool_GetEditorShader())
            entityObj["MaterialComponent"]["type"] = "Editor";
        else
            RPR_ERROR("Scene_Serialize: serializeGameObject: Material Shader not found and saved!");

        i32 textureCount = 0; // TODO: use saved texture count 
        for (u32 i = 0; i < mc->material.textures.size; i++) {
            Texture* texture = mc->material.textures.data[i];
            if (texture != nullptr) {
                const std::string& texturePath = texture->path.sequence;
                entityObj["MaterialComponent"]["texture-" + std::to_string(textureCount)] = texturePath;
                textureCount++;
            }
        }
    }

    if (LightComponent* lc = gameObject.TryGetComponent<LightComponent>()) {
        entityObj["LightComponent"] = nlohmann::json::object();
        entityObj["LightComponent"]["Color"] = { lc->pointLight.color.x, lc->pointLight.color.y, lc->pointLight.color.z };
        entityObj["LightComponent"]["Intensity"] = lc->pointLight.intensity;
        if (lc->shadowmap != nullptr)
            entityObj["LightComponent"]["hasShadowmap"] = true;
        else
            entityObj["LightComponent"]["hasShadowmap"] = false;
    }

    if (ParticleSystemComponent* pc = gameObject.TryGetComponent<ParticleSystemComponent>()) {
        ParticleProps* p = &pc->particleProps;
        entityObj["ParticleSystemComponent"] = nlohmann::json::object();
        entityObj["ParticleSystemComponent"]["position"] = { p->position.x, p->position.y, p->position.z };
        entityObj["ParticleSystemComponent"]["velocity"] = { p->velocity.x, p->position.y, p->position.z };
        entityObj["ParticleSystemComponent"]["velocityVariation"] = 
            { p->velocityVariation.x, p->velocityVariation.y, p->velocityVariation.z };
        entityObj["ParticleSystemComponent"]["colorBegin"] = { p->colorBegin.x, p->colorBegin.y, p->colorBegin.z, p->colorBegin.w };
        entityObj["ParticleSystemComponent"]["colorEnd"] = { p->colorEnd.x, p->colorEnd.y, p->colorEnd.z, p->colorEnd.w };
        entityObj["ParticleSystemComponent"]["sizeBegin"] = p->sizeBegin;
        entityObj["ParticleSystemComponent"]["sizeEnd"] = p->sizeEnd;
        entityObj["ParticleSystemComponent"]["sizeVariation"] = p->sizeVariation;
        entityObj["ParticleSystemComponent"]["lifeTime"] = p->lifeTime;
        entityObj["ParticleSystemComponent"]["billboard"] = p->billboard;

        if(pc->particleSystem->sprite != nullptr) {
            const std::string& texturePath = pc->particleSystem->sprite->path.sequence;
            entityObj["ParticleSystemComponent"]["sprite"] = texturePath;
        }
    }

    gameObjectsArray->push_back(entityObj);
}

void Scene_Serialize(Scene* scene, const std::string& filepath) {
    nlohmann::ordered_json json;
    json["Scene"] = "Untitled";
    json["GameObjects"] = nlohmann::json::array();

    for(auto entity : scene->registry.view<entt::entity>()) {
        if(entity == entt::null) 
            continue;
        RPR_CLIENT_INFO("Iterating over entity: %u", entity);
        GameObject g;
        g.handle = entity;
        g.scene = scene;
        serializeGameObject(&json["GameObjects"], g);
    }

    std::cout << std::setw(4) << json << "\n";

    std::ofstream fout(filepath);
    fout << std::setw(4) << json;
}

void Scene_Deserialize(Scene* scene, const std::string& filepath) {
    std::ifstream fin(filepath);
    //std::cout << "created fin\n";
    nlohmann::ordered_json json = nlohmann::json::parse(fin);

    std::string sceneName = json["Scene"].get<std::string>();
    RPR_CLIENT_INFO("Deserializing scene: %s", sceneName.c_str());

    nlohmann::ordered_json entities = json["GameObjects"];
    //std::cout << std::setw(4) << entities << "\n";

    for (auto it = entities.begin(); it != entities.end(); it++) {

        // Identifier
        nlohmann::ordered_json entityObj = it.value();
        uint64_t uuid = entityObj["Entity"];
        std::cout << "uuid: " << uuid << "\n";

        // Tag
        std::string tag = entityObj["TagComponent"]["Tag"];

        if(tag == "Root") // TODO: hacky
            continue;
        GameObject* deserializedEntity = GameObject_Create(scene, tag.c_str());
        
        // Transform
        if (entityObj.contains("TransformComponent")) {
            TransformComponent& tc = deserializedEntity->GetComponent<TransformComponent>();
            float X = entityObj["TransformComponent"]["position"][0];
            float Y = entityObj["TransformComponent"]["position"][1];
            float Z = entityObj["TransformComponent"]["position"][2];
            tc.position = glm::vec3(X, Y, Z);
            X = entityObj["TransformComponent"]["rotation"][0];
            Y = entityObj["TransformComponent"]["rotation"][1];
            Z = entityObj["TransformComponent"]["rotation"][2];
            tc.rotation = glm::vec3(X, Y, Z);
            X = entityObj["TransformComponent"]["scale"][0];
            Y = entityObj["TransformComponent"]["scale"][1];
            Z = entityObj["TransformComponent"]["scale"][2];
            tc.scale = glm::vec3(X, Y, Z);
        }
        
        // MeshComponent
        if (entityObj.contains("MeshComponent")) {
            std::string relativeDirectory = entityObj["MeshComponent"]["Directory"];
            MeshComponent& mc = deserializedEntity->AddComponent<MeshComponent>();
            //mc.mesh.LoadMesh(relativeDirectory);
            Mesh_Create(&mc.mesh, relativeDirectory);
        }

        // MaterialComponent
        if (entityObj.contains("MaterialComponent")) {
            std::string materialType = entityObj["MaterialComponent"]["type"];
            if (materialType == "PBR") {
                //std::cout << "PBR true" << "\n";
                RPR_INFO("Loading PBR Material");
                deserializedEntity->AddComponent<MaterialComponent>(ShaderPool_GetPBRShader());
            }
            else if (materialType == "Editor") {
                RPR_INFO("Loading Editor Material");
                deserializedEntity->AddComponent<MaterialComponent>(ShaderPool_GetEditorShader());
            }
            // Load textures
            if (deserializedEntity->HasComponent<MaterialComponent>()) {
                MaterialComponent& mc = deserializedEntity->GetComponent<MaterialComponent>();
                for(u32 i = 0; i < mc.material.textures.size; i++) {
                    // TODO: fix string stuff 
                    std::string readString = entityObj["MaterialComponent"]["texture-" + std::to_string(i)];
                    std::filesystem::path texturePath(readString);
                    std::string textureString = texturePath.string();
                    
                    Texture* newTexture = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
                    Texture_Create(newTexture, textureString.c_str());
                    RPR_INFO("Scene_Deserialize: Loading Texture from: %s", textureString.c_str());
                    if(newTexture->loaded) {
                        RPR_CLIENT_INFO("Texture is loaded");
                        mc.material.textures.data[i] = newTexture;
                    } else {
                        RPR_CLIENT_ERROR("Scene_Deserialize: Failed to load texture from: %s", textureString.c_str());
                        Texture_Destroy(newTexture);
                        MEMORY_Free(newTexture, sizeof(Texture), MEMORY_TAG_RENDERER);
                    }
                }
            }
        }

        if (entityObj.contains("LightComponent")) {
            LightComponent& lc = deserializedEntity->AddComponent<LightComponent>();
            float R = entityObj["LightComponent"]["Color"][0];
            float G = entityObj["LightComponent"]["Color"][1];
            float B = entityObj["LightComponent"]["Color"][2];	
            lc.pointLight.color = glm::vec3(R, G, B);

            float intensity = entityObj["LightComponent"]["Intensity"];
            lc.pointLight.intensity = intensity;

            bool hasShadowmap = entityObj["LightComponent"]["hasShadowmap"];
            if (hasShadowmap)
                lc.CreateShadowmap();
            else 
                lc.shadowmap = nullptr;
        }
        
        if (entityObj.contains("ParticleSystemComponent")) {
            ParticleSystemComponent& pc = deserializedEntity->AddComponent<ParticleSystemComponent>();
            ParticleProps& p = pc.particleProps;
            p.position.x = entityObj["ParticleSystemComponent"]["position"][0];
            p.position.y = entityObj["ParticleSystemComponent"]["position"][1];
            p.position.z = entityObj["ParticleSystemComponent"]["position"][2];
            p.velocity.x = entityObj["ParticleSystemComponent"]["velocity"][0];
            p.velocity.y = entityObj["ParticleSystemComponent"]["velocity"][1];
            p.velocity.z = entityObj["ParticleSystemComponent"]["velocity"][2];
            p.velocityVariation.x = entityObj["ParticleSystemComponent"]["velocityVariation"][0];
            p.velocityVariation.y = entityObj["ParticleSystemComponent"]["velocityVariation"][1];
            p.velocityVariation.z = entityObj["ParticleSystemComponent"]["velocityVariation"][2];
            p.colorBegin.x = entityObj["ParticleSystemComponent"]["colorBegin"][0];
            p.colorBegin.y = entityObj["ParticleSystemComponent"]["colorBegin"][1];
            p.colorBegin.z = entityObj["ParticleSystemComponent"]["colorBegin"][2];
            p.colorBegin.w = entityObj["ParticleSystemComponent"]["colorBegin"][3];     
            p.colorEnd.x = entityObj["ParticleSystemComponent"]["colorEnd"][0];
            p.colorEnd.y = entityObj["ParticleSystemComponent"]["colorEnd"][1];
            p.colorEnd.z = entityObj["ParticleSystemComponent"]["colorEnd"][2];
            p.colorEnd.w = entityObj["ParticleSystemComponent"]["colorEnd"][3];           
            p.sizeBegin = entityObj["ParticleSystemComponent"]["sizeBegin"];
            p.sizeEnd = entityObj["ParticleSystemComponent"]["sizeBegin"];
            p.sizeVariation = entityObj["ParticleSystemComponent"]["sizeVariation"];
            p.lifeTime = (float)entityObj["ParticleSystemComponent"]["lifeTime"];
            auto s = entityObj["ParticleSystemComponent"]["billboard"];
            bool b = s.is_number_integer();
            RPR_WARN("%d", b);
            p.billboard =(b8)(int)entityObj["ParticleSystemComponent"]["billboard"];

            bool hasSprite = entityObj["ParticleSystemComponent"].contains("sprite");
            if(hasSprite) {
                std::string readString = entityObj["ParticleSystemComponent"]["sprite"];
                std::filesystem::path texturePath(readString);
                std::string textureString = texturePath.string();
                
                Texture* newTexture = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
                Texture_Create(newTexture, textureString.c_str());
                RPR_INFO("Scene_Deserialize: Loading Texture from: %s", textureString.c_str());
                if(newTexture->loaded) {
                    RPR_CLIENT_INFO("Texture is loaded");
                    pc.particleSystem->sprite = newTexture;
                } else {
                    RPR_CLIENT_ERROR("Scene_Deserialize: Failed to load texture from: %s", textureString.c_str());
                    Texture_Destroy(newTexture);
                    MEMORY_Free(newTexture, sizeof(Texture), MEMORY_TAG_RENDERER);
                }
            }
        }
    }
}