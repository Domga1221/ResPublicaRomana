#pragma once 

#include "defines.hpp"

#include <Renderer/Shader.hpp>

struct ImageBasedLighting {
    std::string hdrPath;
    u32 hdrTexture;
    u32 envCubemap;

    u32 captureFBO;
    u32 captureRBO;
    
    u32 irradianceMap;
    u32 prefilterMap;
    u32 brdfLUTTexture;

    Shader equirectangularToCubemap; // TODO: Shaders don't need to be here 
    Shader skybox;
    Shader irradiance;
    Shader prefilter;
    Shader brdf;
};

void RPR_API ImageBasedLighting_Initialize(ImageBasedLighting* ibl, const char* hdrPath);
void RPR_API ImageBasedLighting_Shutdown(ImageBasedLighting* ibl); // TODO:

// TODO: move view and projection into uniform buffer, so no passing around needed
void RPR_API ImageBasedLighting_RenderSkybox(ImageBasedLighting* ibl, glm::mat4& view, glm::mat4& projection, bool ColorCorrect); 
