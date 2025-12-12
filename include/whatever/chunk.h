#ifndef CHUNK_H
#define CHUNK_H

#include <whatever/block.h>
#include <whatever/FastNoiseLite.h>
#include <whatever/elementBuffer.h>
#include <whatever/vertexArray.h>
#include <whatever/vertexBuffer.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <whatever/textureAtlas.h>
#include <vector>

const int CHUNK_HEIGHT = 256;
const int CHUNK_WIDTH = 16;

struct Chunk {
    Chunk(int chunkX, int chunkZ, int randomSeed);

    Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    std::vector<float> waterVertices;
    std::vector<unsigned int> waterIndices;

    VertexBuffer vbo;
    VertexArray vao;
    VertexBuffer waterVBO;
    VertexArray waterVAO;
    ElementBuffer ebo;
    ElementBuffer waterEBO;

    void generateChunk();
    void setupMesh();
    void drawBlock();
    void drawWater();
    void uploadMeshData(const std::vector<float>& verts,
                        const std::vector<unsigned int>& idx,
                        const std::vector<float>& waterVerts,
                        const std::vector<unsigned int>& waterIdx);

    int chunkX, chunkZ;

private:
    void addFace(int x, int y, int z, int faceDir,unsigned int& vertexCount, BlockType blockType);
    void addWaterFace(int x, int y, int z, int faceDir, unsigned int& waterVertexCount);

    bool isAir(int x, int y, int z);
    bool isWater(int x, int y, int z);

    FastNoiseLite noise;
};

#endif