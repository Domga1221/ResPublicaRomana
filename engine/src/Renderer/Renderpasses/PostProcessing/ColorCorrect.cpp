#include "ColorCorrect.hpp"

#include <string>

#include "Renderer/Shader.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer/Primitives.hpp"
#include "Renderer/RenderCommand.hpp"

#include <glad/glad.h>

static Shader colorCorrectShader;

void ColorCorrect_Initialize(Renderpass* renderpass) {
    std::string currentDir = Filesystem_GetCWD();
    std::string v = currentDir + "/Assets/Shaders/debug_quad.vert";
    std::string f = currentDir + "/Assets/Shaders/PostProcessing/ColorCorrect/ColorCorrect.frag";
    Shader_Create(&colorCorrectShader, v.c_str(), f.c_str());
}

void ColorCorrect_Render(Renderpass* renderpass, RenderProperties* renderProperties) {
    RenderCommand_BindTexture2D(renderProperties->framebuffer->colorIDs.data[0]);
    RenderCommand_ActiveTexture(0);
    Shader_Bind(&colorCorrectShader);
    Shader_SetInt(&colorCorrectShader, "u_texture", 0);

    Primitives_RenderQuad();
}

void ColorCorrect_Resize(Renderpass* renderpass, u32 width, u32 height) {
    
}

void ColorCorrect_Shutdown(Renderpass* renderpass) {
    Shader_Destroy(&colorCorrectShader);
}
