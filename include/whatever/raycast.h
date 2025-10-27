#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <memory>
#include <whatever/chunk.h>

struct RayCastHit {
    int worldX, worldY, worldZ;
    bool hit;
    glm::ivec3 normal;
    
};

RayCastHit rayCastBlock(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, const std::vector<std::unique_ptr<Chunk>>& chunks);
void setBlockAt(int worldX, int worldY, int worldZ, BlockType type, std::vector<std::unique_ptr<Chunk>>& chunks);



#endif