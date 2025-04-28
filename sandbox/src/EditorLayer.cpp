#include "EditorLayer.hpp"

#include <Core/ClientLog.hpp>

#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>

// UI
#include "UI/ContentBrowserPanel.hpp"
#include "UI/SceneHierarchyPanel.hpp"
#include "UI/InspectorPanel.hpp"
#include <imgui-docking/imgui.h>

#include <Memory/List.hpp>

// Renderer
#include <Renderer/Shader.hpp>
#include <Renderer/BufferLayout.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/VertexArray.hpp>
Shader shader;
VertexBuffer* vertexBuffer;
VertexArray* vertexArray;
#include <Renderer/Framebuffer.hpp>
Framebuffer framebuffer;
Shader screenSpaceShader;
VertexBuffer* screenVertexBuffer;
VertexArray* screenVertexArray;
#include <Renderer/RenderCommand.hpp>

#include "Scene/SceneCamera.hpp"
SceneCamera sceneCamera;
#include <Core/Input.hpp>
#include <Core/Keycodes.hpp>
void handleSceneCameraMovement(f32 deltaTime) {
    if(!Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_2)) return;
    if(Input_IsKeyPressed(RPR_KEY_W)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_FORWARD, deltaTime);
    if(Input_IsKeyPressed(RPR_KEY_A)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_LEFT, deltaTime);
    if(Input_IsKeyPressed(RPR_KEY_S)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_BACKWARD, deltaTime);
    if(Input_IsKeyPressed(RPR_KEY_D)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_RIGHT, deltaTime);
    if(Input_IsKeyPressed(RPR_KEY_SPACE)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_UP, deltaTime);
    if(Input_IsKeyPressed(RPR_KEY_LEFT_SHIFT)) SceneCamera_ProcessKeyboard(&sceneCamera, SCENE_CAMERA_MOVEMENT_DOWN, deltaTime);
}
f32 lastMouseX = 1280 / 2;
f32 lastMouseY = 720 / 2;
void handleSceneCameraMouseMovement() {

    f32 currentMouseX = Input_GetMouseX();
    f32 currentMouseY = Input_GetMouseY();

    f32 xOffset = lastMouseX - currentMouseX;
    f32 yOffset = currentMouseY - lastMouseY;
    lastMouseX = currentMouseX;
    lastMouseY = currentMouseY;

    if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_2)) 
        SceneCamera_ProcessMouseMovement(&sceneCamera, xOffset, yOffset);
}

#include "Renderer/Texture.hpp"
#include <glad/glad.h>
Texture texture;

#include "Renderer/Skybox.hpp"
Skybox skybox;
Shader skyboxShader;

#include <Scene/GameObject.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Components.hpp>
static Scene activeScene;

#include <Renderer/Mesh.hpp>
//Mesh mesh;
Mesh* mesh;

#include "Scene/EditorScene.hpp"

#include <Systems/HexagonGrid.hpp>
HexagonGrid* hexagonGrid;

glm::vec2 viewportSize = glm::vec2(1280, 720);

