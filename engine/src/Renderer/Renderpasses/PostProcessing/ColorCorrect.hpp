#pragma once 

#include "defines.hpp"

#include "Renderer/Renderpasses/Renderpass.hpp"
#include "Renderer/Framebuffer.hpp"

RPR_API void ColorCorrect_Initialize(Renderpass* renderpass);
RPR_API void ColorCorrect_Render(Renderpass* renderpass, RenderProperties* renderProperties);
RPR_API void ColorCorrect_Resize(Renderpass* renderpass, u32 width, u32 height);
RPR_API void ColorCorrect_Shutdown(Renderpass* renderpass);