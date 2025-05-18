#pragma once 

#include "defines.hpp"

#include "Renderer/Mesh.hpp"

#include <unordered_map>

#include "Hex.hpp"

struct HexagonGrid {
    u32 sizeX;
    u32 sizeZ;
    
    List<glm::vec3> vertices;
    List<u32> indices;
    VertexArray* vertexArray;
};

RPR_API HexagonGrid* HexagonGrid_Create(u32 sizeX, u32 sizeZ);
RPR_API void HexagonGrid_Render(HexagonGrid* hexagonGrid, glm::mat4& view, glm::mat4& projection); 






