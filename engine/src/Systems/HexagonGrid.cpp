#include "HexagonGrid.hpp"

#include "Memory/Memory.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

HexagonGrid* HexagonGrid_Create(u32 sizeX, u32 sizeZ) {
    HexagonGrid* hexagonGrid = (HexagonGrid*)MEMORY_Allocate(sizeof(HexagonGrid), MEMORY_TAG_RENDERER);
    List_Create(&hexagonGrid->vertices);
    List_Create(&hexagonGrid->indices);

    hexagonGrid->sizeX = sizeX;
    hexagonGrid->sizeZ = sizeZ;

    glm::vec3 vertex = glm::vec3(0.0f);
    List_PushBack(&hexagonGrid->vertices, vertex);
    RPR_INFO("Creating Hexagon");
    for(int i = 0; i < 6; i++) {
        glm::mat3 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 hexPoint = rotate * glm::vec3(1.0f, 0.0f, 0.0f);
        RPR_INFO("%d, (%0.3f, %0.3f, %0.3f), \t length: %f", i, hexPoint.x, hexPoint.y, hexPoint.z, glm::length(hexPoint));
        List_PushBack(&hexagonGrid->vertices, hexPoint);
    }

    u32 indices[18] = { 0, 1, 2,    
                        0, 2, 3, 
                        0, 3, 4, 
                        0, 4, 5, 
                        0, 5, 6, 
                        0, 6, 1 };

    for(u32 i = 0; i < 18; i++)
        List_PushBack(&hexagonGrid->indices, indices[i]);

    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { "aPosition", ShaderDataType::Float3 });
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);

    VertexBuffer* vertexBuffer = VertexBuffer_Create((f32*)hexagonGrid->vertices.data, sizeof(glm::vec3) * hexagonGrid->vertices.size);
    VertexBuffer_SetLayout(vertexBuffer, &bufferLayout);

    hexagonGrid->vertexArray = VertexArray_Create();
    VertexArray_AddVertexBuffer(hexagonGrid->vertexArray, vertexBuffer);

    IndexBuffer* indexBuffer = IndexBuffer_Create(hexagonGrid->indices.data, hexagonGrid->indices.size);
    VertexArray_SetIndexBuffer(hexagonGrid->vertexArray, indexBuffer);
    
    // TODO: destroy vertex and index lists 
    
    return hexagonGrid;
}

#include "Renderer/Shader.hpp"
#include "Renderer/RenderCommand.hpp"
#include <filesystem>
static Shader hexagonGridShader;
static bool isHexagonGridShaderInitialized = false;
void HexagonGrid_Render(HexagonGrid* hexagonGrid, glm::mat4& view, glm::mat4& projection) {
    if(!isHexagonGridShaderInitialized) {
        std::string currentPath = std::filesystem::current_path().string();
        std::string vertPath = currentPath + "/Assets/Shaders/HexGrid/hexagonGrid.vert";
        std::string fragPath = currentPath + "/Assets/Shaders/HexGrid/hexagonGrid.frag";
        Shader_Create(&hexagonGridShader, vertPath, fragPath);
        isHexagonGridShaderInitialized = true;
    }

    VertexArray_Bind(hexagonGrid->vertexArray);
    Shader_Bind(&hexagonGridShader);
    Shader_SetMat4(&hexagonGridShader, "view", view);
    Shader_SetMat4(&hexagonGridShader, "projection", projection);

    for(u32 i = 0; i < hexagonGrid->sizeX; i++) {
        for(u32 j = 0; j < hexagonGrid->sizeZ; j++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 0.0f, 0.0f));
            Shader_SetMat4(&hexagonGridShader, "model", model);
            RenderCommand_DrawIndexed(hexagonGrid->indices.size);
        }
    }
}