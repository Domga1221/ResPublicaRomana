#include "defines.hpp"

struct Application {
    bool isRunning;
};

RPR_API void Application_Initialize(Application* app);
RPR_API void Application_Run(Application* app);