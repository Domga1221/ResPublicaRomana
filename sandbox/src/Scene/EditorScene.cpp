#include "EditorScene.hpp"

#include <Scene/Components.hpp>

#include <Renderer/RenderCommand.hpp>
#include <Renderer/Shader.hpp>

#include <Renderer/PBR/ImageBasedLighting.hpp>
static ImageBasedLighting ibl;
#include <Renderer/Primitives.hpp>

#include <filesystem>

#include "ShaderPool.hpp"

#include <Renderer/Renderpasses/PostProcessing/Bloom.hpp>
#include <Renderer/Renderpasses/PostProcessing/ColorCorrect.hpp>
#include <Renderer/Renderpasses/PostProcessing/GBuffer.hpp>
static Bloom bloom;
static GBuffer gBuffer;

static glm::vec2 viewportSize = glm::vec2(1600, 900);

Shader debugQuadShader;

#include <glm/matrix.hpp>


#include <Renderer/Renderpasses/Renderpass.hpp>
Renderpass bloomRenderpass;
Renderpass colorCorrectRenderpass;
Renderpass ssaoRenderpass;

void EditorScene_Initialze() {
    ShaderPool_Initialize();
    std::string hdrPath = std::string("Assets/HDR/newport_loft.hdr");
    ImageBasedLighting_Initialize(&ibl, hdrPath.c_str());
    //Bloom_Initialize(&bloom);
    //ColorCorrect_Initialize();
    GBuffer_Initialize(&gBuffer);

    // Renderpasses
    Renderpass_Create(&colorCorrectRenderpass, RENDERPASS_COLOR_CORRECT);
    colorCorrectRenderpass.Initialize(&colorCorrectRenderpass);
    Renderpass_Create(&bloomRenderpass, RENDERPASS_BLOOM);
    bloomRenderpass.Initialize(&bloomRenderpass);
    Renderpass_Create(&ssaoRenderpass, RENDERPASS_SSAO);
    ssaoRenderpass.Initialize(&ssaoRenderpass);
    // Renderpasses end

    Renderpass_Create(&bloomRenderpass, RENDERPASS_BLOOM);
    bloomRenderpass.Initialize(&bloomRenderpass);

    std::string currentDir = Filesystem_GetCWD();
    std::string v = currentDir + "/Assets/Shaders/square.vert";
    std::string f = currentDir + "/Assets/Shaders/square.frag";
    Shader_Create(&debugQuadShader, v.c_str(), f.c_str());
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
    Shader* editorShader = ShaderPool_GetEditorShader();
    glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
    //glm::mat4 projection = SceneCamera_GetProjectionMatrix(sceneCamera);
    glm::mat4 projectionRH = SceneCamera_GetProjectionMatrixRH(sceneCamera);

    auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        //RPR_INFO("(%d, %d, %d)", transformComponent.position.x, transformComponent.position.y, transformComponent.position.z);

        glm::mat4 model = transformComponent.GetTransform();

        Shader_Bind(editorShader);
        Shader_SetMat4(editorShader, "model", model);
        Shader_SetMat4(editorShader, "view", view);
        Shader_SetMat4(editorShader, "projection", projectionRH);
        RenderCommand_ActiveTexture(0);
        Texture* albedo = materialComponent.material.textures.data[0];
        if(albedo != nullptr)
            Texture_Bind(albedo);
        Shader_SetInt(editorShader, "texture_0", 0);

        Shader_SetInt(editorShader, "colorCorrect", 0);

        Mesh_Bind(&meshComponent.mesh);
        RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
    }
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    RenderCommand_ActiveTexture(0);
    Shader_Bind(editorShader);
    Shader_SetMat4(editorShader, "model", model);
    Shader_SetMat4(editorShader, "view", view);
    Shader_SetMat4(editorShader, "projection", projectionRH);
    Shader_SetInt(editorShader, "texture_0", 0);
    RenderCommand_BindTexture2D(ibl.hdrTexture);
    Shader_SetMat4(editorShader, "model", model);
    Primitives_RenderQuad();
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f));
    Shader_SetMat4(editorShader, "model", model);
    Primitives_RenderCube();
    
    // ibl
    ImageBasedLighting_RenderSkybox(&ibl, view, projectionRH, true);
}

