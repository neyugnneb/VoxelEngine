#include <whatever/chunk.h>

//Chunk constructor that generates terrain using Perlin noise.
Chunk::Chunk(int chunkX, int chunkZ, int randomSeed) : chunkX(chunkX), chunkZ(chunkZ) {

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);   // controls smoothness of terrain
    noise.SetSeed(randomSeed);         // randomized world generation
    const int SEA_LEVEL = 20;

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {

            // Get world position (so each chunk is unique)
            float worldX = static_cast<float>(x + chunkX * CHUNK_WIDTH);
            float worldZ = static_cast<float>(z + chunkZ * CHUNK_WIDTH);

            // Get noise value [-1, 1]
            float n = noise.GetNoise(worldX, worldZ);

            // Map to height range [0, 40]
            int groundHeight = static_cast<int>((n + 1.0f) * 0.5f * 50.0f);

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y < groundHeight - 3) {
                    blocks[x][y][z] = Block(BlockType::Stone, glm::vec3(x, y, z));
                } else if (y < groundHeight && y < SEA_LEVEL) {
                    blocks[x][y][z] = Block(BlockType::Sand, glm::vec3(x, y, z));
                } else if (y < groundHeight -1) {
                    blocks[x][y][z] = Block(BlockType::Dirt, glm::vec3(x, y, z));
                } else if (y < groundHeight) {
                    blocks[x][y][z] = Block(BlockType::Grass, glm::vec3(x, y, z));
                } else {
                    blocks[x][y][z] = Block(BlockType::Air, glm::vec3(x, y, z));
                }
            }

        }

    }

    //Generates the water blocks up to sea level
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            for (int y = 0; y < SEA_LEVEL; y++) {
                if (blocks[x][y][z].type == BlockType::Air) {
                    blocks[x][y][z].type = BlockType::Water;
                }

            }
        }
    }

    generateTrees(randomSeed);

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            float light = 15;
            for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
                Block& b = blocks[x][y][z];

                if (isSunTransparent(b.type)) {
                    b.lightLevel = light;
                } else {
                    b.lightLevel = 15;
                    //break; // sunlight blocked
                }
            }
        }
    }

}


//"Physically" creates the chunk by creating the indices and vertices for rendering
void Chunk::generateChunk() {
        //Vertices and indices for block generation
        vertices.clear();
        indices.clear();
        waterVertices.clear();
        waterIndices.clear();

        unsigned int vertexCount = 0;
        unsigned int waterVertexCount = 0;

        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {

                    BlockType type = blocks[x][y][z].type;
                    if (type != BlockType::Air && type != BlockType::Water) {
                        if (isAir(x, y, z+1) || isWater(x, y, z+1)) addFace(x, y, z, 0, vertexCount, blocks[x][y][z].type); // front
                        if (isAir(x, y, z-1) || isWater(x, y, z-1)) addFace(x, y, z, 1, vertexCount, blocks[x][y][z].type); // back
                        if (isAir(x, y+1, z) || isWater(x, y+1, z)) addFace(x, y, z, 2, vertexCount, blocks[x][y][z].type); // top
                        if (isAir(x, y-1, z) || isWater(x, y-1, z)) addFace(x, y, z, 3, vertexCount, blocks[x][y][z].type); // bottom
                        if (isAir(x-1, y, z) || isWater(x-1, y, z)) addFace(x, y, z, 4, vertexCount, blocks[x][y][z].type); // left
                        if (isAir(x+1, y, z) || isWater(x+1, y, z)) addFace(x, y, z, 5, vertexCount, blocks[x][y][z].type); // right
                    }

                    if (type == BlockType::Water) {

                        if (isAir(x, y+1, z) && !isWater(x, y+1, z)) addWaterFace(x, y, z, 2, waterVertexCount); // top

                    }

                }
            }
        }
    }

//Checks if a block is air or not
bool Chunk::isAir(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH)
        return true;  // Outside chunk = air
    return blocks[x][y][z].type == BlockType::Air;
}

bool Chunk::isWater(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH)
        return true;  // Outside chunk = water
    return blocks[x][y][z].type == BlockType::Water;
}

