#ifndef BLOCK_H
#define BLOCK_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

enum class BlockType : unsigned char {
    Air,
    Grass,
    Dirt,
    Stone,
    Sand
};

struct BlockTexture {
    int topTexture;
    int bottomTexture;
    int sideTexture;
};

inline BlockTexture getTexture(BlockType type) {
    switch (type) {
        //Returns atlas ID for top, bottom, side textures
        case BlockType::Grass: return {0, 2, 1};
        case BlockType::Dirt: return {2, 2, 2};
        case BlockType::Stone: return {3, 3, 3};
        case BlockType::Sand: return {4, 4, 4};
    };

    //shouldn't happen
    return {0, 0, 0};
};

class Block{
    public:
    Block() : type(BlockType::Air), blockPosition(0.0f, 0.0f, 0.0f) {} 
    Block(BlockType blockType, glm::vec3 blockPosition)
    : type(blockType), blockPosition(blockPosition) {}

    BlockType type;
    glm::vec3 blockPosition;
    BlockTexture tex;
};

#endif