#include "EditorLayer.hpp"

#include <Core/ClientLog.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// UI
#include <imgui-docking/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include "UI/ContentBrowserPanel.hpp"
#include "UI/SceneHierarchyPanel.hpp"
#include "UI/InspectorPanel.hpp"
#include "UI/PostProcessingPanel.hpp"

// Renderer
#include <Renderer/Framebuffer.hpp>
#include <Renderer/RenderCommand.hpp>
#include "Renderer/Texture.hpp"
static Framebuffer framebuffer;

// SceneCamera
#include "Scene/SceneCamera.hpp"
static SceneCamera sceneCamera;
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
static f32 lastMouseX = 1280 / 2;
static f32 lastMouseY = 720 / 2;
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

// Scene
#include <Scene/GameObject.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Components.hpp>
#include "Scene/EditorScene.hpp"
#include "Scene/ShaderPool.hpp"
#include "Scene/SceneSerialization.hpp"
static Scene activeScene;

// Filesystem
#include <Platform/Filesystem.hpp>
void saveSceneAs();
void openScene();


static glm::vec2 viewportSize = glm::vec2(1280, 720);
static glm::vec2 viewportBounds[2];
static glm::vec2 viewportMousePosition;

static Texture playButton;
static Texture stopButton;
static b8 playMode = false;


void EditorLayer_OnAttach() {
    RPR_CLIENT_INFO("Hello from EditorLayer");
    const char* currentPath = Filesystem_GetCWD();
    RPR_CLIENT_INFO("%s", currentPath);


    // Scene
    Scene_Create(&activeScene);

    EditorScene_Initialze();

    // UI
    ContentBrowserPanel_Initialize();
    SceneHierarchyPanel_Initialize(&activeScene);
    PostProcessingPanel_Initialize(); // Does nothing currently
    InspectorPanel_Initialize(); // TODO: move up
    

    // Framebuffer
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RED_INTEGER);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_DEPTH);
    framebufferProperties.width = 1280;
    framebufferProperties.height = 720;

    Framebuffer_Create(&framebuffer, &framebufferProperties);


    SceneCamera_Create(&sceneCamera, glm::vec3(1.0f, 0.5f, -2.0f));

    /*
    GameObject* g1 = GameObject_Create(&activeScene);
    std::string meshPath = "Assets/Models/cube_own.obj";
    g1->AddComponent<MeshComponent>();
    g1->AddComponent<MaterialComponent>(ShaderPool_GetEditorShader());
    */

    Texture_Create(&playButton, "Resources/PlayButton64.png");
    Texture_Create(&stopButton, "Resources/StopButton64.png");
}

void EditorLayer_OnDetach() {
    EditorScene_Shutdown();

    InspectorPanel_Shutdown();
    PostProcessingPanel_Shutdown();
    ContentBrowserPanel_Shutdown();

    Framebuffer_Destroy(&framebuffer);
}

