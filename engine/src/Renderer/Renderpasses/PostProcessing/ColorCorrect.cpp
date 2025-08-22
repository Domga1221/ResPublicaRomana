#include "ColorCorrect.hpp"

#include <string>

#include "Renderer/Shader.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer/Primitives.hpp"

#include <glad/glad.h>

static Shader colorCorrectShader;

void ColorCorrect_Initialize() {
    std::string currentDir = Filesystem_GetCWD();
    std::string v = currentDir + "/Assets/Shaders/debug_quad.vert";
    std::string f = currentDir + "/Assets/Shaders/PostProcessing/ColorCorrect/ColorCorrect.frag";
    Shader_Create(&colorCorrectShader, v.c_str(), f.c_str());
}

void ColorCorrect_Render(Framebuffer* framebuffer) {
    glBindTexture(GL_TEXTURE_2D, framebuffer->colorIDs.data[0]);
    glActiveTexture(GL_TEXTURE0);
    Shader_Bind(&colorCorrectShader);
    Shader_SetInt(&colorCorrectShader, "u_texture", 0);

    Primitives_RenderQuad();
}