#pragma once 

#include "defines.hpp"

#include <Renderer/Shader.hpp>


void ShaderPool_Initialize();
void ShaderPool_Shutdown();

Shader* ShaderPool_GetEditorShader();
Shader* ShaderPool_GetPBRShader();