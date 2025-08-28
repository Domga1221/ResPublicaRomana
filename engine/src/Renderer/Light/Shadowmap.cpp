#include "Shadowmap.hpp"

#include <glad/glad.h>

#include <string>
#include "Platform/Filesystem.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Shadowmap_Create(Shadowmap* shadowmap) {
    // TODO: Shadowmap doesn't render, when looking straight from the top at the origin of the scene
    shadowmap->nearPlane = 1.0f;
    shadowmap->farPlane = 7.5f;
    shadowmap->SHADOW_WIDTH = 2048;
    shadowmap->SHADOW_HEIGHT = 2048;

    glGenFramebuffers(1, &shadowmap->depthMapFBO);

    glGenTextures(1, &shadowmap->depthMap);
    glBindTexture(GL_TEXTURE_2D, shadowmap->depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowmap->SHADOW_WIDTH, shadowmap->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowmap->depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap->depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::string currentPath = Filesystem_GetCWD();
    std::string v = currentPath + "/Assets/Shaders/Light/shadow_mapping_depth.vert";
    std::string f = currentPath + "/Assets/Shaders/Light/shadow_mapping_depth.frag";
    Shader_Create(&shadowmap->simpleDepthShader, v.c_str(), f.c_str());

    v = currentPath + "/Assets/Shaders/debug_quad.vert";
    f = currentPath + "/Assets/Shaders/debug_quad.frag";
    Shader_Create(&shadowmap->debugDepthQuadShader, v.c_str(), f.c_str());
}

void Shadowmap_Destroy(Shadowmap* shadowmap) {
    glDeleteTextures(1, &shadowmap->depthMap);
    glDeleteFramebuffers(1, &shadowmap->depthMapFBO);

    Shader_Destroy(&shadowmap->simpleDepthShader);
    Shader_Destroy(&shadowmap->debugDepthQuadShader);
    shadowmap->SHADOW_WIDTH = 0;
    shadowmap->SHADOW_HEIGHT = 0;
}

