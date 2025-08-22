#pragma once 

#include "defines.hpp"

#include "Renderer/Framebuffer.hpp"

void RPR_API ColorCorrect_Initialize();
// TODO: Shutdown

void RPR_API ColorCorrect_Render(Framebuffer* framebuffer);