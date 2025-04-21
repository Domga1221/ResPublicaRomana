#pragma once 

#include "defines.hpp"

struct WindowProps {
    const char* title;
    u32 width; 
    u32 height; 
};

void Window_Initialize(WindowProps* windowProps);

void Window_Shutdown();

bool Window_ShouldClose();

void Window_Update();

typedef struct GLFWwindow GLFWwindow;
GLFWwindow* Window_GetGLFWWindowPtr();
f32 Window_GetTime(); // TODO: should not be in window, but I don't want to include GLFW in Application.cpp