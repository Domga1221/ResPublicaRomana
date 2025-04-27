#pragma once 

#include "defines.hpp"

#include <glm/glm.hpp>


#include "VertexArray.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 TexCoords;
};

struct Mesh {
    std::string relativePath;
    b8 isLoaded;
    u32 vertexCount;
    u32 indexCount;

    List<Vertex> vertices; // TODO: temporary
    List<u32> indices;

    VertexArray* vertexArray;
};

RPR_API void Mesh_Create(Mesh* mesh, std::string& path);
RPR_API void Mesh_Destroy(Mesh* mesh);

RPR_API void Mesh_Bind(Mesh* mesh);