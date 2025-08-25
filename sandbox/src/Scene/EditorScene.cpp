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

Renderpass editorRenderpass;
Renderpass pbrRenderpass;

Renderpass shadowmapRenderpass;

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

    Renderpass_Create(&editorRenderpass, RENDERPASS_EDITOR);
    editorRenderpass.Initialize(&editorRenderpass);
    Renderpass_Create(&pbrRenderpass, RENDERPASS_PBR);

    Renderpass_Create(&shadowmapRenderpass, RENDERPASS_SHADOWMAP);
    shadowmapRenderpass.Initialize(&shadowmapRenderpass);
    // Renderpasses end

    Renderpass_Create(&bloomRenderpass, RENDERPASS_BLOOM);
    bloomRenderpass.Initialize(&bloomRenderpass);

    std::string currentDir = Filesystem_GetCWD();
    std::string v = currentDir + "/Assets/Shaders/square.vert";
    std::string f = currentDir + "/Assets/Shaders/square.frag";
    Shader_Create(&debugQuadShader, v.c_str(), f.c_str());
}

void EditorScene_OnUpdateEditor(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera) {
    glm::mat4 view = SceneCamera_GetViewMatrix(sceneCamera);
    glm::mat4 projectionRH = SceneCamera_GetProjectionMatrixRH(sceneCamera);
    
    RenderProperties renderProperties;
    renderProperties.deltaTime = deltaTime;
    renderProperties.view = &view;
    renderProperties.projection = &projectionRH;
    renderProperties.registry = &scene->registry;
    renderProperties.editorShader = ShaderPool_GetEditorShader();

    editorRenderpass.Render(&editorRenderpass, &renderProperties);

    // ibl
    ImageBasedLighting_RenderSkybox(&ibl, view, projectionRH, true);
}

// 1. ssaoRenderpass x
// 2. light shadowmap
// 3. PBRRenderpass x
// 4. skybox
// 5. bloomRenderpass x
// 6. colorCorrectRenderpass x
// TODO: function to update renderproperties, where each pass updates them
//       with required information for other passes 
void EditorScene_OnUpdateRuntime(f32 deltaTime, Scene* scene, SceneCamera* sceneCamera, Framebuffer* framebuffer,
    b8 bloomEnabled, b8 ssaoEnabled, b8 colorCorrectEnabled, b8 playMode) {
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
    renderProperties.editorShader = ShaderPool_GetEditorShader();
    renderProperties.cameraPosition = &sceneCamera->position;
    renderProperties.ibl = &ibl;
    renderProperties.ssaoEnabled = ssaoEnabled;
    renderProperties.colorCorrectEnabled = colorCorrectEnabled;
    
    
    
    // TODO: renderpasses 
    if(ssaoEnabled) {            
        ssaoRenderpass.Render(&ssaoRenderpass, &renderProperties);
    }

    // TODO: put into Render
    renderProperties.ssaoBlurColorBuffer = ((GBuffer*)ssaoRenderpass.internalData)->ssaoBlurColorBuffer;

    shadowmapRenderpass.Render(&shadowmapRenderpass, &renderProperties);
    
    RenderCommand_SetViewportSize(viewportSize.x, viewportSize.y);

    // BEGIN GAMEOBJECTS
    RenderCommand_BindTexture2D(0);
    RenderCommand_ActiveTexture(0);
    Framebuffer_Bind(framebuffer);
    
    pbrRenderpass.Render(&pbrRenderpass, &renderProperties);
    // END GAMEOBJECTS

    
    ImageBasedLighting_RenderSkybox(&ibl, view, projectionRH, !colorCorrectEnabled);

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