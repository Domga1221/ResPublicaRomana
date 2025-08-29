#include "Application.hpp"

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Window.hpp"
#include "Memory/Memory.hpp"
#include "Core/LayerStack.hpp"
#include "ImGuiLayer.hpp"

static Application* application;
static Layer* imGuiLayer;

#include "Memory/List.hpp"
#include "Memory/List_Test.hpp"


#include "Event.hpp"
#include "Core/Input.hpp"
#include "Core/MouseCodes.hpp"
#include "Core/Keycodes.hpp"
void Application_OnKeyPressed(Event event) {
    u16 keyPressed = event.KeyPressed;
    //RPR_INFO("Key pressed: %d", keyPressed);
    if(keyPressed == RPR_KEY_ESCAPE) {
        Event quitEvent = {};
        Event_Fire(EVENT_TYPE_APPLICATION_QUIT, quitEvent);
    }
}
void Application_OnMouseButtonPressed(Event event) {
    //u16 mouseButton = event.MouseButtonPressed;
    //RPR_INFO("Mouse button pressed %d", mouseButton);
}
void Application_OnQuit(Event event) {
    RPR_DEBUG("Application_OnQuit received");
    application->isRunning = false;
}

#include "Platform/Filesystem.hpp"
#include "Renderer/Primitives.hpp"

#include "Renderer/ParticleSystem/Random.hpp"
void Application_Initialize(Application* app) {
    application = app;
    app->isRunning = true;

    MEMORY_Initialize();
    LayerStack_Initialize();
    Event_Initialize();
    Filesystem_Initialize();
    Random_Initialize();
    
    WindowProps windowProps { "REngine", 1600, 900 };
    Window_Initialize(&windowProps);
    
    
    std::string memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    
    imGuiLayer = (Layer*)MEMORY_Allocate(sizeof(Layer), MEMORY_TAG_LAYER);
    ImGuiLayer_Initialize(imGuiLayer); 
    // right now imguilayer is pushed as first layer into stack, then sandbox editorlayer is pushed
    LayerStack_PushLayer(imGuiLayer);
    
    
    // event 
    Event_AddListener(EVENT_TYPE_KEY_PRESSED, Application_OnKeyPressed);
    Event_AddListener(EVENT_TYPE_MOUSE_BUTTON_PRESSED, Application_OnMouseButtonPressed);
    Event_AddListener(EVENT_TYPE_APPLICATION_QUIT, Application_OnQuit);
    
    // Renderer
    Primitives_Initialize();
}

f32 lastFrameTime = 0.0f;
void Application_Run(Application* app) {
    std::string memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    //return;
    while(app->isRunning) {
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        f32 time = Window_GetTime();
        float deltaTime = time - lastFrameTime;
        lastFrameTime = time;
        

        if(Input_IsKeyPressed(RPR_KEY_A)) {
            Event keyPressedEvent;
            keyPressedEvent.KeyPressed = RPR_KEY_A;
            Event_Fire(EVENT_TYPE_KEY_PRESSED, keyPressedEvent);
        }

        if(Input_IsMouseButtonPressed(RPR_MOUSE_BUTTON_1)) {
            Event mouseButtonPressedEvent;
            mouseButtonPressedEvent.MouseButtonPressed = RPR_MOUSE_BUTTON_1;
            Event_Fire(EVENT_TYPE_MOUSE_BUTTON_PRESSED, mouseButtonPressedEvent);
        }   

        LayerStack_Update(deltaTime);

        ImGuiContext* context = ImGuiLayer_Begin();
        LayerStack_ImGuiRender(context);
        ImGuiLayer_End();

        Window_Update();
        if(Window_ShouldClose())
            app->isRunning = false;
    }

    app->isRunning = false;

    Filesystem_Shutdown();
    Event_Shutdown();
    LayerStack_Shutdown();
    MEMORY_Shutdown();
    // Random_Shutdown();
}

RPR_API void Application_PushLayer(Layer* layer) {
    RPR_DEBUG("Application_PushLayer called");
    LayerStack_PushLayer(layer);   
}