//Add a face if the adjacent block is air
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

    //Gets light float, needs to static cast because lightLevel is uint8
    float light = (blocks[x][y][z].lightLevel) / 15.0f;

  switch (faceDir) {
    case 0: // Front (+Z)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz+1.0f,  uvSide.uMin, uvSide.vMin, light,
            fx+1.0f,fy,     fz+1.0f,  uvSide.uMax, uvSide.vMin, light,
            fx+1.0f,fy+1.0f,fz+1.0f,  uvSide.uMax, uvSide.vMax, light,
            fx,     fy+1.0f,fz+1.0f,  uvSide.uMin, uvSide.vMax, light
        });
        break;
    case 1: // Back (-Z)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz,  uvSide.uMax, uvSide.vMin, light, 
            fx+1.0f,fy,     fz,  uvSide.uMin, uvSide.vMin, light,
            fx+1.0f,fy+1.0f,fz,  uvSide.uMin, uvSide.vMax, light,
            fx,     fy+1.0f,fz,  uvSide.uMax, uvSide.vMax, light
        });
        break;
    case 2: // Top (+Y)
        vertices.insert(vertices.end(), {
            fx,     fy+1.0f,fz,      uvTop.uMin, uvTop.vMax, light,
            fx+1.0f,fy+1.0f,fz,      uvTop.uMax, uvTop.vMax, light,
            fx+1.0f,fy+1.0f,fz+1.0f, uvTop.uMax, uvTop.vMin, light,
            fx,     fy+1.0f,fz+1.0f, uvTop.uMin, uvTop.vMin, light
        });
        break;
    case 3: // Bottom (-Y)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz,      uvBottom.uMin, uvBottom.vMin, light,
            fx+1.0f,fy,     fz,      uvBottom.uMax, uvBottom.vMin, light,
            fx+1.0f,fy,     fz+1.0f, uvBottom.uMax, uvBottom.vMax, light,
            fx,     fy,     fz+1.0f, uvBottom.uMin, uvBottom.vMax, light
        });
        break;
    case 4: // Left (-X)
        vertices.insert(vertices.end(), {
            fx,     fy,     fz+1.0f,  uvSide.uMax, uvSide.vMin, light,
            fx,     fy,     fz,       uvSide.uMin, uvSide.vMin, light,
            fx,     fy+1.0f,fz,       uvSide.uMin, uvSide.vMax, light,
            fx,     fy+1.0f,fz+1.0f,  uvSide.uMax, uvSide.vMax, light
        });
        break;
    case 5: // Right (+X)
        vertices.insert(vertices.end(), {
            fx+1.0f,fy,     fz,       uvSide.uMax, uvSide.vMin, light,
            fx+1.0f,fy,     fz+1.0f,  uvSide.uMin, uvSide.vMin, light,
            fx+1.0f,fy+1.0f,fz+1.0f,  uvSide.uMin, uvSide.vMax, light,
            fx+1.0f,fy+1.0f,fz,       uvSide.uMax, uvSide.vMax, light
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

void Chunk::addWaterFace(int x, int y, int z, int faceDir,unsigned int& waterVertexCount) {
        float fx = static_cast<float>(x + chunkX * CHUNK_WIDTH);
    float fy = static_cast<float>(y);
    float fz = static_cast<float>(z + chunkZ * CHUNK_WIDTH);

    // Water tile in atlas (change this if needed)
    UVRect uv = getUVForTile(5); //Water tile index is 5

    // To make water flat and avoid Z-fighting with block top surface:
    // slightly lower or raise the top face
    const float WATER_OFFSET = 0.05f;

    float light = (blocks[x][y][z].lightLevel) / 15.0f;

    switch (faceDir)
    {
        case 0: // Front (+Z)
            waterVertices.insert(waterVertices.end(), {
                fx,     fy,     fz+1.0f,  uv.uMin, uv.vMin, light,
                fx+1.0f,fy,     fz+1.0f,  uv.uMax, uv.vMin, light,
                fx+1.0f,fy+1.0f,fz+1.0f,  uv.uMax, uv.vMax, light,
                fx,     fy+1.0f,fz+1.0f,  uv.uMin, uv.vMax, light
            });
            break;

        case 1: // Back (-Z)
            waterVertices.insert(waterVertices.end(), {
                fx,     fy,     fz,  uv.uMax, uv.vMin, light,
                fx+1.0f,fy,     fz,  uv.uMin, uv.vMin, light,
                fx+1.0f,fy+1.0f,fz,  uv.uMin, uv.vMax, light,
                fx,     fy+1.0f,fz,  uv.uMax, uv.vMax, light
            });
            break;

        case 2: // Top (+Y)
            waterVertices.insert(waterVertices.end(), {
                fx,       fy+1.0f - WATER_OFFSET, fz,      uv.uMin, uv.vMax, light,
                fx+1.0f,  fy+1.0f - WATER_OFFSET, fz,      uv.uMax, uv.vMax, light,
                fx+1.0f,  fy+1.0f - WATER_OFFSET, fz+1.0f, uv.uMax, uv.vMin, light,
                fx,       fy+1.0f - WATER_OFFSET, fz+1.0f, uv.uMin, uv.vMin, light
            });
            break;

        case 3: // Bottom (-Y)
            // Usually water bottom is invisible — skip if desired:
            // return;
            waterVertices.insert(waterVertices.end(), {
                fx,     fy,     fz,      uv.uMin, uv.vMin, light,
                fx+1.0f,fy,     fz,      uv.uMax, uv.vMin, light,
                fx+1.0f,fy,     fz+1.0f, uv.uMax, uv.vMax, light,
                fx,     fy,     fz+1.0f, uv.uMin, uv.vMax, light
            });
            break;

        case 4: // Left (-X)
            waterVertices.insert(waterVertices.end(), {
                fx,     fy,     fz+1.0f,  uv.uMax, uv.vMin, light,
                fx,     fy,     fz,       uv.uMin, uv.vMin, light,
                fx,     fy+1.0f,fz,       uv.uMin, uv.vMax, light,
                fx,     fy+1.0f,fz+1.0f,  uv.uMax, uv.vMax, light
            });
            break;

        case 5: // Right (+X)
            waterVertices.insert(waterVertices.end(), {
                fx+1.0f,fy,     fz,       uv.uMax, uv.vMin, light,
                fx+1.0f,fy,     fz+1.0f,  uv.uMin, uv.vMin, light,
                fx+1.0f,fy+1.0f,fz+1.0f,  uv.uMin, uv.vMax, light,
                fx+1.0f,fy+1.0f,fz,       uv.uMax, uv.vMax, light
            });
            break;
    }

    // Add indices
    waterIndices.insert(waterIndices.end(), {
        waterVertexCount, waterVertexCount+1, waterVertexCount+2,
        waterVertexCount, waterVertexCount+2, waterVertexCount+3
    });

    waterVertexCount += 4;
}

void Chunk::setupMesh() {

    // -----------------------------
    // BLOCK MESH
    // -----------------------------
    vao.bind();                  // VAO must be bound BEFORE binding element buffer
    vbo.bind();
    if (!vertices.empty()) {
        vbo.fillData(vertices.data(), vertices.size() * sizeof(float));
    } else {
        // avoid passing nullptr into bad wrappers; create tiny buffer instead
        vbo.fillData(nullptr, 1);
    }

    // Bind EBO while VAO is active — this makes the EBO part of the VAO state
    ebo.bind();
    if (!indices.empty()) {
        ebo.fillData(indices.data(), indices.size() * sizeof(unsigned int));
    } else {
        ebo.fillData(nullptr, 1);
    }

    // Set attributes while VAO bound (pos:3, uv:2, stride = 5 floats)
    vao.setAttr(0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    vao.setAttr(1, 2, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));
    vao.setAttr(2, 1, GL_FLOAT, 6 * sizeof(float), 5 * sizeof(float));

    // -----------------------------
    // WATER MESH
    // -----------------------------
    waterVAO.bind();
    waterVBO.bind();
    if (!waterVertices.empty()) {
        waterVBO.fillData(waterVertices.data(), waterVertices.size() * sizeof(float));
    } else {
        waterVBO.fillData(nullptr, 1);
    }

    waterEBO.bind();
    if (!waterIndices.empty()) {
        waterEBO.fillData(waterIndices.data(), waterIndices.size() * sizeof(unsigned int));
    } else {
        waterEBO.fillData(nullptr, 1);
    }

    waterVAO.setAttr(0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    waterVAO.setAttr(1, 2, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));
    waterVAO.setAttr(2, 1, GL_FLOAT, 6 * sizeof(float), 5 * sizeof(float));

}

void Chunk::drawBlock() {
    if (indices.empty()) return;            // nothing to draw
    if (vertices.empty()) return;

    // Bind block shader and uniforms should be set by caller
    vao.bind();
    GLsizei count = static_cast<GLsizei>(indices.size());
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void Chunk::drawWater() {
    if (waterIndices.empty()) return;
    if (waterVertices.empty()) return;

    waterVAO.bind();
    GLsizei count = static_cast<GLsizei>(waterIndices.size());
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

}

void Chunk::uploadMeshData(const std::vector<float>& verts,
                        const std::vector<unsigned int>& idx,
                        const std::vector<float>& waterVerts,
                        const std::vector<unsigned int>& waterIdx) {

    vertices = verts;
    indices = idx;
    waterVertices = waterVerts;
    waterIndices  = waterIdx;
    
    // Update block mesh data
    vao.bind();
    vbo.bind();
    if (!verts.empty()) {
        vbo.fillData(verts.data(), verts.size() * sizeof(float));
    } else {
        vbo.fillData(nullptr, 1);
    }

    ebo.bind();
    if (!idx.empty()) {
        ebo.fillData(idx.data(), idx.size() * sizeof(unsigned int));
    } else {
        ebo.fillData(nullptr, 1);
    }

    // Update water mesh data
    waterVAO.bind();
    waterVBO.bind();
    if (!waterVerts.empty()) {
        waterVBO.fillData(waterVerts.data(), waterVerts.size() * sizeof(float));
    } else {
        waterVBO.fillData(nullptr, 1);
    }

    waterEBO.bind();
    if (!waterIdx.empty()) {
        waterEBO.fillData(waterIdx.data(), waterIdx.size() * sizeof(unsigned int));
    } else {
        waterEBO.fillData(nullptr, 1);
    }
}

static uint32_t treeHash(uint32_t worldSeed, int wx, int wz) {
    uint32_t h = worldSeed;
    h ^= (uint32_t)wx * 0x27d4eb2d;
    h ^= (uint32_t)wz * 0x165667b1;
    h ^= h >> 15;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    return h;
}

bool Chunk::treeNearby(int x, int y, int z, int radius) {
    for (int dx = -radius; dx <= radius; dx++) {
        for (int dz = -radius; dz <= radius; dz++) {

            int nx = x + dx;
            int nz = z + dz;

            if (nx < 0 || nx >= CHUNK_WIDTH ||
                nz < 0 || nz >= CHUNK_WIDTH)
                continue;

            // Scan vertically for trunks
            for (int ny = y; ny < y + 10 && ny < CHUNK_HEIGHT; ny++) {
                if (blocks[nx][ny][nz].type == BlockType::Log)
                    return true;
            }
        }
    }
    return false;
}


//Tree placement
void Chunk::tryPlaceTree(int x, int y, int z, uint32_t seed) {

    // Only grow on grass
    if (blocks[x][y][z].type != BlockType::Grass)
        return;

    // Avoid water & beaches
    if (y <= 21) return;

    // Avoid chunk edges (important!)
    if (x < 2 || x > CHUNK_WIDTH - 3 ||
        z < 2 || z > CHUNK_WIDTH - 3)
        return;

    int height = 4 + (seed % 4); // 4–7 blocks

    // Vertical space check
    for (int i = 1; i <= height + 2; i++) {
        if (!inBounds(x, y + i, z) ||
            blocks[x][y + i][z].type != BlockType::Air)
            return;
    }

    const int TREE_SPACING = 3;

    //If there is a tree within 3 blocks, do not place
    if (treeNearby(x, y, z, TREE_SPACING))
        return;

    // Convert grass to dirt
    blocks[x][y][z].type = BlockType::Dirt;

    // Trunk
    for (int i = 1; i <= height; i++) {
        blocks[x][y + i][z].type = BlockType::Log;
    }

    // Leaves (simple spherical canopy)
    int cy = y + height;
    for (int dx = -2; dx <= 2; dx++)
    for (int dy = -2; dy <= 1; dy++)
    for (int dz = -2; dz <= 2; dz++) {
        if (dx*dx + dy*dy + dz*dz > 6)
            continue;

        int lx = x + dx;
        int ly = cy + dy;
        int lz = z + dz;

        if (inBounds(lx, ly, lz) &&
            blocks[lx][ly][lz].type == BlockType::Air) {
            blocks[lx][ly][lz].type = BlockType::Leaf;
        }
    }
}

//Generates trees randomly
void Chunk::generateTrees(uint32_t worldSeed) {

    const float TREE_CHANCE = 0.05f; // ~1 tree per 50 columns

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {

            int wx = chunkX * CHUNK_WIDTH + x;
            int wz = chunkZ * CHUNK_WIDTH + z;

            uint32_t seed = treeHash(worldSeed, wx, wz);

            float r = (seed & 0xFFFF) / float(0xFFFF);
            if (r > TREE_CHANCE)
                continue;

            // Find surface
            for (int y = CHUNK_HEIGHT - 2; y > 0; y--) {
                if (blocks[x][y][z].type == BlockType::Grass) {
                    tryPlaceTree(x, y, z, seed);
                    break;
                }
            }
        }
    }
}
