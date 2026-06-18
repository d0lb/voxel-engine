#version 460 core
out vec4 FragColor;

in vec2 vUV;
flat in float vFaceIndex;
flat in float vBlockType;   // block type id passed from vertex shader

uniform sampler2D uAtlas;

const int TILES_PER_ROW = 4;
const float TILE_SIZE = 1.0 / float(TILES_PER_ROW);
const float BIAS = 0.01;

int getTileIndex(int blockType, int face) {
    if (blockType == 0) return 0; // stone
    if (blockType == 1) {         // grass
        if (face == 0) return 1;  // top
        if (face == 1) return 3;  // bottom (dirt)
        return 2;                 // side
    }
    if (blockType == 2) return 3; // dirt
    if (blockType == 3) return 4; // sand
    if (blockType == 4) return 5; // water
    if (blockType == 6) return 6; // Wood -> stone texture (placeholder)
    if (blockType == 7) return 7; // Leaves -> grass top (placeholder)
    return 0;
}

void main() {
    int face = int(vFaceIndex);
    int tileIdx = getTileIndex(int(vBlockType), face);

    float tileX = float(tileIdx % TILES_PER_ROW) * TILE_SIZE;
    float tileY = float(tileIdx / TILES_PER_ROW) * TILE_SIZE;

    vec2 uv = vUV * (TILE_SIZE - 2.0 * BIAS) + BIAS; // shrink to avoid bleeding
    vec2 atlasUV = vec2(tileX, tileY) + uv;

    vec4 texColor = texture(uAtlas, atlasUV);
    FragColor = texColor;
}