#pragma once 

#include "defines.hpp"

#include "Application.hpp"

#ifdef RPRPLATFORM_WINDOWS

extern Application* CreateApplication();

int main(int argc, char**argv) {
    Application* app = CreateApplication();
    Application_Initialize(app);
    Application_Run(app);
    delete app;
}

#endif 