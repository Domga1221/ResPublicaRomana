#include "Renderpass.hpp"

#include "defines.hpp"

RPR_API void PBRRenderpass_Initialize(Renderpass* renderpass);
RPR_API void PBRRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties);
RPR_API void PBRRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height);
RPR_API void PBRRenderpass_Shutdown(Renderpass* renderpass);