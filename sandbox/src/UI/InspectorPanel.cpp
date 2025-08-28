#include "InspectorPanel.hpp"

#include <Scene/GameObject.hpp>
#include <Scene/Components.hpp>

#include <imgui-docking/imgui.h>

#include "ImGuiPayload.hpp"

#include <filesystem>
#include <iostream>

#include "GUIComponents.hpp"

#include "Scene/ShaderPool.hpp"

#include <Renderer/Framebuffer.hpp>
#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Primitives.hpp>
static Framebuffer debugFramebuffer;

static GameObject* selected;

void drawComponents(GameObject* gameObject);


void InspectorPanel_Initialize() {
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_DEPTH);
    framebufferProperties.width = 200;
    framebufferProperties.height = 200;
    Framebuffer_Create(&debugFramebuffer, &framebufferProperties);
}

void InspectorPanel_OnImGuiRender() {
    ImGui::Begin("Inspector");
    if(selected != nullptr) {
        drawComponents(selected);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Add Component").x) * 0.5);
        if(ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");
        
        if(ImGui::BeginPopup("AddComponent")) {
            if(ImGui::MenuItem("Mesh")) {
                if(!selected->HasComponent<MeshComponent>()) {
                    std::string current = std::filesystem::current_path().string();
                    std::string relativePath = std::filesystem::current_path().string() + "/Assets/Models/cube_own.obj";
                    selected->AddComponent<MeshComponent>(relativePath);
                } else {
                    RPR_CLIENT_WARN("GameObject: %u, already has MeshComponent", selected->handle);
                }
            }
            if(ImGui::MenuItem("Material")) {
                if(!selected->HasComponent<MaterialComponent>()) {
                    selected->AddComponent<MaterialComponent>(ShaderPool_GetEditorShader());
                } else {
                    RPR_CLIENT_WARN("GameObject: %u, already has MaterialComponent", selected->handle);
                }
            }
            if(ImGui::MenuItem("Light")) {
                if(!selected->HasComponent<LightComponent>()) {
                    selected->AddComponent<LightComponent>();
                } else {
                    RPR_CLIENT_WARN("GameObject: %u", "already has LightComponent", selected->handle);
                }
            }
            if(ImGui::MenuItem("Particle System")) {
                if(!selected->HasComponent<ParticleSystemComponent>()) {
                    selected->AddComponent<ParticleSystemComponent>();
                } else {
                    RPR_CLIENT_WARN("GameObject: %u", "already has ParticleSystemComponent", selected->handle);
                }
            }

            ImGui::EndPopup();
        }
    }

    
    ImGui::End();
}

void InspectorPanel_Shutdown() {
    Framebuffer_Destroy(&debugFramebuffer);
}

void InspectorPanel_SetSelectedGameObject(GameObject* gameObject) {
    selected = gameObject;
}

void drawComponents(GameObject* gameObject) {
    if(gameObject->HasComponent<TagComponent>()) {
        TagComponent& tagComponent = gameObject->GetComponent<TagComponent>();
        char buffer[256];
        MEMORY_Zero(&buffer, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tagComponent.c_str());
        if(ImGui::InputText("Tag", buffer, sizeof(buffer))) {
            tagComponent.tag = std::string(buffer);
        }
    }

    // TODO: Cleanup 
    /*
    if(TransformComponent* transformComponent = gameObject->TryGetComponent<TransformComponent>()) {
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        

        f32 columnWidth = 120.0f;
        // Position
        const std::string label1 = "position";
        glm::vec3* position = &transformComponent->position;
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label1.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##position", ImVec2(20.0f, 20.0f))) position->x = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Xp", &position->x, 0.1f);

            if(ImGui::Button("Y##position", ImVec2(20.0f, 20.0f))) position->y = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Yp", &position->y, 0.1f);

            if(ImGui::Button("Z##position", ImVec2(20.0f, 20.0f))) position->z = 0.0f; 
            ImGui::SameLine();
            ImGui::DragFloat("##Zp", &position->z, 0.1f);
        
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Rotation
        const std::string label2 = "rotation";
        glm::vec3 rotation = glm::degrees(transformComponent->rotation);
        ImGui::NextColumn();
            ImGui::Text("%s", label2.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##rotation", ImVec2(20.0f, 20.0f))) rotation.x = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Xr", &rotation.x, 0.1f);

            if(ImGui::Button("Y##rotation", ImVec2(20.0f, 20.0f))) rotation.y = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Yr", &rotation.y, 0.1f);

            if(ImGui::Button("Z##rotation", ImVec2(20.0f, 20.0f))) rotation.z = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Zr", &rotation.z, 0.1f);
        transformComponent->rotation = glm::radians(rotation);


        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Scale
        const std::string label3 = "scale";
        glm::vec3* scale = &transformComponent->scale;
        ImGui::NextColumn();
            ImGui::Text("%s", label3.c_str());
        ImGui::NextColumn();
            if(ImGui::Button("X##scale", ImVec2(20.0f, 20.0f))) scale->x = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Xs", &scale->x, 0.1f);

            if(ImGui::Button("Y##scale", ImVec2(20.0f, 20.0f))) scale->y = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Ys", &scale->y, 0.1f);

            if(ImGui::Button("Z##scale", ImVec2(20.0f, 20.0f))) scale->z = 0.0f;
            ImGui::SameLine();
            ImGui::DragFloat("##Zs", &scale->z, 0.1f);

        //ImGui::Text("%f", transformComponent->position.x);
        //ImGui::Text("%f", transformComponent->position.y);
        //ImGui::Text("%f", transformComponent->position.z);

        ImGui::Columns(1);
        ImGui::PopStyleVar();
    }
    */

    GUI_DrawComponent<TransformComponent>("Transform", gameObject, [](TransformComponent* transformComponent) {
        GUI_DrawVec3Control("Translation", transformComponent->position, 0.0f, 150.0f);
        glm::vec3 rotation = glm::degrees(transformComponent->rotation);
        GUI_DrawVec3Control("Rotation", rotation, 0.0f, 150.0f);
        transformComponent->rotation = glm::radians(rotation);
        GUI_DrawVec3Control("Scale", transformComponent->scale, 1.0f, 150.0f);
    });

    GUI_DrawComponent<MeshComponent>("Mesh", gameObject, [](MeshComponent* meshComponent) {
        ImGui::Button(meshComponent->mesh.relativePath.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)); 
        if(ImGui::BeginDragDropTarget()) { // TODO: Think about relative and absolute paths
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_CONTENT_BROWSER_ITEM)) {
                const char* path = (const char*)payload->Data;
                std::filesystem::path modelPath = std::filesystem::path(path);
                std::string s = modelPath.string(); 
                Mesh_Destroy(&meshComponent->mesh);
                Mesh_Create(&meshComponent->mesh, s);
                if(!meshComponent->mesh.isLoaded) 
                    RPR_ERROR("Failed to load Mesh");
            }
        }
    });

    GUI_DrawComponent<MaterialComponent>("Material", gameObject, [](MaterialComponent* materialComponent) {
        const char* items[] = { "Editor", "PBR" };
        static const char* previousItem = NULL;
        static const char* currentItem = NULL;
        if(materialComponent->material.shader == ShaderPool_GetEditorShader())
            previousItem = items[0];
        else if(materialComponent->material.shader == ShaderPool_GetPBRShader())
            previousItem = items[1];
        currentItem = previousItem;
        if(ImGui::BeginCombo("##materialShader", currentItem)) {
            for(int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool isSelected = (currentItem == items[n]);
                if(ImGui::Selectable(items[n], isSelected)) {
                    currentItem = items[n];
                }
            }
            ImGui::EndCombo();
        }

        for(u32 i = 0; i < materialComponent->material.textureCount; i++) {

            ImVec2 buttonSize = ImGui::GetContentRegionAvail(); buttonSize.y = 0.0f;
            Texture* texture = materialComponent->material.textures.data[i];
            const char* buttonString = texture == nullptr ? "No texture" : texture->path.sequence;

            ImGui::PushID(i);
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::Text("%s", materialComponent->material.textureNames.data[i].sequence);
            ImGui::Button(buttonString, buttonSize);
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    const char* path = (const char*)payload->Data;
                    RPR_CLIENT_INFO("loading texture from: %s", path);
                    Texture* newTexture = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
                    Texture_Create(newTexture, path);
                    if (newTexture->loaded) {
                        RPR_CLIENT_INFO("texture is loaded, address: %s", newTexture);
                        MEMORY_Free(texture, sizeof(Texture), MEMORY_TAG_RENDERER);
                        materialComponent->material.textures.data[i] = newTexture;
                    }
                    else {
                        RPR_CLIENT_INFO("texture is NOT loaded\n");
                        MEMORY_Free(newTexture, sizeof(Texture), MEMORY_TAG_RENDERER);
                    }
                }
            }
            ImGui::PopID();
        }

        if(currentItem != previousItem) {
            Shader* shader = ShaderPool_GetEditorShader();
            if(currentItem == items[0])
                shader = ShaderPool_GetEditorShader();
            if(currentItem == items[1])
                shader = ShaderPool_GetPBRShader();
            RPR_INFO("InspectorPanel drawComponents: Changing Material to: %s", currentItem);
            Material_Destroy(&materialComponent->material);
            Material_Create(&materialComponent->material, shader);
            currentItem = previousItem;
        }
    });

    GUI_DrawComponent<LightComponent>("Light", gameObject, [](LightComponent* lightComponent) {
        ImGui::ColorEdit3("Color", &lightComponent->pointLight.color.x);
        ImGui::SliderFloat("Intensity", &lightComponent->pointLight.intensity, 0.0f, 100.0f);

        bool hasShadowmap = lightComponent->shadowmap != nullptr;
        if (ImGui::Checkbox("Shadowmap", &hasShadowmap)) {
            if(lightComponent->shadowmap == nullptr) 
                lightComponent->CreateShadowmap();
            else 
                lightComponent->DestroyShadowmap();
        }

        if (lightComponent->shadowmap != nullptr) {
            Framebuffer_Bind(&debugFramebuffer);
            RenderCommand_Clear(true, true);

            // render depth debug quad
            Shader_Bind(&lightComponent->shadowmap->debugDepthQuadShader);
            Shader_SetFloat(&lightComponent->shadowmap->debugDepthQuadShader, "near_plane", 
                lightComponent->shadowmap->nearPlane);
            Shader_SetFloat(&lightComponent->shadowmap->debugDepthQuadShader, "far_plane", 
                lightComponent->shadowmap->farPlane);
            Shader_SetInt(&lightComponent->shadowmap->debugDepthQuadShader, "depthMap", 0);
            RenderCommand_ActiveTexture(0);
            RenderCommand_BindTexture2D(lightComponent->shadowmap->depthMap);
            Primitives_RenderQuad();
            
            u32 texture = debugFramebuffer.colorIDs.data[0];
            ImGui::Image((ImTextureID)texture, ImVec2{ 400, 400 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            RenderCommand_BindFramebuffer(0);
        }
        
    });

    GUI_DrawComponent<ParticleSystemComponent>("Particle System", gameObject, 
            [](ParticleSystemComponent* particleSystemComponent) {
        	ParticleProps& props = particleSystemComponent->particleProps;
        GUI_DrawVec3Control("Position", props.position);
        GUI_DrawVec3Control("Velocity", props.velocity);
        GUI_DrawVec3Control("Variation", props.velocityVariation);
        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        ImGui::ColorEdit4("Color Begin", &(props.colorBegin.x));
        ImGui::ColorEdit4("Color End", &(props.colorEnd.x));
        ImGui::DragFloat("Size Begin", &props.sizeBegin, 0.1f, 0.0f, 10.0f, "%.2f");
        ImGui::DragFloat("Size End", &props.sizeEnd, 0.1f, 0.0f, 10.0f, "%.2f");
        ImGui::DragFloat("SizeVariation", &props.sizeVariation, 0.1f, 0.0f, 10.0f, "%.2f");
        ImGui::DragFloat("Life Time", &props.lifeTime, 0.1f, 0.0f, 10.0f, "%.2f");
        ImGui::Checkbox("Billboard", (bool*)&props.billboard); // TODO: Maybe memory issue, since sizeof(bool) is either 1 or 4, depending on implementation 

        // sprite
        Texture* texture = particleSystemComponent->particleSystem->sprite;
        ImVec2 buttonSize = ImGui::GetContentRegionAvail(); buttonSize.y = 0.0f;
        std::string buttonString = texture == nullptr ? "No texture" : texture->path.sequence;
        ImGui::Button(buttonString.c_str(), buttonSize);
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const char* path = (const char*)payload->Data;
                std::filesystem::path texturePath(path); std::string textureString = texturePath.string();
                Texture* newTexture = (Texture*)MEMORY_Allocate(sizeof(Texture), MEMORY_TAG_RENDERER);
                Texture_Create(newTexture, textureString.c_str());
                std::cout << "loading texture from: " << textureString << "\n";
                if (newTexture->loaded) {
                    std::cout << "texture is loaded, address:" << newTexture << "\n";
                    delete texture;
                    particleSystemComponent->particleSystem->sprite = newTexture;
                }
                else {
                    std::cout << "texture is NOT loaded\n";
                    MEMORY_Free(newTexture, sizeof(Texture), MEMORY_TAG_RENDERER);
                }
            }
        }

        
        texture = particleSystemComponent->particleSystem->sprite;
        if (texture != nullptr) {
            Framebuffer_Bind(&debugFramebuffer);
            RenderCommand_Clear(true, true);
            u32 textureID = texture->ID;
            ImGui::Image((ImTextureID)textureID, ImVec2{ 200, 200 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            RenderCommand_BindFramebuffer(0);
        }
        
    });
}