void EditorLayer_OnAttach() {
    RPR_CLIENT_INFO("Hello from EditorLayer");


    EditorScene_Initialze();

    // UI
    ContentBrowserPanel_Initialize();
    SceneHierarchyPanel_Initialize(&activeScene);
    InspectorPanel_Initialize();


    std::string currentPath = std::filesystem::current_path().string();

    std::string vertPath = currentPath + "/Assets/Shaders/triangle3D.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/triangle3D.frag";
    Shader_Create(&shader, vertPath, fragPath);
    RPR_CLIENT_INFO("%s", currentPath.c_str());

    
    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { .name = "aPosition", .shaderDataType = ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aColor", ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aTexCoords", ShaderDataType::Float2 });
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);
    
    for(int i = 0; i < bufferLayout.elements.size; i++) {
        BufferElement_Print(&bufferLayout.elements.data[i]);
    }

    
    f32 triangleVertices[24] = {
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,   0.5f, 0.5f,
    };
    
    vertexBuffer = VertexBuffer_Create(triangleVertices, sizeof(triangleVertices));
    VertexBuffer_SetLayout(vertexBuffer, &bufferLayout);
    
    BufferLayout_Print(&vertexBuffer->bufferLayout);
    
    vertexArray = VertexArray_Create();
    VertexArray_AddVertexBuffer(vertexArray, vertexBuffer);


    // Framebuffer
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_DEPTH);
    framebufferProperties.width = 1280;
    framebufferProperties.height = 720;

    Framebuffer_Create(&framebuffer, &framebufferProperties);

    RPR_CLIENT_INFO("%s", currentPath.c_str());
    vertPath = currentPath + "/Assets/Shaders/screenSpace.vert";
    fragPath = currentPath + "/Assets/Shaders/screenSpace.frag";
    Shader_Create(&screenSpaceShader, vertPath, fragPath);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    BufferLayout screenBufferLayout;
    BufferLayout_Create(&screenBufferLayout);
    BufferLayout_AddElement(&screenBufferLayout, { .name = "aPos", .shaderDataType = ShaderDataType::Float2 });
    BufferLayout_AddElement(&screenBufferLayout, { "aTexCoords", ShaderDataType::Float2});
    BufferLayout_CalculateOffsetAndStride(&screenBufferLayout);

    screenVertexBuffer = VertexBuffer_Create(quadVertices, sizeof(quadVertices));
    VertexBuffer_SetLayout(screenVertexBuffer, &screenBufferLayout);
    
    screenVertexArray = VertexArray_Create();
    VertexArray_AddVertexBuffer(screenVertexArray, screenVertexBuffer);

    SceneCamera_Create(&sceneCamera, glm::vec3(1.0f, 0.5f, 2.0f));

    std::string texturePath = currentPath + "/Assets/Textures/bricks10_diffuse_1k.jpg";
    Texture_Create(&texture, texturePath.c_str());
    RPR_CLIENT_INFO("Texture channels: %d", texture.numberOfChannels);

    Skybox_Create(&skybox);


    // Entity test
    GameObject* e = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    GameObject_Create(&activeScene, e);
    RPR_INFO("gameObject handle: %u", e->handle);
    RPR_INFO("active scene: %p, gameObject scene: %p", &activeScene, e->scene);
    TagComponent& tc = e->GetComponent<TagComponent>();
    RPR_INFO("gameObject tag: %s", tc.c_str());
    TransformComponent& transform = e->GetComponent<TransformComponent>();
    RPR_INFO("gameObject transform position: (%d, %d, %d)", transform.position.x, transform.position.y, transform.position.z);

    RPR_INFO("GameObject has transform component: %d", e->HasComponent<TransformComponent>());
    e->RemoveComponent<TransformComponent>();
    RPR_INFO("GameObject has transform component: %d", e->HasComponent<TransformComponent>());
    //transform = e.GetComponent<TransformComponent>();

    activeScene.root = e;
    e->GetComponent<TagComponent>().tag = "Root";
    List_Create(&e->children);

    // TODO: move to own function
    GameObject* g1 = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    List_Create(&g1->children);
    GameObject_Create(&activeScene, g1);
    g1->GetComponent<TagComponent>().tag = "1";
    GameObject* g2 = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    List_Create(&g2->children);
    GameObject_Create(&activeScene, g2);
    g2->GetComponent<TagComponent>().tag = "2";
    

    List_PushBack(&e->children, g1);
    List_PushBack(&e->children, g2);   
    

    // Mesh testing 
    std::string cube_ownPath = currentPath + "/Assets/Models/cube_own.obj";
    //Mesh_Create(&mesh, cube_ownPath);
    GameObject* cube = (GameObject*)MEMORY_Allocate(sizeof(GameObject), MEMORY_TAG_ENTITY);
    List_Create(&cube->children);
    GameObject_Create(&activeScene, cube);
    cube->GetComponent<TagComponent>().tag = "cube";
    MeshComponent& meshComponent = cube->AddComponent<MeshComponent>();
    Mesh_Create(&meshComponent.mesh, cube_ownPath);
    mesh = &meshComponent.mesh;
    List_PushBack(&e->children, cube);

    // Hexagon
    hexagonGrid = HexagonGrid_Create(2, 2);
}

void EditorLayer_OnDetach() {
    
}

