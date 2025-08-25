#pragma once 

#include "defines.hpp"

#include "Renderpass.hpp"

void SkyboxRenderpass_Initialize(Renderpass* renderpass);
void SkyboxRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties);
void SkyboxRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height);
void SkyboxRenderpass_Shutdown(Renderpass* renderpass);