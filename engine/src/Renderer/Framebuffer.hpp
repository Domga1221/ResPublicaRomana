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

void FramebufferProperties_Create(FramebufferProperties* framebufferProperties) {
    List_Create(&framebufferProperties->attachments);
    framebufferProperties->width = 0;
    framebufferProperties->height = 0;
}

void FramebufferProperties_AddAttachment(FramebufferProperties* framebufferProperties, Texture_Format textureFormat) {
    List_PushBack(&framebufferProperties->attachments, textureFormat);
}


struct Framebuffer {
    u32 ID;
    FramebufferProperties framebufferProperties;

    List<Texture_Format> colorAttachments;
    Texture_Format depthAttachment;

    List<u32> colorIDs;
    u32 depthID;
};

void Framebuffer_Create(Framebuffer* framebuffer, FramebufferProperties* framebufferProperties) {
    framebuffer->ID = 0;
    framebuffer->framebufferProperties = *framebufferProperties;

    List_Create(&framebuffer->colorAttachments);
    framebuffer->depthAttachment = TEXTURE_FORMAT_NONE;

    List_Create(&framebuffer->colorIDs);
    framebuffer->depthID = 0;


    for(u32 i = 0; i < framebufferProperties->attachments.size; i++) {
        Texture_Format current = framebufferProperties->attachments.data[i];
        if(current == TEXTURE_FORMAT_DEPTH) {
            if(framebuffer->depthAttachment == TEXTURE_FORMAT_NONE) {
                framebuffer->depthAttachment = current;
            } else {
                RPR_WARN("Framebuffer_Create: Multiple depth attachments specified, but only one allowed, returning...");
                return;
            }
        } else {
            List_PushBack(&framebuffer->colorAttachments, current);
        }
    }


    // OpenGL initialization 
    glCreateFramebuffers(1, &framebuffer->ID);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->ID);

    // TODO: Reserve function for list 
    for(u32 i = 0; i < framebuffer->colorAttachments.size; i++) {
        List_PushBack(&framebuffer->colorIDs, (u32)0);
    }
    glCreateTextures(GL_TEXTURE_2D, framebuffer->colorIDs.size, framebuffer->colorIDs.data);
    
    for(u32 i = 0; i < framebuffer->colorAttachments.size; i++) {
        Texture_Format current = framebufferProperties->attachments.data[i];

        glBindTexture(GL_TEXTURE_2D, framebuffer->colorIDs.data[i]);
        switch(current) { // TODO: Move case code to function, support other Texture Formats
            case TEXTURE_FORMAT_RGBA16F:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
                    framebufferProperties->width, framebufferProperties->height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 
                    framebuffer->colorIDs.data[i], 0);
                break;
            default:
                RPR_ERROR("Framebuffer_Create: color attachment not supported: %d", current);
                break;
        }
    }

    // TODO: Depth attachment
    if(framebuffer->colorAttachments.size > 4) {
        RPR_ERROR("Framebuffer_Create: Only 4 color attachments supported right now");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(framebuffer->colorAttachments.size, buffers);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
        RPR_ERROR("Framebuffer_Create: Framebuffer is not complete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_Bind(Framebuffer* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->ID);
    glViewport(0, 0, framebuffer->framebufferProperties.width, framebuffer->framebufferProperties.height);
}

void Framebuffer_Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}