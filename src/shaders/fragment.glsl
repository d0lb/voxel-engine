#version 460 core
out vec4 FragColor;

in vec2 vUV;
flat in float vFaceIndex;

uniform int uBlockType;
uniform sampler2D uAtlas;

// Atlas: 4x4 tiles, each 32x32 pixels (128x128 total)
const int TILES_PER_ROW = 4;
const float TILE_SIZE = 1.0 / float(TILES_PER_ROW);
const float BIAS = 0.01; // small offset to avoid edge artifacts

// Map block type + face index -> tile index
int getTileIndex(int blockType, int face) {
    if (blockType == 0) return 0; // Stone
    if (blockType == 1) {         // Grass
        if (face == 0) return 1;   // top
        if (face == 1) return 3;   // bottom (dirt)
        return 2;                  // sides (grass_side)
    }
    if (blockType == 2) return 3; // Dirt
    if (blockType == 3) return 4; // Sand
    if (blockType == 4) return 5; // Water
    return 0; // fallback
}

void main() {
    int face = int(vFaceIndex);
    int tileIdx = getTileIndex(uBlockType, face);

    // Compute tile offset in atlas
    float tileX = float(tileIdx % TILES_PER_ROW) * TILE_SIZE;
    float tileY = float(tileIdx / TILES_PER_ROW) * TILE_SIZE;


    vec2 uv = vUV * (TILE_SIZE - 2.0 * BIAS) + BIAS;
    vec2 atlasUV = vec2(tileX, tileY) + uv;

    vec4 texColor = texture(uAtlas, atlasUV);
    FragColor = texColor;
}