void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, Framebuffer* framebuffer,
    b8 bloomEnabled, b8 ssaoEnabled, b8 colorCorrectEnabled) {
        RenderCommand_ActiveTexture(0);
        
        glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
        //glm::mat4 projection = SceneCamera_GetProjectionMatrix(sceneCamera);
        glm::mat4 projectionRH = SceneCamera_GetProjectionMatrixRH(sceneCamera);
        
        RenderProperties renderProperties;
        renderProperties.deltaTime = deltaTime;
        renderProperties.framebuffer = framebuffer;
        renderProperties.view = &view;
        renderProperties.projection = &projectionRH;
        renderProperties.registry = &scene->registry;
        renderProperties.pbrShader = ShaderPool_GetPBRShader();

        // TODO: renderpasses 
        if(ssaoEnabled) {            
            ssaoRenderpass.Render(&ssaoRenderpass, &renderProperties);
        }
        
        auto lights = scene->registry.view<TransformComponent, LightComponent>();
        auto group = scene->registry.group<TransformComponent>(entt::get<MeshComponent, MaterialComponent>);
        
        Shadowmap* shadowmap = nullptr;
        glm::vec3 lightPosition;
        // TODO: check why i can't iterate with for(auto entity : lights)
        lights.each([&shadowmap, &lightPosition](TransformComponent& transformComponent, LightComponent& lightComponent) {
            if(lightComponent.shadowmap != nullptr) {
                shadowmap = lightComponent.shadowmap;
                lightPosition = transformComponent.position;
            }
        });
        if(shadowmap != nullptr) {
            RenderCommand_CullFrontFace();
            // render to depth map 
            shadowmap->directionalLightPosition = lightPosition;
            shadowmap->lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowmap->nearPlane, shadowmap->farPlane);
            shadowmap->lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            shadowmap->lightSpaceMatrix = shadowmap->lightProjection * shadowmap->lightView;
            
            // render scene from lights point of view
        Shader_Bind(&shadowmap->simpleDepthShader);
        Shader_SetMat4(&shadowmap->simpleDepthShader, "lightSpaceMatrix", shadowmap->lightSpaceMatrix);

        RenderCommand_SetViewportSize(shadowmap->SHADOW_WIDTH, shadowmap->SHADOW_HEIGHT);
        RenderCommand_BindFramebuffer(shadowmap->depthMapFBO);
        RenderCommand_Clear(false, true);

        for(entt::entity entity : group) {
            std::tuple<TransformComponent&, MeshComponent&> tuple =
                group.get<TransformComponent, MeshComponent>(entity);

            TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
            MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);

            glm::mat4 model = transformComponent.GetTransform();
            Shader_SetMat4(&shadowmap->simpleDepthShader, "model", model);
            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }    
        RenderCommand_CullBackFace();
    }
    RenderCommand_SetViewportSize(viewportSize.x, viewportSize.y);

    // BEGIN GAMEOBJECTS
    RenderCommand_BindTexture2D(0);
    RenderCommand_ActiveTexture(0);
    Framebuffer_Bind(framebuffer);
    //scene->registry.view<TransformComponent, LightComponent>().each([](auto& t, auto& l) {

    //});
    for(entt::entity entity : group) {
        std::tuple<TransformComponent&, MeshComponent&, MaterialComponent&> tuple =
            group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        TransformComponent& transformComponent = std::get<TransformComponent&>(tuple);
        MeshComponent& meshComponent = std::get<MeshComponent&>(tuple);
        MaterialComponent& materialComponent = std::get<MaterialComponent&>(tuple);

        Shader* shader = materialComponent.material.shader;
        if(shader == ShaderPool_GetEditorShader()) {
            glm::mat4 model = transformComponent.GetTransform();

            Shader_Bind(shader);
            Shader_SetMat4(shader, "model", model);
            Shader_SetMat4(shader, "view", view);
            Shader_SetMat4(shader, "projection", projectionRH);
            RenderCommand_ActiveTexture(0);
            Texture* albedo = materialComponent.material.textures.data[0];
            if(albedo != nullptr)
                Texture_Bind(albedo);
            Shader_SetInt(shader, "texture_0", 0);

            Shader_SetInt(shader, "colorCorrect", colorCorrectEnabled);

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }
        if(shader == ShaderPool_GetPBRShader()) {
            Shader_Bind(shader);
            glm::mat4 model = transformComponent.GetTransform();
            Shader_SetMat4(shader, "model", model);
            Shader_SetMat4(shader, "view", view);
            Shader_SetMat4(shader, "projection", projectionRH);
            Shader_SetVec3(shader, "camPos", sceneCamera->position);
            Material_BindTextures(&materialComponent.material);

            Shader_SetInt(shader, "albedoMap", 0);
            Shader_SetInt(shader, "normalMap", 1);
            Shader_SetInt(shader, "metallicMap", 2);
            Shader_SetInt(shader, "roughnessMap", 3);
            Shader_SetInt(shader, "aoMap", 4);

            RenderCommand_ActiveTexture(5);
            RenderCommand_BindCubemap(ibl.irradianceMap);
            Shader_SetInt(shader, "irradianceMap", 5);
            // Specular IBL 
            RenderCommand_ActiveTexture(6);
            RenderCommand_BindCubemap(ibl.prefilterMap);
            Shader_SetInt(shader, "prefilterMap", 6);
            RenderCommand_ActiveTexture(7);
            RenderCommand_BindTexture2D(ibl.brdfLUTTexture);
            Shader_SetInt(shader, "brdfLUT", 7);

            // TODO: shadowmap
            if(shadowmap != nullptr) {
                Shader_SetVec3(shader, "directionalLightPos", shadowmap->directionalLightPosition);
                Shader_SetMat4(shader, "lightSpaceMatrix", shadowmap->lightSpaceMatrix);
                RenderCommand_ActiveTexture(8);
                RenderCommand_BindTexture2D(shadowmap->depthMap);
                Shader_SetInt(shader, "shadowMap", 8);
                Shader_SetBool(shader, "applyShadow", true);
            }
            else { 
                Shader_SetBool(shader, "applyShadow", false);
            }

            // TODO: SSAO
            if(ssaoEnabled) {
                Shader_SetInt(shader, "applySSAO", 1);
                RenderCommand_ActiveTexture(9);
                RenderCommand_BindTexture2D(((GBuffer*)ssaoRenderpass.internalData)->ssaoBlurColorBuffer);
                //RenderCommand_BindTexture2D(gBuffer.ssaoBlurColorBuffer);
                Shader_SetInt(shader, "SSAOBlurTexture", 9);
                Shader_SetVec3(shader, "u_resolution", glm::vec3(viewportSize.x, viewportSize.y, 0.0));
            } else { 
                Shader_SetBool(shader, "applySSAO", 0);
            }
            // TODO: lights

            // color correct
            Shader_SetInt(shader, "colorCorrect", !colorCorrectEnabled); // TODO: do properly when loading textures or something

            if(lights.size_hint() > 4) 
                RPR_WARN("EditorScene_OnUpdateRuntime: Rendering using more than 4 lights, only 4 lights supported!");
            Shader_SetInt(shader, "lightCount", lights.size_hint());
            if(lights.size_hint() >= 1) {
                int i = 0;
                lights.each([shader, &i](auto& transformComponent, auto& lightComponent) {
                    glm::vec3& lightPos = transformComponent.position;
                    std::string lightPosString = "lightPositions[" + std::to_string(i) + "]";
                    std::string lightColorsString = "lightColors[" + std::to_string(i) + "]";
                    Shader_SetVec3(shader, lightPosString.c_str(), lightPos);
                    Shader_SetVec3(shader, lightColorsString.c_str(), 
                        PointLight_GetIntensifiedColor(&lightComponent.pointLight));
                    i++;
                });
            }

            Mesh_Bind(&meshComponent.mesh);
            RenderCommand_DrawIndexed(meshComponent.mesh.indexCount);
        }
    }
    // END GAMEOBJECTS

    
    ImageBasedLighting_RenderSkybox(&ibl, view, projectionRH, !colorCorrectEnabled);

    // BEGIN PARTICLESYSTEM
    /*
    RenderCommand_EnableBlend();
    RenderCommand_BlendEquation_Add();
    RenderCommand_BlendFunc_SrcAlpha_One(); 
    RenderCommand_DepthMask(false);
    auto particleSystemComponents = scene->registry.view<ParticleSystemComponent, TransformComponent>();
    particleSystemComponents.each([deltaTime, &view, &projectionRH](ParticleSystemComponent& pc, TransformComponent& tc) {
        glm::mat4 model = tc.GetTransform();
        ParticleSystem_Emit(pc.particleSystem, &pc.particleProps);
        ParticleSystem_Update(pc.particleSystem, deltaTime);
        ParticleSystem_Render(pc.particleSystem, &model, &view, &projectionRH, false);
    });
    RenderCommand_DepthMask(true);
    RenderCommand_DisableBlend();
    */
    // END PARTICLESYSTEM


    // Bloom 
    if(bloomEnabled)
        //Bloom_Render(&bloom, framebuffer);
        bloomRenderpass.Render(&bloomRenderpass, &renderProperties);
    if(colorCorrectEnabled)
        colorCorrectRenderpass.Render(&colorCorrectRenderpass, &renderProperties);

    // SSAO Debug, NOTE: change square.frag 
    /*
    if(ssaoEnabled) {
        Framebuffer_Bind(framebuffer);
        RenderCommand_Clear(true, true);
        Shader_Bind(&debugQuadShader);
        RenderCommand_ActiveTexture(0);
        Shader_SetInt(&debugQuadShader, "u_texture", 0);
        RenderCommand_BindTexture2D(gBuffer.ssaoBlurColorBuffer);
        Primitives_RenderQuad();
    }
    */
}


void EditorScene_OnViewportResize(u32 width, u32 height) {
    //Bloom_OnResize(&bloom, width, height);
    bloomRenderpass.Resize(&bloomRenderpass, width, height);
    ssaoRenderpass.Resize(&ssaoRenderpass, width, height);
    GBuffer_OnResize(&gBuffer, width, height);
    viewportSize.x = width;
    viewportSize.y = height;
}