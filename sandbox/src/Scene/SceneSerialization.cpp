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
    }
}