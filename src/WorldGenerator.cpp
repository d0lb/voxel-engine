#include "WorldGenerator.h"

WorldGenerator::WorldGenerator(unsigned int seed) : m_Seed(seed) {
    m_Noise.SetSeed(m_Seed);
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noise.SetFrequency(0.01f);       
    m_Noise.SetFractalOctaves(3);
    m_Noise.SetFractalLacunarity(2.0f);
    m_Noise.SetFractalGain(0.5f);
}

void WorldGenerator::generate(Chunk& chunk) const {
    const int cx = chunk.getChunkX();
    const int cz = chunk.getChunkZ();

    // minimum distance between tree trunks (blocks)
    const int MIN_TREE_DIST = 5;
    // grid of reserved spots for trees (true = tree already planned here)
    bool treeReserved[Chunk::SIZE][Chunk::SIZE] = {};

    for (int x = 0; x < Chunk::SIZE; ++x) {
        for (int z = 0; z < Chunk::SIZE; ++z) {
            int worldX = cx * Chunk::SIZE + x;
            int worldZ = cz * Chunk::SIZE + z;

            float noiseVal = (m_Noise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;
            int terrainHeight = 55 + static_cast<int>(noiseVal * 35);
            BlockType surfaceBlock = BlockType::Air;

            // fill the column with blocks
            for (int y = 0; y < Chunk::HEIGHT; ++y) {
                BlockType block;
                if (y > terrainHeight) {
                    block = BlockType::Air;
                }
                else if (y == terrainHeight) {
                    if (y <= WATER_LEVEL) {
                        block = BlockType::Sand;
                    }
                    else {
                        block = BlockType::Grass;
                    }
                    surfaceBlock = block;
                }
                else if (y >= terrainHeight - 3) {
                    block = BlockType::Dirt;
                }
                else {
                    block = BlockType::Stone;
                }

                if (block == BlockType::Air && y <= WATER_LEVEL) {
                    block = BlockType::Water;
                }

                chunk.setBlock(x, y, z, block);
            }

            // tree planning
            if (surfaceBlock == BlockType::Grass && terrainHeight + 1 < Chunk::HEIGHT) {
                // use separate noise for tree density
                float treeNoise = (m_Noise.GetNoise((float)worldX * 0.8f, (float)worldZ * 0.8f) + 1.0f) * 0.5f;
                if (treeNoise > 0.78f) { // ~22% base chance
                    // check the reserved map for any tree within MIN_TREE_DIST
                    bool tooClose = false;
                    for (int dx = -MIN_TREE_DIST; dx <= MIN_TREE_DIST && !tooClose; ++dx) {
                        for (int dz = -MIN_TREE_DIST; dz <= MIN_TREE_DIST && !tooClose; ++dz) {
                            if (dx == 0 && dz == 0) continue;
                            int nx = x + dx;
                            int nz = z + dz;
                            if (nx >= 0 && nx < Chunk::SIZE && nz >= 0 && nz < Chunk::SIZE) {
                                if (treeReserved[nx][nz]) {
                                    tooClose = true;
                                }
                            }
                        }
                    }
                    if (!tooClose) {
                        // reserve this spot and all cells within MIN_TREE_DIST
                        for (int dx = -MIN_TREE_DIST; dx <= MIN_TREE_DIST; ++dx) {
                            for (int dz = -MIN_TREE_DIST; dz <= MIN_TREE_DIST; ++dz) {
                                int nx = x + dx;
                                int nz = z + dz;
                                if (nx >= 0 && nx < Chunk::SIZE && nz >= 0 && nz < Chunk::SIZE) {
                                    treeReserved[nx][nz] = true;
                                }
                            }
                        }
                        placeTree(chunk, x, terrainHeight, z, static_cast<unsigned int>(worldX * 1000 + worldZ));
                    }
                }
            }
        }
    }
}

void WorldGenerator::placeTree(Chunk& chunk, int x, int y, int z, unsigned int seed) const {
    // trunk height: 6-8 blocks (nice tall trunk)
    int trunkHeight = 6 + (seed % 3);
    // canopy dimensions
    const int CANOPY_RADIUS = 3;
    const int CANOPY_HEIGHT = 5; // total height of the leaf ball
    // canopy starts 2 blocks below the top of the trunk (so 3-4 blocks of trunk stay visible)
    int canopyBaseY = y + 1 + trunkHeight - 2;

    // --- safety check: entire tree must fit inside chunk height ---
    if (canopyBaseY + CANOPY_HEIGHT - 1 >= Chunk::HEIGHT)
        return; // not enough space, skip tree (no ugly cutoff)

    // --- 1. place straight trunk ---
    for (int i = 0; i < trunkHeight; ++i) {
        int cy = y + 1 + i;
        if (cy >= Chunk::HEIGHT) break; // should not happen, already checked
        chunk.setBlock(x, cy, z, BlockType::Wood);
    }

    // --- 2. place canopy (ellipsoid) ---
    for (int dy = 0; dy < CANOPY_HEIGHT; ++dy) {
        // radius decreases as we go up, giving a nice rounded shape
        float t = dy / (float)(CANOPY_HEIGHT - 1); // 0..1
        int layerRadius = (int)(CANOPY_RADIUS * (1.0f - t * 0.7f)); // shrinks to ~0.3*radius at top
        if (layerRadius < 1) layerRadius = 1;

        int layerY = canopyBaseY + dy;
        for (int dx = -layerRadius; dx <= layerRadius; ++dx) {
            for (int dz = -layerRadius; dz <= layerRadius; ++dz) {
                // skip corners for rounder shape
                if (abs(dx) == layerRadius && abs(dz) == layerRadius) continue;
                // never overwrite the trunk (central column) except at the very top layer
                if (dx == 0 && dz == 0 && dy < CANOPY_HEIGHT - 1) continue;

                int lx = x + dx;
                int lz = z + dz;
                if (lx < 0 || lx >= Chunk::SIZE || lz < 0 || lz >= Chunk::SIZE) continue;

                chunk.setBlock(lx, layerY, lz, BlockType::Leaves);
            }
        }
    }

    // --- 3. place a leaf directly on top of the trunk (hides the flat top) ---
    int topTrunkY = y + trunkHeight; // last wood block
    int leafAboveY = topTrunkY + 1;
    if (leafAboveY < Chunk::HEIGHT) {
        chunk.setBlock(x, leafAboveY, z, BlockType::Leaves);
    }
}