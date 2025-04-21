#include "Window.hpp"

#include <glad/glad.h> // TODO: move glad initialization out of window
#include <GLFW/glfw3.h>

#include "Core/Log.hpp"

struct Window {
    i32 width; 
    i32 height;
    std::string title;
    GLFWwindow* glfwWindow;
};

static Window window;
static b8 glfwInitialized = false;

void Window_Initialize(WindowProps* windowProps) {
    if(glfwInitialized) {
        RPR_WARN("Window_Initialize: called twice, returning...");
    }

    window.width = windowProps->width;
    window.height = windowProps->height;
    window.title = windowProps->title;

    RPR_INFO("Creating a window %s, (%d, %d)", windowProps->title, windowProps->width, windowProps->height);

    int success = glfwInit();
    if(!success) {
        RPR_CRITICAL("Could not initialize GLFW!");
        return;
    }

    glfwInitialized = true;

    //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    window.glfwWindow = glfwCreateWindow((int)window.width, (int)window.height, window.title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window.glfwWindow);

    glfwSetWindowUserPointer(window.glfwWindow, (void*)&window);

    
    // initialize glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        RPR_CRITICAL("Window_Initialize: Failed to initialize glad");
        return;
    }
}

void Window_Shutdown() {
    glfwDestroyWindow(window.glfwWindow);
    glfwTerminate();
}

bool Window_ShouldClose() {
    return glfwWindowShouldClose(window.glfwWindow);
}

void Window_Update() {
    glfwPollEvents();
    glfwSwapBuffers(window.glfwWindow);
}

GLFWwindow* Window_GetGLFWWindowPtr() {
    return window.glfwWindow;
}

f32 Window_GetTime() {
    return (f32)glfwGetTime();
}