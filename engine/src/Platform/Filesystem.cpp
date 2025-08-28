#include "Filesystem.hpp"

#include <Windows.h>
#include <commdlg.h>

#include "Core/Window.hpp"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


std::string Filesystem_OpenFile(const char* filter) {
    OPENFILENAMEA ofn;
    CHAR szFile[256] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(Window_GetGLFWWindowPtr());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return std::string();
}


std::string Filesystem_SaveFile(const char* filter) {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(Window_GetGLFWWindowPtr());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetSaveFileNameA(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return std::string();
}

#include <Core/Log.hpp>
#include <filesystem>
static std::string CWD; // TODO: string 
void Filesystem_Initialize() {
    CWD = std::filesystem::current_path().string();
    RPR_INFO("Filesystem_Initialize: current path: %s", CWD);
}

void Filesystem_Shutdown() {

}

const char* Filesystem_GetCWD() {
    return CWD.c_str();
}