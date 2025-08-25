#pragma once 

#include "defines.hpp"

#include "Renderer/Renderpasses/Renderpass.hpp"

RPR_API void SSAO_Initialize(Renderpass* renderpass);
RPR_API void SSAO_Render(Renderpass* renderpass, RenderProperties* renderProperties);
RPR_API void SSAO_Resize(Renderpass* renderpass, u32 width, u32 height);
RPR_API void SSAO_Shutdown(Renderpass* renderpass);