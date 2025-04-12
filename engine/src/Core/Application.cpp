#include "Application.hpp"

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Window.hpp"
#include "Memory/Memory.hpp"
#include "Memory/Vector.hpp"

Vector<i32> testVector;

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
}

void Application_Run(Application* app) {
    const char* memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    return;
    while(app->isRunning) {
        
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Window_Update();
        if(Window_ShouldClose())
            app->isRunning = false;
    }
}