#pragma once
#include "Chunk.h"
#include "WorldGenerator.h"
#include <unordered_map>
#include <memory>
#include <glm.hpp>

struct ChunkCoord {
    int x, z;
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

// hash for unordered_map
struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const {
        return std::hash<int>()(c.x) ^ (std::hash<int>()(c.z) << 1);
    }
};

class World {
public:
    World(unsigned int seed);
    ~World();

    void update(const glm::vec3& cameraPos);
    void draw(const Shader& shader) const;
    BlockType getBlock(int worldX, int worldY, int worldZ);
    Chunk* getChunk(int chunkX, int chunkZ);

private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> m_Chunks;
    WorldGenerator m_Generator;
    int m_ViewDistance = 15;

    void loadChunk(int chunkX, int chunkZ);
    void unloadChunk(int chunkX, int chunkZ);
};