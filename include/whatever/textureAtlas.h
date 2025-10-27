#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#define ATLAS_COLS 4
#define TILE_SIZE (1.0f / ATLAS_COLS)

struct UVRect {
    float uMin, vMin, uMax, vMax;
};

inline UVRect getUVForTile(int tileIndex) {
    int col = tileIndex % ATLAS_COLS;
    int row = tileIndex / ATLAS_COLS;
    UVRect uv;
    uv.uMin = col * TILE_SIZE;
    uv.vMin = 1.0f - (row + 1) * TILE_SIZE;
    uv.uMax = uv.uMin + TILE_SIZE;
    uv.vMax = uv.vMin + TILE_SIZE;
    return uv;
}

#endif