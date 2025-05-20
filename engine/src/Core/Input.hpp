#pragma once 

#include "defines.hpp"

#include "Keycodes.hpp"
#include "MouseCodes.hpp"

void Input_Initialize();
void Input_Shutdown();

RPR_API inline b8 Input_IsKeyPressed(u32 keycode);
RPR_API inline b8 Input_IsMouseButtonPressed(u16 button);
RPR_API inline b8 Input_IsMouseButtonReleased(u16 button);
RPR_API inline f32 Input_GetMouseX();
RPR_API inline f32 Input_GetMouseY();
// TODO: Key Release

