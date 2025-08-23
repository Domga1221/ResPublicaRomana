#include "GBuffer.hpp"

#include "Core/Log.hpp"
#include "Platform/Filesystem.hpp"

#include "Renderer/Shader.hpp"
#include "Renderer/Framebuffer.hpp"
#include <Renderer/RenderCommand.hpp>
#include <Renderer/Primitives.hpp>

#include <glad/glad.h>
#include <entt/entt.hpp>

#include <random>

#include <Scene/Components.hpp>

u32 attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

float lerp(float a, float b, float f) {
	return a + f * (b - a);
}

Shader debugQuadShader;
void GBuffer_Initialize(GBuffer* gBuffer) {
    std::string currentDir = Filesystem_GetCWD();

    std::string v = currentDir + "/Assets/Shaders/PostProcessing/SSAO/g_buffer.vert";
    std::string f = currentDir + "/Assets/Shaders/PostProcessing/SSAO/g_buffer.frag";
    Shader_Create(&gBuffer->gBufferShader, v.c_str(), f.c_str());

    v = currentDir + "/Assets/Shaders/debug_quad.vert";
    f = currentDir + "/Assets/Shaders/PostProcessing/SSAO/ssao.frag";
    Shader_Create(&gBuffer->SSAOShader, v.c_str(), f.c_str());

    v = currentDir + "/Assets/Shaders/debug_quad.vert";
    f = currentDir + "/Assets/Shaders/PostProcessing/SSAO/ssaoBlur.frag";
    Shader_Create(&gBuffer->SSAOBlurShader, v.c_str(), f.c_str());

    //
    v = currentDir + "/Assets/Shaders/square.vert";
    f = currentDir + "/Assets/Shaders/square.frag";
    Shader_Create(&debugQuadShader, v.c_str(), f.c_str());
    //

    // generate gBuffer Framebuffer
    // TODO: Make framebuffer compatible with this, support RGBA8 etc.
    /*
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA8);
    */

    glGenFramebuffers(1, &gBuffer->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->gBuffer);


    // position color buffer
    glGenTextures(1, &gBuffer->gPosition);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->gPosition, 0);

    // normal color buffer
    glGenTextures(1, &gBuffer->gNormal);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer->gNormal, 0);

    // color and specular buffer
    glGenTextures(1, &gBuffer->gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer->gAlbedo, 0);
    glDrawBuffers(3, attachments);

    // depth buffer
    glGenRenderbuffers(1, &gBuffer->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gBuffer->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gBuffer->viewportSize.x, gBuffer->viewportSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer->rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: gBuffer Framebuffer Incomplete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // generate kernel 
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; i++) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = float(i) / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale); // bias around current fragment
        sample *= scale;
        gBuffer->ssaoKernel.push_back(sample);
    }

    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        gBuffer->ssaoNoise.push_back(noise);
    }


    // generate noise texture
    glGenTextures(1, &gBuffer->noiseTexture);
    glBindTexture(GL_TEXTURE_2D, gBuffer->noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &gBuffer->ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);


    // generate SSAO Framebuffer
    glGenFramebuffers(1, &gBuffer->ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->ssaoFBO);

    glGenTextures(1, &gBuffer->ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, gBuffer->ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->ssaoColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: SSAO Framebuffer Incomplete")
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    // generate blur Framebuffer
    glGenFramebuffers(1, &gBuffer->ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->ssaoBlurFBO);
    glGenTextures(1, &gBuffer->ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, gBuffer->ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->ssaoBlurColorBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: SSAO Blur Framebuffer Incomplete");
    }
}

void GBuffer_OnResize(GBuffer* gBuffer, u32 width, u32 height) {
    
    gBuffer->viewportSize.x = width;
    gBuffer->viewportSize.y = height;
    glDeleteFramebuffers(1, &gBuffer->gBuffer);
    glDeleteTextures(1, &gBuffer->gPosition);
    glDeleteTextures(1, &gBuffer->gNormal);
    glDeleteTextures(1, &gBuffer->gAlbedo);
    glDeleteRenderbuffers(1, &gBuffer->rboDepth);

    glDeleteFramebuffers(1, &gBuffer->ssaoFBO);
    glDeleteTextures(1, &gBuffer->noiseTexture);
    glDeleteTextures(1, &gBuffer->ssaoColorBuffer);

    glDeleteFramebuffers(1, &gBuffer->ssaoBlurFBO);
    glDeleteTextures(1, &gBuffer->ssaoBlurColorBuffer);



    // TODO: Put into own function, Think this through
    glGenFramebuffers(1, &gBuffer->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->gBuffer);


    // position color buffer
    glGenTextures(1, &gBuffer->gPosition);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->gPosition, 0);

    // normal color buffer
    glGenTextures(1, &gBuffer->gNormal);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer->gNormal, 0);

    // color and specular buffer
    glGenTextures(1, &gBuffer->gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gBuffer->gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer->gAlbedo, 0);
    glDrawBuffers(3, attachments);

    // depth buffer
    glGenRenderbuffers(1, &gBuffer->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gBuffer->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gBuffer->viewportSize.x, gBuffer->viewportSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer->rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: gBuffer Framebuffer Incomplete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    // generate noise texture
    glGenTextures(1, &gBuffer->noiseTexture);
    glBindTexture(GL_TEXTURE_2D, gBuffer->noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &gBuffer->ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);


    // generate SSAO Framebuffer
    glGenFramebuffers(1, &gBuffer->ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->ssaoFBO);

    glGenTextures(1, &gBuffer->ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, gBuffer->ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->ssaoColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: SSAO Framebuffer Incomplete")
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    // generate blur Framebuffer
    glGenFramebuffers(1, &gBuffer->ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->ssaoBlurFBO);
    glGenTextures(1, &gBuffer->ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, gBuffer->ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gBuffer->viewportSize.x, gBuffer->viewportSize.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer->ssaoBlurColorBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("GBuffer_Initialize: SSAO Blur Framebuffer Incomplete");
    }
}

