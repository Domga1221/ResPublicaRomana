#include "Application.hpp"

#include "print.hpp"

void Application_Initialize(Application* app) {
    int a = 5;
    print(a);

    app->isRunning = true;
}

void Application_Run(Application* app) {
    while(app->isRunning) {
        
    }
}