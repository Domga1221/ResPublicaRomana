#include "HexagonGrid.hpp"

#include "Memory/Memory.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Mesh* Hexagon_CreateMesh() {
    Mesh* mesh = (Mesh*)MEMORY_Allocate(sizeof(Mesh), MEMORY_TAG_RENDERER);
    List_Create(&mesh->vertices);
    List_Create(&mesh->indices);

    //glm::mat3 identity = glm::mat3(1.0f);
    //glm::mat3 rotate_60 = glm::rotate(identity, glm::radians(60.0f));


    //glm::vec3 center = glm::vec3(0.0f);
    glm::mat4 identity = glm::mat4(1.0f);
    
    Vertex v;
    v.position = glm::vec3(0.0f, 0.0f, 0.0f);
    v.normal = glm::vec3(0.0f, 0.0f, 0.0f);
    v.TexCoords = glm::vec2(0.0f, 0.0f);
    List_PushBack(&mesh->vertices, v);
    RPR_INFO("Creating Hexagon");
    for(int i = 0; i < 6; i++) {
        // TODO: hex right now is on x-y plane, put it on ground
        glm::mat3 rotate = glm::rotate(identity, glm::radians(60.0f * i), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 hexPoint = rotate * glm::vec3(0.0f, 1.0f, 0.0f);
        RPR_INFO("%d, (%0.3f, %0.3f, %0.3f), \t length: %f", i, hexPoint.x, hexPoint.y, hexPoint.z, glm::length(hexPoint));
        Vertex v;
        v.position = hexPoint;
        v.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        v.TexCoords = glm::vec2(0.0f, 0.0f);
        List_PushBack(&mesh->vertices, v);
    }

    u32 indices[18] = { 0, 1, 2,    
                        0, 2, 3, 
                        0, 3, 4, 
                        0, 4, 5, 
                        0, 5, 6, 
                        0, 6, 1 };

    for(u32 i = 0; i < 18; i++)
        List_PushBack(&mesh->indices, indices[i]);

    mesh->vertexCount = mesh->vertices.size;
    mesh->indexCount = mesh->indices.size;

    BufferLayout bufferLayout;
    BufferLayout_Create(&bufferLayout);
    BufferLayout_AddElement(&bufferLayout, { "aPosition", ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aNormal", ShaderDataType::Float3 });
    BufferLayout_AddElement(&bufferLayout, { "aTexCoords", ShaderDataType::Float2 });
    BufferLayout_CalculateOffsetAndStride(&bufferLayout);

    VertexBuffer* vertexBuffer = VertexBuffer_Create((f32*)mesh->vertices.data, sizeof(Vertex) * mesh->vertexCount);
    VertexBuffer_SetLayout(vertexBuffer, &bufferLayout);

    mesh->vertexArray = VertexArray_Create();
    VertexArray_AddVertexBuffer(mesh->vertexArray, vertexBuffer);

    IndexBuffer* indexBuffer = IndexBuffer_Create(mesh->indices.data, mesh->indices.size);
    VertexArray_SetIndexBuffer(mesh->vertexArray, indexBuffer);
    
    // TODO: destroy vertex and index lists 
    
    return mesh;
}