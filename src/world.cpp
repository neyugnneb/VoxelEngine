#include <whatever/world.h>
#include <whatever/chunk.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/common.hpp>

#include <stdlib.h>

World::World(int randomSeed_, int renderDistance_) 
    : randomSeed(randomSeed_), renderDistance(renderDistance_), timeOfDay(12.0f), dayLength(40.0f) {
    
}

void World::update(const glm::vec3& playerPosition) {
    int playerChunkX = (int)floor(playerPosition.x / (float)CHUNK_WIDTH);
    int playerChunkZ = (int)floor(playerPosition.z / (float)CHUNK_WIDTH);

    // Load new chunks around the player
    for (int dx = -renderDistance; dx <= renderDistance; dx++) {
        for (int dz = -renderDistance; dz <= renderDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            long long key = computeChunkKey(chunkX, chunkZ);
            if (chunks.find(key) == chunks.end()) {
                loadChunk(chunkX, chunkZ);
            }
        }
    }

    // Unload distant chunks
    unloadChunk(playerChunkX, playerChunkZ);
}

void World::draw(Shader& blockShader, Shader& waterShader, glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{

    //Gets the light levels for the world
    float wl = worldLight();

    // opaque block
    // pass projection matrix to shader (note that in this case it could change every frame)
    blockShader.setFloat("worldLight", wl);
    blockShader.setMat4("projection", projection);
    blockShader.setMat4("view", view);
    blockShader.setMat4("model", model);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    for (auto& p : chunks) {
        p.second->drawBlock();
    }

    // water/transparent blocks
    waterShader.use();
    waterShader.setFloat("worldLight", wl);
    waterShader.setMat4("projection", projection);
    waterShader.setMat4("view", view);
    waterShader.setMat4("model", model);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);  // DO NOT WRITE DEPTH

    for (auto& p : chunks) {
        p.second->drawWater();
    }

    glDepthMask(GL_TRUE); // restore
}


//Loads chunks within render distance
void World::loadChunk(int chunkX, int chunkZ) {
    long long key = computeChunkKey(chunkX, chunkZ);

    
    if (chunks.find(key) != chunks.end()) {
        return; // Chunk already loaded
    }

    auto chunk = std::make_unique<Chunk>(chunkX, chunkZ, randomSeed);
    chunk->generateChunk();
    chunk->uploadMeshData(chunk->vertices, chunk->indices,
                          chunk->waterVertices, chunk->waterIndices);

    chunk->setupMesh();
    chunks.emplace(key, std::move(chunk));
}

void World::unloadChunk(int playerChunkX, int playerChunkZ) {
    std::vector<long long> chunksToUnload;

    for (const auto& p : chunks) {
        long long key = p.first;

        int chunkX = static_cast<int>(key >> 32);
        int chunkZ = static_cast<int>(key & 0xFFFFFFFF);

        int deleteX = chunkX - playerChunkX;
        int deleteZ = chunkZ - playerChunkZ;

        if (abs(deleteX) > renderDistance ||
            abs(deleteZ) > renderDistance) {
            chunksToUnload.push_back(key);
        }
    }

    for (long long key : chunksToUnload) {
        chunks.erase(key);
    }
}

void World::updateTime(float deltaTime) {
    timeOfDay += deltaTime * (24.0f / dayLength);
    if (timeOfDay >= 24.0f) timeOfDay -= 24.0f;
}

float World::worldLight() {
    return glm::clamp(static_cast<float>(sin((timeOfDay / 24.0f) * glm::pi<float>()) * 2.0f), 0.1f, 1.0f);
}