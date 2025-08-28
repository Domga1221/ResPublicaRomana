#pragma once 

#include "defines.hpp"

#include "Memory/List.hpp"

enum Texture_Format {
    TEXTURE_FORMAT_NONE = 0,

    // Color 
    TEXTURE_FORMAT_RGBA8,
    TEXTURE_FORMAT_RGBA16F,
    TEXTURE_FORMAT_RED_INTEGER,

    // Depth / Stencil 
    TEXTURE_FORMAT_DEPTH24STENCIL8,

    // Defaults 
    TEXTURE_FORMAT_DEPTH = TEXTURE_FORMAT_DEPTH24STENCIL8
};


struct FramebufferProperties {
    List<Texture_Format> attachments;
    u32 width;
    u32 height;
};

RPR_API void FramebufferProperties_Create(FramebufferProperties* framebufferProperties);
// TODO: Destroy
RPR_API void FramebufferProperties_AddAttachment(FramebufferProperties* framebufferProperties, 
    Texture_Format textureFormat);


struct Framebuffer {
    u32 ID;
    FramebufferProperties framebufferProperties;

    List<Texture_Format> colorAttachments;
    Texture_Format depthAttachment;

    List<u32> colorIDs;
    u32 depthID;
};

RPR_API void Framebuffer_Create(Framebuffer* framebuffer, FramebufferProperties* framebufferProperties);
RPR_API void Framebuffer_Destroy(Framebuffer* framebuffer);

RPR_API void Framebuffer_Bind(Framebuffer* framebuffer);
RPR_API void Framebuffer_Unbind();

RPR_API void Framebuffer_Resize(Framebuffer* framebuffer, u32 width, u32 height);

RPR_API i32 Framebuffer_ReadPixel(Framebuffer* framebuffer, u32 attachmentIndex, i32 x, i32 y);
RPR_API void Framebuffer_ClearAttachment(Framebuffer* framebuffer, u32 attachmentIndex, i32 value);