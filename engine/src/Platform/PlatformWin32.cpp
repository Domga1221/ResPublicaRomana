#include "Platform.hpp"

#ifdef RPRPLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>

#include <iostream>

void Platform_ConsoleLog(const char* message, u8 logLevel) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
                        //   CRITICAL,    ERROR,      WARN,   INFO,   DEBUG,  TRACE
                        //  RED+BLACK,      RED,    YELLOW,  GREEN,    BLUE,  WHITE
    static u8 colors[6] = {        64,        4,         6,      2,       1,  7};


    SetConsoleTextAttribute(handle, colors[logLevel]);
    OutputDebugStringA(message);
    u64 stringLength = strlen(message);
    LPDWORD written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)stringLength, written, 0);
}

void Platform_ConsoleLogError(const char* message, u8 logLevel) {
    HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
                        //   CRITICAL,    ERROR,      WARN,   INFO,   DEBUG,  TRACE
                        //  RED+BLACK,      RED,    YELLOW,  GREEN,    BLUE,  WHITE
    static u8 colors[6] = {        64,        4,         6,      2,       1,  7};


    SetConsoleTextAttribute(handle, colors[logLevel]);
    OutputDebugStringA(message);
    u64 stringLength = strlen(message);
    LPDWORD written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)stringLength, written, 0);
}

#endif