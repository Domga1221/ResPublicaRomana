#include "defines.hpp"

#include "Layer.hpp"

struct Application {
    bool isRunning;
};

RPR_API void Application_Initialize(Application* app);
RPR_API void Application_Run(Application* app);

RPR_API void Application_PushLayer(Layer* layer);