#include "Application.hpp"

#include <glad/glad.h>
#include "Core/Window.hpp"

void Application_Initialize(Application* app) {

    WindowProps windowProps { "title", 1280, 720 };
    Window_Initialize(&windowProps);

    app->isRunning = true;
}

void Application_Run(Application* app) {
    while(app->isRunning) {
        
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Window_Update();
        if(Window_ShouldClose())
            app->isRunning = false;
    }
}