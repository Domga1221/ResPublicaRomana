#include "Application.hpp"

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Window.hpp"
#include "Memory/Memory.hpp"
#include "Core/LayerStack.hpp"
#include "ImGuiLayer.hpp"
#include <filesystem>
#include "Renderer/Shader.hpp"

#include "Renderer/BufferLayout.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexArray.hpp"

Layer imGuiLayer;
Shader shader;
VertexBuffer vertexBuffer;
VertexArray vertexArray;

#include "Memory/List.hpp"
#include "Memory/List_Test.hpp"

#include "Renderer/Framebuffer.hpp"
Framebuffer framebuffer;
Shader screenSpaceShader;
VertexBuffer screenVertexBuffer;
VertexArray screenVertexArray;

void Application_Initialize(Application* app) {

    MEMORY_Initialize();
    LayerStack_Initialize();

    WindowProps windowProps { "title", 1280, 720 };
    Window_Initialize(&windowProps);

    app->isRunning = true;

    std::string memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    
    ImGuiLayer_Initialize(&imGuiLayer); 
    // right now imguilayer is pushed as first layer into stack, then sandbox editorlayer is pushed
    LayerStack_PushLayer(&imGuiLayer);

    
    std::string currentPath = std::filesystem::current_path().parent_path().parent_path().string();
    currentPath = currentPath + "/Assets/Shaders/";
    std::string vertPath = currentPath + "triangle2.vert";
    std::string fragPath = currentPath + "triangle2.frag";
    Shader_Create(&shader, vertPath, fragPath);
    RPR_DEBUG("%s", currentPath);

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    
    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { .name = "aPosition", .shaderDataType = ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aColor", ShaderDataType::Float3});
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);
    
    for(int i = 0; i < bufferLayout.elements.size; i++) {
        BufferElement_Print(&bufferLayout.elements.data[i]);
    }
    
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    
    f32 triangleVertices[18] = {
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,
    };
    
    VertexBuffer_Create(&vertexBuffer, triangleVertices, sizeof(triangleVertices));
    VertexBuffer_SetLayout(&vertexBuffer, &bufferLayout);
    
    BufferLayout_Print(&vertexBuffer.bufferLayout);
    
    VertexArray_Create(&vertexArray);
    VertexArray_AddVertexBuffer(&vertexArray, &vertexBuffer);
    
    
    //List_Test();



    // Framebuffer
    FramebufferProperties framebufferProperties;
    FramebufferProperties_Create(&framebufferProperties);
    FramebufferProperties_AddAttachment(&framebufferProperties, TEXTURE_FORMAT_RGBA16F);
    framebufferProperties.width = 1280;
    framebufferProperties.height = 720;

    Framebuffer_Create(&framebuffer, &framebufferProperties);

    RPR_DEBUG("%s", currentPath);
    vertPath = currentPath + "screenSpace.vert";
    fragPath = currentPath + "screenSpace.frag";
    Shader_Create(&screenSpaceShader, vertPath, fragPath);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    BufferLayout screenBufferLayout;
    BufferLayout_Create(&screenBufferLayout);
    BufferLayout_AddElement(&screenBufferLayout, { .name = "aPos", .shaderDataType = ShaderDataType::Float2 });
    BufferLayout_AddElement(&screenBufferLayout, { "aTexCoords", ShaderDataType::Float2});
    BufferLayout_CalculateOffsetAndStride(&screenBufferLayout);

    VertexBuffer_Create(&screenVertexBuffer, quadVertices, sizeof(quadVertices));
    VertexBuffer_SetLayout(&screenVertexBuffer, &screenBufferLayout);
    
    VertexArray_Create(&screenVertexArray);
    VertexArray_AddVertexBuffer(&screenVertexArray, &screenVertexBuffer);
}

void Application_Run(Application* app) {
    std::string memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    //return;
    while(app->isRunning) {
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Framebuffer_Bind(&framebuffer);
        glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Shader_Bind(&shader);
        VertexArray_Bind(&vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        Framebuffer_Unbind();

        Shader_Bind(&screenSpaceShader);
        VertexArray_Bind(&screenVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        LayerStack_Update();

        ImGuiContext* context = ImGuiLayer_Begin();
        LayerStack_ImGuiRender(context);
        ImGuiLayer_End();

        Window_Update();
        if(Window_ShouldClose())
            app->isRunning = false;
    }
}

RPR_API void Application_PushLayer(Layer* layer) {
    RPR_DEBUG("Application_PushLayer called");
    LayerStack_PushLayer(layer);   
}