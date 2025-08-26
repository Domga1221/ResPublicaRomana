#pragma once 

#include "defines.hpp"

#include "Renderpass.hpp"

RPR_API void ParticleSystemRenderpass_Initialize(Renderpass* renderpass);
RPR_API void ParticleSystemRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties);
RPR_API void ParticleSystemRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height);
RPR_API void ParticleSystemRenderpass_Shutdown(Renderpass* renderpass);