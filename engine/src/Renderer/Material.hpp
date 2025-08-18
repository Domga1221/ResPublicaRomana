#pragma once 

#include "defines.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "Memory/List.hpp"
#include "Uniform.hpp"

typedef struct Material {
    Shader* shader;
    u32 textureCount;
    
    List<Texture*> textures;
    List<Uniform> uniforms;
} Material;

RPR_API void Material_Create(Material* material, Shader* shader);
RPR_API void Material_Destroy(Material* material);

RPR_API void Material_SendToShader(Material* material, Uniform* uniform, void* data);


