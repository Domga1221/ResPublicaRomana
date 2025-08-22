#include "Bloom.hpp"

#include <glad/glad.h>

#include <Platform/Filesystem.hpp>
#include <Renderer/RenderCommand.hpp>
#include <Renderer/Primitives.hpp>

static Shader debugQuadShader;
void Bloom_Initialize(Bloom* bloom) {
    // setup framebuffers and textures 
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_DEPTH);
    framebufferProperties.width = 1600;
    framebufferProperties.height = 900;
    Framebuffer_Create(&bloom->framebuffer, &framebufferProperties);
    // additional parameters
    glBindTexture(GL_TEXTURE_2D, bloom->framebuffer.colorIDs.data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // guassian blur 
    Framebuffer_Create(&bloom->pingPongFramebuffers[0], &framebufferProperties);
    glBindTexture(GL_TEXTURE_2D, bloom->pingPongFramebuffers[0].colorIDs.data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    Framebuffer_Create(&bloom->pingPongFramebuffers[1], &framebufferProperties);
    glBindTexture(GL_TEXTURE_2D, bloom->pingPongFramebuffers[1].colorIDs.data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    Framebuffer_Create(&bloom->combineFramebuffer, &framebufferProperties);

    glBindTexture(GL_TEXTURE_2D, 0);


    // shaders
    std::string currentDir = Filesystem_GetCWD();

    std::string v = currentDir + "/Assets/Shaders/debug_quad.vert";
    std::string f = currentDir + "/Assets/Shaders/PostProcessing/Bloom/brightPixel.frag";
    Shader_Create(&bloom->brightPixelShader, v.c_str(), f.c_str());

    v = currentDir + "/Assets/Shaders/debug_quad.vert";
    f = currentDir + "/Assets/Shaders/PostProcessing/Bloom/gaussianBlur.frag";
    Shader_Create(&bloom->blurShader, v.c_str(), f.c_str());

    v = currentDir + "/Assets/Shaders/debug_quad.vert";
    f = currentDir + "/Assets/Shaders/PostProcessing/Bloom/combine.frag";
    Shader_Create(&bloom->combineShader, v.c_str(), f.c_str());

    v = currentDir + "/Assets/Shaders/debug_quad.vert";
    f = currentDir + "/Assets/Shaders/debug_quad.frag";
    Shader_Create(&debugQuadShader, v.c_str(), f.c_str());
}

void Bloom_Shutdown() {

}

void Bloom_Render(Bloom* bloom, Framebuffer* framebuffer) {
    Bloom_RenderToTexture(bloom, framebuffer->colorIDs.data[0]);
    Framebuffer_Bind(framebuffer);
    glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, bloom->combineFramebuffer.colorIDs.data[0]);
    glBindTexture(GL_TEXTURE_2D, bloom->framebuffer.colorIDs.data[0]);
    Shader_Bind(&debugQuadShader);
    Shader_SetInt(&debugQuadShader, "u_texture", 0);
    Primitives_RenderQuad();
}

void Bloom_OnResize(Bloom* bloom, u32 x, u32 y) {
    // framebuffers should all be the same size
    const FramebufferProperties& spec = bloom->framebuffer.framebufferProperties;
    if ((spec.width != x || spec.height != y)) {
        Framebuffer_Resize(&bloom->framebuffer, x, y);
        Framebuffer_Resize(&bloom->pingPongFramebuffers[0], x, y);
        Framebuffer_Resize(&bloom->pingPongFramebuffers[1], x, y);
        Framebuffer_Resize(&bloom->combineFramebuffer, x, y);
    }
}

void Bloom_RenderToTexture(Bloom* bloom, const u32 hdrTexture) {
    Framebuffer_Bind(&bloom->framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader_Bind(&bloom->brightPixelShader);
    RenderCommand_ActiveTexture(0);
    RenderCommand_BindTexture2D(hdrTexture);
    Shader_SetInt(&bloom->brightPixelShader, "hdrTexture", 0);
    Primitives_RenderQuad();

    /*
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // gussian blur
    bool horizontal = true, first_iteration = true;
    int amount = 10;
    Shader_Bind(&bloom->blurShader);
    Shader_SetInt(&bloom->blurShader, "image", 0);
    for(u32 i = 0; i < amount; i++) {
        Framebuffer_Bind(&bloom->pingPongFramebuffers[horizontal]);
        
        //https://computergraphics.stackexchange.com/questions/3912/ping-pong-between-same-framebuffer-two-color-attachments
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader_SetInt(&bloom->blurShader, "horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D,
            first_iteration ? bloom->framebuffer.colorIDs.data[0] : 
            bloom->pingPongFramebuffers[!horizontal].colorIDs.data[0]);                
        Primitives_RenderQuad();
        horizontal = !horizontal;
        if(first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // combine
    Framebuffer_Bind(&bloom->combineFramebuffer);

    Shader_Bind(&bloom->combineShader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    Shader_SetInt(&bloom->combineShader, "scene", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloom->pingPongFramebuffers[!horizontal].colorIDs.data[0]);
    Shader_SetInt(&bloom->combineShader, "bloomBlur", 1);
    float exposure = 1.0f;
    Shader_SetFloat(&bloom->combineShader, "exposure", exposure);
    Primitives_RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
}