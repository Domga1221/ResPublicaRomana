#include "ImageBasedLighting.hpp"

#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Platform/Filesystem.hpp"
#include "Renderer/Primitives.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::mat4 captureProjection = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
static glm::mat4 captureViews[6] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};


void ImageBasedLighting_Initialize(ImageBasedLighting* ibl, const char* hdrPath) {
    
    const char* CWD = Filesystem_GetCWD();

    
    // shaders
    std::string vertex; std::string fragment;
    
    vertex = std::string(CWD) + "/Assets/Shaders/PBR/equirectangularToCubemap.vert";
    fragment = std::string(CWD) + "/Assets/Shaders/PBR/equirectangularToCubemap.frag";
    Shader_Create(&ibl->equirectangularToCubemap, vertex.c_str(), fragment.c_str());

    vertex = std::string(CWD) + "/Assets/Shaders/PBR/skybox.vert";
    fragment = std::string(CWD) + "/Assets/Shaders/PBR/skybox.frag";
    Shader_Create(&ibl->skybox, vertex.c_str(), fragment.c_str());

    vertex = std::string(CWD) + "/Assets/Shaders/PBR/equirectangularToCubemap.vert";
    fragment = std::string(CWD) + "/Assets/Shaders/PBR/irradiance_convolution.frag";
    Shader_Create(&ibl->irradiance, vertex.c_str(), fragment.c_str());

    vertex = std::string(CWD) + "/Assets/Shaders/PBR/equirectangularToCubemap.vert";
    fragment = std::string(CWD) + "/Assets/Shaders/PBR/specular_convolution_prefilter.frag";
    Shader_Create(&ibl->prefilter, vertex.c_str(), fragment.c_str());

    vertex = std::string(CWD) + "/Assets/Shaders/PBR/brdfConvolution.vert";
    fragment = std::string(CWD) + "/Assets/Shaders/PBR/brdfConvolution.frag";
    Shader_Create(&ibl->brdf, vertex.c_str(), fragment.c_str());



    // load HDR
    // TODO: own function to swap out HDR 

    std::string toLoad = CWD + std::string("/") + std::string(hdrPath);

    RPR_WARN("ImageBasedLighting_Initialize: loading HDR from: %s", toLoad);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(toLoad.c_str(), &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(false);
    if (data) {
        glGenTextures(1, &ibl->hdrTexture);
        glBindTexture(GL_TEXTURE_2D, ibl->hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        RPR_ERROR("ImageBasedLighting_Initialize: FAILED TO LOAD HDR AT: %s", toLoad);
    }


    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // setup framebuffer 
    glGenFramebuffers(1, &ibl->captureFBO);
    glGenRenderbuffers(1, &ibl->captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, ibl->captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ibl->captureRBO);
    glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ibl->captureRBO);


    // setup cubemap
    glGenTextures(1, &ibl->envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->envCubemap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // convert HDR to cubemap
    Shader_Bind(&ibl->equirectangularToCubemap);
    Shader_SetInt(&ibl->equirectangularToCubemap, "equirectangularMap", 0);
    Shader_SetMat4(&ibl->equirectangularToCubemap, "projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ibl->hdrTexture);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        Shader_SetMat4(&ibl->equirectangularToCubemap, "view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ibl->envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Primitives_RenderCube();
    }
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        RPR_ERROR("ImageBasedLighting_Initialize: Framebuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup irradiance map
    glGenTextures(1, &ibl->irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->irradianceMap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, ibl->captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


    // convolute
    Shader_Bind(&ibl->irradiance);
    Shader_SetInt(&ibl->irradiance, "environmentMap", 0);
    Shader_SetMat4(&ibl->irradiance, "projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->envCubemap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        Shader_SetMat4(&ibl->irradiance, "view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ibl->irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Primitives_RenderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup prefilter map
    glGenTextures(1, &ibl->prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->prefilterMap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable trilinear filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // capture prefilter map 
    Shader_Bind(&ibl->prefilter);
    Shader_SetInt(&ibl->prefilter, "environmentMap", 0);
    Shader_SetMat4(&ibl->prefilter, "projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, ibl->captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        Shader_SetFloat(&ibl->prefilter, "roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            Shader_SetMat4(&ibl->prefilter, "view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ibl->prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Primitives_RenderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // setup LUTT texture
    glGenTextures(1, &ibl->brdfLUTTexture);

    glBindTexture(GL_TEXTURE_2D, ibl->brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // brdf convolution
    glBindFramebuffer(GL_FRAMEBUFFER, ibl->captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, ibl->captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ibl->brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    Shader_Bind(&ibl->brdf);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Primitives_RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ImageBasedLighting_RenderSkybox(ImageBasedLighting* ibl, glm::mat4& view, glm::mat4& projection, bool ColorCorrect) {
    glDepthFunc(GL_LEQUAL); // TODO: MOVE TO RENDERCOMMAND
    Shader_Bind(&ibl->skybox);
    Shader_SetMat4(&ibl->skybox, "view", view);
    Shader_SetMat4(&ibl->skybox, "projection", projection);
    Shader_SetBool(&ibl->skybox, "colorCorrect", ColorCorrect);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->envCubemap); 
    //glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->irradianceMap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->prefilterMap);
    Shader_SetInt(&ibl->skybox, "environmentMap", 0);
    Primitives_RenderCube();
    glDepthFunc(GL_LESS);
}

