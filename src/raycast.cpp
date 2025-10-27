#include <whatever/raycast.h>

    RayCastHit rayCastBlock(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, const std::vector<std::unique_ptr<Chunk>>& chunks) {
        glm::vec3 pos = origin;
        glm::vec3 step = glm::normalize(direction) * 0.1f;
        glm::vec3 prevPos = pos;

        for (float d = 0.0f; d < maxDistance; d += 0.1f) {
            int bx = static_cast<int>(std::floor(pos.x));
            int by = static_cast<int>(std::floor(pos.y));
            int bz = static_cast<int>(std::floor(pos.z));

            // Find which chunk this block belongs to
            int chunkX = std::floor((float)bx / CHUNK_WIDTH);
            int chunkZ = std::floor((float)bz / CHUNK_WIDTH);

            //Check all chunks till we find a match
            for (auto& chunk : chunks) {
                if (chunk->chunkX == chunkX && chunk->chunkZ == chunkZ) {
                    //the x and z coordinates in that specific chunk, ranges from [0, 15]
                    int localX = ((bx % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;
                    int localZ = ((bz % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;

                    if (by >= 0 && by < CHUNK_HEIGHT) {
                        //If the type is already not air, "break" the block by replacing it with air
                        if (chunk->blocks[localX][by][localZ].type != BlockType::Air) {

                            //find the face that was hit
                            int prevX = static_cast<int>(std::floor(prevPos.x));
                            int prevY = static_cast<int>(std::floor(prevPos.y));
                            int prevZ = static_cast<int>(std::floor(prevPos.z));

                            glm::ivec3 delta = glm::ivec3(bx - prevX, by - prevY, bz - prevZ);
                            glm::ivec3 normal = -delta; // opposite of direction stepped into

                            return { bx, by, bz, true, normal };
                        }
                    }
                }
            }

            prevPos = pos;
            pos += step;
            
        }

    return {0, 0, 0, false, glm::ivec3(0)};

    }

    void setBlockAt(int worldX, int worldY, int worldZ, BlockType newType, std::vector<std::unique_ptr<Chunk>>& chunks) {
        int chunkX = std::floor((float)worldX / CHUNK_WIDTH);
        int chunkZ = std::floor((float)worldZ / CHUNK_WIDTH);

        for (auto& chunk : chunks) {
            if (chunk->chunkX == chunkX && chunk->chunkZ == chunkZ) {
                int localX = ((worldX % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;
                int localZ = ((worldZ % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;

                if (worldY < 0 || worldY >= CHUNK_HEIGHT)
                    return;

                chunk->blocks[localX][worldY][localZ].type = newType;
                chunk->generateChunk();
                chunk->setupMesh();
                return;
            }
        }
    }
