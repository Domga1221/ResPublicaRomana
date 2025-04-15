#include "Application.hpp"

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Window.hpp"
#include "Memory/Memory.hpp"
#include "Memory/Vector.hpp"
#include "Core/LayerStack.hpp"
#include "ImGuiLayer.hpp"

Vector<i32> testVector;

Layer imGuiLayer;

void Application_Initialize(Application* app) {

    MEMORY_Initialize();

    WindowProps windowProps { "title", 1280, 720 };
    Window_Initialize(&windowProps);

    app->isRunning = true;

    RPR_WARN("Vector size %d", testVector.Size());
    testVector.PushBack(5);
    RPR_WARN("Vector size %d", testVector.Size());
    testVector.PushBack(10);
    RPR_WARN("Vector size %d", testVector.Size());


    const char* memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    MEMORY_Free((void*)memoryUsageString, strlen(memoryUsageString) + 1, MEMORY_TAG_STRING);

    ImGuiLayer_Initialize(&imGuiLayer); 
    // right now imguilayer is pushed as first layer into stack, then sandbox editorlayer is pushed
    LayerStack_PushLayer(&imGuiLayer);
}

void Application_Run(Application* app) {
    const char* memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    //return;
    while(app->isRunning) {
        
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        LayerStack_Update();

        ImGuiLayer_Begin();
        LayerStack_ImGuiRender();
        ImGuiLayer_End();

        Window_Update();
        if(Window_ShouldClose())
            app->isRunning = false;
    }
}

RPR_API void Application_PushLayer(Layer* layer) {
    RPR_DEBUG("Application_PushLayer called");
    LayerStack_PushLayer(layer);   
}