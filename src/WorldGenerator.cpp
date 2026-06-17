#include "WorldGenerator.h"

WorldGenerator::WorldGenerator(unsigned int seed) : m_Seed(seed) {
    m_Noise.SetSeed(m_Seed);
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noise.SetFrequency(0.001f);
    m_Noise.SetFractalOctaves(2);
    m_Noise.SetFractalLacunarity(2.0f);
    m_Noise.SetFractalGain(0.5f);
}

void WorldGenerator::generate(Chunk& chunk) const {
    const int cx = chunk.getChunkX();
    const int cz = chunk.getChunkZ();

    for (int x = 0; x < Chunk::SIZE; ++x) {
        for (int z = 0; z < Chunk::SIZE; ++z) {
            // world coordinates of this column
            int worldX = cx * Chunk::SIZE + x;
            int worldZ = cz * Chunk::SIZE + z;

            // 2D noise gives height between 0 and 1
            float noiseVal = (m_Noise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;
            // map to height range 55..100 (hills up to 100, valleys down to 55)
            int terrainHeight = 55 + static_cast<int>(noiseVal * 60);

            // fill column
            for (int y = 0; y < Chunk::HEIGHT; ++y) {
                BlockType block;
                if (y > terrainHeight) {
                    block = BlockType::Air;
                }
                else if (y == terrainHeight) {
                    // surface block
                    if (y < WATER_LEVEL + 2) {
                        block = BlockType::Sand;  // beach / underwater
                    }
                    else {
                        block = BlockType::Grass;
                    }
                }
                else if (y >= terrainHeight - 3) {
                    block = BlockType::Dirt;
                }
                else {
                    block = BlockType::Stone;
                }

                // place water in empty spaces below sea level
                if (block == BlockType::Air && y <= WATER_LEVEL) {
                    block = BlockType::Water;
                }

                chunk.setBlock(x, y, z, block);
            }
        }
    }
}