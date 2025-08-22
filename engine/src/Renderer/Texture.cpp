#include "Texture.hpp"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <Core/Log.hpp>

#include <Platform/Filesystem.hpp>

void Texture_Create(Texture* texture, const char* path) {
    RPR_DEBUG("Loading Texture at: %s", path);

    const char* CWD = Filesystem_GetCWD();
    std::string fullPath = CWD + std::string("/") + std::string(path); 

    stbi_set_flip_vertically_on_load(false);

    i32 width; i32 height; i32 numberOfComponents;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &numberOfComponents, 0);
    if(!data) {
        RPR_ERROR("Texture_Create: Failed to load texture at: %s", fullPath);
        RPR_ERROR("Texture_Create: stbi_failure_reason: %s", stbi_failure_reason());
        stbi_image_free(data);
        return;
    }
    
    GLenum format;
    switch(numberOfComponents) {
        case 1:     format = GL_RED;    break;
        case 3:     format = GL_RGB;    break;
        case 4:     format = GL_RGBA;   break;
        default:    format = GL_RED;    break;
    }
    
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture_Unbind();

    stbi_image_free(data);

    String_Create(&texture->path, path);    // TODO: Texture should probably only save relative path 
    texture->width = width;                 //       and working directory stored somewhere else 
    texture->height = height;
    texture->numberOfChannels = numberOfComponents;
    texture->loaded = true;
}

void Texture_Destroy(Texture* texture) {
    glDeleteTextures(1, &texture->ID);
 
    String_Destroy(&texture->path);
    texture->ID = 0;
    texture->width = 0;
    texture->height = 0;
    texture->numberOfChannels = 0;
    texture->loaded = false;
}

void Texture_Bind(Texture* texture) {
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}

void Texture_Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}