#include "SceneSerialization.hpp"

#include <Scene/GameObject.hpp>

#include <nlohmannjson/json.hpp>

#include <iostream>
#include <fstream>

#include <Core/Log.hpp>

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
        
        //entityObj["MaterialComponent"] = nlohmann::json::object();

        /*
        if (mc.material->GetShader() == ShaderPool::Get().GetPBRShader())
            entityObj["MaterialComponent"]["type"] = "PBR";
        else if (mc.material->GetShader() == ShaderPool::Get().GetEditorShader())
            entityObj["MaterialComponent"]["type"] = "Editor";
        else
            RE_CORE_ERROR("SCENESERIALIZER:: Material Shader not found and saved!");

        int textureCount = 0;
        for (Texture* texture : mc.material->GetTextures()) {
            if (texture != nullptr) {
                const std::string& texturePath = texture->GetPath();
                entityObj["MaterialComponent"]["texture-" + std::to_string(textureCount)] = texturePath;

                textureCount++;
            }
        }
        */
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
            TransformComponent tc = deserializedEntity->GetComponent<TransformComponent>();
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
    }
}