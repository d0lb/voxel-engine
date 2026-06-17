#pragma once
#include "Chunk.h"
#include "../dependencies/FastNoiseLite.h"

class WorldGenerator {
public:
    WorldGenerator(unsigned int seed);
    void generate(Chunk& chunk) const;

private:
    FastNoiseLite m_Noise;
    unsigned int m_Seed;
    static const int WATER_LEVEL = 64;
};