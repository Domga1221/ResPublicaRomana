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
static Renderpass bloomRenderpass;
static Renderpass colorCorrectRenderpass;
static Renderpass ssaoRenderpass;

static Renderpass editorRenderpass;
static Renderpass pbrRenderpass;

static Renderpass shadowmapRenderpass;

static Renderpass skyboxRenderpass;

static Renderpass particleSystemRenderpass;

static List<Renderpass*> renderpasses;

void EditorScene_Initialze() {
    ShaderPool_Initialize();
    std::string hdrPath = std::string("Assets/HDR/newport_loft.hdr");
    ImageBasedLighting_Initialize(&ibl, hdrPath.c_str());
    //Bloom_Initialize(&bloom);
    //ColorCorrect_Initialize();
    GBuffer_Initialize(&gBuffer);
    
    
    // Renderpasses
    // 1. ssaoRenderpass 
    // 2. light shadowmap 
    // 3. PBRRenderpass 
    // 4. skybox 
    // 5. bloomRenderpass 
    // 6. colorCorrectRenderpass 
    List_Create(&renderpasses);

    Renderpass_Create(&ssaoRenderpass, RENDERPASS_SSAO);
    List_PushBack(&renderpasses, &ssaoRenderpass);

    Renderpass_Create(&shadowmapRenderpass, RENDERPASS_SHADOWMAP);
    List_PushBack(&renderpasses, &shadowmapRenderpass);

    Renderpass_Create(&editorRenderpass, RENDERPASS_EDITOR);
    List_PushBack(&renderpasses, &editorRenderpass);
    
    Renderpass_Create(&pbrRenderpass, RENDERPASS_PBR);
    List_PushBack(&renderpasses, &pbrRenderpass);
    
    Renderpass_Create(&skyboxRenderpass, RENDERPASS_SKYBOX);
    List_PushBack(&renderpasses, &skyboxRenderpass);

    Renderpass_Create(&particleSystemRenderpass, RENDERPASS_PARTICLESYSTEM);
    List_PushBack(&renderpasses, &particleSystemRenderpass);

    Renderpass_Create(&bloomRenderpass, RENDERPASS_BLOOM);
    List_PushBack(&renderpasses, &bloomRenderpass);

    Renderpass_Create(&colorCorrectRenderpass, RENDERPASS_COLOR_CORRECT);
    List_PushBack(&renderpasses, &colorCorrectRenderpass);
    // Renderpasses end


    // TODO: renderer dispatch function and initialize that takes care of this 
    for(u32 i = 0; i < renderpasses.size; i++) {
        renderpasses.data[i]->Initialize(renderpasses.data[i]);
    }


    std::string currentDir = Filesystem_GetCWD();
    std::string v = currentDir + "/Assets/Shaders/square.vert";
    std::string f = currentDir + "/Assets/Shaders/square.frag";
    Shader_Create(&debugQuadShader, v.c_str(), f.c_str());
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, Framebuffer* framebuffer) {
    glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
    glm::mat4 projectionRH = SceneCamera_GetProjectionMatrixRH(sceneCamera);
    
    RenderProperties renderProperties;
    renderProperties.deltaTime = deltaTime;
    renderProperties.framebuffer = framebuffer;
    renderProperties.view = &view;
    renderProperties.projection = &projectionRH;
    renderProperties.registry = &scene->registry;
    renderProperties.editorShader = ShaderPool_GetEditorShader();
    renderProperties.ibl = &ibl;
    renderProperties.colorCorrectEnabled = false; // for skybox

    editorRenderpass.Render(&editorRenderpass, &renderProperties);

    // ibl
    skyboxRenderpass.Render(&skyboxRenderpass, &renderProperties);

    /*
    Framebuffer_Bind(framebuffer);
    Shader_Bind(&debugQuadShader);
    RenderCommand_ActiveTexture(0);
    Shader_SetInt(&debugQuadShader, "u_texture", 0);
    RenderCommand_BindTexture2D(framebuffer->colorIDs.data[1]);
    Primitives_RenderQuad();
    */
}

// TODO: maybe function to update renderproperties, where each pass updates them
//       with required information for other passes, right now it's at the end of render
void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, Framebuffer* framebuffer,
    b8 bloomEnabled, b8 ssaoEnabled, b8 colorCorrectEnabled, b8 playMode) {
    RenderCommand_ActiveTexture(0);

    bloomRenderpass.enabled = bloomEnabled;
    ssaoRenderpass.enabled = ssaoEnabled;
    colorCorrectRenderpass.enabled = colorCorrectEnabled;
    editorRenderpass.enabled = false;
    
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
    renderProperties.editorShader = ShaderPool_GetEditorShader();
    renderProperties.cameraPosition = &sceneCamera->position;
    renderProperties.ibl = &ibl;
    renderProperties.ssaoEnabled = ssaoEnabled;
    renderProperties.colorCorrectEnabled = colorCorrectEnabled;
    renderProperties.viewportSize = viewportSize;
    
    
    for(u32 i = 0; i < renderpasses.size; i++) {
        if(renderpasses.data[i]->enabled)
            renderpasses.data[i]->Render(renderpasses.data[i], &renderProperties);
    }
    
    // TODO: old just for reference, remove later
    /*
    if(ssaoEnabled) {            
        ssaoRenderpass.Render(&ssaoRenderpass, &renderProperties);
    }

    shadowmapRenderpass.Render(&shadowmapRenderpass, &renderProperties);
    
    RenderCommand_SetViewportSize(viewportSize.x, viewportSize.y);

    // BEGIN GAMEOBJECTS
    pbrRenderpass.Render(&pbrRenderpass, &renderProperties);
    // END GAMEOBJECTS

    
    ImageBasedLighting_RenderSkybox(&ibl, view, projectionRH, !colorCorrectEnabled);
    */

    // BEGIN PARTICLESYSTEM
    // TODO: Blend breaks SSAO 
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
    
    /*
    if(bloomEnabled)
        //Bloom_Render(&bloom, framebuffer);
        bloomRenderpass.Render(&bloomRenderpass, &renderProperties);
    if(colorCorrectEnabled)
        colorCorrectRenderpass.Render(&colorCorrectRenderpass, &renderProperties);
    */

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