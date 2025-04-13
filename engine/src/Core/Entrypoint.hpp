#pragma once 

#include "defines.hpp"

#include "Application.hpp"

#ifdef RPRPLATFORM_WINDOWS

// TODO: Wrap in struct, sandbox then sets function pointers of struct 
extern Application* CreateApplication();
extern void AfterAppInitialization(Application* app);

int main(int argc, char**argv) {
    Application* app = CreateApplication();
    Application_Initialize(app);
    AfterAppInitialization(app);
    Application_Run(app);
    delete app;
}

#endif 