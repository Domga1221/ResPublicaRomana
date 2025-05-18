#include "HexagonGrid.hpp"

#include "Memory/Memory.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma clang diagnostic push // So that it doesn't complain about vsprintf being unsafe 
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <glm/gtx/string_cast.hpp>
#pragma clang diagnostic pop

// TODO: Hash Table implementation
static std::unordered_map<int, Hex> hexMap;
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
        glm::vec3 hexPoint = rotate * glm::vec3(0.0f, 0.0f, 1.0f);
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
    


    // grid layout
    for(int y = 0; y < sizeZ; y++) {
        for(int x = 0; x < sizeX; x++) {
            Hex hex = Hex(0, 0, 0);
            hex.col = x;
            hex.row = y;
            if(y % 2 == 1) // odd
                hex.OffsetToCube(Hex::ODD);
            else 
                hex.OffsetToCube(Hex::EVEN);
            
            hexMap.insert(std::pair(Hex_Hash(hex), hex));
        }
    }   



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

    RPR_WARN("--- Hex to Point ---");
    for(u32 y = 0; y < hexagonGrid->sizeZ; y++) {
        for(u32 x = 0; x < hexagonGrid->sizeX; x++) {

            static float hexSize = 1.0f;
            static float vertical = 3.0f / 2.0f * hexSize;
            static float horizontal = (float)glm::sqrt(3.0f) * hexSize;
            glm::vec3 position = glm::vec3(x * horizontal, 0, y * vertical);
            glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

            Hex hex;
            hex.col = x;
            hex.row = y;

            if(y % 2 == 1) { // ODD
                position.x += horizontal / 2.0f;
                color = glm::vec3(1.0f, 1.0f, 0.0f);

                hex.OffsetToCube(Hex::ODD);
            } else 
                hex.OffsetToCube(Hex::EVEN);

            RPR_INFO("hex: %s, position: %s, Hex_ToPoint: %s", 
                Hex_ToString(hex), glm::to_string(position).c_str(), glm::to_string(Hex_ToPoint(hex, 1.0f)));

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            Shader_SetMat4(&hexagonGridShader, "model", model);
            Shader_SetVec3(&hexagonGridShader, "hexColor", color);
            RenderCommand_DrawIndexed(hexagonGrid->indices.size);
        }
    }
}