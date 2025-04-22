#include "Input.hpp"

#include "Window.hpp"
#include "Core/Log.hpp"

#include <GLFW/glfw3.h>

void Input_Initialize() {

}

void Input_Shutdown() {

}

b8 Input_IsKeyPressed(u32 keycode) {
    GLFWwindow* window = Window_GetGLFWWindowPtr();
    i32 state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

b8 Input_IsMouseButtonPressed(u16 button) {
    GLFWwindow* window = Window_GetGLFWWindowPtr();
    i32 state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

f32 Input_GetMouseX() {
    GLFWwindow* window = Window_GetGLFWWindowPtr();
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return (f32)xPos;
}

f32 Input_GetMouseY() {
    GLFWwindow* window = Window_GetGLFWWindowPtr();
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return (f32)yPos;
}

