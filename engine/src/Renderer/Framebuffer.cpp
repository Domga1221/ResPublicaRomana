#include "Framebuffer.hpp"

#include <glad/glad.h>

void FramebufferProperties_Create(FramebufferProperties* framebufferProperties) {
    List_Create(&framebufferProperties->attachments);
    framebufferProperties->width = 0;
    framebufferProperties->height = 0;
}

void FramebufferProperties_AddAttachment(FramebufferProperties* framebufferProperties, Texture_Format textureFormat) {
    List_PushBack(&framebufferProperties->attachments, textureFormat);
}


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
            case TEXTURE_FORMAT_RED_INTEGER:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, 
                    framebufferProperties->width, framebufferProperties->height, 0,
                    GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
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

    if(framebuffer->colorAttachments.size > 1) {
        if(framebuffer->colorAttachments.size > 4)
            RPR_ERROR("Framebuffer_Create: Only 4 color attachments supported right now");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(framebuffer->colorAttachments.size, buffers);
    }

    if(framebuffer->depthAttachment != TEXTURE_FORMAT_NONE) {
        glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer->depthID);
        glBindTexture(GL_TEXTURE_2D, framebuffer->depthID);
        switch(framebuffer->depthAttachment) {
            case TEXTURE_FORMAT_DEPTH24STENCIL8: 
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, 
                    framebufferProperties->width, framebufferProperties->height);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 
                    framebuffer->depthID, 0);
                break;
            default:
                    RPR_ERROR("Framebuffer_Create: depth attachment not supported");
        }
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


void Framebuffer_Resize(Framebuffer* framebuffer, u32 width, u32 height) {
    framebuffer->framebufferProperties.width = width;
    framebuffer->framebufferProperties.height = height;

    if(framebuffer->ID != 0) {
        glDeleteFramebuffers(1, &framebuffer->ID);
        glDeleteTextures(framebuffer->colorAttachments.size, framebuffer->colorIDs.data);
        glDeleteTextures(1, &framebuffer->depthID);

        List_Destroy(&framebuffer->colorAttachments);
    }    

    Framebuffer_Create(framebuffer, &framebuffer->framebufferProperties);
}

i32 Framebuffer_ReadPixel(Framebuffer* framebuffer, u32 attachmentIndex, i32 x, i32 y) {
    if(attachmentIndex >= framebuffer->colorAttachments.size) {
        RPR_ERROR("Framebuffer_ReadPixel: attachmentIndex too big");
    }
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData = 10;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) 
        RPR_ERROR("Framebuffer_ReadPixel: glError: %d", error);
    return pixelData;
}

void Framebuffer_ClearAttachment(Framebuffer* framebuffer, u32 attachmentIndex, i32 value) {
    if(attachmentIndex >= framebuffer->colorIDs.size) {
        RPR_ERROR("Framebuffer_ClearAttachment: Trying to access colorattachments with too big of an index");
    }
    glClearTexImage(framebuffer->colorIDs.data[attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) 
        RPR_ERROR("Framebuffer_ClearAttachment: glError: %d", error);
}