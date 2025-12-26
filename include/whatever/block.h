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
    Sand,
    Water,
    Log,
    Leaf
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
        case BlockType::Water: return {5, 5, 5};
        case BlockType::Log: return {6, 6, 6};
        case BlockType::Leaf: return {7, 7, 7};
    };

    //shouldn't happen
    return {0, 0, 0};
};

inline bool isSunTransparent(BlockType type) {
    return type == BlockType::Water || type == BlockType::Air || type == BlockType::Leaf;
};

class Block{
    public:
    Block() : type(BlockType::Air), blockPosition(0.0f, 0.0f, 0.0f), lightLevel(0) {} 
    Block(BlockType blockType, glm::vec3 blockPosition)
    : type(blockType), blockPosition(blockPosition), lightLevel(0) {}

    BlockType type;
    glm::vec3 blockPosition;
    float lightLevel;

};

#endif