#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

// TODO: should these be inline?

RPR_API inline void RenderCommand_SetClearColor(const glm::vec4* color) {
    glClearColor(color->r, color->g, color->b, color->a);
}

RPR_API inline void RenderCommand_Clear(b8 color, b8 depth) {
    GLbitfield mask = 0;
    if(color) mask |= GL_COLOR_BUFFER_BIT;
    if(depth) mask |= GL_DEPTH_BUFFER_BIT;
    glClear(mask);
}

RPR_API inline void RenderCommand_Draw(u32 count) {
    glDrawArrays(GL_TRIANGLES, 0, count);
}


RPR_API inline void RenderCommand_ActiveTexture(u16 textureIndex) {
    glActiveTexture(GL_TEXTURE0 + textureIndex);
}

RPR_API inline void RenderCommand_BindTexture2D(u32 textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
}