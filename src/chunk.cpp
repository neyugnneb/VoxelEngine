#include <whatever/chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <whatever/vertexArray.h>
#include <whatever/textureAtlas.h>

Chunk::Chunk(int chunkX, int chunkZ, int randomSeed) : chunkX(chunkX), chunkZ(chunkZ) {

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);   // controls smoothness of terrain
    noise.SetSeed(randomSeed);         // randomized world generation

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {

            // Get world position (so each chunk is unique)
            float worldX = static_cast<float>(x + chunkX * CHUNK_WIDTH);
            float worldZ = static_cast<float>(z + chunkZ * CHUNK_WIDTH);

            // Get noise value [-1, 1]
            float n = noise.GetNoise(worldX, worldZ);

            // Map to height range [0, 40]
            int groundHeight = static_cast<int>((n + 1.0f) * 0.5f * 40.0f);

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y < groundHeight - 3) {
                    blocks[x][y][z] = Block(BlockType::Stone, glm::vec3(x, y, z));
                } else if (y < groundHeight - 1) {
                    blocks[x][y][z] = Block(BlockType::Dirt, glm::vec3(x, y, z));
                } else if (y < groundHeight) {
                    blocks[x][y][z] = Block(BlockType::Grass, glm::vec3(x, y, z));
                } else {
                    blocks[x][y][z] = Block(BlockType::Air, glm::vec3(x, y, z));
                }
            }
        }
    }
}

void Chunk::generateChunk() {
        //Vertices and indices for block generation
        vertices.clear();
        indices.clear();

        unsigned int vertexCount = 0;

        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    if (blocks[x][y][z].type == BlockType::Air) {
                        continue;
                    }
                    if (isAir(x, y, z+1)) addFace(x, y, z, 0, vertexCount, blocks[x][y][z].type); // front
                    if (isAir(x, y, z-1)) addFace(x, y, z, 1, vertexCount, blocks[x][y][z].type); // back
                    if (isAir(x, y+1, z)) addFace(x, y, z, 2, vertexCount, blocks[x][y][z].type); // top
                    if (isAir(x, y-1, z)) addFace(x, y, z, 3, vertexCount, blocks[x][y][z].type); // bottom
                    if (isAir(x-1, y, z)) addFace(x, y, z, 4, vertexCount, blocks[x][y][z].type); // left
                    if (isAir(x+1, y, z)) addFace(x, y, z, 5, vertexCount, blocks[x][y][z].type); // right

                }
            }
        }
    }

bool Chunk::isAir(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH)
        return true;  // Outside chunk = air
    return blocks[x][y][z].type == BlockType::Air;
}

void Chunk::addFace(int x, int y, int z, int faceDir,unsigned int& vertexCount, BlockType blockType) {
    float fx = static_cast<float>(x + chunkX * CHUNK_WIDTH);
    float fy = static_cast<float>(y);
    float fz = static_cast<float>(z + chunkZ * CHUNK_WIDTH);

    BlockTexture textureID = getTexture(blockType);
    int topTexture = textureID.topTexture;
    int sideTexture = textureID.sideTexture;
    int bottomTexture = textureID.bottomTexture;

    UVRect uvTop = getUVForTile(topTexture);
    UVRect uvSide = getUVForTile(sideTexture);
    UVRect uvBottom = getUVForTile(bottomTexture);


  switch (faceDir) {
    case 0: // Front (+Z)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz+1.0f,  uvSide.uMin, uvSide.vMin,
            fx+1.0f,fy,     fz+1.0f,  uvSide.uMax, uvSide.vMin,
            fx+1.0f,fy+1.0f,fz+1.0f,  uvSide.uMax, uvSide.vMax,
            fx,     fy+1.0f,fz+1.0f,  uvSide.uMin, uvSide.vMax
        });
        break;
    case 1: // Back (-Z)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz,  uvSide.uMax, uvSide.vMin, 
            fx+1.0f,fy,     fz,  uvSide.uMin, uvSide.vMin,
            fx+1.0f,fy+1.0f,fz,  uvSide.uMin, uvSide.vMax,
            fx,     fy+1.0f,fz,  uvSide.uMax, uvSide.vMax
        });
        break;
    case 2: // Top (+Y)
        vertices.insert(vertices.end(), {
            fx,     fy+1.0f,fz,      uvTop.uMin, uvTop.vMax,
            fx+1.0f,fy+1.0f,fz,      uvTop.uMax, uvTop.vMax,
            fx+1.0f,fy+1.0f,fz+1.0f, uvTop.uMax, uvTop.vMin,
            fx,     fy+1.0f,fz+1.0f, uvTop.uMin, uvTop.vMin
        });
        break;
    case 3: // Bottom (-Y)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz,      uvBottom.uMin, uvBottom.vMin,
            fx+1.0f,fy,     fz,      uvBottom.uMax, uvBottom.vMin,
            fx+1.0f,fy,     fz+1.0f, uvBottom.uMax, uvBottom.vMax,
            fx,     fy,     fz+1.0f, uvBottom.uMin, uvBottom.vMax
        });
        break;
    case 4: // Left (-X)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz+1.0f,  uvSide.uMax, uvSide.vMin,
            fx,     fy,     fz,       uvSide.uMin, uvSide.vMin,
            fx,     fy+1.0f,fz,       uvSide.uMin, uvSide.vMax,
            fx,     fy+1.0f,fz+1.0f,  uvSide.uMax, uvSide.vMax
        });
        break;
    case 5: // Right (+X)
        vertices.insert(vertices.end(), {
            fx+1.0f,fy,     fz,       uvSide.uMax, uvSide.vMin,
            fx+1.0f,fy,     fz+1.0f,  uvSide.uMin, uvSide.vMin,
            fx+1.0f,fy+1.0f,fz+1.0f,  uvSide.uMin, uvSide.vMax,
            fx+1.0f,fy+1.0f,fz,       uvSide.uMax, uvSide.vMax
        });
        break;
    }

    // Add indices (two triangles per face)
    indices.insert(indices.end(), {
        vertexCount, vertexCount+1, vertexCount+2,
        vertexCount, vertexCount+2, vertexCount+3
    });
    vertexCount += 4;

}

void Chunk::setupMesh() {

    vbo.fillData(vertices.data(), vertices.size() * sizeof(float));
    ebo.fillData(indices.data(), indices.size() * sizeof(unsigned int));

    vao.setAttr(0, 3, GL_FLOAT, 5 * sizeof(float), 0);
    vao.setAttr(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
}

void Chunk::draw() {
    vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}