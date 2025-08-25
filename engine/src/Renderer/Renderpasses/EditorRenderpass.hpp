#include "Renderpass.hpp"

#include "defines.hpp"

RPR_API void EditorRenderpass_Initialize(Renderpass* renderpass);
RPR_API void EditorRenderpass_Render(Renderpass* renderpass, RenderProperties* renderProperties);
RPR_API void EditorRenderpass_Resize(Renderpass* renderpass, u32 width, u32 height);
RPR_API void EditorRenderpass_Shutdown(Renderpass* renderpass);