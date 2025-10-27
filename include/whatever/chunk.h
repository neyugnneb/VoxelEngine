#ifndef CHUNK_H
#define CHUNK_H

#include <whatever/block.h>
#include <whatever/FastNoiseLite.h>
#include <whatever/elementBuffer.h>
#include <whatever/vertexArray.h>
#include <whatever/vertexBuffer.h>
#include <vector>

const int CHUNK_HEIGHT = 256;
const int CHUNK_WIDTH = 16;

struct Chunk {
    Chunk(int chunkX, int chunkZ, int randomSeed);

    Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    VertexBuffer vbo;
    VertexArray vao;
    ElementBuffer ebo;

    void generateChunk();
    void setupMesh();
    void draw();

    int chunkX, chunkZ;


private:
    void addFace(int x, int y, int z, int faceDir,unsigned int& vertexCount, BlockType BlockType);

    bool isAir(int x, int y, int z);

    FastNoiseLite noise;
};

#endif