void EditorLayer_OnUpdate(f32 deltaTime) {

    // resize
    if(viewportSize.x > 0.0f && viewportSize.y > 0.0f && 
            (framebuffer.framebufferProperties.width != (u32)viewportSize.x
            || framebuffer.framebufferProperties.height != (u32)viewportSize.y)) {
        Framebuffer_Resize(&framebuffer, (u32)viewportSize.x, (u32)viewportSize.y);
        SceneCamera_SetViewportSize(&sceneCamera, (u32)viewportSize.x, (u32)viewportSize.y); 
        //__builtin_debugtrap();
    }
    //RPR_INFO("viewportSize.x: %d, framebufferwidth: %d", (u32)viewportSize.x, (u32)framebuffer.framebufferProperties.width);
    //RPR_INFO("viewportSize.y: %d, framebufferheight: %d", (u32)viewportSize.y, (u32)framebuffer.framebufferProperties.height);



    handleSceneCameraMovement(deltaTime);
    handleSceneCameraMouseMovement();

    RenderCommand_EnableDepthTest(true);
    
    // TODO: FRAMEBUFFER HAS NO DEPTH ATTACHMENT
    Framebuffer_Bind(&framebuffer);
    glm::vec4 color = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f);
    //glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    RenderCommand_SetClearColor(&color);
    RenderCommand_Clear(true, true);

    glm::mat4 view = SceneCamera_GetViewMatrix(&sceneCamera);
    glm::mat4 projection = SceneCamera_GetProjectinoMatrix(&sceneCamera);
    // -- triangle
    Shader_Bind(&shader);
    glm::mat4 model = glm::mat4(1.0f);
    Shader_SetMat4(&shader, "model", model);
    Shader_SetMat4(&shader, "view", view);
    Shader_SetMat4(&shader, "projection", projection);
    Shader_SetInt(&shader, "texture_0", 0);
    RenderCommand_ActiveTexture(0);
    Texture_Bind(&texture);
    VertexArray_Bind(vertexArray);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    RenderCommand_Draw(3);

    // -- cube 
    //Mesh_Bind(&mesh);
    //RenderCommand_DrawIndexed(mesh.indexCount);

    // -- EditorScene
    ///EditorScene_OnUpdateEditor(deltaTime, &activeScene, &sceneCamera);

    // -- Hexagon 
    HexagonGrid_Render(hexagonGrid, view, projection);

    // -- Skybox
    Skybox_Render(&skybox, &view, &projection);
    // -- Skybox End
    Framebuffer_Unbind();
    RenderCommand_EnableDepthTest(false);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, framebuffer.colorIDs.data[0]);
    RenderCommand_ActiveTexture(0);
    RenderCommand_BindTexture2D(framebuffer.colorIDs.data[0]);
    Shader_Bind(&screenSpaceShader);
    VertexArray_Bind(screenVertexArray);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    RenderCommand_Draw(6);
}

void EditorLayer_OnImGuiRender(ImGuiContext* context) {
    ImGui::SetCurrentContext(context); // ImGui global does not persist across dll boundaries 

    //return;
    
    static bool dockSpace = true;
    static bool opt_fullscreen_persistent = true;
    bool opt_fullscreen = opt_fullscreen_persistent;
    // to avoid two dockable targets 
    // use ImGuiDockNodeFlags_None to make the parent window not dockable into
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // Fullscreen setup 
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if(opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize 
            | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Dockspace", &dockSpace, window_flags);
    ImGui::PopStyleVar();

    if(opt_fullscreen)  
        ImGui::PopStyleVar(2);

    // Dockspace 
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            //  ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
    
            if (ImGui::MenuItem("New", "Ctrl+N")) std::cout << "New\n";
            if (ImGui::MenuItem("Open...", "Ctrl+O")) std::cout << "Open\n";
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) std::cout << "Save As\n";
    
            if (ImGui::MenuItem("Exit")) std::cout << "Exit\n";
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    
    // Panels
    ContentBrowserPanel_OnImGuiRender();
    SceneHierarchyPanel_OnImGuiRender();
    InspectorPanel_OnImGuiRender();


    // viewport 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    //ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    //ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    //ImVec2 viewportOffset = ImGui::GetWindowPos();
    
    // TODO: for some reason clear color of framebuffer color attachment is not rendered in imgui image 
    u32 textureID = framebuffer.colorIDs.data[0];
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    viewportSize = glm::vec2(viewportPanelSize.x, viewportPanelSize.y);
    //RPR_WARN("viewportPanelSize: %d, %d", (i32)viewportPanelSize.x, (i32)viewportPanelSize.y);
    ImGui::Image((ImTextureID)textureID, ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::PopStyleVar();

    ImGui::End(); 
    
}