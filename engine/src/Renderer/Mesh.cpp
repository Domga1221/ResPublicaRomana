#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

#include "Memory/List.hpp"

#include "IndexBuffer.hpp"

void processNode(Mesh* mesh, aiNode* node, const aiScene* scene);
void processMesh(Mesh* mesh, aiMesh* aiMesh, const aiScene* scene);

void Mesh_Create(Mesh* mesh, std::string& path) {

    mesh->isLoaded = false;

    RPR_WARN("Mesh_Create: Loading mesh at: %s", path.c_str());  
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path relativePath = std::filesystem::relative(std::filesystem::path(path), currentPath);
    RPR_WARN("Relative path: %s", relativePath.string());

    mesh->relativePath = relativePath.string();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene ||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        RPR_ERROR("Mesh_Create: No mesh found at: %s", path.c_str());
        return;
    }

    List_Create(&mesh->vertices);
    List_Create(&mesh->indices);

    if(scene != 0) 
        processNode(mesh, scene->mRootNode, scene);
    
    mesh->vertexCount = mesh->vertices.size;
    mesh->indexCount = mesh->indices.size;
    RPR_WARN("vertex count: %d", mesh->vertexCount);
    RPR_WARN("index count: %d", mesh->indexCount);

    // Generate buffers
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
    if(mesh->indexCount > 0) {
        IndexBuffer* indexBuffer = IndexBuffer_Create(mesh->indices.data, mesh->indices.size);
        VertexArray_SetIndexBuffer(mesh->vertexArray, indexBuffer);
    }

    List_Destroy(&mesh->vertices);
    List_Destroy(&mesh->indices);
    mesh->isLoaded = true;
}

void Mesh_Destroy(Mesh* mesh) {
    VertexArray_Destroy(mesh->vertexArray);
    mesh->isLoaded = false;
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
}

void Mesh_Bind(Mesh* mesh) {
    VertexArray_Bind(mesh->vertexArray);
}

void processNode(Mesh* mesh, aiNode* node, const aiScene* scene) {
    for(u32 i = 0; i < node->mNumMeshes; i++) {
        aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, aiMesh, scene);
    }
    for(u32 i = 0; i < node->mNumChildren; i++) {
        processNode(mesh, node->mChildren[i], scene);
    }
}

void processMesh(Mesh* mesh, aiMesh* aiMesh, const aiScene* scene) {

    for(u32 i = 0; i < aiMesh->mNumVertices; i++) {
        Vertex v;
        
        // positions
        v.position.x = aiMesh->mVertices[i].x;
        v.position.y = aiMesh->mVertices[i].y;
        v.position.z = aiMesh->mVertices[i].z;

        // normals
        if(aiMesh->HasNormals()) {
            v.normal.x = aiMesh->mNormals[i].x;
            v.normal.y = aiMesh->mNormals[i].y;
            v.normal.z = aiMesh->mNormals[i].z;
        }

        // texture coordinates
        if(aiMesh->mTextureCoords[0]) {
            v.TexCoords.x = aiMesh->mTextureCoords[0][i].x;
            v.TexCoords.y = aiMesh->mTextureCoords[0][i].y;
        } else {
            v.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        List_PushBack(&mesh->vertices, v);
    }

    for(u32 i = 0; i < aiMesh->mNumFaces; i++) {
        aiFace face = aiMesh->mFaces[i];
        for(u32 j = 0; j < face.mNumIndices; j++) {
            List_PushBack(&mesh->indices, face.mIndices[j]);
        }
    }
}