void EditorLayer_OnUpdate(f32 deltaTime) {

    // resize
    if(viewportSize.x > 0.0f && viewportSize.y > 0.0f && 
            (framebuffer.framebufferProperties.width != (u32)viewportSize.x
            || framebuffer.framebufferProperties.height != (u32)viewportSize.y)) {
        Framebuffer_Resize(&framebuffer, (u32)viewportSize.x, (u32)viewportSize.y);
        SceneCamera_SetViewportSize(&sceneCamera, (u32)viewportSize.x, (u32)viewportSize.y); 
        //__builtin_debugtrap();
        // TODO: handle resizing properly with events 
        EditorScene_OnViewportResize((u32)viewportSize.x, (u32)viewportSize.y);
    }

    // -- Input
    static bool pressed = false;
    if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_1) && pressed == false) {
        
    }
    if(Input_IsMouseButtonReleased(RPR_MOUSE_BUTTON_1))
        pressed = false;
    if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_3)) {
        sceneCamera.position = glm::vec3(0.0f, 0.5f, -1.0f);
        sceneCamera.front = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 front;
        front.x = cos(glm::radians(sceneCamera.yaw)) * cos(glm::radians(sceneCamera.pitch));
        front.y = sin(glm::radians(sceneCamera.pitch));
        front.z = sin(glm::radians(sceneCamera.yaw)) * cos(glm::radians(sceneCamera.pitch));
        sceneCamera.front = glm::normalize(front);
        sceneCamera.right = glm::normalize(glm::cross(sceneCamera.front, sceneCamera.worldUp));
        sceneCamera.up = glm::normalize(glm::cross(sceneCamera.right, sceneCamera.front));
        RPR_WARN("%f, %f, %f", sceneCamera.position.x, sceneCamera.position.y, sceneCamera.position.z);
    }
    handleSceneCameraMovement(deltaTime);
    handleSceneCameraMouseMovement();


    // -- Rendering
    RenderCommand_EnableDepthTest(true);
    
    Framebuffer_Bind(&framebuffer);
    glm::vec4 color = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f);
    RenderCommand_SetClearColor(&color);
    RenderCommand_Clear(true, true);

    // -- EditorScene
    Framebuffer_ClearAttachment(&framebuffer, 1, -1);
    if(!playMode)
        EditorScene_OnUpdateEditor(deltaTime, &activeScene, &sceneCamera, &framebuffer);
    else 
        EditorScene_OnUpdateRuntime(deltaTime, &activeScene, &sceneCamera, &framebuffer,
            PostProcessingPanel_GetBloom(), PostProcessingPanel_GetSSAO(), PostProcessingPanel_GetColorCorrect(), 
            playMode);

    // -- Mousepicking
    i32 mouseX = viewportMousePosition.x;
    i32 mouseY = viewportMousePosition.y;
    if(mouseX >= 0 && mouseY >= 0 && mouseX < (i32)viewportSize.x && mouseY < (i32)viewportSize.y && !playMode) {
        i32 pixelData = Framebuffer_ReadPixel(&framebuffer, 1, mouseX, mouseY);
        if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_1)) {
            for(u32 i = 0; i < activeScene.root->children.size; i++) {
                if((u32)activeScene.root->children.data[i]->handle == pixelData) {
                    SceneHierarchyPanel_SetSelectedGameObject(activeScene.root->children.data[i]); // TODO: event system 
                    InspectorPanel_SetSelectedGameObject(activeScene.root->children.data[i]);
                }
            }
        }
    }

    Framebuffer_Unbind();
    RenderCommand_EnableDepthTest(false);


    if(Input_IsKeyPressed(RPR_KEY_ESCAPE)) {
        std::exit(0);    // TODO: Do this properly
    }
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
    float minWinSizeX = 370.0f;
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {    
            if (ImGui::MenuItem("New", "Ctrl+N")) RPR_INFO("New");
            if (ImGui::MenuItem("Open...", "Ctrl+O")) openScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) saveSceneAs();
    
            if (ImGui::MenuItem("Exit")) RPR_INFO("Exit");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    
    // Panels
    ContentBrowserPanel_OnImGuiRender();
    SceneHierarchyPanel_OnImGuiRender();
    InspectorPanel_OnImGuiRender();
    PostProcessingPanel_OnImGuiRender();


    // toolbar
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{ 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    const auto& buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5));

    ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoTitleBar
        //ImGuiWindowFlags_None
    );

    float size = ImGui::GetWindowHeight() - 4.0f;
    Texture* icon = playMode ? &stopButton : &playButton;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    if (ImGui::ImageButton("PlayButton", (ImTextureID)icon->ID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1))) {
        playMode = !playMode;
        const char* state = playMode ? "Play" : "Edit";
        RPR_INFO("Scene State Changed: %s", state);
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();


    // viewport 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2 viewportOffset = ImGui::GetWindowPos();
    viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
    
    // TODO: for some reason clear color of framebuffer color attachment is not rendered in imgui image 
    u32 textureID = framebuffer.colorIDs.data[0];
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    viewportSize = glm::vec2(viewportPanelSize.x, viewportPanelSize.y);
    //RPR_WARN("viewportPanelSize: %d, %d", (i32)viewportPanelSize.x, (i32)viewportPanelSize.y);
    ImGui::Image((ImTextureID)textureID, ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::PopStyleVar();
    
    // Mouse position
    ImVec2 mousePos = { 0.0f, 0.0f };
    if(ImGui::GetCurrentContext() != nullptr)
        mousePos = ImGui::GetMousePos();
    float mouseX = mousePos.x - viewportBounds[0].x;
    float mouseY = mousePos.y - viewportBounds[0].y;
    glm::vec2 viewportSize = viewportBounds[1] - viewportBounds[0];
    mouseY = viewportSize.y - mouseY;
    
    viewportMousePosition = glm::vec2(mouseX, mouseY); 


    // Gizmos
    // TODO:
    /*
    GameObject* selected = SceneHierarchyPanel_GetSelectedGameObject();
    if(selected != nullptr) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::AllowAxisFlip(false);
        ImGuizmo::SetDrawlist();
        f32 windowWidth = ImGui::GetWindowWidth();
        f32 windowHeight = ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        //ImGuizmo::SetOwnerWindowName();
        glm::mat4 view = SceneCamera_GetViewMatrix(&sceneCamera);
        glm::mat4 projection = SceneCamera_GetProjectionMatrixRH(&sceneCamera);
        TransformComponent& tc = selected->GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();
        ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(transform), 1);
        if(ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), 
            ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform)))
            tc.position = glm::vec3(transform[3]);
            //RPR_WARN("x: %f, y: %f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        //if(ImGuizmo::IsUsing()) {
        //}
    }
    */
    ImGui::End(); 
    
}

void openScene() {
    std::string filepath = Filesystem_OpenFile("Scene (*.scene)\0*.scene\0");
    if(filepath.empty()) {
        RPR_CLIENT_ERROR("openScene: filepath is empty");
        return;
    }

    // TODO: maybe return scene as pointer or something in creation and free it in destroy as well 
    Scene_Destroy(&activeScene);
    Scene_Create(&activeScene);
    Scene_Deserialize(&activeScene, filepath);

    SceneHierarchyPanel_SetSelectedGameObject(nullptr); // TODO: event system 
    InspectorPanel_SetSelectedGameObject(nullptr);
}

void saveSceneAs() {
    std::string filepath = Filesystem_SaveFile("Scene (*.scene)\0*.scene\0"); // TODO:
    filepath += ".scene";
    if(filepath.empty()) {
        RPR_CLIENT_ERROR("saveSceneAs: filepath is empty");
        return;
    }

    RPR_CLIENT_INFO("saveSceneAs: filepath: %s", filepath.c_str());
    Scene_Serialize(&activeScene, filepath);
}