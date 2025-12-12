#ifndef WORLD_H
#define WORLD_H
#include <whatever/chunk.h>
#include <whatever/shader.h>
#include <memory>
#include <unordered_map>

class World {
    public:
        World(int randomSeed, int renderDistance);
        void update(const glm::vec3& playerPosition);
        void draw(Shader& blockShader, Shader& waterShader);

    private:
        int randomSeed;
        int renderDistance;

        std::unordered_map<long long, std::unique_ptr<Chunk>> chunks;

        long long computeChunkKey(int chunkX, int chunkZ) const {
            return ((long long)chunkX << 32) | (unsigned long long)chunkZ;
        }

        void loadChunk(int chunkX, int chunkZ);
        void unloadChunk(int playerChunkX, int playerChunkZ);

};

#endif