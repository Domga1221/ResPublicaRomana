#include "ShaderPool.hpp"

#include <Platform/Filesystem.hpp>

static Shader editorShader;
static Shader pbrShader;

void ShaderPool_Initialize() {
    std::string currentPath = Filesystem_GetCWD();

    std::string vertPath = currentPath + "/Assets/Shaders/Editor/editorShader.vert";
    std::string fragPath = currentPath + "/Assets/Shaders/Editor/editorShader.frag";
    Shader_Create(&editorShader, vertPath, fragPath);

    vertPath = currentPath + "/Assets/Shaders/PBR/PBR.vert";
    fragPath = currentPath + "/Assets/Shaders/PBR/PBR.frag";
    Shader_Create(&pbrShader, vertPath, fragPath);
}

void ShaderPool_Shutdown() {
    // TODO: 
}


Shader* ShaderPool_GetEditorShader() {
    return &editorShader;
}

Shader* ShaderPool_GetPBRShader() {
    return &pbrShader;
}